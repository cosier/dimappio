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
    assert(mm_parse_to_midi("c2") == 24);
    assert(mm_parse_to_midi("c#2") == 25);
    assert(mm_parse_to_midi("d2") == 26);
    assert(mm_parse_to_midi("d#2") == 27);
    assert(mm_parse_to_midi("e2") == 28);
    assert(mm_parse_to_midi("f2") == 29);
    assert(mm_parse_to_midi("f#2") == 30);
    assert(mm_parse_to_midi("g2") == 31);
    assert(mm_parse_to_midi("g#2") == 32);
    assert(mm_parse_to_midi("a2") == 33);
    assert(mm_parse_to_midi("a#2") == 34);
    assert(mm_parse_to_midi("b2") == 35);

    assert(mm_parse_to_midi("c8") == 96);
    assert(mm_parse_to_midi("c#8") == 97);
    assert(mm_parse_to_midi("d8") == 98);
    assert(mm_parse_to_midi("d#8") == 99);
    assert(mm_parse_to_midi("e8") == 100);
    assert(mm_parse_to_midi("f8") == 101);
    assert(mm_parse_to_midi("f#8") == 102);
    assert(mm_parse_to_midi("g8") == 103);
    assert(mm_parse_to_midi("g#8") == 104);
    assert(mm_parse_to_midi("a8") == 105);
    assert(mm_parse_to_midi("a#8") == 106);
    assert(mm_parse_to_midi("b8") == 107);

    assert(mm_parse_to_midi("C#3") == 37);
    assert(mm_parse_to_midi("c#3") == 37);
    assert(mm_parse_to_midi("d#3") == 39);
    assert(mm_parse_to_midi("d#8") == 99);

    assert(mm_parse_to_midi("1") == 1);
    assert(mm_parse_to_midi("9") == 9);
    assert(mm_parse_to_midi("32") == 32);
    assert(mm_parse_to_midi("64") == 64);
    assert(mm_parse_to_midi("114") == 114);
}

int main() {
    printf("Running note_parser tests\n");

    midi_to_note_transform();
    note_to_midi_transform();

    return 0;
}
