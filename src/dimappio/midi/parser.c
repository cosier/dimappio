#include "dimappio/midi/parser.h"

static const char* SHARPS[12] = {"C",  "C#", "D",  "D#", "E",  "F",
                                 "F#", "G",  "G#", "A",  "A#", "B"};

static const char* FLATS[12] = {"C",  "Db", "D",  "Eb", "E",  "F",
                                "Gb", "G",  "Ab", "A",  "Bb", "B"};

dm_note* dm_midi_to_note(int midi, bool sharp) {
    dm_note* n = malloc(sizeof(dm_note));
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

int dm_parse_to_midi(char* input) {
    if (input == NULL) {
        util_error("Bad midi input: NULL");
        return -1;
    }

    if (!*input) {
        printf("INVALID INPUT!\n");
    }

    unsigned char f = input[0];

    // printf("f: %c\n", f);

    // cast first char to unsigned uppercase, for digit check.
    if (is_char_number(toupper(f))) {
        return atoi(input);
    }

    // Container for input letters.
    // Using unsigned to ensure correct isdigit checks in the upmost bits.
    unsigned char c[4] = {0};

    int raise = 0;
    int midi = -1;
    int oct = -1;
    int i = 1;

    c[0] = toupper(input[0]);

    while (input[i] != '\0') {
        c[i] = toupper(input[i]);
        if (c[i] == '#') {
            raise++;
        } else if (c[i] == 'S') {
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
        util_error("parser: midi(%d) less than zero.\n\n", midi);
        return 0;

    } else if (midi > 127) {
        util_error("parser: midi(%d) greater than 127\n\n", midi);
        return 127;
    }

    // apply octave transform and +/- for sharp/flats
    midi = (oct * 12) + midi + raise;

    return midi;
}

char* dm_midi_to_note_display(int midi) {
    dm_note* n = dm_midi_to_note(midi, true);
    char* printed = dm_note_print(n);

    free(n);
    return printed;
}

char* dm_note_print(dm_note* n) {
    int size = sizeof(char*) * 4;
    char* buf = malloc(size);
    assert(n->letter != NULL);
    assert(n->oct >= 0 && n->oct <= 12);

    snprintf(buf, size, "%s%d", n->letter, n->oct);
    return buf;
}

dm_device* dm_parse_device(char* str) {
    util_debug("parser: dm_parse_device(%s)\n", str);
    dm_device* dev = malloc(sizeof(dm_device) * 1);

    util_debug("parser: dm_parse_device malloc success\n");
    dev->client = 0;
    dev->port = 0;

    if (strstr(str, ":") == NULL) {
        util_error("client_with_port(%s) does not contain \":\" delimiter",
                   str);
        return NULL;
    }

    char* container;
    str = strdup(str);
    util_debug("parser: dm_parse_device: attempting to strtok_r\n");
    char* token = strtok_r(str, ":", &container);

    if (token != NULL) {
        dev->client = atoi(token);
    } else {
        util_error("parsing of (%s) failed due to client token", str);
    }

    token = strtok_r(NULL, "", &container);

    if (token != NULL) {
        dev->port = atoi(token);
    } else {
        util_error("parsing of (%s) failed due to port token", str);
    }

    free(str);
    util_debug("parser: dma_parse_device: success\n");
    return dev;
}
