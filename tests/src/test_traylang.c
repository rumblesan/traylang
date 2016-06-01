#include "minunit.h"

#include "traylang.h"

static TrayLang *tray = NULL;


static char * test_init() {
    tray = traylang_init();
    mu_assert(tray != NULL, "Failed to initialize TrayLang");
    return 0;
}

static char *test_interpret() {
    bstring program = bfromcstr("(add 1 (add 2 3))");
    int result = traylang_interpret_string(tray, program);
    mu_assert(result == 0, "Result should be zero");
    return 0;
}

static char *test_cleanup() {
    traylang_cleanup(tray);
    return 0;
}

char *all_tests() {
    mu_suite_start();

    mu_run_test(test_init);
    mu_run_test(test_interpret);
    mu_run_test(test_cleanup);

    return NULL;
}

RUN_TESTS(all_tests);
