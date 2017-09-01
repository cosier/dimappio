#include "midi/mapping.h"

/**
 * Dumps a description of a mapping to the provided char buffer
 */
void mm_mapping_dump(mm_mapping* mapping, char* buf) {
    if (mapping == NULL) {
        error("Mappings Null");
        return;
    }

    buf[0] = '\0';

    if (mapping->count) {
        sprintf(buf, "%sUser Mapping:%s", BLUE, RESET);
        for (int i = 0; i < mapping->group_count; ++i) {
            mm_key_group_dump(mapping->mapped[i], buf);
        }
    }
}

void mm_key_group_dump(mm_key_group* g, char* buf) {
    for (int i = 0; i < g->count; ++i) {
        mm_key_map_dump(g->maps[i], buf);
    }
}

void mm_key_map_dump(mm_key_map* k, char* buf) {
    char* display = mm_midi_to_note_display(k->key);
    // sprintf(buf, "OK");

    // retun;
    mm_kitty(&buf, "\n • ");
    mm_kitty(&buf, RED);
    mm_kitty(&buf, display);
    free(display);

    mm_kitty(&buf, RESET);
    mm_kitty(&buf, CYAN);

    if (k->src_set->count > 1) {
        int istr_size = 16;
        char* istr = malloc(sizeof(char*) * istr_size);
        snprintf(istr, istr_size, " [%d: ->", k->src_set->count);
        mm_kitty(&buf, istr);
        free(istr);

        for (int isrc = 0; isrc < k->src_set->count; ++isrc) {
            char* note_display_buf = malloc(sizeof(char*) * 6);
            char* display2 = mm_midi_to_note_display(k->src_set->keys[isrc]);
            snprintf(note_display_buf, 6, " %s ", display2);
            free(display2);

            mm_kitty(&buf, note_display_buf);
            free(note_display_buf);
        }

        mm_kitty(&buf, " ]->");
    } else {
        mm_kitty(&buf, " ->");
    }

    for (int di = 0; di < k->dst_set->count; ++di) {
        char* display3 = mm_midi_to_note_display(k->dst_set->keys[di]);
        mm_kitty(&buf, " ");
        mm_kitty(&buf, display3);
        mm_kitty(&buf, " ");
        free(display3);
    }

    mm_kitty(&buf, RESET);
}

mm_key_group* mm_get_key_group(mm_mapping* m, int src) { return m->index[src]; }

void mm_mapping_free(mm_mapping* mapping) {
    for (int i = 0; i < mapping->count; ++i) {
        mm_key_group* grp = mapping->mapped[i];

        printf("mapping: freeing group(%d)\n", grp->src);
        for (int i2 = 0; i2 < grp->count; ++i2) {
            mm_key_map* k = grp->maps[i2];

            free(k->dst_set->keys);
            free(k->src_set->keys);

            free(k->dst_set);
            free(k->src_set);
            free(k);
        }

        free(grp->maps);
        free(grp);
    }

    free(mapping->index);
    free(mapping->mapped);
    free(mapping);
}

/**
 * Build and Initialize a Mapping object on the heap.
 */
mm_mapping* mm_build_mapping() {
    mm_mapping* mapping = malloc(sizeof(mm_mapping));

    mapping->count = 0;
    mapping->group_count = 0;
    mapping->index = malloc(sizeof(mm_key_group*) * MAX_MIDI_NOTES);
    mapping->mapped = malloc(sizeof(mm_key_group*) * MAX_MIDI_NOTES);

    // We expect to lookup any midi note at any given time,
    // therefore we must pre-initialise sparse array of pointers for
    // NULL comparison check.
    for (int i = 0; i < MAX_MIDI_NOTES; ++i) {
        mapping->index[i] = NULL;
    }

    return mapping;
}

