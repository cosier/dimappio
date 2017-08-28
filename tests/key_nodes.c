
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <midimap/core.h>
#define KEY_LIMIT 128


void test_header(char *name) {
    printf("\n--------------------------------\n%s test\n\n", name);
}

mm_key_node *create_list(int count) {
    mm_key_node *tail = mm_key_node_head();
    for (int i = 1; i < (count + 1) && i < KEY_LIMIT; ++i) {
       mm_key_node_insert(&tail, mm_key_node_create(i));
    }

    return tail;
}

void insertion_test() {
    test_header("insertion");
    mm_key_node *tail = create_list(3);

    // enough allocation to hold the resulting string.
    mm_key_node_list *list = mm_key_node_get_list(tail);

    /* printf("list(%lu): %s\n", strlen(list), list); */
    /* char *expected = "C#0, D0, D#0"; */
    /* // List should appear as expected, all nodes lined up. */
    /* assert(strcmp(expected, list) == 0); */
}

void removal_test() {
    test_header("removal");

    mm_key_node *tail = create_list(10);
    mm_key_node_list *list = mm_key_node_get_list(tail);
    printf("created list: %s\n\n", mm_key_node_print_list(list));

    printf("attempting to search!\n");
    mm_key_node *node = mm_key_node_search(&tail, 5);
    printf("found node: %s\n", mm_midi_to_note(node->key, true)->letter);
    // Test for node removal
    mm_key_node_remove(&tail, node);

    list = mm_key_node_get_list(tail);

    /* printf("list(%lu): \n[%s]\n\n", strlen(list), list); */
    /* char *expected = "C#0, D0, D#0, E0, F#0, G0, G#0, A0, A#0"; */

    // List should be missing the second node (#20)
    // not including the initial head (0).
    /* assert(strcmp(expected, list) == 0); */
}

void search_test() {
    test_header("search");
    mm_key_node *tail = create_list(25);

    mm_key_node *get_10 = mm_key_node_search(&tail, 10);
    assert(get_10 != NULL);

    mm_key_node *get_5 = mm_key_node_search(&tail, 5);
    assert(get_5 != NULL);
    assert(get_5->key == 5);
}

void single_trigger_test() {
    test_header("single_trigger");
    mm_key_node *tail = create_list(1);
    mm_key_node *node = mm_key_node_search(&tail, 1);
    printf("created list: %s\n\n", mm_key_node_print_tail(tail));
    assert(node != NULL);

    mm_key_node_remove(&tail, node);
    printf("updated list: %s\n\n", mm_key_node_print_tail(tail));

    printf("node = %d\n", node->key);
    printf("tail = %d\n", tail->key);

    mm_key_node_insert(&tail, mm_key_node_create(3));
    printf("updated list: %s\n\n", mm_key_node_print_tail(tail));

    mm_key_node_insert(&tail, mm_key_node_create(3));
    printf("updated list: %s\n\n", mm_key_node_print_tail(tail));

    printf("\n\n");
}

/**
 * Testing the linkage of a mm_key_node's singly linked list.
 */
int main(int argc, char **argv) {
    printf("Running key_node tests:\n");

    insertion_test();
    removal_test();
    search_test();
    single_trigger_test();

    printf("\n-----------------------------------\n%s",
           "key_nodes test successful\n\n");

}
