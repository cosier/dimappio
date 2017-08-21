#ifndef MIDI_UTILS_H
#define MIDI_UTILS_H

#include <stdbool.h>

void pdebug(const char *format, ...);
void error(const char *format, ...);

bool contains_bit(unsigned val, unsigned bitindex);

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>

char *CFStringRefToChars(CFStringRef string);
CFStringRef CharToCFStringRef(char *c);
#endif

#endif
