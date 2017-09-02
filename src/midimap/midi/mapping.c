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

    int* id_index = calloc(sizeof(int) * KEY_MAP_ID_SIZE, sizeof(int));

    if (mapping->group_count) {
        sprintf(buf, "%sUser Mapping:%s", BLUE, RESET);
        for (int i = 0; i < mapping->group_count; ++i) {
            mm_key_group_dump(mapping->mapped[i], buf, id_index);
        }
    }
}

void mm_key_group_dump(mm_key_group* g, char* buf, int* id_index) {
    for (int i = 0; i < g->count; ++i) {
        mm_key_map* m = g->maps[i];
        if (!id_index[m->id]) {
            id_index[m->id] = 1;
            mm_key_map_dump(g->maps[i], buf);
        }
    }
}

void mm_key_map_dump(mm_key_map* k, char* buf) {
    char* id = malloc(sizeof(char*) * 32);
    char* ptr = buf;
    sprintf(id, "%05d", k->id);
    // retun;
    mm_cat(&ptr, "\n • ");
    mm_cat(&ptr, id);
    mm_cat(&ptr, RED);

    if (k->src_set->count > 1) {
        mm_cat(&ptr, " ");

        for (int isrc = 0; isrc < k->src_set->count; ++isrc) {
            char* note_display_buf = malloc(sizeof(char*) * 6);
            char* display2 = mm_midi_to_note_display(k->src_set->keys[isrc]);
            snprintf(note_display_buf, 6, "%s", display2);

            mm_cat(&ptr, note_display_buf);
            if (isrc < k->src_set->count - 1) {
                mm_cat(&ptr, ", ");
            }

            free(display2);
            free(note_display_buf);
        }

        mm_cat(&buf, RESET);
        mm_cat(&buf, "->");

    } else {
        char* widthbuf = calloc(sizeof(char) * 4, sizeof(char));
        char* display = mm_midi_to_note_display(k->key);
        snprintf(widthbuf, 4, "%3s", display);
        mm_cat(&ptr, widthbuf);
        free(display);
        free(widthbuf);

        mm_cat(&ptr, RESET);
        mm_cat(&ptr, " ->");
    }

    mm_cat(&ptr, CYAN);
    for (int di = 0; di < k->dst_set->count; ++di) {
        char* display3 = mm_midi_to_note_display(k->dst_set->keys[di]);
        mm_cat(&ptr, " ");
        mm_cat(&ptr, display3);
        mm_cat(&ptr, " ");
        free(display3);
    }

    mm_cat(&ptr, RESET);
}

mm_key_group* mm_get_key_group(mm_mapping* m, int src) { return m->index[src]; }

