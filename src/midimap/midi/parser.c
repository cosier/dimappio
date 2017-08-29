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

static bool is_char_number(unsigned ch) {
    if (!isdigit(ch)) {
        return false;
    }

    return true;
}

int mm_parse_to_midi(char* input) {
    // cast first char to unsigned uppercase, for digit check.
    if (is_char_number(toupper((unsigned char)input[0]))) {
        return atoi(input);
    }

    // Container for input letters.
    // Using unsigned to ensure correct isdigit checks in the upmost bits.
    unsigned char c[4] = {0};

    int raise = 0;
    int midi = -1;
    int oct = -1;
    int i = 0;

    while (input[i] != '\0') {
        c[i] = toupper(input[i]);
        if (i > 0) {
            if (c[i] == '#') {
                raise++;
            } else if (c[i] == 'B') {
                raise--;
            } else {
                if (is_char_number(c[i])) {
                    if (input[i + 1] != '\0') {
                        oct = 10;
                    } else if (oct < 0) {
                        oct = atoi(&input[i]);
                    }
                }
            }
        }
        i++;
    }

    if (oct < 0) {
        oct = 4;
    }

    switch (c[0]) {
    case 'C':
        midi = 0;
        break;
    case 'D':
        midi = 2;
        break;
    case 'E':
        midi = 4;
        break;
    case 'F':
        midi = 5;
        break;
    case 'G':
        midi = 7;
        break;
    case 'A':
        midi = 9;
        break;
    case 'B':
        midi = 11;
        break;
    default: {
        printf("unknown note: %c\n", c[0]);
        midi = -2;
    }
    }

    if (midi < 0) {
        error("parser: midi(%d) less than zero.\n\n", midi);
        return 0;

    } else if (midi > 127) {
        error("parser: midi(%d) greater than 127\n\n", midi);
        return 127;
    }

    // apply octave transform and +/- for sharp/flats
    midi = (oct * 12) + midi + raise;

    return midi;
}

char* mm_midi_to_note_display(int midi) {
    mm_note* n = mm_midi_to_note(midi, true);
    return mm_note_print(n);
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
