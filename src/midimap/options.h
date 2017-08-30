#ifndef MM_OPTIONS_H
#define MM_OPTIONS_H

#include "midi/mapping.h"

typedef struct mm_options {
    mm_mapping* mapping;
    int first;
    int keys;

    char* source;
    char* target;

} mm_options;

mm_options* mm_create_options();

#endif
