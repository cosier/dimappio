
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <midimap/core.h>

/**
 * Testing the linkage of a mm_key_node's singly linked list.
 */
int main(int argc, char **argv) {
    printf("Running key_node tests:\n");

    mm_key_node * node_1 = mm_key_node_create(10);
    mm_key_node * node_2 = mm_key_node_create(20);
    mm_key_node * node_3 = mm_key_node_create(30);

    mm_key_node *tail = mm_key_node_head();

    mm_key_node_insert(&tail, node_1);

    printf("%d == %d\n", tail->key, node_1->key);
    assert(tail->key == node_1->key);

    mm_key_node_insert(&tail, node_2);

    printf("%d == %d\n", tail->key, node_2->key);
    assert(tail->key == node_2->key);

    mm_key_node_insert(&tail, node_3);

    printf("%d == %d\n", tail->key, node_3->key);
    assert(tail->key == node_3->key);

    // enough allocation to hold the resulting string.
    char *list = malloc(sizeof(char *) * 128);
    mm_key_node_list(list, tail);

    printf("list(%lu): %s\n", strlen(list), list);
    char *list_expected = "0, 10, 20, 30, ";

    // List should appear as expected, all nodes lined up.
    assert(strcmp(list_expected, list) == 0);

    // Test for node removal
    mm_key_node_remove(&tail, node_2);

    char *list_1 = malloc(sizeof(char *) * 128);
    mm_key_node_list(list_1, tail);

    printf("list(%lu): %s\n\n", strlen(list_1), list_1);
    char *list_expected_1 = "0, 10, 30, ";

    // List should be missing the second node (#20)
    // not including the initial head (0).
    assert(strcmp(list_expected_1, list_1) == 0);


    mm_key_node *get_0 = mm_key_node_search(&tail, 0);
    assert(get_0 != NULL);
    printf("tail->next->key(%d) = get_0->key(%d)\n", tail->next->key, get_0->key);
    assert(tail->next->key == get_0->key);

    mm_key_node *get_30 = mm_key_node_search(&tail, 30);
    assert(get_30 != NULL);

    printf("\n-----------------------------------\n%s",
           "key_nodes test successful\n\n");

}
