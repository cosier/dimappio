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
static bool is_number(char* input) {
    unsigned char ch = toupper((unsigned char)input[0]);
    int i = 0;

    while (ch != '\0') {
        /* printf("is_number(%c)\n", ch); */
        if (!isdigit(ch)) {
            return false;
        }
        i++;
        ch = toupper((unsigned char)input[i]);
    }

    return true;
}

int mm_parse_to_midi(char* input) {
    if (is_number(input)) {
        return atoi(input);
    }

    // raise or lower note for sharp/flat
    int raise = 0;
    int midi = -1;
    char note = tolower(input[0]);

    // Default fourth octave if not specified
    int oct = 4;

    int len = strlen(input);

    if (len == 3) {
        oct = atoi(&input[2]);
    } else {
        oct = atoi(&input[1]);
    }

    if (len == 2 || len == 3) {
        if (input[1] == '#') {
            raise++;
        } else if (input[1] == 'b') {
            raise--;
        }
    }

    switch (note) {
    case 'c':
        midi = 0;
        break;
    case 'd':
        midi = 2;
        break;
    case 'e':
        midi = 4;
        break;
    case 'f':
        midi = 5;
        break;
    case 'g':
        midi = 7;
        break;
    case 'a':
        midi = 9;
        break;
    case 'b':
        midi = 11;
        break;
    default: {
        printf("unknown note: %c\n", note);
        midi = -2;
    }
    }

    /* printf("determined base midi: %d\n", midi); */

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
