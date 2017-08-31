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
    strcat(buf, "\n • ");
    strcat(buf, RED);
    strcat(buf, display);
    strcat(buf, RESET);
    strcat(buf, CYAN);
    free(display);

    if (k->src_set->count > 1) {
        sprintf(buf, "%s [%d: -> ", buf, k->src_set->count);
        for (int isrc = 0; isrc < k->src_set->count; ++isrc) {
            sprintf(buf, "%s %s ", buf,
                    mm_midi_to_note_display(k->src_set->keys[isrc]));
        }
        strcat(buf, " ]->");
    } else {
        strcat(buf, " ->");
    }
    for (int di = 0; di < k->dst_set->count; ++di) {
        char* display = mm_midi_to_note_display(k->dst_set->keys[di]);
        strcat(buf, " ");
        strcat(buf, display);
        strcat(buf, " ");
        free(display);
    }

    strcat(buf, RESET);
}

mm_key_group* mm_get_key_group(mm_mapping* m, int src) { return m->index[src]; }

void mm_mapping_free(mm_mapping* mapping) {
    if (mapping->dst_tokens != NULL) {
        free(mapping->dst_tokens);
    }

    if (mapping->src_tokens != NULL) {
        free(mapping->src_tokens);
    }

    if (mapping->key_tokens != NULL) {
        free(mapping->key_tokens);
    }

    for (int i = 0; i < mapping->count; ++i) {
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

    char **key_tokens = malloc(sizeof(char*) * 256),
         **dst_tokens = malloc(sizeof(char*) * 256),
         **src_tokens = malloc(sizeof(char*) * 256);

    int kt_count = mm_tokenize(list, ",", key_tokens);
    char *key, *res, *src_grp, *dst_grp;
    int src, src_count, dst_count;

    for (int i = 0; i < kt_count; ++i) {
        key = key_tokens[i];
        res = NULL;

        if (!strstr(key, ":")) {
            error("Could not parse mapping: %s", key);
            return NULL;
        }

        src_grp = strtok_r(key, ":", &res);
        dst_grp = strtok_r(NULL, ":", &res);

        if (strstr(src_grp, "|")) {
            src_count = mm_tokenize(src_grp, "|", src_tokens);
        } else {
            src_count = 1;
            src_tokens[0] = src_grp;
        }

        if (strstr(dst_grp, "|")) {
            dst_count = mm_tokenize(dst_grp, "|", dst_tokens);
        } else {
            dst_count = 1;
            dst_tokens[0] = dst_grp;
        }

        for (int isrc = 0; isrc < src_count; ++isrc) {
            src = mm_parse_to_midi(src_tokens[isrc]);
            if (mapping->index[src] == NULL) {
                // Stash our new group into the index
                mapping->index[src] = create_key_group(
                    mapping, src, src_tokens, dst_tokens, src_count, dst_count);
                mapping->group_count++;

            } else {
                update_key_group(mapping->index[src], src, src_tokens,
                                 dst_tokens, src_count, dst_count);
            }

            free(src_tokens[isrc]);
        }

        // free(key_tokens[kt_count]);
    }

    // Store for later cleanup
    mapping->key_tokens = key_tokens;
    mapping->dst_tokens = dst_tokens;
    mapping->src_tokens = src_tokens;

    mm_debug("mapping: mm_mapping_from_list successful");
    return mapping;
}

void update_key_group(mm_key_group* group, int src, char** src_tokens,
                      char** dst_tokens, int src_count, int dst_count) {
    group->maps[group->count] =
        create_key_map(src, src_tokens, dst_tokens, src_count, dst_count);

    group->parent->count++;
    group->count++;
}

mm_key_group* create_key_group(mm_mapping* m, int src, char** src_tokens,
                               char** dst_tokens, int src_count,
                               int dst_count) {

    mm_key_group* group = NULL;
    group = malloc(sizeof(mm_key_group));
    group->count = 1;
    group->src = src;

    group->parent = m;
    group->parent->mapped[group->parent->count] = group;
    group->parent->count++;

    // Allocate enough space for all future key_maps
    // in this group (expected to be a relatively small need)
    group->maps = malloc(MAX_GROUPED_KEY_MAPS * sizeof(mm_key_map*));

    // Allocate the first key_map
    group->maps[0] =
        create_key_map(src, src_tokens, dst_tokens, src_count, dst_count);
    return group;
}

mm_key_map* create_key_map(int src, char** src_tokens, char** dst_tokens,
                           int src_count, int dst_count) {

    mm_key_map* km = malloc(sizeof(mm_key_map));
    km->key = src;

    km->src_set = mm_create_key_set(src_count);
    km->dst_set = mm_create_key_set(dst_count);

    for (int idst = 0; idst < dst_count; ++idst) {
        km->dst_set->keys[idst] = mm_parse_to_midi(dst_tokens[idst]);
    }

    for (int isrc = 0; isrc < src_count; ++isrc) {
        km->src_set->keys[isrc] = mm_parse_to_midi(src_tokens[isrc]);
    }

    for (int i = 0; i < dst_count; ++i) {
        char* t = dst_tokens[i];
        if (t != NULL) {
            free(t);
        }
    }

    return km;
}

mm_key_set* mm_create_key_set(int count) {
    mm_key_set* set = malloc(sizeof(mm_key_set));
    set->keys = malloc(sizeof(int*) * count);
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
    if (substract->count <= 0) {
        mm_debug("mapping: bailing from mm_remove_set!\n");
        return;
    }

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

    set->count = index;
    assert(set->count == new_size);

    free(set->keys);
    free(substract->keys);
    free(substract);
    set->keys = new_keys;
}
