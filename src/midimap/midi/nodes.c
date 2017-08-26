#include "midi/nodes.h"

mm_key_node* mm_key_node_create(int key) {
    mm_key_node* n = malloc(sizeof(mm_key_node));
    n->next = NULL;
    n->map = NULL;
    n->key = key;
    return n;
}

char* mm_key_node_list(char* buf, mm_key_node* n) {
    mm_key_node* ptr = n->next;

    if (ptr->next == n) {
        sprintf(buf, "[]");
    }

    do {
        if (ptr != NULL) {
            sprintf(buf, "%s%d, ", buf, ptr->key);
            ptr = ptr->next;
        } else {
            error("mm_key_node_list: ptr null during list iteration");
            exit(EXIT_FAILURE);
        }

    } while (ptr != n->next);

    return buf;
}

/**
 * Search a list (singly linked list) for a given key
 */
mm_key_node* mm_key_node_search(mm_key_node** tail, int key) {
    mm_key_node *handle = *tail;

    if (handle == NULL) {
        error("mm_key_node_search: handle is null, cannot possibly search for "
              "a key node!");
        return NULL;
    }

    if (handle->key == key) {
        return handle;
    }

    int start = handle->key;
    mm_key_node* it = handle->next;

    if (it->key == key) {
        return it;
    }

    if (it == NULL) {
        error("mm_key_node_search: invalid list iterator");
        return NULL;
    }
    printf("looking for: %d\n", key);
    while (it->key != key) {
        printf("searching on: %d\n", it->key);
        if (it->key == key) {
            return it;
        }

        // we have searched the list and returned back
        // to the original position, time to bail.
        else if (it->key == start) {
            printf("iterator has reached the start\n\n");
            return NULL;
        }

        // set the iterator to the next and continue!
        else {
            it = it->next;
        }
    }

    error("mm_key_node_search: search was unsuccessful :(");
    return NULL;
}

/**
 * Insert (append) a node onto a linked structure
 */
void mm_key_node_insert(mm_key_node** tail, mm_key_node* node) {

    // Set existing tail->next to be the new 'next' of
    // the node being appended to the end of the list.
    //
    // In this case:
    //   tail->next = HEAD
    //   .. becomes ..
    //   node->next = HEAD
    node->next = (*tail)->next;

    // Set the old tail to point to the new node tail
    (*tail)->next = node;

    pdebug("mm_key_node inserted %d -> %d", (*tail)->key, (*tail)->next->key);

    // Turn a pointer to a pointer into a pointer,
    // then make that pointer point to node, which is a pointer.
    *tail = node;
}

mm_key_node* mm_key_node_head() {
    mm_key_node* head = malloc(sizeof(mm_key_node));
    head->next = head;
    head->key = 0;
    return head;
}

/**
 * Remove this node in the list.
 *
 * Done by copying the next node into itself,
 * thus freeing the adjacent node instead.
 */
void mm_key_node_remove(mm_key_node** tail, mm_key_node* n) {
    mm_key_node* next = n->next;

    n->next = next->next;
    n->key = next->key;
    n->map = next->map;

    // Catch the tail and shift it back, if necessary
    if (*tail == next) {
        *tail = n;
    }

    free(next);
}
