#include <stdlib.h>

#include "dbg.h"

#include "interpreter_interpret.h"

#include "interpreter_funcs.h"
#include "interpreter.h"
#include "object.h"
#include "object_funcs.h"

#include "bclib/hashmap.h"
#include "bclib/stack.h"
#include "bclib/bstrlib.h"

Object *interpret(Interpreter *interpreter, Block *block) {
    return interpret_block(interpreter, block);
}

Object *interpret_block(Interpreter *interpreter, Block *block) {
    Object *return_val = NULL;
    LIST_FOREACH(block->elements, first, next, cur) {
        return_val = interpret_element(interpreter, cur->value);
        check(return_val, "Error interpreting element");
        check(interpreter->error != 1, "Error whilst interpreting");
    }
    return return_val;
error:
    return NULL;
}

Object *interpret_element(Interpreter *interpreter, Element *element) {
    switch(element->elementType) {
        case VARDEFINITIONEL: return interpret_vardef(interpreter, element->varDefinition); break;
        case APPLICATIONEL:   return interpret_application(interpreter, element->application); break;
    }
}

Object *interpret_vardef(Interpreter *interpreter, VarDefinition *vardef) {
    Object *expr_value = interpret_expression(interpreter, vardef->expression);
    check(interpreter->error != 1, "Error interpreting vardef");
    check(expr_value, "Error evaluating vardef");
    interpreter_set_variable(interpreter, bstrcpy(vardef->name), expr_value);
    check(interpreter->error != 1, "Error interpreting vardef");
    return expr_value;
error:
    if (interpreter->error == 0) {
        interpreter_error(interpreter, bfromcstr("Error interpreting vardef"));
    }
    return NULL;
}

Object *interpret_lambda(Interpreter *interpreter, Lambda *lambda) {
    List *arg_names = list_create();
    LIST_FOREACH(lambda->arg_names, last, prev, cur) {
        list_push(arg_names, bstrcpy(cur->value));
    }
    Object *lambda_object = object_lambda(interpreter, arg_names, lambda->body);
    check(lambda_object, "Error evaluating vardef");
    return lambda_object;
error:
    if (interpreter->error == 0) {
        interpreter_error(interpreter, bfromcstr("Error interpreting lambda"));
    }
    return NULL;
}

Stack *interpreter_push_args(Interpreter *interpreter, List *args) {
    Object *val;
    // Iterate list in reverse so first arg is highest on stack
    LIST_FOREACH(args, last, prev, cur) {
        val = interpret_expression(interpreter, cur->value);
        interpreter_stack_push(interpreter, val);
        check(interpreter->error != 1, "Error whilst interpreting");
    }
    return interpreter->call_stack;
error:
    return NULL;
}

Interpreter *interpreter_assign_args(Interpreter *interpreter, List *arg_names, List *args) {
    int arg_count = list_count(args);
    check(list_count(arg_names) == arg_count, "Different arg names and values length");

    bstring name;
    Expression *val;
    Object *evaled;
    int i;
    for (i = 0; i < arg_count; i += 1) {
        name = list_get(arg_names, i);
        check(name, "Couldn't get argument name");
        val = list_get(args, i);
        evaled = interpret_expression(interpreter, val);
        check(evaled, "arg expression didn't evaluate");

        interpreter_set_variable(interpreter, name, evaled);
        check(interpreter->error != 1, "Error whilst interpreting");
    }
    return interpreter;
error:
    interpreter_error(
        interpreter,
        bfromcstr("Error assigning args")
    );
    return NULL;
}

Object *interpret_application(Interpreter *interpreter, Application *application) {
    int arg_num = list_count(application->args);
    if (interpreter->debug_mode) {
        debug("Application");
        debug("Arg num: %d", arg_num);
    }

    Object *func_obj = interpret_expression(interpreter, application->expr);
    Object *result;
    switch(func_obj->type) {
        case CFUNCTION:
            result = interpret_call_c_function(
                interpreter, func_obj->cfunction, application->args
            );
            break;
        case LAMBDA:
            result = interpret_call_lambda(
                interpreter, func_obj->lambda, application->args
            );
            break;
        default:
            interpreter_error(
                interpreter,
                bfromcstr("Could not apply object")
            );
            break;
    }
    check(interpreter->error != 1, "Error whilst interpreting");
    return result;
error:
    return NULL;
}

Object *interpret_call_c_function(Interpreter *interpreter, c_func func, List *args) {
    Stack *stack = interpreter_push_args(interpreter, args);
    check(stack, "Error whilst pushing args to stack");

    interpreter_enter_scope(interpreter);
    Object *result = func(interpreter, list_count(args));
    interpreter_leave_scope(interpreter);
    return result;
error:
    return NULL;
}

Object *interpret_call_lambda(Interpreter *interpreter, LambdaObject *lambda, List *args) {
    if (interpreter->debug_mode) {
        debug("Applying lambda");
    }
    interpreter_enter_scope(interpreter);
    Interpreter *i = interpreter_assign_args(
        interpreter,
        lambda->arg_names,
        args
    );
    check(i, "Error whilst assigning args");

    Object *result = interpret(interpreter, lambda->body);
    interpreter_leave_scope(interpreter);
    return result;
error:
    return NULL;
}

Object *interpret_expression(Interpreter *interpreter, Expression *expression) {
    Object *v = NULL;
    switch(expression->expressionType) {
        case APPLICATIONEXPR:
            if (interpreter->debug_mode) {
                debug("interpret application");
            }
            v = interpret_application(interpreter, expression->application);
            break;
        case LAMBDAEXPR:
            if (interpreter->debug_mode) {
                debug("interpret lambda");
            }
            v = interpret_lambda(interpreter, expression->lambda);
            break;
        case NUMBEREXPR:
            if (interpreter->debug_mode) {
                debug("interpret number");
            }
            v = interpret_number(interpreter, expression->number);
            break;
        case STRINGEXPR:
            if (interpreter->debug_mode) {
                debug("interpret string");
            }
            v = interpret_string(interpreter, expression->string);
            break;
        case VARIABLEEXPR:
            if (interpreter->debug_mode) {
                debug("interpret variable");
            }
            v = interpreter_get_variable(interpreter, expression->variable->name);
            break;
    }
    return v;
}

Object *interpret_number(Interpreter *interpreter, Number *number) {
    Object *dv = object_number(interpreter, number->value);
    check(dv != NULL, "Could not create object")
    return dv;
error:
    if (interpreter->error == 0) {
        interpreter_error(interpreter, bfromcstr("Error whilst creating number object"));
    }
    return NULL;
}

Object *interpret_string(Interpreter *interpreter, String *string) {
    Object *dv = object_string(interpreter, bstrcpy(string->value));
    check(dv != NULL, "Could not create object")
    return dv;
error:
    if (interpreter->error == 0) {
        interpreter_error(interpreter, bfromcstr("Error whilst creating number object"));
    }
    return NULL;
}
