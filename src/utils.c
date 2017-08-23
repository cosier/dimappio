#include "utils.h"

#ifdef __APPLE__
char* cf_string_ref_to_chars(CFStringRef string) {
    if (string == NULL) {
        return NULL;
    }

    CFIndex length = CFStringGetLength(string);
    CFIndex maxSize =
        CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8) + 1;

    // Allocate memory
    char* buffer = (char*)malloc(maxSize);

    // Attempt conversion into the buffer
    if (CFStringGetCString(string, buffer, maxSize, kCFStringEncodingUTF8)) {
        return buffer;
    }

    // Could not convert CFStringRef, free then return NULL.
    free(buffer);
    return NULL;
}

CFStringRef char_to_cf_string_ref(char* c) {
    return CFStringCreateWithCStringNoCopy(NULL, c, kCFStringEncodingUTF8,
                                           NULL);
}

#endif

void pdebug(const char* format, ...) {
    if (mm_driver_debug_mode) {
        va_list ap;
        va_start(ap, format);
        vfprintf(stdout, format, ap);
        va_end(ap);
        putc('\n', stdout);
    }
}

void error(const char* format, ...) {
    va_list ap;
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    va_end(ap);
    putc('\n', stderr);
}

bool contains_bit(unsigned val, unsigned bitindex) {
    return (val & (1 << bitindex)) != 0;
}

int mm_tokenize(char* src, char* delim, char** result) {
    char* container = NULL;
    char* token = NULL;
    int i = 0;
    do {
        token = strtok_r(src, delim, &container);
        src = NULL;
        if (token != NULL) {
            result[i] = strdup(token);
            i++;
        }
    } while (token != NULL);

    return i;
}
