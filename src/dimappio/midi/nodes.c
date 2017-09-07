#include "midi/nodes.h"

dm_key_node* dm_key_node_create(int key) {
    dm_key_node* n = malloc(sizeof(dm_key_node));
    n->next = NULL;
    // n->map = NULL;
    n->key = key;
    return n;
}

dm_key_node_list* dm_key_node_get_list(dm_key_node* node) {
    dm_key_node_list* list = malloc(sizeof(dm_key_node_list));

    // Reasonable maximum - 32 nodes at any given time.
    // How many fingers do you have to press them keys simultaneously?
    list->nodes = malloc(sizeof(dm_key_node*) * 32);
    list->size = 0;

    dm_key_node* p = node;

    do {
        if (p->key >= 0) {
            list->nodes[list->size] = p;
            list->size++;
        }

        p = p->next;
    } while (p != node);

    return list;
}

char* dm_key_node_print_tail(dm_key_node* tail) {
    return dm_key_node_print_list(dm_key_node_get_list(tail));
}

char* dm_key_node_print_list(dm_key_node_list* list) {
    if (list->size == 0) {
        return "";
    }

    dm_key_node* ptr = NULL;

    char* buf = malloc(sizeof(char*) * (8 * list->size));
    buf[0] = 0;

    for (int i = 0; i < list->size; ++i) {
        ptr = list->nodes[i];

        dm_note* n = dm_midi_to_note(ptr->key, true);
        char* note = dm_note_print(n);
        free(n);

        if (note != NULL && strlen(note) > 1) {
            strcat(buf, note);
            // if we have not reached the end, append a comma
            if (i != (list->size - 1)) {
                strcat(buf, ", ");
            }

            free(note);
        }
    }

    return buf;
}

void dm_key_node_list_free(dm_key_node_list* list) {
    free(list->nodes);
    free(list);
}

int dm_key_node_contains(dm_key_node* tail, int key) {
    dm_key_node* res = dm_key_node_search(&tail, key);
    if (res != NULL) {
        return key;
    } else {
        return 0;
    }
}

/**
 * Search a list (singly linked list) for a given key
 */
dm_key_node* dm_key_node_search(dm_key_node** tail, int key) {
    dm_key_node* handle = *tail;

    if (handle == NULL) {
        error("dm_key_node_search: handle is null, cannot possibly search for "
              "a key node!");
        return NULL;
    }

    if (handle->key == key) {
        return handle;
    }

    int start = handle->key;
    dm_key_node* it = handle->next;
    if (it == NULL) {
        error("dm_key_node_search: invalid list iterator");
        return NULL;
    }

    if (it->key == key) {
        return it;
    }

    while (it->key != key) {
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

    /* error("dm_key_node_search: search for (%d) was unsuccessful", key); */
    return NULL;
}

/**
 * Insert (append) a node onto a linked structure
 */
void dm_key_node_insert(dm_key_node** tail, dm_key_node* node) {
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

    // Turn a pointer to a pointer into a pointer,
    // then make that pointer point to node, which is a pointer.
    /* printf("\ntail update: %p = %p", *tail, node); */
    *tail = node;
}

dm_key_node* dm_key_node_head() {
    dm_key_node* head = malloc(sizeof(dm_key_node));
    head->next = head;
    head->key = -1;
    return head;
}

/**
 * Remove this node in the list.
 *
 * Done by copying the next node into itself,
 * thus freeing the adjacent node instead.
 */
void dm_key_node_remove(dm_key_node** tail, dm_key_node* n) {
    dm_key_node* next = n->next;

    // Catch attempt to remove last node of a 2 node chain.
    // ie. [0, 30]
    //
    //         tail = 30
    //         node = 30
    //         next = 0
    //   next->next = 30 # the next->next is circular! bad!
    if (n == next->next) {
        /* printf("\n%p(%d):  - self_detected(%d)", *tail, (*tail)->key,
         * n->key); */
        // assume next->next = head
        // ergo, assign head to itself (node len = 1)
        next->next = next;
        // update tail pointer to head
        /* (*tail)->next = *tail; */
        *tail = next;

        // free myself and exit
        free(n);
        return;
    }

    n->next = next->next;
    n->key = next->key;
    // n->map = next->map;

    // Catch the tail and shift it back, if necessary
    if (*tail == next) {
        *tail = n;
    }

    if (next != NULL) {
        free(next);
    }
}
