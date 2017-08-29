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

int mm_count_lines(char* input) {
    int i = 0;
    int lines = 1;
    while (input[i] != '\0') {
        ++i;
        if (input[i] == '\n') {
            lines++;
        }
    }
    return lines;
}

void mm_clear(int lines) {
    for (int i = 0; i < lines; ++i) {
        printf("\33[2K\r");
        printf("\33[1A\r");
        printf("\33[2K\r");
    }
}

void pdebug(const char* format, ...) {
    if (mm_driver_debug_mode) {
        va_list ap;
        va_start(ap, format);
        vfprintf(stdout, format, ap);
        va_end(ap);
        putc('\n', stdout);
    }
}

void error(char* format, ...) {
    va_list ap;
    va_start(ap, format);
    char* fmt = malloc(sizeof(char*) * 64);
    sprintf(fmt, "\033[32;1m %s \033[0m", format);
    vfprintf(stderr, fmt, ap);
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
