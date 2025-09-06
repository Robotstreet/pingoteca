#include "file_utils.h"
#include "string_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

bool file_exists(const char* path) {
    if (!path) return false;
    struct stat st;
    return stat(path, &st) == 0;
}

bool file_is_directory(const char* path) {
    if (!path) return false;
    struct stat st;
    if (stat(path, &st) != 0) return false;
    return S_ISDIR(st.st_mode);
}

size_t file_get_size(const char* path) {
    if (!path) return 0;
    struct stat st;
    if (stat(path, &st) != 0) return 0;
    return st.st_size;
}

FileResult file_get_info(const char* path, FileInfo* info) {
    if (!path || !info) return FILE_ERROR_INVALID_PATH;

    memset(info, 0, sizeof(FileInfo));
    strncpy(info->path, path, sizeof(info->path) - 1);

    struct stat st;
    if (stat(path, &st) != 0) {
        info->exists = false;
        return FILE_ERROR_NOT_FOUND;
    }

    info->exists = true;
    info->size = st.st_size;
    info->is_directory = S_ISDIR(st.st_mode);
    info->is_readable = access(path, R_OK) == 0;
    info->is_writable = access(path, W_OK) == 0;

    return FILE_SUCCESS;
}

FileResult file_delete(const char* path) {
    if (!path) return FILE_ERROR_INVALID_PATH;

    if (remove(path) == 0) {
        return FILE_SUCCESS;
    }

    switch (errno) {
        case ENOENT: return FILE_ERROR_NOT_FOUND;
        case EACCES: return FILE_ERROR_PERMISSION;
        default: return FILE_ERROR_UNKNOWN;
    }
}

FileResult file_copy(const char* source, const char* destination) {
    if (!source || !destination) return FILE_ERROR_INVALID_PATH;

    FILE* src = fopen(source, "rb");
    if (!src) return FILE_ERROR_NOT_FOUND;

    FILE* dst = fopen(destination, "wb");
    if (!dst) {
        fclose(src);
        return FILE_ERROR_WRITE_FAILED;
    }

    char buffer[8192];
    size_t bytes_read;
    FileResult result = FILE_SUCCESS;

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), src)) > 0) {
        if (fwrite(buffer, 1, bytes_read, dst) != bytes_read) {
            result = FILE_ERROR_WRITE_FAILED;
            break;
        }
    }

    fclose(src);
    fclose(dst);

    if (result != FILE_SUCCESS) {
        remove(destination);
    }

    return result;
}

FileResult file_move(const char* source, const char* destination) {
    if (!source || !destination) return FILE_ERROR_INVALID_PATH;

    if (rename(source, destination) == 0) {
        return FILE_SUCCESS;
    }

    FileResult result = file_copy(source, destination);
    if (result == FILE_SUCCESS) {
        file_delete(source);
    }

    return result;
}

FileResult directory_create(const char* path) {
    if (!path) return FILE_ERROR_INVALID_PATH;

    if (mkdir(path, 0755) == 0) {
        return FILE_SUCCESS;
    }

    switch (errno) {
        case EEXIST: return FILE_SUCCESS; // Directory already exists
        case EACCES: return FILE_ERROR_PERMISSION;
        case ENOENT: return FILE_ERROR_NOT_FOUND;
        default: return FILE_ERROR_UNKNOWN;
    }
}

FileResult directory_create_recursive(const char* path) {
    if (!path) return FILE_ERROR_INVALID_PATH;

    char* path_copy = string_duplicate(path);
    if (!path_copy) return FILE_ERROR_MEMORY;

    char* p = path_copy;
    FileResult result = FILE_SUCCESS;

    if (*p == '/') p++;

    while (*p) {
        p = strchr(p, '/');
        if (p) *p = '\0';

        if (!file_exists(path_copy)) {
            result = directory_create(path_copy);
            if (result != FILE_SUCCESS) {
                break;
            }
        }

        if (p) {
            *p = '/';
            p++;
        } else {
            break;
        }
    }

    free(path_copy);
    return result;
}

