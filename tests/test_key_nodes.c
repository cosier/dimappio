
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "helpers/helpers.h"
#include <midimap/core.h>

#define KEY_LIMIT 128

dm_key_node* create_list(int count) {
    dm_key_node* tail = dm_key_node_head();
    for (int i = 1; i < (count + 1) && i < KEY_LIMIT; ++i) {
        dm_key_node_insert(&tail, dm_key_node_create(i));
    }

    return tail;
}

void insertion_test() {
    dm_key_node* tail = create_list(3);

    // enough allocation to hold the resulting string.
    dm_key_node_list* list = dm_key_node_get_list(tail->next);

    printf("list: %s\n\n", dm_key_node_print_list(list));

    /* // List should appear as expected, all nodes lined up. */
    assert(list->nodes[0]->key == 1);
    assert(list->nodes[1]->key == 2);
    assert(list->nodes[2]->key == 3);
}

void removal_test() {
    dm_key_node* tail = create_list(5);
    dm_key_node_list* list = dm_key_node_get_list(tail);
    printf("created list: %s\n\n", dm_key_node_print_list(list));

    dm_key_node* node = dm_key_node_search(&tail, 3);
    printf("found node: %s\n", dm_midi_to_note(node->key, true)->letter);

    // Test for node removal
    dm_key_node_remove(&tail, node);

    // Get a new list as the previous iterator is now invalidated.
    list = dm_key_node_get_list(tail->next);

    // List should be missing the second node (#20)
    // not including the initial head (0).
    assert(list->nodes[0]->key == 1);
    assert(list->nodes[1]->key == 2);
    // notice, node(3) has been removed
    assert(list->nodes[2]->key == 4);
    assert(list->nodes[3]->key == 5);
}

void search_test() {
    dm_key_node* tail = create_list(25);

    dm_key_node* get_10 = dm_key_node_search(&tail, 10);
    assert(get_10 != NULL);

    dm_key_node* get_5 = dm_key_node_search(&tail, 5);
    assert(get_5 != NULL);
    assert(get_5->key == 5);
}

void single_trigger_test() {
    dm_key_node_list* list = NULL;
    dm_key_node* tail = create_list(1);
    dm_key_node* node = dm_key_node_search(&tail, 1);
    assert(node != NULL);

    dm_key_node_remove(&tail, node);

    list = dm_key_node_get_list(tail);
    assert(list->size == 0);

    dm_key_node_insert(&tail, dm_key_node_create(3));
    dm_key_node_insert(&tail, dm_key_node_create(3));
    list = dm_key_node_get_list(tail);
    assert(list->size == 2);

    assert(tail != NULL);
    assert(tail->next != NULL);
    assert(tail->next != NULL);
}

/**
 * Testing the linkage of a dm_key_node's singly linked list.
 */
int main() {
    printf("Running key_node tests:\n");

    test_run("insertions", &insertion_test);
    test_run("removals", &removal_test);
    test_run("search", &search_test);
    test_run("single_trigger", &single_trigger_test);

    printf("\n-----------------------------------\n%s",
           "key_nodes test successful\n\n");
}
