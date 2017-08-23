#include "midi/mapping.h"


void mm_mapping_print(mm_mapping *mappings) {
    if (mappings == NULL) {
        printf("Mappings Null");
        return;
    }

    printf("Mappings: {}\n");
}

mm_mapping *mm_build_mapping() {
    mm_mapping *mappings = malloc(sizeof(mm_mapping));
    return mappings;
}

mm_mapping *mm_mapping_from_list(char *list) {
    mm_mapping * mappings = mm_build_mapping();

    if (list == NULL) {
        return mappings;
    }

    printf("Mapping Source: %s\n", list);
    exit(EXIT_FAILURE);

    return mappings;
}
