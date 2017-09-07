#ifndef MIDI_NOTES_H
#define MIDI_NOTES_H

typedef struct dm_note_participation {
    int** current;
    int csize;

    int* index[128];

} dm_note_participation;

typedef struct dm_note_participation_list {
    int src;
    int* list;
    int size;
} dm_note_participation_list;

typedef enum Accent { SHARP, FLAT } Accent;

typedef struct dm_note {
    const char* letter;
    Accent acc;
    int midi;
    int oct;
} dm_note;

dm_note_participation* dm_note_participation_create();

void dm_note_participation_insert(dm_note_participation* p,
                                  dm_note_participation_list* list);

dm_note_participation_list*
dm_note_participation_pending_list(dm_note_participation* p, int midi);

void dm_note_participation_pending(dm_note_participation* p);

#endif
