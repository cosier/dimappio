#ifndef MIDI_UTILS_H
#define MIDI_UTILS_H

void debugp(char *);

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>

char *CFStringRefToChars(CFStringRef string);
CFStringRef CharToCFStringRef(char *c);
#endif

#endif
