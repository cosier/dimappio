#ifndef MIDI_MAPPINGS_H
#define MIDI_MAPPINGS_H
#include <stdlib.h>
#include "utils.h"

#define MAX_MIDI_NOTES 128

typedef struct mm_key_map {
    int *dst_group;
    int dst_count;

    int *party_group;
    int party_count;
    int key;
} mm_key_map;

typedef struct mm_mapping {
    mm_key_map index[MAX_MIDI_NOTES];
    short *mapped;
} mm_mapping;

void mm_mapping_print(mm_mapping *mappings);

mm_mapping *mm_build_mapping();
mm_mapping *mm_mapping_from_list(char *list);

#endif
