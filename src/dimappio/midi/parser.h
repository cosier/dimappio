#ifndef MIDI_PARSER_H
#define MIDI_PARSER_H
#ifndef __STDC_WANT_LIB_EXT1__
#define __STDC_WANT_LIB_EXT1__ 1
#endif

#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "dimappio/midi/device.h"
#include "dimappio/midi/notes.h"
#include <ubelt/utils.h>

dm_device* dm_parse_device(char* client_with_port);
dm_note* dm_midi_to_note(int midi, bool sharp);
char* dm_midi_to_note_display(int midi);
char* dm_note_print(dm_note* n);
int dm_parse_to_midi(char* input);

#endif
