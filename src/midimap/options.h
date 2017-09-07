#ifndef DM_OPTIONS_H
#define DM_OPTIONS_H

#include "midi/mapping.h"

typedef struct dm_options {
    dm_mapping* mapping;
    int first;
    int keys;

    char* source;
    char* target;

} dm_options;

dm_options* dm_create_options();
void dm_options_free(dm_options* options);
#endif
