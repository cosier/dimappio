
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "helpers/helpers.h"
#include <midimap/core.h>

void assert_mapping_list_stage_one(char* list) {
    mm_mapping* mapping = NULL;
    mapping = mm_mapping_from_list(list);

    printf("mapping groups: %d\n", mapping->group_count);
    assert(mapping->group_count == 5);
    // assert(mapping->group_count == 2);
}

void map_building_from_list() {
    char* list_1 = "g3:G2|F3|A3|Bb3|D4,"
                   "F3:F2|G#3|C4|D#4|G4,"
                   "C3:D#2|F3|G3|G4,"
                   "C4|C#4:C4|C#4|g4|g#4";
    // assert(1 == 2);
    assert_mapping_list_stage_one(list_1);
}

int main() {
    // assert(1 == 3);
    test_run("map_building_from_list", &map_building_from_list);
    return 0;
}
