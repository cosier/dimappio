#ifndef MIDI_MAPPINGS_H
#define MIDI_MAPPINGS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "midi/parser.h"
#include "utils.h"

#define MAX_MIDI_NOTES 128
#define MAX_GROUPED_KEY_MAPS 16
#define MAX_KEY_MAP_DSTS 16

struct mm_key_map;
struct mm_key_group;
struct mm_mapping;

typedef struct mm_tokens {
    char** tokens;
    int count;
} mm_tokens;

typedef struct mm_key_set {
    int* keys;
    int count;
} mm_key_set;

/**
 * Container definition for each Key
 * that is mapped inside a mm_mapping struct.
 */
typedef struct mm_key_map {
    // Set of target keys to activate
    mm_key_set* dst_set;

    // Set of related party members. All of which
    // must be present to initiate the key map.
    mm_key_set* src_set;

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

    // Pointer back to the containing parent
    struct mm_mapping* parent;

    // track origin src key for this group;
    int src;

} mm_key_group;

/**
 * Manager of a group of key mappings.
 * Allow constant lookup for the presence of
 * any key_map(s) organized by src key.
 */
typedef struct mm_mapping {

    // Preallocated, sparse array of key maps pointers.
    // Indexed by origin key and initialized to NULL.
    struct mm_key_group** index;
    int group_count;

    // Array of key_map(s)
    struct mm_key_group** mapped;

    // Total key_map(s) in this mapping
    int count;

} mm_mapping;

mm_mapping* mm_build_mapping();
mm_mapping* mm_mapping_from_list(char* list);

void mm_mapping_free(mm_mapping* mapping);

void mm_mapping_dump(mm_mapping* mapping, char* buf);
void mm_key_group_dump(mm_key_group* g, char* buf);
void mm_key_map_dump(mm_key_map* k, char* buf);

mm_key_set* mm_mapping_group_single_src_dsts(mm_key_group* group);
mm_key_set* mm_mapping_group_srcs(mm_key_group* group);

mm_key_group* mm_get_key_group(mm_mapping* m, int src);

void mm_combine_key_set(mm_key_set* set, mm_key_set* addition);
void mm_remove_key_set(mm_key_set* set, mm_key_set* addition);

void create_src_group(mm_tokens* src_tokens, mm_tokens* dst_tokens);

mm_key_group* create_key_group(mm_mapping* m, int src, mm_tokens* src_tokens,
                               mm_tokens* dst_tokens);

mm_key_map* create_key_map(int src, mm_tokens* src_tokens,
                           mm_tokens* dst_tokens);

mm_key_set* mm_create_key_set(int count);

void update_key_group(mm_key_group* group, int src, mm_tokens* src_tokens,
                      mm_tokens* dst_tokens);

mm_key_set* mm_key_set_copy(mm_key_set* set);
void mm_key_set_remove_single_key(mm_key_set* set, int key);

int mm_token_count(const char* src, char delim);
mm_tokens* mm_token_split(const char* src, char delim);
// mm_mapping_token_set* mm_mapping_tokens(char* src, char* set_delim,
//                                         char* token_delim);

char* mm_tokens_dump(mm_tokens* tokens);

#endif
