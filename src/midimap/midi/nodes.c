#include "midi/nodes.h"

mm_key_node* mm_key_node_create(int key) {
    mm_key_node* n = malloc(sizeof(mm_key_node));
    n->next = NULL;
    n->map = NULL;
    n->key = key;
    return n;
}

mm_key_node_list* mm_key_node_get_list(mm_key_node* node) {
    mm_key_node_list* list = malloc(sizeof(mm_key_node_list));

    // Reasonable maximum - 32 nodes at any given time.
    // How many fingers do you have to press them keys simultaneously?
    list->nodes = malloc(sizeof(mm_key_node*) * 32);
    list->size = 0;

    mm_key_node* p = node;

    do {
        if (p->key >= 0) {
            list->nodes[list->size] = p;
            list->size++;
        }

        p = p->next;
    } while (p != node);

    return list;
}

char* mm_key_node_print_tail(mm_key_node* tail) {
    return mm_key_node_print_list(mm_key_node_get_list(tail));
}

char* mm_key_node_print_list(mm_key_node_list* list) {
    if (list->size == 0) {
        return "";
    }

    mm_key_node* ptr = NULL;

    char* buf = malloc(sizeof(char*) * (8 * list->size));
    buf[0] = 0;

    for (int i = 0; i < list->size; ++i) {
        ptr = list->nodes[i];

        mm_note* n = mm_midi_to_note(ptr->key, true);
        char* note = mm_note_print(n);
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

void mm_key_node_list_free(mm_key_node_list* list) {
    free(list->nodes);
    free(list);
}

int mm_key_node_contains(mm_key_node* tail, int key) {
    mm_key_node* res = mm_key_node_search(&tail, key);
    if (res != NULL) {
        return key;
    } else {
        return 0;
    }
}

/**
 * Search a list (singly linked list) for a given key
 */
mm_key_node* mm_key_node_search(mm_key_node** tail, int key) {
    mm_key_node* handle = *tail;

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

    /* error("mm_key_node_search: search for (%d) was unsuccessful", key); */
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

    // Turn a pointer to a pointer into a pointer,
    // then make that pointer point to node, which is a pointer.
    /* printf("\ntail update: %p = %p", *tail, node); */
    *tail = node;
}

mm_key_node* mm_key_node_head() {
    mm_key_node* head = malloc(sizeof(mm_key_node));
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
void mm_key_node_remove(mm_key_node** tail, mm_key_node* n) {
    mm_key_node* next = n->next;

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
    n->map = next->map;

    // Catch the tail and shift it back, if necessary
    if (*tail == next) {
        *tail = n;
    }

    if (next != NULL) {
        free(next);
    }
}
