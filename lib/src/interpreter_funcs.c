#include <stdlib.h>

#include "dbg.h"

#include "interpreter_funcs.h"

#include "interpreter.h"
#include "interpreter_stackframe.h"
#include "object.h"
#include "object_funcs.h"

#include "bclib/hashmap.h"
#include "bclib/stack.h"
#include "bclib/bstrlib.h"

Interpreter *interpreter_create() {
    Interpreter *interpreter = malloc(sizeof(Interpreter));
    check_mem(interpreter);

    Hashmap *globals = hashmap_create(NULL, NULL);
    check_mem(globals);

    Stack *call_stack = stack_create();
    check_mem(call_stack);

    Stack *scopes = stack_create();
    check_mem(scopes);
    StackFrame *stackframe = stackframe_create();
    check_mem(stackframe);
    stack_push(scopes, stackframe);

    List *objects = list_create();
    check_mem(objects);

    interpreter->error = 0;
    interpreter->globals = globals;
    interpreter->call_stack = call_stack;
    interpreter->scopes = scopes;
    interpreter->objects = objects;
    interpreter->max_objects = INITIAL_GC_THRESHOLD;

    return interpreter;
error:
    return NULL;
}

int globals_hashmap_object_destroy(HashmapNode *node) {
    Object *obj = node->data;
    object_destroy(obj);
    return 0;
}

void interpreter_destroy(Interpreter *interpreter) {
    if (interpreter) {
        if (interpreter->globals) {
            debug("Destroying globals");
            hashmap_destroy(interpreter->globals);
        }
        if (interpreter->call_stack) {
            debug("Destroying call stack");
            stack_destroy(interpreter->call_stack);
        }
        if (interpreter->scopes) {
            debug("Destroying scopes");
            stack_destroy(interpreter->scopes);
        }
        if (interpreter->objects) {
            debug("Destroying objects");
            object_list_destroy(interpreter->objects);
        }
        free(interpreter);
    }
}

void interpreter_error(Interpreter *interpreter, bstring err_message) {
    interpreter->error = 1;
    interpreter->err_message = err_message;
}

void interpreter_clear_error(Interpreter *interpreter) {
    interpreter->error = 0;
    bdestroy(interpreter->err_message);
    interpreter->err_message = bfromcstr("");
}

Interpreter *interpreter_enter_scope(Interpreter *interpreter) {
    debug("Entering scope");
    StackFrame *stackframe = stackframe_create();
    check(stackframe, "Could not create new stackframe");
    stack_push(interpreter->scopes, stackframe);
    return interpreter;
error:
    return NULL;
}

Interpreter *interpreter_leave_scope(Interpreter *interpreter) {
    debug("Leaving scope");
    StackFrame *stackframe = stack_pop(interpreter->scopes);
    check(stackframe != NULL, "Could not leave previous scope");

    stackframe_destroy(stackframe);
    return interpreter;
error:
    interpreter_error(interpreter, bfromcstr("Could not leave scope"));
    return NULL;
}

Object *interpreter_set_global(Interpreter *interpreter, bstring name, Object *value) {
    debug("Set global: %s", name->data);
    check(name, "NULL key passed for global name");
    check(value, "NULL value passed for global value: %s", name->data);
    int result = hashmap_set(interpreter->globals, name, value);
    check(result == 0, "Could not set value in globals hashmap");
    return value;
error:
    interpreter_error(interpreter, bfromcstr("Could not set global"));
    return NULL;
}

Object *interpreter_set_variable(Interpreter *interpreter, bstring name, Object *value) {
    debug("Set variable: %s", name->data);
    check(name, "NULL key passed for variable name");
    check(value, "NULL value passed for variable value: %s", name->data);
    StackFrame *stackframe = interpreter->scopes->stack->value;
    check(stackframe, "Couldn't get stackframe");
    int result = hashmap_set(stackframe->variables, name, value);
    check(result == 0, "Could not set value in variables hashmap");
    return value;
error:
    interpreter_error(interpreter, bfromcstr("Could not set variable"));
    return NULL;
}

Object *interpreter_get_variable(Interpreter *interpreter, bstring name) {
    debug("Get variable: %s", name->data);
    check(name, "NULL key passed for variable name");
    StackFrame *stackframe = interpreter->scopes->stack->value;
    check(stackframe, "Couldn't get stackframe");
    Object *value = hashmap_get(stackframe->variables, name);
    if (value == NULL) {
        value = hashmap_get(interpreter->globals, name);
    }
    check(value, "Could not get variable: %s", name->data);
    return value;
error:
    interpreter_error(interpreter, bfromcstr("Could not get variable"));
    return NULL;
}

Object *interpreter_stack_push(Interpreter *interpreter, Object *value) {
    check(value, "NULL value pushed to stack");
    Stack *stack = stack_push(interpreter->call_stack, value);
    check(stack, "Error pushing value to stack");
    return value;
error:
    interpreter_error(interpreter, bfromcstr("Error pushing value to stack"));
    return NULL;
}

Object *interpreter_stack_pop(Interpreter *interpreter) {
    Object *value = stack_pop(interpreter->call_stack);
    check(value, "Error getting value from stack");
    return value;
error:
    interpreter_error(interpreter, bfromcstr("Error getting value from stack"));
    return NULL;
}

Object *get_obj(Interpreter *interpreter) {
    return interpreter_stack_pop(interpreter);
}