mm_key_set* mm_mapping_group_single_src_dsts(mm_key_group* grp) {
    mm_key_set* combined_set = malloc(sizeof(mm_key_set));
    // hold up to 128 keys in this set
    combined_set->keys = malloc(sizeof(int*) * 128);
    combined_set->count = 0;

    for (int i = 0; i < grp->count; ++i) {
        mm_key_map* map = grp->maps[i];
        if (map->src_set->count == 1) {
            for (int isrc = 0; isrc < map->dst_set->count; isrc++) {
                combined_set->keys[combined_set->count] =
                    map->dst_set->keys[isrc];
                combined_set->count++;
            }
        }
    }

    return combined_set;
}

int mm_token_count(const char* src, char delim) {
    int count = 0;
    char* cursor = strdup(src);

    while (*cursor) {
        if (*cursor == delim) {
            ++count;
        }

        ++cursor;
    }

    // Actual number of tokens is delim count + 1
    return count + 1;
}

mm_tokens* mm_token_split(const char* src, char delim) {
    int buf_size = strlen(src);
    char** buf = malloc(sizeof(char**) * buf_size);
    int tokens = 0;
    const char* cursor = src;

    // printf("mm_token_split(%s) on %c\n", cursor, delim);

    for (int i = 0; i < buf_size; ++i) {
        int token_tracker = 0;
        char* token = NULL;

        if (!*cursor) {
            break;
        }

        while (*cursor) {
            if (*cursor == ' ') {
                // skip whitespace in the parser
                ++cursor;
            } else if (*cursor == delim) {
                // printf("cursor(%c) == delim(%c)\n", *cursor, delim);
                // printf("token finished: %s\n", token);
                ++cursor;
                break;
            } else {
                char* appendage = malloc(sizeof(char*) * 3);
                if (token != NULL) {
                    sprintf(appendage, "%s%c", token, *cursor);
                    free(token);
                } else {
                    sprintf(appendage, "%c", *cursor);
                }

                token = appendage;
                ++token_tracker;
                ++cursor;
            }
        }

        if (token_tracker > 0) {
            // printf("processed buf[%d] = token: %s\n", tokens, token);
            if (tokens < buf_size) {
                buf[tokens] = token;
                ++tokens;
            }
        }
    }

    // printf("token_split done, found(%d) tokens\n\n", tokens);
    // return buf;
    mm_tokens* result = malloc(sizeof(mm_tokens));

    char** mbuf = malloc(sizeof(char**) * tokens);
    for (int b = 0; b < tokens; ++b) {
        mbuf[b] = buf[b];
    }

    free(buf);

    result->count = tokens;
    result->tokens = mbuf;
    // free(buf);

    return result;
}

char* mm_tokens_dump(mm_tokens* tokens) {
    char* buf = NULL;
    int last_size = 1;

    for (int i = 0; i < tokens->count; ++i) {
        char* token = tokens->tokens[i];
        int size = strlen(token) + 32;

        if (buf == NULL) {
            char* new = malloc(sizeof(char*) * size);
            snprintf(new, size, "token[%d]: %s\n", i, token);
            last_size = size;
            buf = new;
        } else {
            int new_size = size + last_size;
            last_size = new_size;

            char* new = malloc(sizeof(char*) * new_size);
            snprintf(new, new_size, "%stoken[%d]: %s\n", buf, i, token);
            free(buf);
            buf = new;
        }
    }

    return buf;
}

/**
 * Builds a complete mapping from a string of characters.
 * Char* list is delimited to form a list of key mappings for parsing.
 */
