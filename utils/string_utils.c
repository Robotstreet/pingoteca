#include "string_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

char* string_duplicate(const char* str) {
    if (!str) return NULL;
    size_t len = strlen(str);
    char* copy = malloc(len + 1);
    if (!copy) return NULL;
    strcpy(copy, str);
    return copy;
}

char* string_copy(char* dest, const char* src, size_t dest_size) {
    if (!dest || !src || dest_size == 0) return dest;
    strncpy(dest, src, dest_size - 1);
    dest[dest_size - 1] = '\0';
    return dest;
}

char* string_concatenate(char* dest, const char* src, size_t dest_size) {
    if (!dest || !src || dest_size == 0) return dest;
    strncat(dest, src, dest_size - strlen(dest) - 1);
    return dest;
}

size_t string_length(const char* str) {
    return str ? strlen(str) : 0;
}

int string_compare(const char* str1, const char* str2) {
    if (!str1 && !str2) return 0;
    if (!str1) return -1;
    if (!str2) return 1;
    return strcmp(str1, str2);
}

int string_compare_case_insensitive(const char* str1, const char* str2) {
    if (!str1 && !str2) return 0;
    if (!str1) return -1;
    if (!str2) return 1;
    return strcasecmp(str1, str2);
}

bool string_equals(const char* str1, const char* str2) {
    return string_compare(str1, str2) == 0;
}

bool string_equals_case_insensitive(const char* str1, const char* str2) {
    return string_compare_case_insensitive(str1, str2) == 0;
}

char* string_find(const char* haystack, const char* needle) {
    if (!haystack || !needle) return NULL;
    return strstr(haystack, needle);
}

char* string_find_case_insensitive(const char* haystack, const char* needle) {
    if (!haystack || !needle) return NULL;
    return strcasestr(haystack, needle);
}

char* string_find_last(const char* haystack, const char* needle) {
    if (!haystack || !needle) return NULL;
    char* last = NULL;
    char* current = (char*)haystack;
    while ((current = strstr(current, needle)) != NULL) {
        last = current;
        current++;
    }
    return last;
}

int string_find_index(const char* haystack, const char* needle) {
    char* pos = string_find(haystack, needle);
    return pos ? (int)(pos - haystack) : -1;
}

int string_find_last_index(const char* haystack, const char* needle) {
    char* pos = string_find_last(haystack, needle);
    return pos ? (int)(pos - haystack) : -1;
}

bool string_starts_with(const char* str, const char* prefix) {
    if (!str || !prefix) return false;
    return strncmp(str, prefix, strlen(prefix)) == 0;
}

bool string_ends_with(const char* str, const char* suffix) {
    if (!str || !suffix) return false;
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);
    if (suffix_len > str_len) return false;
    return strcmp(str + str_len - suffix_len, suffix) == 0;
}

bool string_contains(const char* str, const char* substring) {
    return string_find(str, substring) != NULL;
}

void string_to_upper(char* str) {
    if (!str) return;
    while (*str) {
        *str = toupper(*str);
        str++;
    }
}

void string_to_lower(char* str) {
    if (!str) return;
    while (*str) {
        *str = tolower(*str);
        str++;
    }
}

void string_trim(char* str) {
    string_trim_left(str);
    string_trim_right(str);
}

void string_trim_left(char* str) {
    if (!str) return;
    char* start = str;
    while (*start && isspace(*start)) start++;
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }
}

void string_trim_right(char* str) {
    if (!str) return;
    char* end = str + strlen(str) - 1;
    while (end >= str && isspace(*end)) {
        *end = '\0';
        end--;
    }
}

void string_reverse(char* str) {
    if (!str) return;
    size_t len = strlen(str);
    for (size_t i = 0; i < len / 2; i++) {
        char temp = str[i];
        str[i] = str[len - 1 - i];
        str[len - 1 - i] = temp;
    }
}

void string_replace_char(char* str, char old_char, char new_char) {
    if (!str) return;
    while (*str) {
        if (*str == old_char) *str = new_char;
        str++;
    }
}

int string_replace(char* dest, size_t dest_size, const char* str, const char* old_substr, const char* new_substr) {
    if (!dest || !str || !old_substr || !new_substr) return -1;

    char* temp = malloc(dest_size);
    if (!temp) return -1;

    const char* pos = str;
    char* result_pos = temp;
    size_t old_len = strlen(old_substr);
    size_t new_len = strlen(new_substr);
    int replacements = 0;

    while ((pos = strstr(pos, old_substr)) != NULL) {
        size_t prefix_len = pos - str;
        if (result_pos - temp + prefix_len + new_len >= dest_size - 1) break;

        strncpy(result_pos, str, prefix_len);
        result_pos += prefix_len;
        strcpy(result_pos, new_substr);
        result_pos += new_len;
        pos += old_len;
        str = pos;
        replacements++;
    }

    strcpy(result_pos, str);
    strcpy(dest, temp);
    free(temp);
    return replacements;
}

