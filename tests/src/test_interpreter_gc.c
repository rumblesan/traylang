#include "minunit.h"

#include "interpreter.h"
#include "interpreter_funcs.h"
#include "interpreter_gc.h"
#include "object.h"
#include "object_funcs.h"

#include "bclib/stack.h"

static Interpreter *interpreter = NULL;


static char * test_create() {
    interpreter = interpreter_create();
    mu_assert(interpreter != NULL, "failed to create interpreter");
    return 0;
}

static char *test_simple_gc() {
    object_number(interpreter, 1);
    object_number(interpreter, 2);
    object_number(interpreter, 3);
    object_number(interpreter, 4);
    mu_assert(list_count(interpreter->objects) == 4, "Should be four objects");
    interpreter_gc(interpreter);
    mu_assert(list_count(interpreter->objects) == 0, "Should be zero objects");
    return 0;
}

static char *test_globals_gc() {
    interpreter_set_global(
        interpreter, bfromcstr("1"), object_number(interpreter, 1)
    );
    interpreter_set_global(
        interpreter, bfromcstr("2"), object_number(interpreter, 2)
    );
    mu_assert(list_count(interpreter->objects) == 2, "Should be two objects");
    interpreter_gc(interpreter);
    mu_assert(list_count(interpreter->objects) == 2, "Should still be 2 objects");
    return 0;
}

static char *test_scoped_variables_gc() {
    interpreter_enter_scope(interpreter);
    interpreter_set_variable(
        interpreter, bfromcstr("3"), object_number(interpreter, 1)
    );
    interpreter_set_variable(
        interpreter, bfromcstr("4"), object_number(interpreter, 2)
    );
    mu_assert(list_count(interpreter->objects) == 4, "Should be four objects");
    interpreter_gc(interpreter);
    mu_assert(list_count(interpreter->objects) == 4, "Should still be 4 objects");
    interpreter_leave_scope(interpreter);
    interpreter_gc(interpreter);
    mu_assert(list_count(interpreter->objects) == 2, "Should now be 2 objects");
    return 0;
}

static char *test_auto_run_gc() {
    interpreter->max_objects = 5;
    mu_assert(list_count(interpreter->objects) == 2, "Should be 2 objects");
    object_number(interpreter, 3);
    object_number(interpreter, 4);
    object_number(interpreter, 5);
    // This object creation will trigger GC, and then this object is
    // added to objects list
    object_number(interpreter, 5);

    mu_assert(list_count(interpreter->objects) == 3, "Should be 3 objects");
    // max objects will be reset to INITIAL_GC_THRESHOLD
    mu_assert(interpreter->max_objects == INITIAL_GC_THRESHOLD, "Max objects should be INITIAL_GC_THRESHOLD");
    return 0;
}

static char *test_destroy() {
    interpreter_destroy(interpreter);
    return 0;
}

char *all_tests() {
    mu_suite_start();

    mu_run_test(test_create);
    mu_run_test(test_simple_gc);
    mu_run_test(test_globals_gc);
    mu_run_test(test_scoped_variables_gc);
    mu_run_test(test_auto_run_gc);
    mu_run_test(test_destroy);

    return NULL;
}

RUN_TESTS(all_tests);
