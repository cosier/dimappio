#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <midimap/core.h>

void midi_to_note_transform() {
    mm_note* note = mm_midi_to_note(20, true);
    char* note_print = mm_note_print(note);
    printf("note: %s\n\n", note_print);

    assert(strcmp(note_print, "G#1") == 0);
}

void note_to_midi_transform() {
    char* n = "C#3";
    int midi = 36;

    assert(mm_parse_anything_to_midi(n) == midi);
}

int main() {
    printf("Running note_parser tests\n");

    midi_to_note_transform();
    note_to_midi_transform();

    return 0;
}