bool directory_exists(const char* path) {
    return file_exists(path) && file_is_directory(path);
}

FileResult directory_list(const char* path, char*** files, size_t* count) {
    if (!path || !files || !count) return FILE_ERROR_INVALID_PATH;

    DIR* dir = opendir(path);
    if (!dir) return FILE_ERROR_NOT_FOUND;

    *count = 0;
    *files = NULL;

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            (*count)++;
        }
    }

    if (*count == 0) {
        closedir(dir);
        return FILE_SUCCESS;
    }

    *files = malloc(*count * sizeof(char*));
    if (!*files) {
        closedir(dir);
        return FILE_ERROR_MEMORY;
    }

    rewinddir(dir);
    size_t index = 0;
    while ((entry = readdir(dir)) != NULL && index < *count) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            (*files)[index] = string_duplicate(entry->d_name);
            index++;
        }
    }

    closedir(dir);
    return FILE_SUCCESS;
}

void directory_list_free(char** files, size_t count) {
    if (!files) return;
    for (size_t i = 0; i < count; i++) {
        free(files[i]);
    }
    free(files);
}

char* file_read_text(const char* path) {
    if (!path) return NULL;

    FILE* file = fopen(path, "r");
    if (!file) return NULL;

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* content = malloc(size + 1);
    if (!content) {
        fclose(file);
        return NULL;
    }

    size_t read_size = fread(content, 1, size, file);
    content[read_size] = '\0';

    fclose(file);
    return content;
}

FileResult file_write_text(const char* path, const char* content) {
    if (!path || !content) return FILE_ERROR_INVALID_PATH;

    FILE* file = fopen(path, "w");
    if (!file) return FILE_ERROR_WRITE_FAILED;

    size_t content_len = strlen(content);
    size_t written = fwrite(content, 1, content_len, file);

    fclose(file);

    return (written == content_len) ? FILE_SUCCESS : FILE_ERROR_WRITE_FAILED;
}

FileResult file_append_text(const char* path, const char* content) {
    if (!path || !content) return FILE_ERROR_INVALID_PATH;

    FILE* file = fopen(path, "a");
    if (!file) return FILE_ERROR_WRITE_FAILED;

    size_t content_len = strlen(content);
    size_t written = fwrite(content, 1, content_len, file);

    fclose(file);

    return (written == content_len) ? FILE_SUCCESS : FILE_ERROR_WRITE_FAILED;
}

char** file_read_lines(const char* path, size_t* line_count) {
    if (!path || !line_count) return NULL;

    char* content = file_read_text(path);
    if (!content) return NULL;

    char** lines = string_split(content, "\n", line_count);
    free(content);

    return lines;
}

void file_free_lines(char** lines, size_t line_count) {
    string_split_free(lines, line_count);
}

FileResult file_write_lines(const char* path, char** lines, size_t line_count) {
    if (!path || !lines) return FILE_ERROR_INVALID_PATH;

    FILE* file = fopen(path, "w");
    if (!file) return FILE_ERROR_WRITE_FAILED;

    for (size_t i = 0; i < line_count; i++) {
        if (lines[i]) {
            fprintf(file, "%s", lines[i]);
            if (i < line_count - 1) fprintf(file, "\n");
        }
    }

    fclose(file);
    return FILE_SUCCESS;
}

BinaryData file_read_binary(const char* path) {
    BinaryData data = {NULL, 0, false};

    if (!path) return data;

    FILE* file = fopen(path, "rb");
    if (!file) return data;

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    data.data = malloc(size);
    if (!data.data) {
        fclose(file);
        return data;
    }

    data.size = fread(data.data, 1, size, file);
    data.owns_data = true;

    fclose(file);
    return data;
}

FileResult file_write_binary(const char* path, const void* data, size_t size) {
    if (!path || !data) return FILE_ERROR_INVALID_PATH;

    FILE* file = fopen(path, "wb");
    if (!file) return FILE_ERROR_WRITE_FAILED;

    size_t written = fwrite(data, 1, size, file);
    fclose(file);

    return (written == size) ? FILE_SUCCESS : FILE_ERROR_WRITE_FAILED;
}

