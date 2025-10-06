#include "util.h"

size_t string_length(const char* str) {
    size_t length = 0;
    while (str[length] != '\0') {
        length++;
    }
    return length;
}

int string_compare(const char* a, const char* b) {
    size_t i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) {
            return (int)((unsigned char)a[i] - (unsigned char)b[i]);
        }
        i++;
    }
    return (int)((unsigned char)a[i] - (unsigned char)b[i]);
}

int string_starts_with(const char* str, const char* prefix) {
    size_t i = 0;
    while (prefix[i] != '\0') {
        if (str[i] != prefix[i]) {
            return 0;
        }
        i++;
    }
    return 1;
}

void string_copy(char* dest, const char* src) {
    size_t i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

int string_to_int(const char* str, int* value) {
    int result = 0;
    int sign = 1;
    size_t i = 0;

    if (str[0] == '-') {
        sign = -1;
        i = 1;
    }

    if (str[i] == '\0') {
        return 0;
    }

    for (; str[i] != '\0'; i++) {
        if (str[i] < '0' || str[i] > '9') {
            return 0;
        }
        result = result * 10 + (str[i] - '0');
    }

    *value = result * sign;
    return 1;
}