mm_mapping* mm_mapping_from_list(char* list) {
    mm_debug("mapping: mm_mapping_from_list(%s)\n", list);
    mm_mapping* mapping = mm_build_mapping();

    if (list == NULL) {
        return mapping;
    }

    if (strstr(list, ":") == NULL) {
        error("Invalid mapping: %s\n", list);
        error("Need to specify at least a src & dest key\n");
        error("\nFor example: 12:36\n\n");
        return NULL;
    }

    mm_tokens* mappings = mm_token_split(list, ',');
    // printf("token split complete!\n");

    int src_used[128] = {0};

    for (int i = 0; i < mappings->count; ++i) {
        // process individual mappings from the list.
        // break into src and dst groups.
        char* input = mappings->tokens[i];
        // printf("\ninput = %s\n", input);

        mm_tokens* tokens = mm_token_split(input, ':');
        // printf("tokens->count(%d)\n", tokens->count);
        assert(tokens->count == 2 && tokens->count > 0);

        mm_tokens* src_tokens = mm_token_split(tokens->tokens[0], '|');
        mm_tokens* dst_tokens = mm_token_split(tokens->tokens[1], '|');

        for (int isrc = 0; isrc < src_tokens->count; ++isrc) {
            int src = mm_parse_to_midi(src_tokens->tokens[isrc]);
            assert(src_used[src] == 0);
            src_used[src] = 1;

            if (mapping->index[src] == NULL) {
                // create a new mapping for this midi key
                mapping->index[src] =
                    create_key_group(mapping, src, src_tokens, dst_tokens);
                mapping->group_count++;

            } else {
                // update existing mapping group for this midi key.
                // This means inserting a new key_map into an existing group.
                update_key_group(mapping->index[src], src, src_tokens,
                                 dst_tokens);
            }
        }

        mm_tokens_free(src_tokens);
        mm_tokens_free(dst_tokens);
        mm_tokens_free(tokens);
    }

    mm_tokens_free(mappings);
    mm_debug("mapping: mm_mapping_from_list successful");
    return mapping;
}

void update_key_group(mm_key_group* group, int src, mm_tokens* src_tokens,
                      mm_tokens* dst_tokens) {
    group->maps[group->count] = create_key_map(src, src_tokens, dst_tokens);
    group->parent->count++;
    group->count++;
}

mm_key_group* create_key_group(mm_mapping* m, int src, mm_tokens* src_tokens,
                               mm_tokens* dst_tokens) {

    mm_key_group* group = malloc(sizeof(mm_key_group));
    group->count = 1;
    group->src = src;

    group->parent = m;
    m->mapped[group->parent->count] = group;
    m->count++;

    // Allocate enough space for all future key_maps
    // in this group (expected to be a relatively small need)
    group->maps = malloc(MAX_GROUPED_KEY_MAPS * sizeof(mm_key_map*));
    for (int i = 0; i < MAX_GROUPED_KEY_MAPS; ++i) {
        group->maps[i] = NULL;
    }

    // Allocate the first key_map
    group->maps[0] = create_key_map(src, src_tokens, dst_tokens);
    return group;
}

mm_key_map* create_key_map(int src, mm_tokens* src_tokens,
                           mm_tokens* dst_tokens) {

    // printf("create_key_map(key %d -> %d, %d)\n", src, src_tokens->count,
    //        dst_tokens->count);
    mm_key_map* km = malloc(sizeof(mm_key_map));
    km->key = src;

    km->src_set = mm_create_key_set(src_tokens->count);
    km->dst_set = mm_create_key_set(dst_tokens->count);

    for (int idst = 0; idst < dst_tokens->count; ++idst) {
        km->dst_set->keys[idst] = mm_parse_to_midi(dst_tokens->tokens[idst]);
    }

    for (int isrc = 0; isrc < src_tokens->count; ++isrc) {
        // printf("isrc: start(%d)\n", isrc);
        char* s = src_tokens->tokens[isrc];
        // printf("isrc: %s\n", s);
        km->src_set->keys[isrc] = mm_parse_to_midi(s);
    }

    for (int i = 0; i < dst_tokens->count; ++i) {
        char* t = dst_tokens->tokens[i];
        if (t != NULL) {
            // free(t);
        }
    }

    return km;
}

mm_key_set* mm_create_key_set(int count) {
    mm_key_set* set = malloc(sizeof(mm_key_set));
    set->keys = malloc(sizeof(int) * count);
    set->count = count;
    return set;
}

