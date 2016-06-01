#ifndef TRAYLANG_INTERPRETER_FUNCS_H
#define TRAYLANG_INTERPRETER_FUNCS_H

#include "interpreter.h"
#include "object.h"

#include "bclib/list.h"
#include "bclib/bstrlib.h"

Interpreter *interpreter_create();

void interpreter_destroy(Interpreter *interpreter);

void interpreter_set_debug(Interpreter *interpreter, int debug_value);

void interpreter_error(Interpreter *interpreter, bstring err_message);

Interpreter *interpreter_enter_scope(Interpreter *interpreter);

Interpreter *interpreter_leave_scope(Interpreter *interpreter);

Object *interpreter_set_global(Interpreter *interpreter, bstring name, Object *value);

Object *interpreter_set_variable(Interpreter *interpreter, bstring name, Object *value);

Object *interpreter_get_variable(Interpreter *interpreter, bstring name);

Object *interpreter_stack_push(Interpreter *interpreter, Object *value);

Object *interpreter_stack_pop(Interpreter *interpreter);

Object *get_obj(Interpreter *interpreter);

#endif
