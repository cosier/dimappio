#ifndef MIDI_PARSER_H
#define MIDI_PARSER_H
#ifndef __STDC_WANT_LIB_EXT1__
#define __STDC_WANT_LIB_EXT1__ 1
#endif

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "midi/device.h"
#include "midi/notes.h"
#include "utils.h"

mm_device* mm_parse_device(char* client_with_port);
mm_note* mm_midi_to_note(int midi, bool sharp);
char* mm_midi_to_note_display(int midi);
char* mm_note_print(mm_note* n);
int mm_tone_to_note(char* tone);
int mm_parse_anything_to_midi(char* input);

#endif
