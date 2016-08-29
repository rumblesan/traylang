#ifndef TRAYLANG_OBJECT_FUNCS_H
#define TRAYLANG_OBJECT_FUNCS_H

#include "object.h"

#include "interpreter.h"
#include "ast.h"
#include "bclib/list.h"
#include "bclib/bstrlib.h"

Object *object_c_function(Interpreter *interpreter, c_func func);

Object *object_lambda(Interpreter *interpreter, List *arg_names, List *body);

Object *object_list(Interpreter *interpreter, List *list);

Object *object_nothing(Interpreter *interpreter);

Object *object_number(Interpreter *interpreter, double number);

Object *object_string(Interpreter *interpreter, bstring string);

Object *object_cdata(Interpreter *interpreter, void *cdata);

void object_destroy(Object *object);

void object_list_destroy(List *list);

void object_mark(Object *object);

#endif
