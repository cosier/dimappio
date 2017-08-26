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

char* mm_note_print(mm_note* n) {
    char *buf = malloc(sizeof(char *) * 16);
    sprintf(buf, "%s%d", n->letter, n->oct);
    return buf;
}

ClientPort* parse_client_port(char* client_with_port) {
    ClientPort* cp = malloc(sizeof(ClientPort));
    cp->client = 0;
    cp->port = 0;

    if (strstr(client_with_port, ":") == NULL) {
        error("client_with_port(%s) does not contain \":\" delimiter",
              client_with_port);
        return NULL;
    }

    char* container;
    client_with_port = strdup(client_with_port);
    char* token = strtok_r(client_with_port, ":", &container);

    if (token != NULL) {
        cp->client = atoi(token);
    } else {
        error("parsing of (%s) failed due to client token", client_with_port);
    }

    token = strtok_r(NULL, "", &container);

    if (token != NULL) {
        cp->port = atoi(token);
    } else {
        error("parsing of (%s) failed due to port token", client_with_port);
    }

    return cp;
}
