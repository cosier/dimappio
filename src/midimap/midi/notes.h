#ifndef MIDI_NOTES_H
#define MIDI_NOTES_H

typedef struct mm_note_participation {
    int** current;
    int csize;

    int* index[128];

} mm_note_participation;

typedef struct mm_note_participation_list {
    int src;
    int* list;
    int size;
} mm_note_participation_list;

typedef enum Accent { SHARP, FLAT } Accent;

typedef struct mm_note {
    const char* letter;
    Accent acc;
    int midi;
    int oct;
} mm_note;

mm_note_participation* mm_note_participation_create();

void mm_note_participation_insert(mm_note_participation* p,
                                  mm_note_participation_list* list);

mm_note_participation_list*
mm_note_participation_pending_list(mm_note_participation* p, int midi);

void mm_note_participation_pending(mm_note_participation* p);

#endif
