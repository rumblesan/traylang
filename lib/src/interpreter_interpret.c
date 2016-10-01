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

Object *interpret(Interpreter *interpreter, Program *program) {
    return interpret_program(interpreter, program);
}

Object *interpret_program(Interpreter *interpreter, Program *program) {
    Object *return_val = NULL;
    LIST_FOREACH(program->forms, first, next, cur) {
        return_val = interpret_form(interpreter, cur->value);
        check(return_val != NULL, "Error interpreting form");
        check(interpreter->error != 1, "Error whilst interpreting");
    }
    return return_val;
error:
    if (interpreter->error == 0) {
        interpreter_error(interpreter, bfromcstr("Error interpreting form"));
    }
    return NULL;
}

Object *interpret_form(Interpreter *interpreter, Form *form) {
    switch(form->formType) {
        case DEFINITIONFORM: return interpret_definition(interpreter, form->definition); break;
        case EXPRESSIONFORM: return interpret_expression(interpreter, form->expression); break;
    }
}

Object *interpret_definition(Interpreter *interpreter, Definition *definition) {
    switch(definition->definitionType) {
        case VARIABLEDEFINITION: return interpret_vardef(interpreter, definition->varDefinition); break;
    }
}

Object *interpret_vardef(Interpreter *interpreter, VarDefinition *vardef) {
    Object *expr_value = interpret_expression(interpreter, vardef->expression);
    check(interpreter->error != 1, "Error interpreting vardef");
    check(expr_value != NULL, "Error evaluating vardef");
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
    check(lambda_object != NULL, "Error evaluating lambda");
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
        check(val != NULL, "Error whilst evaluating expression");
        interpreter_stack_push(interpreter, val);
        check(interpreter->error != 1, "Error whilst interpreting");
    }
    return interpreter->call_stack;
error:
    if (interpreter->error == 0) {
        interpreter_error(interpreter, bfromcstr("Error pushing args"));
    }
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
        check(name != NULL, "Couldn't get argument name");
        val = list_get(args, i);
        evaled = interpret_expression(interpreter, val);
        check(evaled != NULL, "arg expression didn't evaluate");

        interpreter_set_variable(interpreter, name, evaled);
        check(interpreter->error != 1, "Error whilst interpreting");
    }
    return interpreter;
error:
    if (interpreter->error == 0) {
        interpreter_error(interpreter, bfromcstr("Error assigning args"));
    }
    return NULL;
}