void binary_data_free(BinaryData* data) {
    if (!data) return;
    if (data->owns_data && data->data) {
        free(data->data);
    }
    data->data = NULL;
    data->size = 0;
    data->owns_data = false;
}

FileResult file_read_struct(const char* path, void* data, size_t size) {
    if (!path || !data) return FILE_ERROR_INVALID_PATH;

    FILE* file = fopen(path, "rb");
    if (!file) return FILE_ERROR_READ_FAILED;

    size_t read_size = fread(data, 1, size, file);
    fclose(file);

    return (read_size == size) ? FILE_SUCCESS : FILE_ERROR_READ_FAILED;
}

FileResult file_write_struct(const char* path, const void* data, size_t size) {
    return file_write_binary(path, data, size);
}

FileResult file_append_struct(const char* path, const void* data, size_t size) {
    if (!path || !data) return FILE_ERROR_INVALID_PATH;

    FILE* file = fopen(path, "ab");
    if (!file) return FILE_ERROR_WRITE_FAILED;

    size_t written = fwrite(data, 1, size, file);
    fclose(file);

    return (written == size) ? FILE_SUCCESS : FILE_ERROR_WRITE_FAILED;
}

void path_get_directory(const char* path, char* directory, size_t size) {
    string_path_get_directory(path, directory, size);
}

void path_get_filename(const char* path, char* filename, size_t size) {
    string_path_get_filename(path, filename, size);
}

void path_get_extension(const char* path, char* extension, size_t size) {
    string_path_get_extension(path, extension, size);
}

void path_remove_extension(const char* path, char* result, size_t size) {
    if (!path || !result) return;

    string_copy(result, path, size);
    char* dot = strrchr(result, '.');
    if (dot && strchr(dot, '/') == NULL && strchr(dot, '\\') == NULL) {
        *dot = '\0';
    }
}

void path_combine(const char* path1, const char* path2, char* result, size_t size) {
    string_path_combine(result, size, path1, path2);
}

void path_normalize(const char* path, char* result, size_t size) {
    if (!path || !result) return;
    string_copy(result, path, size);
    string_path_normalize(result);
}

bool path_is_absolute(const char* path) {
    if (!path) return false;
    return path[0] == '/' || (path[0] != '\0' && path[1] == ':');
}

bool path_is_relative(const char* path) {
    return !path_is_absolute(path);
}

FileConfig* fileconfig_create(void) {
    FileConfig* config = malloc(sizeof(FileConfig));
    if (!config) return NULL;

    config->entries = NULL;
    config->count = 0;
    config->capacity = 0;

    return config;
}

void fileconfig_destroy(FileConfig* config) {
    if (!config) return;
    free(config->entries);
    free(config);
}

static void fileconfig_ensure_capacity(FileConfig* config, size_t needed) {
    if (config->count + needed > config->capacity) {
        size_t new_capacity = config->capacity == 0 ? 16 : config->capacity * 2;
        while (new_capacity < config->count + needed) {
            new_capacity *= 2;
        }

        ConfigEntry* new_entries = realloc(config->entries, new_capacity * sizeof(ConfigEntry));
        if (new_entries) {
            config->entries = new_entries;
            config->capacity = new_capacity;
        }
    }
}

static ConfigEntry* fileconfig_find_entry(const FileConfig* config, const char* key) {
    if (!config || !key) return NULL;

    for (size_t i = 0; i < config->count; i++) {
        if (strcmp(config->entries[i].key, key) == 0) {
            return &config->entries[i];
        }
    }
    return NULL;
}

bool fileconfig_get_string(const FileConfig* config, const char* key, char* value, size_t size) {
    ConfigEntry* entry = fileconfig_find_entry(config, key);
    if (!entry) return false;

    string_copy(value, entry->value, size);
    return true;
}

