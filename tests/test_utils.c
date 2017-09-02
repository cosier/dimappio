
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "helpers/helpers.h"
#include <midimap/core.h>

void test_cats() {
    int bsize = 128;
    char* buf = malloc(sizeof(char) * bsize);
    char* ptr = buf;

    mm_cat(&ptr, "Kitty");
    mm_cat(&ptr, "Cat");
    printf("buf: %s\n", buf);

    assert(strcmp(buf, "KittyCat") == 0);
    assert(strcmp(ptr, "") == 0);
}

int main() {
    test_run("Cats", &test_cats);
    return 0;
}
