#include "midi/mapping.h"

/**
 * Dumps a description of a mapping to the provided char buffer
 */
void mm_mapping_dump(mm_mapping* mapping, char* buf) {
    if (mapping == NULL) {
        error("Mappings Null");
        return;
    }

    sprintf(buf, "Key Mapping: ");
    for (int i = 0; i < mapping->group_count; ++i) {
        mm_key_group_dump(mapping->mapped[i], buf);
    }
}

void mm_key_group_dump(mm_key_group* g, char* buf) {
    /* sprintf(buf, "%s\n  %d. group(%d)", buf, i, g->src); */
    for (int i = 0; i < g->count; ++i) {
        mm_key_map_dump(g->maps[i], buf);
    }
}

void mm_key_map_dump(mm_key_map* k, char* buf) {
    sprintf(buf, "%s\n - %s:%d ", buf, mm_midi_to_note_display(k->key), k->key);

    if (k->src_set->count > 1) {
        sprintf(buf, "%s [%d: -> ", buf, k->src_set->count);
        for (int isrc = 0; isrc < k->src_set->count; ++isrc) {
            sprintf(buf, "%s %d ", buf, k->src_set->keys[isrc]);
        }
        sprintf(buf, "%s]->", buf);
    } else {
        sprintf(buf, "%s ->", buf);
    }
    for (int di = 0; di < k->dst_set->count; ++di) {
        sprintf(buf, "%s %s:%d ", buf,
                mm_midi_to_note_display(k->dst_set->keys[di]),
                k->dst_set->keys[di]);
    }
}

mm_key_group* mm_get_key_group(mm_mapping* m, int src) { return m->index[src]; }

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
    mm_key_set* combined_set = malloc(sizeof(mm_key_set*));
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

    char **key_tokens = malloc(sizeof(char*)),
         **dst_tokens = malloc(sizeof(char*)),
         **src_tokens = malloc(sizeof(char*));

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
        }
    }

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

    km->src_set = create_key_set(src_count);
    km->dst_set = create_key_set(dst_count);

    for (int idst = 0; idst < dst_count; ++idst) {
        km->dst_set->keys[idst] = mm_parse_to_midi(dst_tokens[idst]);
    }

    for (int isrc = 0; isrc < src_count; ++isrc) {
        km->src_set->keys[isrc] = mm_parse_to_midi(src_tokens[isrc]);
    }

    return km;
}

mm_key_set* create_key_set(int count) {
    mm_key_set* set = malloc(sizeof(mm_key_set));
    set->keys = malloc(sizeof(int*) * count);
    set->count = count;
    return set;
}
