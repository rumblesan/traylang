#include <stdio.h>
#include <stdlib.h>

#include "ast-print.h"
#include "ast.h"
#include "bclib/list.h"

#define DEPTH 2

void indent(int i) {
    printf("%*s", i, " ");
}

void ast_print(Program *program) {
    ast_program_print(program, 0);
}

void ast_program_print(Program *program, int indentation) {
    indent(indentation);
    printf("AST Program with %d forms\n", list_count(program->forms));

    LIST_FOREACH(program->forms, first, next, cur) {
        ast_form_print(cur->value, indentation + DEPTH);
    }
}

void ast_form_print(Form *form, int indentation) {
    switch(form->formType) {
        case DEFINITIONFORM:
            ast_definition_print(form->definition, indentation + DEPTH);
            break;
        case EXPRESSIONFORM:
            ast_expression_print(form->expression, indentation + DEPTH);
            break;
    }
}

void ast_definition_print(Definition *definition, int indentation) {
    switch(definition->definitionType) {
        case VARIABLEDEFINITION:
            ast_vardef_print(definition->varDefinition, indentation + DEPTH);
            break;
    }
}

void ast_vardef_print(VarDefinition *vardef, int indentation) {
    indent(indentation);
    printf("Var Definition: %s\n", vardef->name->data);
    ast_expression_print(vardef->expression, indentation + DEPTH);
}

void ast_expression_print(Expression *expression, int indentation) {
    indent(indentation);
    printf("Expression\n");
    switch(expression->expressionType) {
        case APPLICATIONEXPR:
            ast_application_print(expression->application, indentation + DEPTH);
            break;
        case LETEXPR:
            ast_let_print(expression->let, indentation + DEPTH);
            break;
        case NUMBEREXPR:
            ast_number_print(expression->number, indentation + DEPTH);
            break;
        case BOOLEANEXPR:
            ast_boolean_print(expression->boolean, indentation + DEPTH);
            break;
        case STRINGEXPR:
            ast_string_print(expression->string, indentation + DEPTH);
            break;
        case VARIABLEEXPR:
            ast_variable_print(expression->variable, indentation + DEPTH);
            break;
        case LAMBDAEXPR:
            ast_lambda_print(expression->lambda, indentation + DEPTH);
            break;
        case IFEXPR:
            ast_if_print(expression->ifExpr, indentation + DEPTH);
            break;
    }
}

void ast_expression_list_print(List *expressions, int indentation) {
    LIST_FOREACH(expressions, first, next, cur) {
        ast_expression_print(cur->value, indentation + DEPTH);
    }
}

void ast_application_print(Application *application, int indentation) {
    indent(indentation);
    printf("Application\n");
    ast_expression_print(application->expr, indentation + DEPTH);
    ast_arg_list_print(application->args_expressions, indentation + DEPTH);
}

void ast_arg_list_print(List *args, int indentation) {
    indent(indentation);
    printf("%d args\n", list_count(args));
    LIST_FOREACH(args, first, next, cur) {
        ast_expression_print(cur->value, indentation + DEPTH);
    }
}

void ast_let_print(Let *let, int indentation) {
    indent(indentation);
    printf("Let\n");
    LIST_FOREACH(let->bindings, first, next, cur) {
      ast_let_binding_print(cur->value, indentation + DEPTH);
    }
    ast_expression_list_print(let->expressions, indentation + DEPTH);
}

void ast_let_binding_print(LetBinding *letBinding, int indentation) {
    indent(indentation);
    bstring name = letBinding->name;
    printf("%s:\n", bdata(name));
    ast_expression_print(letBinding->expression, indentation + DEPTH);
}

void ast_number_print(Number *number, int indentation) {
    indent(indentation);
    printf("Number: %f\n", number->value);
}

void ast_boolean_print(Boolean *boolean, int indentation) {
    indent(indentation);
    printf("Boolean: ");
    switch(boolean->value) {
    case BOOLEANTRUE:  printf("true\n"); break;
    case BOOLEANFALSE: printf("false\n"); break;
    }
}

void ast_string_print(String *string, int indentation) {
    indent(indentation);
    printf("String: %s\n", string->value->data);

}

void ast_variable_print(Variable *variable, int indentation) {
    indent(indentation);
    printf("Variable: %s\n", variable->name->data);
}

void ast_lambda_print(Lambda *lambda, int indentation) {
    indent(indentation);
    printf("Lambda: %d args\n", list_count(lambda->arg_names));
    indent(indentation + DEPTH);
    bstring name;
    LIST_FOREACH(lambda->arg_names, first, next, cur) {
        name = cur->value;
        printf("%s  ", bdata(name));
    }
    printf("\n");
}

void ast_if_print(If *ifExpr, int indentation) {
    indent(indentation);
    printf("If\n");

    indent(indentation);
    printf("predicate:\n");
    ast_expression_print(ifExpr->predicate, indentation + DEPTH);

    indent(indentation);
    printf("if block:\n");
    ast_expression_print(ifExpr->ifBlock, indentation + DEPTH);

    if (ifExpr->elseBlock != NULL) {
        indent(indentation);
        printf("else block:\n");
        ast_expression_print(ifExpr->elseBlock, indentation + DEPTH);
    }
    printf("\n");
}
