#include "midi/monitor.h"
#include "colours.h"

static char* COLOUR_SHARP = BGBLACK;
static char* COLOUR_KEY = BGGREY1;
static char* COLOUR_PRESSED = BGRED;
static char* COLOUR_PRESSED_SHARP = BGRED2;
static char* COLOUR_MAPPED = BGGREEN;
static char* COLOUR_MAPPED_SHARP = BGGREEN2;

void mm_monitor_render(mm_mapping* mapping, mm_key_node* tail,
                       mm_key_set* key_set) {

    if (mapping != NULL) {
    }

    int i = 0;
    int sharp = 0;
    int oct = 0;
    int c = 0;

    int keys = 89;
    int sharp_lookup[89] = {0};
    int lookup[89] = {0};

    char* mkeys = malloc(sizeof(char*) * 32);
    mkeys[0] = 0;

    if (key_set != NULL) {
        sprintf(mkeys, "%s -> ", CYAN);
        for (int i = 0; i < key_set->count; ++i) {
            lookup[key_set->keys[i]] = -1;
            // Render Mapped keys
            sprintf(mkeys, "%s%s ", mkeys,
                    mm_midi_to_note_display(key_set->keys[i]));
        }
        sprintf(mkeys, "%s%s", mkeys, RESET);
    }

    mm_key_node_list* list = mm_key_node_get_list(tail);
    mm_clear(5);

    if (list->size > 0) {
        printf("\n%s♬  NOTES:%s %s%s%s %s\n\n", BLUE, RESET, RED,
               mm_key_node_print_list(list), RESET, mkeys);
    } else {
        printf("\n%s♬  NOTES:%s Waiting for MIDI input... \n\n", BLUE, RESET);
    }

    for (int l = 0; l < list->size; ++l) {
        lookup[list->nodes[l]->key] = 1;
    }

    char* colour = NULL;
    char* piano = malloc(sizeof(char*) * keys * 4);
    piano[0] = 0;

    while (i < keys) {
        if (c == 1 || c == 3 || c == 6 || c == 8 || c == 10) {
            sharp_lookup[i] = 1;
            sharp = 1;
        } else {
            sharp = 0;
        }

        if (sharp) {
            colour = COLOUR_SHARP;
        } else {
            colour = COLOUR_KEY;
        }

        if (lookup[i]) {
            if (lookup[i] == -1) {
                if (sharp) {
                    colour = COLOUR_MAPPED_SHARP;
                } else {
                    colour = COLOUR_MAPPED;
                }
            } else {
                if (sharp) {
                    colour = COLOUR_PRESSED_SHARP;
                } else {
                    colour = COLOUR_PRESSED;
                }
            }
        }

        sprintf(piano, "%s%s%s%s", piano, colour, " ", RESET);

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
        if (lookup[i] && !sharp_lookup[i]) {
            if (lookup[i] == -1) {
                colour = COLOUR_MAPPED;
            } else {
                colour = COLOUR_PRESSED;
            }
        } else {
            colour = COLOUR_KEY;
        }
        sprintf(piano, "%s%s%s%s", piano, colour, " ", RESET);
        ++i;
    }

    printf("%s\n", piano);

    mm_key_node_list_free(list);
    fflush(stdout);
}