char* mm_key_set_dump(mm_key_set* set) {
    char* buf = malloc(sizeof(char*) * set->count * 10);
    buf[0] = '\0';
    for (int i = 0; i < set->count; ++i) {
        int size = sizeof(char*) * 12;
        char* append = malloc(size);
        snprintf(append, size, " %d = %d\n", i, set->keys[i]);
        // sns sprintf(buf, "%s %d = %d\n", buf, i, set->keys[i]);
        strcat(buf, append);
        free(append);
    }

    return buf;
}

void mm_combine_key_set(mm_key_set* set, mm_key_set* addition) {
    /* mm_debug("Attempting to combine key_set(%d) with addition(%d)\n", */
    /*          set->count, addition->count); */

    int* new_keys = malloc(sizeof(int) * (set->count + addition->count));
    int lookup[128] = {0};

    for (int i = 0; i < set->count; ++i) {
        // mark original key existence
        lookup[set->keys[i]] = 1;

        new_keys[i] = set->keys[i];
    }

    int added = 0;
    for (int i2 = 0; i2 < addition->count; ++i2) {
        int new_additional_key = addition->keys[i2];

        if (!lookup[new_additional_key]) {
            lookup[new_additional_key] = 1;
            new_keys[added + set->count] = new_additional_key;
            ++added;
        }
    }
    // assign new keys over previous keys
    set->count = set->count + added;

    // Free the original keys before reassignment
    free(set->keys);
    set->keys = new_keys;

    /* mm_debug("updated st: \n%s\n", mm_key_set_dump(set)); */

    // cleanup old keys and entire new addition
    free(addition->keys);
    free(addition);
}

void mm_remove_key_set(mm_key_set* set, mm_key_set* substract) {
    // mm_debug("mapping: mm_remove_key_set()");
    // if (substract->count <= 0) {
    //     mm_debug("mapping: bailing from mm_remove_set!\n");
    //     return;
    // }

    int sub_lookup[128] = {0};
    int substractable = 0;

    for (int i = 0; i < substract->count; ++i) {
        sub_lookup[substract->keys[i]] = 1;
    }

    for (int i1 = 0; i1 < set->count; ++i1) {
        int key = set->keys[i1];
        if (sub_lookup[key]) {
            ++substractable;
        }
    }

    int new_size = set->count - substractable;
    int* new_keys = malloc(sizeof(int) * new_size);

    int index = 0;
    for (int i2 = 0; i2 < set->count; ++i2) {
        int key = set->keys[i2];
        assert(key >= 0 && key <= 128);

        if (key >= 0 && !sub_lookup[key]) {
            mm_debug("mm_remove_key_set: assigning %d = (%d) from set(%d) "
                     "sub(%d)\n",
                     key, index, set->count, substract->count);
            new_keys[index] = key;
            ++index;
        }
    }

    free(set->keys);

    set->count = index;
    set->keys = new_keys;

    assert(set->count == new_size);
}

mm_key_set* mm_key_set_copy(mm_key_set* set) {
    mm_key_set* new_set = malloc(sizeof(mm_key_set));
    new_set->count = set->count;
    new_set->keys = malloc(sizeof(int) * set->count);

    for (int i = 0; i < set->count; ++i) {
        new_set->keys[i] = set->keys[i];
    }

    return new_set;
}

void mm_key_set_remove_single_key(mm_key_set* set, int key) {
    int size = set->count - 1;
    int* keys = malloc(sizeof(int) * size);

    int index = 0;
    for (int i = 0; i < set->count; ++i) {
        int k = set->keys[i];
        if (k != key) {
            keys[index] = k;
            ++index;
        }
    }

    assert(size == index);

    free(set->keys);
    set->keys = keys;
    set->count = index;
}

void mm_tokens_free(mm_tokens* tokens) {
    for (int i = 0; i < tokens->count; ++i) {
        free(tokens->tokens[i]);
        tokens->tokens[i] = NULL;
    }

    free(tokens->tokens);
    free(tokens);
    tokens = NULL;
}