bool fileconfig_get_int(const FileConfig* config, const char* key, int* value) {
    ConfigEntry* entry = fileconfig_find_entry(config, key);
    if (!entry) return false;

    *value = string_to_int(entry->value);
    return true;
}

bool fileconfig_get_float(const FileConfig* config, const char* key, float* value) {
    ConfigEntry* entry = fileconfig_find_entry(config, key);
    if (!entry) return false;

    *value = string_to_float(entry->value);
    return true;
}

bool fileconfig_get_bool(const FileConfig* config, const char* key, bool* value) {
    ConfigEntry* entry = fileconfig_find_entry(config, key);
    if (!entry) return false;

    *value = string_to_bool(entry->value);
    return true;
}

void fileconfig_set_string(FileConfig* config, const char* key, const char* value) {
    if (!config || !key || !value) return;

    ConfigEntry* entry = fileconfig_find_entry(config, key);
    if (entry) {
        string_copy(entry->value, value, sizeof(entry->value));
        return;
    }

    fileconfig_ensure_capacity(config, 1);
    if (config->count < config->capacity) {
        string_copy(config->entries[config->count].key, key, sizeof(config->entries[config->count].key));
        string_copy(config->entries[config->count].value, value, sizeof(config->entries[config->count].value));
        config->count++;
    }
}

void fileconfig_set_int(FileConfig* config, const char* key, int value) {
    char buffer[32];
    int_to_string(value, buffer, sizeof(buffer));
    fileconfig_set_string(config, key, buffer);
}

void fileconfig_set_float(FileConfig* config, const char* key, float value) {
    char buffer[64];
    float_to_string(value, buffer, sizeof(buffer), 6);
    fileconfig_set_string(config, key, buffer);
}

void fileconfig_set_bool(FileConfig* config, const char* key, bool value) {
    char buffer[16];
    bool_to_string(value, buffer, sizeof(buffer));
    fileconfig_set_string(config, key, buffer);
}

bool fileconfig_has_key(const FileConfig* config, const char* key) {
    return fileconfig_find_entry(config, key) != NULL;
}

void fileconfig_remove_key(FileConfig* config, const char* key) {
    if (!config || !key) return;

    for (size_t i = 0; i < config->count; i++) {
        if (strcmp(config->entries[i].key, key) == 0) {
            for (size_t j = i; j < config->count - 1; j++) {
                config->entries[j] = config->entries[j + 1];
            }
            config->count--;
            break;
        }
    }
}

FileResult fileconfig_load(FileConfig* config, const char* path) {
    if (!config || !path) return FILE_ERROR_INVALID_PATH;

    char* content = file_read_text(path);
    if (!content) return FILE_ERROR_NOT_FOUND;

    size_t line_count;
    char** lines = string_split(content, "\n", &line_count);
    free(content);

    if (!lines) return FILE_ERROR_MEMORY;

    for (size_t i = 0; i < line_count; i++) {
        if (!lines[i] || lines[i][0] == '#' || lines[i][0] == '\0') continue;

        char* equals = strchr(lines[i], '=');
        if (equals) {
            *equals = '\0';
            
            string_trim(lines[i]);
            string_trim(equals + 1);
            
            char* key = string_duplicate(lines[i]);
            char* value = string_duplicate(equals + 1);
            
            if (key && value) {
                fileconfig_set_string(config, key, value);
            }
            
            free(key);
            free(value);
        }
    }

    string_split_free(lines, line_count);
    return FILE_SUCCESS;
}

FileResult fileconfig_save(const FileConfig* config, const char* path) {
    if (!config || !path) return FILE_ERROR_INVALID_PATH;

    FILE* file = fopen(path, "w");
    if (!file) return FILE_ERROR_WRITE_FAILED;

    for (size_t i = 0; i < config->count; i++) {
        fprintf(file, "%s=%s\n", config->entries[i].key, config->entries[i].value);
    }

    fclose(file);
    return FILE_SUCCESS;
}
