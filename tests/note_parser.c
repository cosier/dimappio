#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <midimap/core.h>

void midi_to_note_transform() {
    mm_note *note = mm_midi_to_note(20);
    assert(strcmp(mm_note_print(note), "Ab0") == 0);
}

int main(int argc, char **argv) {
  printf("Running note_parser tests");

  midi_to_note_transform();

  return 0;
}
