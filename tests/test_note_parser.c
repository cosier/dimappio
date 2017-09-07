#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <midimap/core.h>

void midi_to_note_transform() {
    dm_note* note = dm_midi_to_note(20, true);
    char* note_print = dm_note_print(note);
    printf("note: %s\n\n", note_print);

    assert(strcmp(note_print, "G#1") == 0);
}

void note_to_midi_transform() {
    assert(dm_parse_to_midi("c10") == 120);

    assert(dm_parse_to_midi("c0") == 0);
    assert(dm_parse_to_midi("c#0") == 1);
    assert(dm_parse_to_midi("d0") == 2);
    assert(dm_parse_to_midi("d#0") == 3);
    assert(dm_parse_to_midi("e0") == 4);
    assert(dm_parse_to_midi("f0") == 5);
    assert(dm_parse_to_midi("f#0") == 6);
    assert(dm_parse_to_midi("g0") == 7);
    assert(dm_parse_to_midi("g#0") == 8);
    assert(dm_parse_to_midi("a0") == 9);
    assert(dm_parse_to_midi("a#0") == 10);
    assert(dm_parse_to_midi("b0") == 11);

    assert(dm_parse_to_midi("c2") == 24);
    assert(dm_parse_to_midi("c#2") == 25);
    assert(dm_parse_to_midi("d2") == 26);
    assert(dm_parse_to_midi("d#2") == 27);
    assert(dm_parse_to_midi("e2") == 28);
    assert(dm_parse_to_midi("f2") == 29);
    assert(dm_parse_to_midi("f#2") == 30);
    assert(dm_parse_to_midi("g2") == 31);
    assert(dm_parse_to_midi("g#2") == 32);
    assert(dm_parse_to_midi("a2") == 33);
    assert(dm_parse_to_midi("a#2") == 34);
    assert(dm_parse_to_midi("b2") == 35);

    assert(dm_parse_to_midi("c") == 48);
    assert(dm_parse_to_midi("c#") == 49);
    assert(dm_parse_to_midi("d") == 50);
    assert(dm_parse_to_midi("d#") == 51);
    assert(dm_parse_to_midi("e") == 52);
    assert(dm_parse_to_midi("f") == 53);
    assert(dm_parse_to_midi("f#") == 54);
    assert(dm_parse_to_midi("g") == 55);
    assert(dm_parse_to_midi("g#") == 56);
    assert(dm_parse_to_midi("a") == 57);
    assert(dm_parse_to_midi("a#") == 58);
    assert(dm_parse_to_midi("b") == 59);

    assert(dm_parse_to_midi("c8") == 96);
    assert(dm_parse_to_midi("c#8") == 97);
    assert(dm_parse_to_midi("d8") == 98);
    assert(dm_parse_to_midi("d#8") == 99);
    assert(dm_parse_to_midi("e8") == 100);
    assert(dm_parse_to_midi("f8") == 101);
    assert(dm_parse_to_midi("f#8") == 102);
    assert(dm_parse_to_midi("g8") == 103);
    assert(dm_parse_to_midi("g#8") == 104);
    assert(dm_parse_to_midi("a8") == 105);
    assert(dm_parse_to_midi("a#8") == 106);
    assert(dm_parse_to_midi("b8") == 107);

    assert(dm_parse_to_midi("c10") == 120);
    assert(dm_parse_to_midi("c#10") == 121);
    assert(dm_parse_to_midi("d10") == 122);
    assert(dm_parse_to_midi("d#10") == 123);
    assert(dm_parse_to_midi("e10") == 124);
    assert(dm_parse_to_midi("f10") == 125);
    assert(dm_parse_to_midi("f#10") == 126);
    assert(dm_parse_to_midi("g10") == 127);

    assert(dm_parse_to_midi("C#3") == 37);
    assert(dm_parse_to_midi("c#3") == 37);
    assert(dm_parse_to_midi("d#3") == 39);
    assert(dm_parse_to_midi("d#8") == 99);

    assert(dm_parse_to_midi("1") == 1);
    assert(dm_parse_to_midi("9") == 9);
    assert(dm_parse_to_midi("32") == 32);
    assert(dm_parse_to_midi("64") == 64);
    assert(dm_parse_to_midi("114") == 114);
}

int main() {
    printf("Running note_parser tests\n");

    midi_to_note_transform();
    note_to_midi_transform();

    return 0;
}