char** string_split(const char* str, const char* delimiter, size_t* count) {
    if (!str || !delimiter || !count) return NULL;

    *count = 0;
    char* str_copy = string_duplicate(str);
    if (!str_copy) return NULL;

    char* token = strtok(str_copy, delimiter);
    while (token) {
        (*count)++;
        token = strtok(NULL, delimiter);
    }

    if (*count == 0) {
        free(str_copy);
        return NULL;
    }

    char** result = malloc(*count * sizeof(char*));
    if (!result) {
        free(str_copy);
        return NULL;
    }

    strcpy(str_copy, str);
    token = strtok(str_copy, delimiter);
    for (size_t i = 0; i < *count && token; i++) {
        result[i] = string_duplicate(token);
        token = strtok(NULL, delimiter);
    }

    free(str_copy);
    return result;
}

void string_split_free(char** parts, size_t count) {
    if (!parts) return;
    for (size_t i = 0; i < count; i++) {
        free(parts[i]);
    }
    free(parts);
}

char* string_join(char** strings, size_t count, const char* separator) {
    if (!strings || count == 0) return NULL;

    size_t total_len = 0;
    size_t sep_len = separator ? strlen(separator) : 0;

    for (size_t i = 0; i < count; i++) {
        if (strings[i]) total_len += strlen(strings[i]);
        if (i < count - 1) total_len += sep_len;
    }

    char* result = malloc(total_len + 1);
    if (!result) return NULL;

    result[0] = '\0';
    for (size_t i = 0; i < count; i++) {
        if (strings[i]) strcat(result, strings[i]);
        if (i < count - 1 && separator) strcat(result, separator);
    }

    return result;
}

int string_to_int(const char* str) {
    return str ? atoi(str) : 0;
}

float string_to_float(const char* str) {
    return str ? (float)atof(str) : 0.0f;
}

double string_to_double(const char* str) {
    return str ? atof(str) : 0.0;
}

bool string_to_bool(const char* str) {
    if (!str) return false;
    return string_equals_case_insensitive(str, "true") ||
           string_equals_case_insensitive(str, "1") ||
           string_equals_case_insensitive(str, "yes");
}

char* int_to_string(int value, char* buffer, size_t buffer_size) {
    if (!buffer || buffer_size == 0) return NULL;
    snprintf(buffer, buffer_size, "%d", value);
    return buffer;
}

char* float_to_string(float value, char* buffer, size_t buffer_size, int precision) {
    if (!buffer || buffer_size == 0) return NULL;
    snprintf(buffer, buffer_size, "%.*f", precision, value);
    return buffer;
}

char* bool_to_string(bool value, char* buffer, size_t buffer_size) {
    if (!buffer || buffer_size == 0) return NULL;
    snprintf(buffer, buffer_size, "%s", value ? "true" : "false");
    return buffer;
}

bool string_is_empty(const char* str) {
    return !str || str[0] == '\0';
}

bool string_is_whitespace(const char* str) {
    if (!str) return true;
    while (*str) {
        if (!isspace(*str)) return false;
        str++;
    }
    return true;
}

bool string_is_numeric(const char* str) {
    if (!str || *str == '\0') return false;
    if (*str == '-' || *str == '+') str++;
    bool has_digit = false;
    bool has_dot = false;

    while (*str) {
        if (isdigit(*str)) {
            has_digit = true;
        } else if (*str == '.' && !has_dot) {
            has_dot = true;
        } else {
            return false;
        }
        str++;
    }
    return has_digit;
}

bool string_is_integer(const char* str) {
    if (!str || *str == '\0') return false;
    if (*str == '-' || *str == '+') str++;
    while (*str) {
        if (!isdigit(*str)) return false;
        str++;
    }
    return true;
}

bool string_is_float(const char* str) {
    return string_is_numeric(str) && strchr(str, '.');
}

bool string_is_alpha(const char* str) {
    if (!str) return false;
    while (*str) {
        if (!isalpha(*str)) return false;
        str++;
    }
    return true;
}

bool string_is_alphanumeric(const char* str) {
    if (!str) return false;
    while (*str) {
        if (!isalnum(*str)) return false;
        str++;
    }
    return true;
}

int string_format(char* buffer, size_t buffer_size, const char* format, ...) {
    if (!buffer || !format) return -1;
    va_list args;
    va_start(args, format);
    int result = vsnprintf(buffer, buffer_size, format, args);
    va_end(args);
    return result;
}

int string_format_append(char* buffer, size_t buffer_size, const char* format, ...) {
    if (!buffer || !format) return -1;
    size_t current_len = strlen(buffer);
    if (current_len >= buffer_size) return -1;

    va_list args;
    va_start(args, format);
    int result = vsnprintf(buffer + current_len, buffer_size - current_len, format, args);
    va_end(args);
    return result;
}

bool string_is_valid_utf8(const char* str) {
    if (!str) return false;
    while (*str) {
        unsigned char c = *str;
        if (c < 0x80) {
            str++;
        } else if ((c >> 5) == 0x06) {
            if ((*++str & 0xC0) != 0x80) return false;
            str++;
        } else if ((c >> 4) == 0x0E) {
            if ((*++str & 0xC0) != 0x80) return false;
            if ((*++str & 0xC0) != 0x80) return false;
            str++;
        } else if ((c >> 3) == 0x1E) {
            if ((*++str & 0xC0) != 0x80) return false;
            if ((*++str & 0xC0) != 0x80) return false;
            if ((*++str & 0xC0) != 0x80) return false;
            str++;
        } else {
            return false;
        }
    }
    return true;
}

