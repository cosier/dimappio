#include "midi/monitor.h"

void mm_monitor_render(mm_mapping* mapping, mm_key_node* tail,
                       mm_key_set* key_set) {

    if (mapping != NULL) {
    }

    mm_key_node_list* list = mm_key_node_get_list(tail);
    mm_clear(1);

    if (list->size > 0) {
        printf("\n♬  NOTE: (%d) %s", list->size, mm_key_node_print_list(list));
    } else {
        printf("\n♬  NOTE: []");
    }

    if (key_set != NULL) {
        char* dst_str = malloc(sizeof(char*) * (key_set->count * 32));
        dst_str[0] = 0;

        for (int i = 0; i < key_set->count; ++i) {
            int dst_midi = key_set->keys[i];

            mm_note* note = mm_midi_to_note(dst_midi, true);
            sprintf(dst_str, "%s%s%d(%d), ", dst_str, note->letter, note->oct,
                    dst_midi);
        }
    }

    mm_key_node_list_free(list);
    fflush(stdout);
}