void mm_mapping_free(mm_mapping* mapping) {
    for (int i = 0; i < mapping->group_count; ++i) {
        mm_key_group* grp = mapping->mapped[i];

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

    mapping->group_count = 0;

    mapping->index =
        calloc(sizeof(mm_key_group*) * MAX_MIDI_NOTES, sizeof(mm_key_group*));
    mapping->mapped =
        calloc(sizeof(mm_key_group*) * MAX_MIDI_NOTES, sizeof(mm_key_group*));

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

mm_key_set* mm_mapping_group_all_dsts(mm_key_group* grp, mm_key_node* tail,
                                      int note_on, int chan) {
    mm_key_set* combined_set = malloc(sizeof(mm_key_set));
    // hold up to 128 keys in this set
    combined_set->keys = malloc(sizeof(int*) * 128);
    combined_set->count = 0;

    for (int i = 0; i < grp->count; ++i) {
        mm_key_map* map = grp->maps[i];
        if (chan != map->channel_in) {
            continue;
        }

        if (map->src_set->count == 1) {
            for (int isrc = 0; isrc < map->dst_set->count; isrc++) {
                combined_set->keys[combined_set->count] =
                    map->dst_set->keys[isrc];
                combined_set->count++;
            }
        } else {
            int srcs_checked = 0;
            for (int isrc = 0; isrc < map->src_set->count; isrc++) {
                int src = map->src_set->keys[isrc];
                if (note_on && !mm_key_node_contains(tail, src)) {
                    break;
                }

                ++srcs_checked;
                if (srcs_checked == map->src_set->count) {
                    for (int idst = 0; idst < map->dst_set->count; ++idst) {
                        combined_set->keys[combined_set->count] =
                            map->dst_set->keys[idst];
                        combined_set->count++;
                    }
                }
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
    char** buf = malloc(sizeof(char**) * buf_size + 10);
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
                int ap_size = sizeof(char*) * 4;
                char* appendage = malloc(ap_size);
                if (token != NULL) {
                    snprintf(appendage, ap_size, "%s%c", token, *cursor);
                    free(token);
                } else {
                    snprintf(appendage, ap_size, "%c", *cursor);
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

    char** mbuf = malloc(sizeof(char**) * tokens + 1);
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

char* mm_mapping_extract_channel(const char* src, int* ch) {
    // strlen - 2 for the `()` parens - still not accounting for inner digits.
    int src_size = sizeof(char) * (strlen(src) - 2);
    int csize = sizeof(char) * 4;

    char* collector = calloc(csize, sizeof(char));
    char* prestine = calloc(src_size, sizeof(char));

    char* cptr = collector;
    char* pptr = prestine;

    int open = 0;
    // printd("scanning src: %s\n", src);
    fflush(stdout);

    while (*src) {
        if (*src == '(') {
            open = 1;
        } else if (*src == ')') {
            open = 0;
        } else {
            if (open) {
                *cptr = *src;
                ++cptr;
            } else {
                *pptr = *src;
                ++pptr;
            }
        }

        ++src;
    }

    *ch = atoi(collector);
    free(collector);

    return prestine;
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

    for (int i = 0; i < mappings->count; ++i) {
        // process individual mappings from the list.
        // break into src and dst groups.
        char* input = mappings->tokens[i];

        mm_tokens* tokens = mm_token_split(input, ':');
        assert(tokens->count == 2 && tokens->count > 0);

        char* src_input = tokens->tokens[0];
        char* dst_input = tokens->tokens[1];
        int ich = 0;
        int och = 0;

        if (strstr(src_input, "(") != NULL) {
            int* pich = calloc(sizeof(int), sizeof(int));
            src_input = mm_mapping_extract_channel(src_input, pich);
            ich = *pich;
            free(pich);
        }

        if (strstr(dst_input, "(") != NULL) {
            int* poch = calloc(sizeof(int), sizeof(int));
            dst_input = mm_mapping_extract_channel(dst_input, poch);
            och = *poch;
            free(poch);
        }

        mm_tokens* src_tokens = mm_token_split(src_input, '|');
        mm_tokens* dst_tokens = mm_token_split(dst_input, '|');

        for (int isrc = 0; isrc < src_tokens->count; ++isrc) {
            int src = mm_parse_to_midi(src_tokens->tokens[isrc]);

            if (mapping->index[src] == NULL) {
                // printd("[src:%d] creating key_group\n", src);
                // create a new mapping for this midi key
                mapping->index[src] = create_key_group(mapping, src, ich, och,
                                                       src_tokens, dst_tokens);

                // printd("[src:%d] mapping->group_count++ -> %d\n", src,
                // mapping->group_count);

            } else {
                // printd("[src:%d] updating key_group\n", src);
                // update existing mapping group for this midi key.
                // This means inserting a new key_map into an existing
                // group.
                update_key_group(mapping->index[src], src, ich, och, src_tokens,
                                 dst_tokens);
            }
        }

        if (src_input != tokens->tokens[0]) {
            free(src_input);
        }

        if (dst_input != tokens->tokens[1]) {
            free(dst_input);
        }

        mm_tokens_free(src_tokens);
        mm_tokens_free(dst_tokens);
        mm_tokens_free(tokens);
    }

    mm_tokens_free(mappings);
    mm_debug("mapping: mm_mapping_from_list successful");
    return mapping;
}

void update_key_group(mm_key_group* group, int src, int ich, int och,
                      mm_tokens* src_tokens, mm_tokens* dst_tokens) {
    // printd("inserting key_map into: %d\n", group->count);
    group->maps[group->count] =
        create_key_map(src, ich, och, src_tokens, dst_tokens);
    group->count++;
}

mm_key_group* create_key_group(mm_mapping* m, int src, int ich, int och,
                               mm_tokens* src_tokens, mm_tokens* dst_tokens) {

    mm_key_group* group = malloc(sizeof(mm_key_group));
    group->count = 1;
    group->src = src;

    group->parent = m;
    m->mapped[m->group_count] = group;
    m->group_count++;

    // Allocate enough space for all future
    // key_maps
    // in this group (expected to be a relatively
    // small need)
    group->maps = malloc(MAX_GROUPED_KEY_MAPS * sizeof(mm_key_map*));
    for (int i = 0; i < MAX_GROUPED_KEY_MAPS; ++i) {
        group->maps[i] = NULL;
    }

    // Allocate the first key_map
    group->maps[0] = create_key_map(src, ich, och, src_tokens, dst_tokens);
    return group;
}

mm_key_map* create_key_map(int src, int ich, int och, mm_tokens* src_tokens,
                           mm_tokens* dst_tokens) {

    // printf("create_key_map(key %d -> %d,
    // %d)\n", src, src_tokens->count,
    //        dst_tokens->count);
    mm_key_map* km = malloc(sizeof(mm_key_map));
    km->key = src;
    km->channel_in = ich;
    km->channel_out = och;
    km->id = 0;

    km->src_set = mm_create_key_set(src_tokens->count);
    km->dst_set = mm_create_key_set(dst_tokens->count);

    for (int idst = 0; idst < dst_tokens->count; ++idst) {
        km->dst_set->keys[idst] = mm_parse_to_midi(dst_tokens->tokens[idst]);
    }

    char* id = calloc(sizeof(char) * 32, sizeof(char));
    char* p = id;
    // mm_cat(&id, "123");
    sprintf(id, "01");

    for (int isrc = 0; isrc < src_tokens->count; ++isrc) {
        char* s = src_tokens->tokens[isrc];
        // mm_cat(&id, s);
        int midi = mm_parse_to_midi(s);
        km->src_set->keys[isrc] = midi;
        char* m = malloc(sizeof(char*) * 6);
        sprintf(m, "%d%d%d", ich, och, midi);
        mm_cat(&p, m);
    }

    km->id = atoi(id) % KEY_MAP_ID_SIZE;
    free(id);

    return km;
}

mm_key_set* mm_create_key_set(int count) {
    mm_key_set* set = malloc(sizeof(mm_key_set));
    set->keys = calloc(sizeof(int) * count, sizeof(int));
    set->count = count;
    return set;
}

char* mm_key_set_dump(mm_key_set* set) {
    int size = set->count * 10;
    if (size < 10) {
        size = 10;
    }

    char* buf = calloc(sizeof(char*) * size, sizeof(char));
    char* ptr = buf;

    for (int i = 0; i < set->count; ++i) {
        int size = sizeof(char*) * 12;
        char* append = calloc(size, sizeof(char));
        snprintf(append, size, " %d = %d\n", i, set->keys[i]);
        mm_cat(&ptr, append);
        free(append);
    }

    return buf;
}

void mm_combine_key_set(mm_key_set* set, mm_key_set* addition) {
    /* mm_debug("Attempting to combine
     * key_set(%d) with addition(%d)\n", */
    /*          set->count, addition->count); */

    int* new_keys =
        calloc(sizeof(int) * (set->count + addition->count), sizeof(int));
    int lookup[256] = {0};

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

    /* mm_debug("updated st: \n%s\n",
     * mm_key_set_dump(set)); */

    // cleanup old keys and entire new addition
    free(addition->keys);
    free(addition);
}

void mm_remove_key_set(mm_key_set* set, mm_key_set* substract) {
    // mm_debug("mapping: mm_remove_key_set()");
    // if (substract->count <= 0) {
    //     mm_debug("mapping: bailing from
    //     mm_remove_set!\n");
    //     return;
    // }

    int sub_lookup[256] = {0};
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
            mm_debug("mm_remove_key_set: "
                     "assigning %d = (%d) from "
                     "set(%d) "
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
    int size = set->count;
    int* keys = malloc(sizeof(int) * size);

    int index = 0;
    for (int i = 0; i < set->count; ++i) {
        int k = set->keys[i];
        if (k != key) {
            keys[index] = k;
            ++index;
        }
    }

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
