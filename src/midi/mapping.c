#include "midi/mapping.h"

void mm_mapping_dump(mm_mapping* mapping, char* buf) {
    if (mapping == NULL) {
        error("Mappings Null");
        return;
    }

    sprintf(buf, "Mappings: {}\n");
}

mm_mapping* mm_build_mapping() {
    mm_mapping* mapping = malloc(sizeof(mm_mapping));

    mapping->mapped = NULL;
    mapping->mcount = 0;
    mapping->group_count = 0;
    mapping->index = malloc(sizeof(mm_key_group*) * MAX_MIDI_NOTES);

    // Initialise sparse array of pointers to all NULL
    for (int i = 0; i < MAX_MIDI_NOTES; ++i) {
        mapping->index[i] = NULL;
    }

    return mapping;
}

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

    char** key_tokens = malloc(sizeof(char*));
    char delim[2] = ",";

    int kt_count = mm_tokenize(list, delim, key_tokens);
    char *key, *res;
    int src, dst;

    for (int i = 0; i < kt_count; ++i) {
        key = key_tokens[i];
        res = NULL;

        // TODO: [grouped] parsing for multiple party members
        src = atoi(strtok_r(key, ":", &res));

        // TODO: [grouped] parsing for multiple dsts
        dst = atoi(strtok_r(NULL, ":", &res));

        if (mapping->index[src] == NULL) {
            pdebug("creating new key_group_map");

            // Finally, stash our new group into the index
            mapping->index[src] = create_key_group(src, dst);
            mapping->group_count++;

        } else {
            pdebug("updating existing key_group_map");
            update_key_group(mapping->index[src], src, dst);
        }

        pdebug("processed key_map: [%d:%d]\n", src, dst);
    }

    char* buf = malloc(sizeof(char) * 128 * kt_count);
    mm_mapping_dump(mapping, buf);

    pdebug("%s", buf);

    exit(EXIT_FAILURE);
    return mapping;
}

static void update_key_group(mm_key_group* group, int src, int dst) {
    group->maps[group->count] = create_key_map(src, dst);
    group->count++;
}

static mm_key_group* create_key_group(int src, int dst) {
    mm_key_group* group = NULL;
    group = malloc(sizeof(mm_key_group));
    group->count = 1;

    // Allocate enough space for all future key_maps
    // in this group (expected to be a relatively small need)
    group->maps = malloc(MAX_GROUPED_KEY_MAPS * sizeof(mm_key_map*));

    // Allocate the first key_map
    group->maps[0] = create_key_map(src, dst);
    return group;
}

static mm_key_map* create_key_map(int src, int dst) {
    mm_key_map* km = malloc(sizeof(mm_key_map));
    km->key = src;
    km->dst_group = malloc(sizeof(int*) * MAX_KEY_MAP_DSTS);
    km->dst_group[0] = dst;
    km->dst_count = 1;
    return km;
}
