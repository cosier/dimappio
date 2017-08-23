#ifndef MIDI_MAPPINGS_H
#define MIDI_MAPPINGS_H
#include <stdlib.h>
#include <string.h>

#include "utils.h"

#define MAX_MIDI_NOTES 128
#define MAX_GROUPED_KEY_MAPS 16
#define MAX_KEY_MAP_DSTS 16

/**
 * Container definition for each Key
 * that is mapped inside a mm_mapping struct.
 */
typedef struct mm_key_map {

    // Array of target keys to activate
    int* dst_group;
    // Target keys array size
    int dst_count;

    // Array of related party members. All of which
    // must be present to initiate the key map.
    int* party_group;

    // Party array member size
    int party_count;

    // Source key for this definition
    int key;
} mm_key_map;

/**
 * Groups key_maps by their origin src key.
 * Allowing multiple keys actions and groups to be
 * looked up by a single key.
 */
typedef struct mm_key_group {
    // Will always be greater than zero,
    // otherwise this key group would not exist.
    int count;

    // Contains at least one mm_key_map
    mm_key_map** maps;

} mm_key_group;

/**
 * Manager of a group of key mappings.
 * Allow constant lookup for the presence of
 * any key_map(s) organized by src key.
 */
typedef struct mm_mapping {

    // Preallocated, sparse array of key maps pointers.
    // Indexed by origin key and initialized to NULL.
    mm_key_group** index;
    int group_count;

    // Array of key_map(s)
    short* mapped;

    // Total key_map(s) in this mapping
    short* mcount;

} mm_mapping;

void mm_mapping_dump(mm_mapping* mapping, char* container);

mm_mapping* mm_build_mapping();
mm_mapping* mm_mapping_from_list(char* list);

static mm_key_group* create_key_group(int src, int dst);
static mm_key_map* create_key_map(int src, int dst);
static void update_key_group(mm_key_group* group, int src, int dst);

#endif
