#include "options.h"

mm_options* mm_create_options() {
    mm_options* options = malloc(sizeof(mm_options));
    options->keys = 89;
    options->first = 1;

    options->target = NULL;
    options->source = NULL;
    options->mapping = NULL;

    return options;
}

void mm_options_free(mm_options* options) {
    // delegate cleanup to mapping func
    if (options->mapping != NULL) {
        mm_mapping_free(options->mapping);
    }

    if (options->target != NULL) {
        free(options->target);
    }

    if (options->source != NULL) {
        free(options->source);
    }

    free(options);
}
