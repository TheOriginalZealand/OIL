#ifndef text_utils_h
#define text_utils_h

#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include <ctype.h> // Just for fun

static inline bool string_has_prefix(const char *x, const char *y) { return strncmp(x, y, strlen(y)) == 0; }
static inline size_t string_find_nth_character(const char *x, char y, bool *ok) {
    *ok = true;
    char *z = strchr(x, y);
    if (z == NULL) { *ok = false; return 0; }
    return (size_t) (z - x);
}
static inline size_t string_find_end_of_digits(const char *x) { return strspn(x, "0123456789"); }
static inline size_t string_find_end_of_identifier(const char *x) { return strspn(x, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_"); }
static inline size_t string_find_end_of_whitespace(const char *x) { return strspn(x, " \n\t\r"); }
static inline size_t string_find_end_of_string_literal(const char *x, bool *ok) {
    *ok = true;
    for (size_t i = 1; x[i] != '\0'; ++i) { // skip first character
        if (x[i] == '\\') ++i; // zero-cost abstraction
        else if (x[i] == '\"') return i;
    }
    *ok = false;
    return 0;
}

#endif
