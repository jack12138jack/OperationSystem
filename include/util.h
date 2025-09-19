#ifndef EDUOS_UTIL_H
#define EDUOS_UTIL_H

#include "common.h"

size_t string_length(const char* str);
int string_compare(const char* a, const char* b);
int string_starts_with(const char* str, const char* prefix);
void string_copy(char* dest, const char* src);
int string_to_int(const char* str, int* value);

#endif /* EDUOS_UTIL_H */
