#ifndef MIDI_NODES_H
#define MIDI_NODES_H

#include <stdint.h>
#include <stdlib.h>

#include "midi/mapping.h"
#include "midi/parser.h"
#include "utils.h"

/**
 * Singly linked list for effecient insertion
 * of keys during event processing.
 *
 * stored is the corresponding MIDI key, and any key mappings.
 * while presence in of itself represents active state.
 */
typedef struct mm_key_node {
    struct mm_key_node* next;
    mm_key_map* map;
    int key;
} mm_key_node;

typedef struct mm_key_node_list {
    mm_key_node** nodes;
    int size;
} mm_key_node_list;

mm_key_node* mm_key_node_head();
mm_key_node* mm_key_node_create(int key);

mm_key_node_list* mm_key_node_get_list(mm_key_node* n);
void mm_key_node_list_free(mm_key_node_list* list);

char* mm_key_node_print_list(mm_key_node_list* list);
char* mm_key_node_print_tail(mm_key_node* tail);

void mm_key_node_insert(mm_key_node** tail, mm_key_node* node);
void mm_key_node_remove(mm_key_node** tail, mm_key_node* node);
mm_key_node* mm_key_node_search(mm_key_node** tail, int key);

int mm_key_node_contains(mm_key_node* tail, int key);

#endif