size_t string_utf8_length(const char* str) {
    if (!str) return 0;
    size_t len = 0;
    while (*str) {
        if ((*str & 0xC0) != 0x80) len++;
        str++;
    }
    return len;
}

char* string_utf8_substring(const char* str, size_t start, size_t length) {
    if (!str) return NULL;
    return string_duplicate(str + start);
}

unsigned int string_hash(const char* str) {
    if (!str) return 0;
    unsigned int hash = 5381;
    while (*str) {
        hash = ((hash << 5) + hash) + *str;
        str++;
    }
    return hash;
}

unsigned int string_hash_case_insensitive(const char* str) {
    if (!str) return 0;
    unsigned int hash = 5381;
    while (*str) {
        hash = ((hash << 5) + hash) + tolower(*str);
        str++;
    }
    return hash;
}

StringBuilder* string_builder_create(size_t initial_capacity) {
    StringBuilder* sb = malloc(sizeof(StringBuilder));
    if (!sb) return NULL;

    sb->capacity = initial_capacity > 0 ? initial_capacity : 64;
    sb->data = malloc(sb->capacity);
    if (!sb->data) {
        free(sb);
        return NULL;
    }

    sb->length = 0;
    sb->data[0] = '\0';
    return sb;
}

void string_builder_destroy(StringBuilder* sb) {
    if (!sb) return;
    free(sb->data);
    free(sb);
}

static void string_builder_ensure_capacity(StringBuilder* sb, size_t needed) {
    if (sb->length + needed >= sb->capacity) {
        size_t new_capacity = sb->capacity * 2;
        while (new_capacity <= sb->length + needed) {
            new_capacity *= 2;
        }
        char* new_data = realloc(sb->data, new_capacity);
        if (new_data) {
            sb->data = new_data;
            sb->capacity = new_capacity;
        }
    }
}

void string_builder_append(StringBuilder* sb, const char* str) {
    if (!sb || !str) return;
    size_t str_len = strlen(str);
    string_builder_ensure_capacity(sb, str_len + 1);
    strcpy(sb->data + sb->length, str);
    sb->length += str_len;
}

void string_builder_append_char(StringBuilder* sb, char c) {
    if (!sb) return;
    string_builder_ensure_capacity(sb, 2);
    sb->data[sb->length++] = c;
    sb->data[sb->length] = '\0';
}

void string_builder_append_int(StringBuilder* sb, int value) {
    if (!sb) return;
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%d", value);
    string_builder_append(sb, buffer);
}

void string_builder_append_float(StringBuilder* sb, float value, int precision) {
    if (!sb) return;
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%.*f", precision, value);
    string_builder_append(sb, buffer);
}

void string_builder_clear(StringBuilder* sb) {
    if (!sb) return;
    sb->length = 0;
    if (sb->data) sb->data[0] = '\0';
}

char* string_builder_to_string(StringBuilder* sb) {
    if (!sb || !sb->data) return NULL;
    return string_duplicate(sb->data);
}

size_t string_builder_length(const StringBuilder* sb) {
    return sb ? sb->length : 0;
}

bool string_builder_is_empty(const StringBuilder* sb) {
    return !sb || sb->length == 0;
}

void string_path_normalize(char* path) {
    if (!path) return;
    string_replace_char(path, '\\', '/');
}

void string_path_combine(char* dest, size_t dest_size, const char* path1, const char* path2) {
    if (!dest || !path1 || !path2) return;
    snprintf(dest, dest_size, "%s/%s", path1, path2);
    string_path_normalize(dest);
}

void string_path_get_directory(const char* path, char* directory, size_t directory_size) {
    if (!path || !directory) return;
    char* last_slash = strrchr(path, '/');
    if (!last_slash) last_slash = strrchr(path, '\\');

    if (last_slash) {
        size_t dir_len = last_slash - path;
        if (dir_len < directory_size) {
            strncpy(directory, path, dir_len);
            directory[dir_len] = '\0';
        }
    } else {
        directory[0] = '\0';
    }
}

void string_path_get_filename(const char* path, char* filename, size_t filename_size) {
    if (!path || !filename) return;
    char* last_slash = strrchr(path, '/');
    if (!last_slash) last_slash = strrchr(path, '\\');

    const char* fname = last_slash ? last_slash + 1 : path;
    string_copy(filename, fname, filename_size);
}

void string_path_get_extension(const char* path, char* extension, size_t extension_size) {
    if (!path || !extension) return;
    char* last_dot = strrchr(path, '.');
    if (last_dot && strchr(last_dot, '/') == NULL && strchr(last_dot, '\\') == NULL) {
        string_copy(extension, last_dot, extension_size);
    } else {
        extension[0] = '\0';
    }
}
