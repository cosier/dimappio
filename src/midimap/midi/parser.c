#include "midi/parser.h"

static const char* SHARPS[12] = {"C",  "C#", "D",  "D#", "E",  "F",
                                 "F#", "G",  "G#", "A",  "A#", "B"};

static const char* FLATS[12] = {"C",  "Db", "D",  "Eb", "E",  "F",
                                "Gb", "G",  "Ab", "A",  "Bb", "B"};

mm_note* mm_midi_to_note(int midi, bool sharp) {
    mm_note* n = malloc(sizeof(mm_note));
    n->oct = floor(midi / 12);
    n->midi = midi;

    int index = midi % 12;

    if (sharp) {
        n->letter = SHARPS[index];
    } else {
        n->letter = FLATS[index];
    }

    return n;
}

int mm_tone_to_note(char* tone) {
    if (tone != NULL) {
        // TODO:
    }
}

char* mm_note_print(mm_note* n) {
    char* buf = malloc(sizeof(char*) * 16);
    sprintf(buf, "%s%d", n->letter, n->oct);
    return buf;
}

mm_device* mm_parse_device(char* str) {
    mm_device* dev = malloc(sizeof(mm_device));
    dev->client = 0;
    dev->port = 0;

    if (strstr(str, ":") == NULL) {
        error("client_with_port(%s) does not contain \":\" delimiter", str);
        return NULL;
    }

    char* container;
    str = strdup(str);
    char* token = strtok_r(str, ":", &container);

    if (token != NULL) {
        dev->client = atoi(token);
    } else {
        error("parsing of (%s) failed due to client token", str);
    }

    token = strtok_r(NULL, "", &container);

    if (token != NULL) {
        dev->port = atoi(token);
    } else {
        error("parsing of (%s) failed due to port token", str);
    }

    return dev;
}
