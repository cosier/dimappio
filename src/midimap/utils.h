#ifndef MIDI_UTILS_H
#define MIDI_UTILS_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern bool mm_driver_debug_mode;

void pdebug(const char* format, ...);
void error(char* format, ...);
void mm_clear(int lines);

int mm_count_lines(char* input);
bool contains_bit(unsigned val, unsigned bitindex);

int mm_tokenize(char* src, char* delim, char** result);

//////////////////////////////////////////////////////////
// HERE BE APPLES
#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>

char* cf_string_ref_to_chars(CFStringRef string);
CFStringRef char_to_cf_string_ref(char* c);
#endif

#endif
