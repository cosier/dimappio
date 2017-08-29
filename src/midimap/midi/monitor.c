#include "midi/monitor.h"
#include "colours.h"

void mm_monitor_render(mm_mapping* mapping, mm_key_node* tail,
                       mm_key_set* key_set) {

    if (mapping != NULL) {
    }

    mm_key_node_list* list = mm_key_node_get_list(tail);
    mm_clear(5);

    if (list->size > 0) {
        printf("\n♬  NOTEs: (%d) %s\n\n", list->size,
               mm_key_node_print_list(list));
    } else {
        printf("\n♬  NOTEs: \n\n");
    }

    int i = 0;
    int sharp = 0;
    int oct = 0;
    int c = 0;

    int keys = 89;
    int lookup[89] = {0};

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

    printf("%s\n", piano);

    /* if (key_set != NULL) { */
    /*     char* dst_str = malloc(sizeof(char*) * (key_set->count * 32)); */
    /*     dst_str[0] = 0; */

    /*     for (int i = 0; i < key_set->count; ++i) { */
    /*         int dst_midi = key_set->keys[i]; */

    /*         mm_note* note = mm_midi_to_note(dst_midi, true); */
    /*         sprintf(dst_str, "%s%s%d(%d), ", dst_str, note->letter,
     * note->oct, */
    /*                 dst_midi); */
    /*     } */
    /* } */

    mm_key_node_list_free(list);
    fflush(stdout);
}
