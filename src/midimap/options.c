#include "options.h"

mm_options* mm_create_options() {
    mm_options* options = malloc(sizeof(mm_options*));
    options->keys = 89;
    options->first = 1;

    options->target = NULL;
    options->source = NULL;
    options->mapping = NULL;

    return options;
}
