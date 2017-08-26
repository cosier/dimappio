#include "midi/nodes.h"

mm_key_node* mm_key_node_create() {
    mm_key_node* n = malloc(sizeof(mm_key_node));
    n->next = NULL;
    n->map = NULL;
    n->key = 0;
}

char* mm_key_node_list(char* buf2, mm_key_node* n) {
    char *buf = malloc(sizeof(char*) * 100);
    mm_key_node* ptr = n;

    if (ptr->next == n) {
        sprintf(buf, "[]");
    }

    while (ptr->key > 0) {
        sprintf(buf, "%s %d, ", buf, ptr->key);
        ptr = ptr->next;
    }

    return buf;
}

void mm_key_node_insert(mm_key_node** index, mm_key_node **tail,
                        mm_key_node* node) {

    mm_key_node *otail = *tail;
    // Store the node in the index
    index[node->key] = node;

    // Set the next node to the previous node's next.
    // In this case:
    //   tail->next = HEAD
    node->next = (*tail)->next;

    // Set the previous node to point to this node
    (*tail)->next = node;
    pdebug("mm_key_node inserted %d -> %d", (*tail)->key, (*tail)->next->key);

    // Switch the tail to the new node
    *tail = node;
}

/**
 * Remove this node in the list.
 *
 * Done by copying the next node into itself,
 * thus freeing the adjacent node instead.
 */
void mm_key_node_remove(mm_key_node** index, mm_key_node* n) {
    index[n->key] = NULL;
    mm_key_node* next = n->next;
    pdebug("mm_key_node removing %d -> %d", n->key, next->key);

    n->next = next->next;
    n->key = next->key;
    n->map = next->map;

    /* free(next); */
}
