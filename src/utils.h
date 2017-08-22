#ifndef MIDI_UTILS_H
#define MIDI_UTILS_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

extern bool MM_driver_debug_mode;

void pdebug(const char* format, ...);
void error(const char* format, ...);

bool contains_bit(unsigned val, unsigned bitindex);

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>

char* CFStringRefToChars(CFStringRef string);
CFStringRef CharToCFStringRef(char* c);
#endif

#endif
