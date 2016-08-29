#ifndef AST_DISPLAY_H
#define AST_DISPLAY_H

#include "ast.h"
#include "bclib/list.h"

void ast_print(Program *program);

void ast_program_print(Program *program, int indentation);

void ast_form_print(Form *form, int indentation);

void ast_definition_print(Definition *definition, int indentation);

void ast_vardef_print(VarDefinition *vardef, int indentation);

void ast_expression_print(Expression *expression, int indentation);

void ast_application_print(Application *application, int indentation);

void ast_arg_list_print(List *list, int indentation);

void ast_let_print(Let *let, int indentation);

void ast_let_binding_print(LetBinding *letBinding, int indentation);

void ast_number_print(Number *number, int indentation);

void ast_string_print(String *string, int indentation);

void ast_variable_print(Variable *variable, int indentation);

void ast_lambda_print(Lambda *lambda, int indentation);

#endif
