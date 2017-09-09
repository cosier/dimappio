#ifndef MIDI_NODES_H
#define MIDI_NODES_H

#include <stdint.h>
#include <stdlib.h>

#include "dimappio/midi/parser.h"
#include <ubelt/utils.h>

struct dm_key_map;

/**
 * Singly linked list for effecient insertion
 * of keys during event processing.
 *
 * stored is the corresponding MIDI key, and any key mappings.
 * while presence in of itself represents active state.
 */
typedef struct dm_key_node {
    struct dm_key_node* next;
    int key;
} dm_key_node;

typedef struct dm_key_node_list {
    dm_key_node** nodes;
    int size;
} dm_key_node_list;

dm_key_node* dm_key_node_head();
dm_key_node* dm_key_node_create(int key);

dm_key_node_list* dm_key_node_get_list(dm_key_node* n);
void dm_key_node_list_free(dm_key_node_list* list);

char* dm_key_node_print_list(dm_key_node_list* list);
char* dm_key_node_print_tail(dm_key_node* tail);

void dm_key_node_insert(dm_key_node** tail, dm_key_node* node);
void dm_key_node_remove(dm_key_node** tail, dm_key_node* node);
dm_key_node* dm_key_node_search(dm_key_node** tail, int key);

int dm_key_node_contains(dm_key_node* tail, int key);

#endif
