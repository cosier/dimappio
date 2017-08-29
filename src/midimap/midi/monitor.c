#include "midi/monitor.h"
#include "colours.h"

void mm_monitor_render(mm_mapping* mapping, mm_key_node* tail,
                       mm_key_set* key_set) {

    if (mapping != NULL) {
    }

    int i = 0;
    int sharp = 0;
    int oct = 0;
    int c = 0;

    int keys = 89;
    int lookup[89] = {0};
    for (int lc = 0; lc < 89; ++lc) {
        lookup[lc] = 0;
    }

    char* mkeys = malloc(sizeof(char*) * 32);
    mkeys[0] = 0;

    if (key_set != NULL) {
        sprintf(mkeys, "%s", PURPLE);
        for (int i = 0; i < key_set->count; ++i) {
            lookup[key_set->keys[i]] = 1;
            sprintf(mkeys, "%s%d ", mkeys, key_set->keys[i]);
        }
        sprintf(mkeys, "%s%s", mkeys, RESET);
    }

    mm_key_node_list* list = mm_key_node_get_list(tail);
    mm_clear(4);

    if (list->size > 0) {
        printf("\n♬  NOTEs: %s(%d)%s %s [%s]\n\n", BLUE, list->size, RESET,
               mm_key_node_print_list(list), mkeys);
    } else {
        printf("\n♬  NOTEs: [%s]\n\n", mkeys);
    }

    for (int l = 0; l < list->size; ++l) {
        lookup[list->nodes[l]->key] = 1;
    }

    char* colour = NULL;
    char* piano = malloc(sizeof(char*) * keys * 4);
    piano[0] = 0;

    while (i < keys) {
        if (c == 1 || c == 3 || c == 6 || c == 8 || c == 10) {
            sharp = 1;
        } else {
            sharp = 0;
        }

        if (sharp) {
            colour = BLACK;
        } else {
            colour = WHITE;
        }

        if (lookup[i]) {
            colour = MAGENTA;
        }

        sprintf(piano, "%s%s%s%s", piano, colour, "█", RESET);

        if (c == 11) {
            c = -1;
            ++oct;
        }

        ++c;
        ++i;
    }

    i = 0;
    sprintf(piano, "%s\n", piano);
    colour = NULL;
    while (i < keys) {
        if (lookup[i]) {
            colour = MAGENTA;
        } else {
            colour = WHITE;
        }
        sprintf(piano, "%s%s%s%s", piano, colour, "█", RESET);
        ++i;
    }

    printf("%s", piano);

    mm_key_node_list_free(list);
    fflush(stdout);
}
