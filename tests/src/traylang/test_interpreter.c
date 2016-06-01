#include "minunit.h"

#include "interpreter.h"
#include "interpreter_funcs.h"
#include "object.h"
#include "object_funcs.h"

#include "bclib/stack.h"

static Interpreter *interpreter = NULL;


static char * test_create() {
    interpreter = interpreter_create();
    mu_assert(interpreter != NULL, "failed to create interpreter");
    mu_assert(stack_count(interpreter->call_stack) == 0, "Should be 0 items pon stack");
    mu_assert(stack_count(interpreter->scopes) == 1, "Should only be 1 stack frame");
    return 0;
}

static char *test_global_variable() {
    double num = 3;
    Object *obj = object_number(interpreter, num);
    Object *set = interpreter_set_global(interpreter, bfromcstr("foo"), obj);
    mu_assert(set != NULL, "Failed to set global variable");
    Object *get = interpreter_get_variable(interpreter, bfromcstr("foo"));
    mu_assert(get != NULL, "Failed to set global variable");
    mu_assert(get->number == num, "Number was incorrect");
    return 0;
}

static char *test_local_variable() {
    double num = 5;
    Object *obj = object_number(interpreter, num);
    Object *set = interpreter_set_variable(interpreter, bfromcstr("bar"), obj);
    mu_assert(set != NULL, "Failed to set local variable");
    Object *get = interpreter_get_variable(interpreter, bfromcstr("bar"));
    mu_assert(get != NULL, "Failed to set local variable");
    mu_assert(get->number == num, "Number was incorrect");
    return 0;
}

static char *test_variable_error() {
    Object *get = interpreter_get_variable(interpreter, bfromcstr("nothere"));
    mu_assert(get == NULL, "Variable should not exist");
    mu_assert(interpreter->error == 1, "Interpreter should have errored");
    interpreter_clear_error(interpreter);
    return 0;
}

static char *test_scoping() {
    bstring varname = bfromcstr("baz");
    Object *obj1 = object_number(interpreter, 7);
    Object *set1 = interpreter_set_variable(interpreter, varname, obj1);
    mu_assert(set1 != NULL, "Failed to set local");
    Interpreter *i1 = interpreter_enter_scope(interpreter);
    mu_assert(i1 != NULL, "Failed to enter scope");

    mu_assert(stack_count(interpreter->scopes) == 2, "Should be 2 stack frame");

    Object *obj2 = object_number(interpreter, 9);
    Object *set2 = interpreter_set_variable(interpreter, varname, obj2);
    mu_assert(set2 != NULL, "Failed to set local");
    Object *get2 = interpreter_get_variable(interpreter, varname);
    mu_assert(get2 != NULL, "Failed to set global");
    mu_assert(get2->number == 9, "Number was incorrect");

    Interpreter *i2 = interpreter_leave_scope(interpreter);
    mu_assert(i2 != NULL, "Failed to leave scope");

    Object *get3 = interpreter_get_variable(interpreter, varname);
    mu_assert(get3 != NULL, "Failed to set global");
    mu_assert(get3->number == 7, "Number was incorrect");

    return 0;
}

static char *test_scoping_error() {
    Interpreter *i1 = interpreter_leave_scope(interpreter);
    mu_assert(i1 != NULL, "Error leaving scope");
    mu_assert(stack_count(interpreter->scopes) == 0, "Should be 0 stack frame");
    Interpreter *i2 = interpreter_leave_scope(interpreter);
    mu_assert(i2 == NULL, "Expected error leaving scope");
    mu_assert(interpreter->error == 1, "Interpreter should have errored");
    interpreter_clear_error(interpreter);

    return 0;
}

static char *test_stack() {
    Object *push1 = interpreter_stack_push(interpreter,
        object_number(interpreter, 1)
    );
    mu_assert(push1 != NULL, "Failed to push push1 to stack");
    Object *push2 = interpreter_stack_push(interpreter,
        object_number(interpreter, 2)
    );
    mu_assert(push2 != NULL, "Failed to push push2 to stack");
    Object *push3 = interpreter_stack_push(interpreter,
        object_number(interpreter, 3)
    );
    mu_assert(push3 != NULL, "Failed to push push3 to stack");

    Object *obj3 = interpreter_stack_pop(interpreter);
    mu_assert(obj3 != NULL, "Failed to pop obj3 from stack");
    Object *obj2 = interpreter_stack_pop(interpreter);
    mu_assert(obj2 != NULL, "Failed to pop obj2 from stack");
    Object *obj1 = get_obj(interpreter);
    mu_assert(obj1 != NULL, "Failed to pop obj1 from stack");

    return 0;
}

static char *test_stack_error() {
    Object *obj1 = get_obj(interpreter);
    mu_assert(obj1 == NULL, "Should be nothing on the stack");
    mu_assert(interpreter->error == 1, "Interpreter should have errored");
    interpreter_clear_error(interpreter);

    return 0;
}


static char *test_destroy() {
    interpreter_destroy(interpreter);
    return 0;
}

char *all_tests() {
    mu_suite_start();

    mu_run_test(test_create);
    mu_run_test(test_global_variable);
    mu_run_test(test_local_variable);
    mu_run_test(test_variable_error);
    mu_run_test(test_scoping);
    mu_run_test(test_scoping_error);
    mu_run_test(test_stack);
    mu_run_test(test_stack_error);
    mu_run_test(test_destroy);

    return NULL;
}

RUN_TESTS(all_tests);