Object *interpret_application(Interpreter *interpreter, Application *application) {
    debug("Application - arg num: %d", list_count(application->args_expressions));

    Object *func_obj = interpret_expression(interpreter, application->expr);
    Object *result;
    switch(func_obj->type) {
        case CFUNCTION:
            result = interpret_call_c_function(
                interpreter, func_obj->cfunction, application->args_expressions
            );
            break;
        case LAMBDA:
            result = interpret_call_lambda(
                interpreter, func_obj->lambda, application->args_expressions
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
    if (interpreter->error == 0) {
        interpreter_error(interpreter, bfromcstr("Error interpreting application"));
    }
    return NULL;
}

Object *interpret_call_c_function(Interpreter *interpreter, c_func func, List *args) {
    Stack *stack = interpreter_push_args(interpreter, args);
    check(stack != NULL, "Error whilst pushing args to stack");

    Interpreter *i1 = interpreter_enter_scope(interpreter);
    check(i1 != NULL, "Error whilst entering scope");
    Object *result = func(interpreter, list_count(args));

    Interpreter *i2 = interpreter_leave_scope(interpreter);
    check(i2 != NULL, "Error whilst leaving scope");
    return result;
error:
    if (interpreter->error == 0) {
        interpreter_error(interpreter, bfromcstr("Error calling C function"));
    }
    return NULL;
}

Object *interpret_call_lambda(Interpreter *interpreter, LambdaObject *lambda, List *args) {
    debug("Applying lambda");
    interpreter_enter_scope(interpreter);
    Interpreter *i1 = interpreter_assign_args(
        interpreter,
        lambda->arg_names,
        args
    );
    check(i1 != NULL, "Error whilst assigning args");

    Object *return_val = NULL;
    LIST_FOREACH(lambda->body, first, next, cur) {
        return_val = interpret_form(interpreter, cur->value);
        check(return_val != NULL, "Error interpreting lambda body form");
        check(interpreter->error != 1, "Error whilst interpreting");
    }
    return return_val;

    Interpreter *i2 = interpreter_leave_scope(interpreter);
    check(i2 != NULL, "Error whilst leaving scope");
    return return_val;
error:
    if (interpreter->error == 0) {
        interpreter_error(interpreter, bfromcstr("Error calling lambda"));
    }
    return NULL;
}

Object *interpret_expression(Interpreter *interpreter, Expression *expression) {
    Object *v = NULL;
    switch(expression->expressionType) {
        case APPLICATIONEXPR:
            debug("interpret application");
            v = interpret_application(interpreter, expression->application);
            break;
        case LAMBDAEXPR:
            debug("interpret lambda");
            v = interpret_lambda(interpreter, expression->lambda);
            break;
        case LETEXPR:
            debug("interpret let");
            v = interpret_let(interpreter, expression->let);
            break;
        case BOOLEANEXPR:
            debug("interpret boolean");
            v = interpret_boolean(interpreter, expression->boolean);
            break;
        case IFEXPR:
            debug("interpret if");
            v = interpret_if(interpreter, expression->ifExpr);
            break;
        case LISTEXPR:
            debug("interpret list");
            v = interpret_expression_list(interpreter, expression->list);
            break;
        case NUMBEREXPR:
            debug("interpret number");
            v = interpret_number(interpreter, expression->number);
            break;
        case STRINGEXPR:
            debug("interpret string");
            v = interpret_string(interpreter, expression->string);
            break;
        case VARIABLEEXPR:
            debug("interpret variable");
            v = interpreter_get_variable(interpreter, expression->variable->name);
            break;
    }
    return v;
}

Object *interpret_expression_list(Interpreter *interpreter, List *expressions) {
    Object *return_val = NULL;
    LIST_FOREACH(expressions, first, next, cur) {
        return_val = interpret_expression(interpreter, cur->value);
        check(return_val != NULL, "Error interpreting expression");
        check(interpreter->error != 1, "Error whilst interpreting");
    }
    return return_val;
error:
    if (interpreter->error == 0) {
        interpreter_error(interpreter, bfromcstr("Error interpreting expression list"));
    }
    return NULL;
}

Object *interpret_number(Interpreter *interpreter, Number *number) {
    Object *dv = object_number(interpreter, number->value);
    check(dv != NULL, "Could not create object")
    return dv;
error:
    if (interpreter->error == 0) {
        interpreter_error(interpreter, bfromcstr("Error interpreting number"));
    }
    return NULL;
}

Object *interpret_boolean(Interpreter *interpreter, Boolean *boolean) {
    int bool_value = 0;
    switch(boolean->value) {
        case BOOLEANTRUE:  bool_value = 1; break;
        case BOOLEANFALSE: bool_value = 0; break;
    }
    Object *dv = object_boolean(interpreter, bool_value);
    check(dv != NULL, "Could not create object")
    return dv;
error:
    if (interpreter->error == 0) {
        interpreter_error(interpreter, bfromcstr("Error interpreting number"));
    }
    return NULL;
}

Object *interpret_if(Interpreter *interpreter, If *ifExpr) {
    Object *pred_value = interpret_expression(interpreter, ifExpr->predicate);
    check(pred_value != NULL, "Error evaluating predicate");
    if (pred_value->type != BOOLEAN) {
        interpreter_error(interpreter, bfromcstr("If predicate must be boolean"));
    }
    Object *return_value = object_nothing(interpreter);
    if (pred_value->boolean == 1) {
        return_value = interpret_expression(interpreter, ifExpr->ifBlock);
    }
    if (ifExpr->elseBlock != NULL && pred_value->boolean == 0) {
        return_value = interpret_expression(interpreter, ifExpr->elseBlock);
    }
    check(return_value != NULL, "Error evaluating if");
    return return_value;
error:
    if (interpreter->error == 0) {
        interpreter_error(interpreter, bfromcstr("Error interpreting if"));
    }
    return NULL;
}

Object *interpret_string(Interpreter *interpreter, String *string) {
    Object *dv = object_string(interpreter, bstrcpy(string->value));
    check(dv != NULL, "Could not create object")
    return dv;
error:
    if (interpreter->error == 0) {
        interpreter_error(interpreter, bfromcstr("Error interpreting number"));
    }
    return NULL;
}

Object *interpret_let(Interpreter *interpreter, Let *let) {
    Object *let_value = NULL;
    interpreter_enter_scope(interpreter);
    LIST_FOREACH(let->bindings, first, next, cur) {
        let_value = interpret_let_binding(interpreter, cur->value);
        check(interpreter->error != 1, "Error interpreting let expression");
        check(let_value != NULL, "Error evaluating let expression");
    }

    Object *return_val = interpret_expression_list(interpreter, let->expressions);
    check(return_val != NULL, "Error evaluating let expressions");

    Interpreter *i2 = interpreter_leave_scope(interpreter);
    check(i2 != NULL, "Error whilst leaving scope");
    return return_val;
error:
    if (interpreter->error == 0) {
        interpreter_error(interpreter, bfromcstr("Error interpreting let expression"));
    }
    return NULL;
}

Object *interpret_let_binding(Interpreter *interpreter, LetBinding *letBinding) {
    Object *expression_value = interpret_expression(interpreter, letBinding->expression);
    check(interpreter->error != 1, "Error interpreting let binding");
    check(expression_value != NULL, "Error evaluating let binding");
    interpreter_set_variable(interpreter, bstrcpy(letBinding->name), expression_value);
    check(interpreter->error != 1, "Error interpreting let binding");
    return expression_value;
error:
    if (interpreter->error == 0) {
        interpreter_error(interpreter, bfromcstr("Error interpreting let binding"));
    }
    return NULL;
}
