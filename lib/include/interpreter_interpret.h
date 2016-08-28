#ifndef TRAYLANG_INTERPRETER_INTERPRET_H
#define TRAYLANG_INTERPRETER_INTERPRET_H

#include "interpreter.h"
#include "ast.h"
#include "object.h"

#include "bclib/list.h"
#include "bclib/bstrlib.h"

Object *interpret(Interpreter *interpreter, Block *block);

Object *interpret_block(Interpreter *interpreter, Block *block);

Object *interpret_element(Interpreter *interpreter, Element *element);

Object *interpret_vardef(Interpreter *interpreter, VarDefinition *vardef);

Object *interpret_lambda(Interpreter *interpreter, Lambda *lambda);

Object *interpret_let(Interpreter *interpreter, Let *let);

Object *interpret_let_variable(Interpreter *interpreter, LetVariable *letVariable);

Object *interpret_application(Interpreter *interpreter, Application *application);

Object *interpret_call_lambda(Interpreter *interpreter, LambdaObject *lambda, List *args);

Object *interpret_call_c_function(Interpreter *interpreter, c_func func, List *args);

Object *interpret_expression(Interpreter *interpreter, Expression *expression);

Object *interpret_number(Interpreter *interpreter, Number *number);

Object *interpret_string(Interpreter *interpreter, String *string);

#endif
