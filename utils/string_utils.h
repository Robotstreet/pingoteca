#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <stdbool.h>
#include <stddef.h>

char* string_duplicate(const char* str);
char* string_copy(char* dest, const char* src, size_t dest_size);
char* string_concatenate(char* dest, const char* src, size_t dest_size);
size_t string_length(const char* str);
int string_compare(const char* str1, const char* str2);
int string_compare_case_insensitive(const char* str1, const char* str2);
bool string_equals(const char* str1, const char* str2);
bool string_equals_case_insensitive(const char* str1, const char* str2);

char* string_find(const char* haystack, const char* needle);
char* string_find_case_insensitive(const char* haystack, const char* needle);
char* string_find_last(const char* haystack, const char* needle);
int string_find_index(const char* haystack, const char* needle);
int string_find_last_index(const char* haystack, const char* needle);
bool string_starts_with(const char* str, const char* prefix);
bool string_ends_with(const char* str, const char* suffix);
bool string_contains(const char* str, const char* substring);

void string_to_upper(char* str);
void string_to_lower(char* str);
void string_trim(char* str);
void string_trim_left(char* str);
void string_trim_right(char* str);
void string_reverse(char* str);
void string_replace_char(char* str, char old_char, char new_char);
int string_replace(char* dest, size_t dest_size, const char* str, const char* old_substr, const char* new_substr);

char** string_split(const char* str, const char* delimiter, size_t* count);
void string_split_free(char** parts, size_t count);
char* string_join(char** strings, size_t count, const char* separator);

int string_to_int(const char* str);
float string_to_float(const char* str);
double string_to_double(const char* str);
bool string_to_bool(const char* str);
char* int_to_string(int value, char* buffer, size_t buffer_size);
char* float_to_string(float value, char* buffer, size_t buffer_size, int precision);
char* bool_to_string(bool value, char* buffer, size_t buffer_size);

bool string_is_empty(const char* str);
bool string_is_whitespace(const char* str);
bool string_is_numeric(const char* str);
bool string_is_integer(const char* str);
bool string_is_float(const char* str);
bool string_is_alpha(const char* str);
bool string_is_alphanumeric(const char* str);

int string_format(char* buffer, size_t buffer_size, const char* format, ...);
int string_format_append(char* buffer, size_t buffer_size, const char* format, ...);

bool string_is_valid_utf8(const char* str);
size_t string_utf8_length(const char* str);
char* string_utf8_substring(const char* str, size_t start, size_t length);

unsigned int string_hash(const char* str);
unsigned int string_hash_case_insensitive(const char* str);

typedef struct {
    char* data;
    size_t length;
    size_t capacity;
} StringBuilder;

StringBuilder* string_builder_create(size_t initial_capacity);
void string_builder_destroy(StringBuilder* sb);
void string_builder_append(StringBuilder* sb, const char* str);
void string_builder_append_char(StringBuilder* sb, char c);
void string_builder_append_int(StringBuilder* sb, int value);
void string_builder_append_float(StringBuilder* sb, float value, int precision);
void string_builder_clear(StringBuilder* sb);
char* string_builder_to_string(StringBuilder* sb);
size_t string_builder_length(const StringBuilder* sb);
bool string_builder_is_empty(const StringBuilder* sb);

void string_path_normalize(char* path);
void string_path_combine(char* dest, size_t dest_size, const char* path1, const char* path2);
void string_path_get_directory(const char* path, char* directory, size_t directory_size);
void string_path_get_filename(const char* path, char* filename, size_t filename_size);
void string_path_get_extension(const char* path, char* extension, size_t extension_size);

#endif // STRING_UTILS_H
