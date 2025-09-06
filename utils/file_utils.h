#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

typedef enum {
    FILE_SUCCESS,
    FILE_ERROR_NOT_FOUND,
    FILE_ERROR_PERMISSION,
    FILE_ERROR_INVALID_PATH,
    FILE_ERROR_READ_FAILED,
    FILE_ERROR_WRITE_FAILED,
    FILE_ERROR_MEMORY,
    FILE_ERROR_UNKNOWN
} FileResult;

typedef struct {
    char path[256];
    size_t size;
    bool exists;
    bool is_directory;
    bool is_readable;
    bool is_writable;
} FileInfo;

typedef struct {
    void* data;
    size_t size;
    bool owns_data;
} BinaryData;

bool file_exists(const char* path);
bool file_is_directory(const char* path);
size_t file_get_size(const char* path);
FileResult file_get_info(const char* path, FileInfo* info);
FileResult file_delete(const char* path);
FileResult file_copy(const char* source, const char* destination);
FileResult file_move(const char* source, const char* destination);

FileResult directory_create(const char* path);
FileResult directory_create_recursive(const char* path);
bool directory_exists(const char* path);
FileResult directory_list(const char* path, char*** files, size_t* count);
void directory_list_free(char** files, size_t count);

char* file_read_text(const char* path);
FileResult file_write_text(const char* path, const char* content);
FileResult file_append_text(const char* path, const char* content);
char** file_read_lines(const char* path, size_t* line_count);
void file_free_lines(char** lines, size_t line_count);
FileResult file_write_lines(const char* path, char** lines, size_t line_count);

BinaryData file_read_binary(const char* path);
FileResult file_write_binary(const char* path, const void* data, size_t size);
void binary_data_free(BinaryData* data);

FileResult file_read_struct(const char* path, void* data, size_t size);
FileResult file_write_struct(const char* path, const void* data, size_t size);
FileResult file_append_struct(const char* path, const void* data, size_t size);

void path_get_directory(const char* path, char* directory, size_t size);
void path_get_filename(const char* path, char* filename, size_t size);
void path_get_extension(const char* path, char* extension, size_t size);
void path_remove_extension(const char* path, char* result, size_t size);
void path_combine(const char* path1, const char* path2, char* result, size_t size);
void path_normalize(const char* path, char* result, size_t size);
bool path_is_absolute(const char* path);
bool path_is_relative(const char* path);

typedef struct {
    char key[64];
    char value[256];
} ConfigEntry;

typedef struct {
    ConfigEntry* entries;
    size_t count;
    size_t capacity;
} FileConfig;

FileConfig* fileconfig_create(void);
void fileconfig_destroy(FileConfig* config);
FileResult fileconfig_load(FileConfig* config, const char* path);
FileResult fileconfig_save(const FileConfig* config, const char* path);
bool fileconfig_get_string(const FileConfig* config, const char* key, char* value, size_t size);
bool fileconfig_get_int(const FileConfig* config, const char* key, int* value);
bool fileconfig_get_float(const FileConfig* config, const char* key, float* value);
bool fileconfig_get_bool(const FileConfig* config, const char* key, bool* value);
void fileconfig_set_string(FileConfig* config, const char* key, const char* value);
void fileconfig_set_int(FileConfig* config, const char* key, int value);
void fileconfig_set_float(FileConfig* config, const char* key, float value);
void fileconfig_set_bool(FileConfig* config, const char* key, bool value);
bool fileconfig_has_key(const FileConfig* config, const char* key);
void fileconfig_remove_key(FileConfig* config, const char* key);

#endif // FILE_UTILS_H
