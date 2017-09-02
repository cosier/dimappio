#include "midi/monitor.h"
#include "colours.h"

static char* COLOUR_SHARP = BGBLACK;
static char* COLOUR_KEY = BGGREY1;
static char* COLOUR_PRESSED = BGRED;
static char* COLOUR_PRESSED_SHARP = BGRED2;
static char* COLOUR_MAPPED = BGGREEN;
static char* COLOUR_MAPPED_SHARP = BGGREEN2;

void mm_monitor_render(mm_options* options, mm_key_node* tail,
                       mm_key_set* key_set) {

    if (options->first) {
        mm_debug("mm_monitor_render() : called for the first time\n");
        // Buffer the terminal for the preceeding mm_clear()
        printf("\n\n\n");
        options->first = 0;
    }

    int i = 0;
    int sharp = 0;
    int c = 0;

    int keys = options->keys;

    // Enforce a hard limit on keys due to midi 128 limit.
    if (keys > 128) {
        keys = 128;
    } else if (keys < 25) {
        keys = 25;
    }

    int sharp_lookup[128] = {0};
    int lookup[128] = {0};
    int clear_count = 4;
    int midi_start = 60 - (keys / 2);

    if (midi_start < 0) {
        midi_start = 0;
    }

    // mm_debug("midi_start(%d)\n", midi_start);

    ++clear_count;
    if (options->mapping->group_count) {
    }

    char* colour = NULL;
    char* mkeys = malloc(sizeof(char*) * 512);
    char* piano = malloc(sizeof(char*) * keys * 8);

    mkeys[0] = '\0';
    piano[0] = ' ';
    piano[1] = '\0';

    if (key_set != NULL && key_set->count > 0) {
        /* mm_debug("\nRendering key_set(%d)\n", key_set->count); */
        strcat(mkeys, " -> ");
        strcat(mkeys, CYAN);
        for (int i = 0; i < key_set->count; ++i) {
            int key = key_set->keys[i];
            assert(key >= 0 && key <= 128);
            lookup[key] = -1;
            // Render Mapped keys
            char* display = mm_midi_to_note_display(key_set->keys[i]);
            strcat(mkeys, display);
            strcat(mkeys, " ");
            free(display);
        }
        strcat(mkeys, RESET);
    }

    mm_clear(clear_count);
    mm_key_node_list* list = mm_key_node_get_list(tail);

    if (list->size > 0) {
        char* notes = mm_key_node_print_list(list);
        printf("\n%s♬  NOTES:%s %s%s%s %s\n\n", BLUE, RESET, RED, notes, RESET,
               mkeys);
        free(notes);
    } else {
        printf("\n%s♬  NOTES:%s Waiting for MIDI input... \n\n", BLUE, RESET);
    }

    // Iterated currently active mm_key_node(s) into a lookup hit table
    for (int l = 0; l < list->size; ++l) {
        lookup[list->nodes[l]->key] = 1;
    }

    // Build top row of piano keys, taking care with the sharps.
    while (i < keys) {
        int index = i + midi_start;
        c = index % 12;

        if (c == 1 || c == 3 || c == 6 || c == 8 || c == 10) {
            sharp_lookup[index] = 1;
            sharp = 1;
        } else {
            sharp = 0;
        }

        if (sharp) {
            colour = COLOUR_SHARP;
        } else {
            colour = COLOUR_KEY;
        }

        if (lookup[index]) {
            if (lookup[index] == -1) {
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
        strcat(piano, colour);
        strcat(piano, " ");
        strcat(piano, RESET);
        ++i;
    }

    i = 0;
    strcat(piano, "\n ");
    colour = NULL;

    while (i < keys) {
        int index = i + midi_start;
        if (lookup[index] && !sharp_lookup[index]) {
            if (lookup[index] == -1) {
                colour = COLOUR_MAPPED;
            } else {
                colour = COLOUR_PRESSED;
            }
        } else {
            colour = COLOUR_KEY;
        }
        strcat(piano, colour);
        strcat(piano, " ");
        strcat(piano, RESET);

        ++i;
    }

    printf("%s\n", piano);
    free(piano);
    free(mkeys);
    mm_key_node_list_free(list);
    fflush(stdout);
}
