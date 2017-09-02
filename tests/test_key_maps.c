
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "helpers/helpers.h"
#include <midimap/core.h>

void assert_mapping_list(char* list, int ich, int och, int groups,
                         int dst_count, int min_src_count, int max_src_count) {
    printf("Asserting mapping:\nlist: %s\n"
           "channel: %d<>%d, groups: %d, dst_count:%d, src_count:%d/%d\n\n",
           list, ich, och, groups, dst_count, min_src_count, max_src_count);
    fflush(stdout);

    mm_mapping* mapping = NULL;
    mapping = mm_mapping_from_list(list);

    assert(mapping->group_count == groups);

    mm_key_map* index[KEY_MAP_ID_SIZE] = {NULL};

    printf("creating index(%d) with %lu bytes per element\n", KEY_MAP_ID_SIZE,
           sizeof(mm_key_map*));

    for (int i = 0; i < mapping->group_count; ++i) {
        mm_key_group* grp = mapping->mapped[i];

        if (grp == NULL) {
            error("mapping->group_count: %d/%d", i, mapping->group_count);
        }

        assert(grp != NULL);
        assert(grp->count = 1);

        for (int k = 0; k < grp->count; ++k) {
            mm_key_map* km = grp->maps[k];

            if (index[km->id]) {
                mm_key_map* other = index[km->id];
                char* buf = calloc(sizeof(char) * 512, sizeof(char));
                mm_key_map_dump(km, buf);
                mm_key_map_dump(other, buf);

                printf("detected collision with: %s\n", buf);
                free(buf);

                assert(other->src_set->count == km->src_set->count);
                assert(other->dst_set->count == km->dst_set->count);

                // deep check of src_sets against km and other
                for (int isrc = 0; isrc < other->src_set->count; ++isrc) {
                    assert(other->src_set->keys[isrc] ==
                           km->src_set->keys[isrc]);
                }

                // deep check of dst_sets against km and other
                for (int idst = 0; idst < other->dst_set->count; ++idst) {
                    assert(other->dst_set->keys[idst] ==
                           km->dst_set->keys[idst]);
                }

                assert(other->key != km->key);

            } else {
                index[km->id] = km;
            }

            assert(km->channel_in == ich);
            assert(km->channel_out == och);

            // char* kmbuf = calloc(sizeof(char*), sizeof(char*));
            // mm_key_map_dump(km, kmbuf);
            // printd("key_map_dump: \n%s", kmbuf);
            // printd("km->dst_set->count(%d)\n", km->dst_set->count);

            assert(km->dst_set->count == dst_count);
            assert(km->src_set->count <= max_src_count);
            assert(km->src_set->count >= min_src_count);
        }
    }

    printf("\n\n");
}

void map_building_from_list() {
    char* list_1 = "G3:G2|F3|A3|Bb3|D4,"
                   "F3:F2|G#3|C4|D#4|G4,"
                   "C3:D#2|F3|G3|G4|G#4,"
                   "C4|C#4:C4|C#4|G4|G#4|Eb4";

    // ich(0), och(0), groups(5), dst_count(5), min_src(1), max_src(2)
    assert_mapping_list(list_1, 0, 0, 5, 5, 1, 2);

    char* list_2 = "G3|G4:G2|F3|A3|Bb3|D4,"
                   "F3|F2:F2|G#3|C4|D#4|G4,"
                   "C3|C4:D#2|F3|G3|G4|G#4,"
                   "C4|C#4:C4|C#4|G4|G#4|Eb4";

    // ich(0), och(0), groups(7), dst_count(5), min_src(2), max_src(2)
    assert_mapping_list(list_2, 0, 0, 7, 5, 2, 2);

    char* list_3 = "(1)G3|G4:G2|F3|A3|Bb3|D4,"
                   "(1)F3|F2:F2|G#3|C4|D#4|G4,"
                   "(1)C3|C4:D#2|F3|G3|G4|G#4,"
                   "(1)C4|C#4:C4|C#4|G4|G#4|Eb4";

    // ich(1), och(0), groups(7), dst_count(5), min_src(2), max_src(2)
    assert_mapping_list(list_3, 1, 0, 7, 5, 2, 2);

    char* list_4 = "(3)G3|G4:(2)G2|F3|A3|Bb3|D4,"
                   "(3)F3|F2:F2|G#3|C4|D#4|G4(2),"
                   "(3)C3|C4:D#2|F3|G3|G4|G#4(2),"
                   "(3)C4|C#4:C4|C#4|G4|G#4|Eb4(2)";

    // ich(3), och(2), groups(7), dst_count(5), min_src(2), max_src(2)
    assert_mapping_list(list_4, 3, 2, 7, 5, 2, 2);
}

void assert_whitespaced_list(char* list, int group_count, int isrc, int idst) {
    mm_mapping* mapping = mm_mapping_from_list(list);
    assert(mapping->group_count == group_count);
    for (int i = 0; i < mapping->group_count; ++i) {
        mm_key_group* grp = mapping->mapped[i];
        for (int k = 0; k < grp->count; ++k) {
            mm_key_map* km = grp->maps[k];
            assert(km->src_set->count == isrc);
            assert(km->dst_set->count == idst);
        }
    }
}

void list_handle_whitespace() {
    char* list_1 = "(3) G3|G4 : (2) G2|F3|A3 |Bb3|D4 ";
    assert_whitespaced_list(list_1, 2, 2, 5);

    char* list_2 = "(3) G3|G4 : (2) G2|F3|A3 |Bb3|D4 ,"
                   "(3) G3 | G4 : (2) G2 | F3|   A3 |Bb3|D4 ";
    assert_whitespaced_list(list_2, 2, 2, 5);
}

int main() {
    // assert(1 == 3);
    test_run("list_handle_whitespace", &list_handle_whitespace);
    test_run("map_building_from_list", &map_building_from_list);
    return 0;
}
