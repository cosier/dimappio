#include "helpers.h"

void test_header(char* name) {
    printf("---------------------------------\n");
    printf("[%s] Test Executing\n", name);
}

void test_footer(char* name) { printf("\n[%s] Finished\n\n", name); }

void test_run(char* name, void (*func)(void)) {
    test_header(name);
    func();
    test_footer(name);
}
