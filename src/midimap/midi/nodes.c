#include "midi/nodes.h"

mm_key_node* mm_key_node_create(int key) {
    mm_key_node* n = malloc(sizeof(mm_key_node));
    n->next = NULL;
    n->map = NULL;
    n->key = key;
    return n;
}

char* mm_key_node_list(mm_key_node* n) {
    char *buf = malloc(sizeof(char *) * 128);
    mm_key_node* ptr = n->next;
    mm_key_node* last_ptr = NULL;

    /* printf("\n%p(%d): mm_key_node_list(%d)", n, n->key, n->key); */

    if (ptr == n) {
        printf("%d", ptr->key);
        return buf;
    }

    if (ptr == NULL) {
        error("\nmm_key_node_list: ptr null\n");
        exit(EXIT_FAILURE);
    }

    bool first = true;
    int safety = 0;
    sprintf(buf, "[]");

    do {
        if (ptr == last_ptr) {
            break;
        } else {
            last_ptr = ptr;
        }

        ++safety;
        if (safety > 32) {
            break;
        }

        if (ptr != NULL) {
            if (first) {
                first = false;
                printf("%d", ptr->key);
            } else {
                printf(", %d", ptr->key);
            }
            ptr = ptr->next;
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
    if (it == NULL) {
        error("mm_key_node_search: invalid list iterator");
        return NULL;
    }

    if (it->key == key) {
        return it;
    }

    /* printf("starting at (%d), looking for: %d\n", it->key, key); */

    while (it->key != key) {
        /* printf("searching on: %d\n", it->key); */
        if (it->key == key) {
            return it;
        }

        // we have searched the list and returned back
        // to the original position, time to bail.
        else if (it->key == start) {
            /* printf("iterator has reached the start\n\n"); */
            return NULL;
        }

        // set the iterator to the next and continue!
        else {
            it = it->next;
        }
    }

    if (it->key == key) {
        return it;
    }

    /* error("mm_key_node_search: search for (%d) was unsuccessful", key); */
    return NULL;
}

/**
 * Insert (append) a node onto a linked structure
 */
void mm_key_node_insert(mm_key_node **tail, mm_key_node* node) {
    /* printf("\n\n%p(%d)\" mm_key_node_insert(%d)", *tail, (*tail)->key, node->key); */
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

    /* pdebug("mm_key_node inserted %d -> %d", (*tail)->key, (*tail)->next->key); */

    // Turn a pointer to a pointer into a pointer,
    // then make that pointer point to node, which is a pointer.
    /* printf("\ntail update: %p = %p", *tail, node); */
    *tail = node;
    /* printf("\ntail done:   %p = %p\n", *tail, node); */
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
    /* printf("\n%p(%d): mm_key_node_remove(%d)", *tail, (*tail)->key, n->key); */

    // Catch attempt to remove last node of a 2 node chain.
    // ie. [0, 30]
    //
    //         tail = 30
    //         node = 30
    //         next = 0
    //   next->next = 30 # the next->next is circular! bad!
    if (n == next->next) {
        /* printf("\n%p(%d):  - self_detected(%d)", *tail, (*tail)->key, n->key); */
        // assume next->next = head
        // ergo, assign head to itself (node len = 1)
        next->next = next;
        // update tail pointer to head
        /* (*tail)->next = *tail; */
        *tail = next;

        // free myself and exit
        /* free(n); */
        return;
    }


    n->next = next->next;
    n->key = next->key;
    n->map = next->map;

    // Catch the tail and shift it back, if necessary
    if (*tail == next) {
        *tail = n;
    }

    if (next != NULL) {
        /* printf("freeing: %d\n", next->key); */
        /* free(next); */
    }
}
