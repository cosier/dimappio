#ifndef MIDI_UTILS_H
#define MIDI_UTILS_H

#include "colours.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <assert.h>
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>

extern bool mm_driver_debug_mode;
void mm_debug(const char* format, ...);
void error(char* format, ...);
void printd(const char* format, ...);
void mm_clear(int lines);
int64_t mm_micros();

int mm_count_lines(char* input);
bool contains_bit(unsigned val, unsigned bitindex);

int mm_tokenize(char* src, char* delim, char** result);
void mm_cat(char** buf, char* src);

//////////////////////////////////////////////////////////
// HERE BE APPLES
#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>

char* cf_string_ref_to_chars(CFStringRef string);
CFStringRef char_to_cf_string_ref(char* c);
#endif

#endif
