#ifndef MIDI_PARSER_H
#define MIDI_PARSER_H
#ifndef __STDC_WANT_LIB_EXT1__
#define __STDC_WANT_LIB_EXT1__ 1
#endif

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "utils.h"

typedef struct ClientPort {
    int client;
    int port;
} ClientPort;

typedef enum Accent { SHARP, FLAT } Accent;

typedef struct mm_note {
    const char* letter;
    Accent acc;
    int midi;
    int oct;
} mm_note;

ClientPort* parse_client_port(char* client_with_port);
mm_note* mm_midi_to_note(int midi, bool sharp);
char* mm_note_print(mm_note* n);

#endif
