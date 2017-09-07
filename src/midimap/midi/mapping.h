#ifndef MIDI_MAPPINGS_H
#define MIDI_MAPPINGS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "midi/nodes.h"
#include "midi/parser.h"
#include "utils.h"

#define MAX_MIDI_NOTES 128
#define MAX_GROUPED_KEY_MAPS 16
#define MAX_KEY_MAP_DSTS 16
#define KEY_MAP_ID_SIZE 10000

struct dm_key_map;
struct dm_key_group;
struct dm_mapping;

typedef struct dm_tokens {
    char** tokens;
    int count;
} dm_tokens;

typedef struct dm_keylet {
    int key;
    int ch;
} dm_keylet;

typedef struct dm_key_set {
    dm_keylet** keys;
    int count;
} dm_key_set;

/**
 * Container definition for each Key
 * that is mapped inside a dm_mapping struct.
 */
typedef struct dm_key_map {
    // Set of target keys to activate
    dm_key_set* dst_set;

    // Set of related party members. All of which
    // must be present to initiate the key map.
    dm_key_set* src_set;

    int channel_in;
    int channel_out;

    // Source key for this definition
    int key;

    unsigned int id;

} dm_key_map;

/**
 * Groups key_maps by their origin src key.
 * Allowing multiple keys actions and groups to be
 * looked up by a single key.
 */
typedef struct dm_key_group {
    // Will always be greater than zero,
    // otherwise this key group would not exist.
    int count;

    // Contains at least one dm_key_map
    dm_key_map** maps;

    // Pointer back to the containing parent
    struct dm_mapping* parent;

    // track origin src key for this group;
    int src;

} dm_key_group;

/**
 * Manager of a group of key mappings.
 * Allow constant lookup for the presence of
 * any key_map(s) organized by src key.
 */
typedef struct dm_mapping {

    // Preallocated, sparse array of key maps pointers.
    // Indexed by origin key and initialized to NULL.
    struct dm_key_group** index;
    int group_count;

    // Array of key_map(s)
    struct dm_key_group** mapped;

    // Total key_map(s) in this mapping
    // int count;

} dm_mapping;

dm_mapping* dm_build_mapping();
dm_mapping* dm_mapping_from_list(char* list);

char* dm_mapping_extract_channel(const char* src, int* ch);

void dm_mapping_free(dm_mapping* mapping);

void dm_mapping_dump(dm_mapping* mapping, char* buf);
void dm_key_group_dump(dm_key_group* g, char* buf, int* id_index);
void dm_key_map_dump(dm_key_map* k, char* buf);

dm_key_set* dm_mapping_group_single_src_dsts(dm_key_group* group);
dm_key_set* dm_mapping_group_all_dsts(dm_key_group* grp, dm_key_node* tail,
                                      int note_on, int chan);
dm_key_set* dm_mapping_group_srcs(dm_key_group* group);

dm_key_group* dm_get_key_group(dm_mapping* m, int src);

void dm_combine_key_set(dm_key_set* set, dm_key_set* addition);
void dm_remove_key_set(dm_key_set* set, dm_key_set* addition);

void create_src_group(dm_tokens* src_tokens, dm_tokens* dst_tokens);

dm_key_group* create_key_group(dm_mapping* m, int src, int ich, int och,
                               dm_tokens* src_tokens, dm_tokens* dst_tokens);

dm_key_map* create_key_map(int src, int ich, int och, dm_tokens* src_tokens,
                           dm_tokens* dst_tokens);

dm_key_set* dm_create_key_set(int count);

void update_key_group(dm_key_group* group, int src, int ich, int och,
                      dm_tokens* src_tokens, dm_tokens* dst_tokens);

dm_key_set* dm_key_set_copy(dm_key_set* set);
char* dm_key_set_dump(dm_key_set* set);
void dm_key_set_remove_single_key(dm_key_set* set, int key);

int dm_token_count(const char* src, char delim);
dm_tokens* dm_token_split(const char* src, char delim);

char* dm_tokens_dump(dm_tokens* tokens);
void dm_tokens_free(dm_tokens* tokens);

dm_keylet* dm_keylet_create(int key, int ch);
void dm_keylets_free(dm_keylet** k, int c);
#endif
