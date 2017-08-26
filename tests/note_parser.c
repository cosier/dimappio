#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <midimap/core.h>

void midi_to_note_transform() {
    mm_note *note = mm_midi_to_note(20, true);
    char * note_print = mm_note_print(note);
    printf("note: %s\n\n", note_print);

    assert(strcmp(note_print, "G#1") == 0);
}

int main(int argc, char **argv) {
  printf("Running note_parser tests\n");

  midi_to_note_transform();

  return 0;
}
