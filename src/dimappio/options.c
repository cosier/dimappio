#include "dimappio/options.h"

dm_options* dm_create_options() {
    dm_options* options = malloc(sizeof(dm_options));
    options->keys = 89;
    options->first = 1;

    options->target = NULL;
    options->source = NULL;
    options->mapping = NULL;

    return options;
}

void dm_options_free(dm_options* options) {
    // delegate cleanup to mapping func
    if (options->mapping != NULL) {
        dm_mapping_free(options->mapping);
    }

    if (options->target != NULL) {
        free(options->target);
    }

    if (options->source != NULL) {
        free(options->source);
    }

    free(options);
}
