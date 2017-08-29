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
    assert(mm_parse_to_midi("c10") == 120);

    assert(mm_parse_to_midi("c0") == 0);
    assert(mm_parse_to_midi("c#0") == 1);
    assert(mm_parse_to_midi("d0") == 2);
    assert(mm_parse_to_midi("d#0") == 3);
    assert(mm_parse_to_midi("e0") == 4);
    assert(mm_parse_to_midi("f0") == 5);
    assert(mm_parse_to_midi("f#0") == 6);
    assert(mm_parse_to_midi("g0") == 7);
    assert(mm_parse_to_midi("g#0") == 8);
    assert(mm_parse_to_midi("a0") == 9);
    assert(mm_parse_to_midi("a#0") == 10);
    assert(mm_parse_to_midi("b0") == 11);

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

    assert(mm_parse_to_midi("c") == 48);
    assert(mm_parse_to_midi("c#") == 49);
    assert(mm_parse_to_midi("d") == 50);
    assert(mm_parse_to_midi("d#") == 51);
    assert(mm_parse_to_midi("e") == 52);
    assert(mm_parse_to_midi("f") == 53);
    assert(mm_parse_to_midi("f#") == 54);
    assert(mm_parse_to_midi("g") == 55);
    assert(mm_parse_to_midi("g#") == 56);
    assert(mm_parse_to_midi("a") == 57);
    assert(mm_parse_to_midi("a#") == 58);
    assert(mm_parse_to_midi("b") == 59);

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

    assert(mm_parse_to_midi("c10") == 120);
    assert(mm_parse_to_midi("c#10") == 121);
    assert(mm_parse_to_midi("d10") == 122);
    assert(mm_parse_to_midi("d#10") == 123);
    assert(mm_parse_to_midi("e10") == 124);
    assert(mm_parse_to_midi("f10") == 125);
    assert(mm_parse_to_midi("f#10") == 126);
    assert(mm_parse_to_midi("g10") == 127);

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
