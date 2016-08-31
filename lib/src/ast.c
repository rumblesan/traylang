#include <stdlib.h>

#include "ast.h"
#include "bclib/list.h"
#include "bclib/bstrlib.h"

/* TODO
 * Add proper error handling
 */

/* Program AST Node */
Program *ast_program_create(List *forms) {
    Program *program = malloc(sizeof(Program));
    program->forms = forms;
    return program;
}

void ast_program_cleanup(Program *program) {
    LIST_FOREACH(program->forms, first, next, cur) {
        ast_form_cleanup(cur->value);
    }
    free(program);
}

/* Form AST Node */
Form *ast_form_create() {
    Form *form = malloc(sizeof(Form));
    return form;
}

void ast_form_cleanup(Form *form) {
    switch(form->formType) {
        case DEFINITIONFORM:
            ast_definition_cleanup(form->definition);
            break;
        case EXPRESSIONFORM:
            ast_expression_cleanup(form->expression);
            break;
    }
    free(form);
}

Form *ast_definition_form(Definition *definition) {
    Form *form = ast_form_create();
    form->formType = DEFINITIONFORM;
    form->definition = definition;
    return form;
}

Form *ast_expression_form(Expression *expression) {
    Form *form = ast_form_create();
    form->formType = EXPRESSIONFORM;
    form->expression = expression;
    return form;
}

/* Definition AST Node */
Definition *ast_definition_create() {
    Definition *definition = malloc(sizeof(Definition));
    return definition;
}

void ast_definition_cleanup(Definition *definition) {
    switch(definition->definitionType) {
        case VARIABLEDEFINITION:
            ast_vardef_cleanup(definition->varDefinition);
            break;
    }
    free(definition);
}

Definition *ast_variable_definition(VarDefinition *varDefinition) {
    Definition *definition = ast_definition_create();
    definition->definitionType = VARIABLEDEFINITION;
    definition->varDefinition = varDefinition;
    return definition;
}

/* Variable Definition AST Node */
VarDefinition *ast_vardef_create(bstring name, Expression *expression) {
    VarDefinition *varDef = malloc(sizeof(VarDefinition));
    varDef->name = name;
    varDef->expression = expression;

    return varDef;
}

void ast_vardef_cleanup(VarDefinition *vardef) {
    bdestroy(vardef->name);
    ast_expression_cleanup(vardef->expression);
    free(vardef);
}

/* Expression AST Node */
Expression *ast_expression_create() {
    Expression *expression = malloc(sizeof(Expression));
    return expression;
}

void ast_expression_cleanup(Expression *expression) {
    switch(expression->expressionType) {
        case NUMBEREXPR:
            ast_number_cleanup(expression->number);
            break;
        case BOOLEANEXPR:
            ast_boolean_cleanup(expression->boolean);
            break;
        case STRINGEXPR:
            ast_string_cleanup(expression->string);
            break;
        case VARIABLEEXPR:
            ast_variable_cleanup(expression->variable);
            break;
        case LAMBDAEXPR:
            ast_lambda_cleanup(expression->lambda);
            break;
        case APPLICATIONEXPR:
            ast_application_cleanup(expression->application);
            break;
        case LETEXPR:
            ast_let_cleanup(expression->let);
            break;
        case IFEXPR:
            ast_if_cleanup(expression->ifExpr);
            break;
    }
    free(expression);
}

void ast_expression_list_cleanup(List *expressions) {
    LIST_FOREACH(expressions, first, next, cur) {
        ast_expression_cleanup(cur->value);
    }
}

Expression *ast_application_expression(Application *application) {
    Expression *expression = ast_expression_create();
    expression->expressionType = APPLICATIONEXPR;
    expression->application = application;
    return expression;
}

Expression *ast_let_expression(Let *let) {
    Expression *expression = ast_expression_create();
    expression->expressionType = LETEXPR;
    expression->let = let;
    return expression;
}

Expression *ast_number_expression(Number *number) {
    Expression *expression = ast_expression_create();
    expression->expressionType = NUMBEREXPR;
    expression->number = number;
    return expression;
}

Expression *ast_if_expression(If *ifExpr) {
    Expression *expression = ast_expression_create();
    expression->expressionType = IFEXPR;
    expression->ifExpr = ifExpr;
    return expression;
}

Expression *ast_boolean_expression(Boolean *boolean) {
    Expression *expression = ast_expression_create();
    expression->expressionType = BOOLEANEXPR;
    expression->boolean = boolean;
    return expression;
}

Expression *ast_string_expression(String *string) {
    Expression *expression = ast_expression_create();
    expression->expressionType = STRINGEXPR;
    expression->string = string;
    return expression;
}

Expression *ast_variable_expression(Variable *variable) {
    Expression *expression = ast_expression_create();
    expression->expressionType = VARIABLEEXPR;
    expression->variable = variable;
    return expression;
}

Expression *ast_lambda_expression(Lambda *lambda) {
    Expression *expression = ast_expression_create();
    expression->expressionType = LAMBDAEXPR;
    expression->lambda = lambda;
    return expression;
}

/* Application AST Node */
Application *ast_application_create(Expression *expr, List *args_expressions) {
    Application *application = malloc(sizeof(Application));
    application->expr = expr;
    application->args_expressions = args_expressions;

    return application;
}

void ast_application_cleanup(Application *application) {
    ast_expression_cleanup(application->expr);
    bstring argname;
    LIST_FOREACH(application->args_expressions, first, next, cur) {
        argname = cur->value;
        bdestroy(argname);
    }
    list_destroy(application->args_expressions);
    free(application);
}


LetBinding *ast_let_binding_create(bstring name, Expression *expression) {
    LetBinding *letBinding = malloc(sizeof(LetBinding));
    letBinding->name = name;
    letBinding->expression = expression;
    return letBinding;
}

void ast_let_binding_cleanup(LetBinding *letBinding) {
    bdestroy(letBinding->name);
    ast_expression_cleanup(letBinding->expression);
    free(letBinding);
}

/* If AST Node */
If *ast_if_create(
    Expression *predicate,
    Expression *ifBlock,
    Expression *elseBlock
) {
    If *ifExpr = malloc(sizeof(If));
    ifExpr->predicate = predicate;
    ifExpr->ifBlock   = ifBlock;
    ifExpr->elseBlock = elseBlock;
    return ifExpr;
}

void ast_if_cleanup(If *ifExpr) {
    ast_expression_cleanup(ifExpr->predicate);
    ast_expression_cleanup(ifExpr->ifBlock);
    ast_expression_cleanup(ifExpr->elseBlock);
    free(ifExpr);
}

/* Let AST Node */
Let *ast_let_create(List *bindings, List *expressions) {
    Let *let = malloc(sizeof(Let));
    let->bindings = bindings;
    let->expressions = expressions;
    return let;
}

void ast_let_cleanup(Let *let) {
    LetBinding *letBinding;
    LIST_FOREACH(let->bindings, first, next, cur) {
        letBinding = cur->value;
        ast_let_binding_cleanup(letBinding);
    }
    ast_expression_list_cleanup(let->expressions);
    free(let);
}

/* Lambda AST Node */
Lambda *ast_lambda_create(List *arg_names, List *body) {
    Lambda *lambda = malloc(sizeof(Lambda));
    lambda->arg_names = arg_names;
    lambda->body = body;
    return lambda;
}

void ast_lambda_cleanup(Lambda *lambda) {
    bstring argname;
    LIST_FOREACH(lambda->arg_names, first, next, cur) {
        argname = cur->value;
        bdestroy(argname);
    }
    ast_expression_list_cleanup(lambda->body);
    free(lambda);
}

/* Variable AST Node */
Variable *ast_variable_create(bstring name) {
    Variable *variable = malloc(sizeof(Variable));
    variable->name = name;

    return variable;
}

void ast_variable_cleanup(Variable *variable) {
    bdestroy(variable->name);
    free(variable);
}

/* String Literal AST Node */
String *ast_string_create(bstring value) {
    String *string = malloc(sizeof(String));
    string->value = value;

    return string;
}

void ast_string_cleanup(String *string) {
    bdestroy(string->value);
    free(string);
}

/* Boolean Literal AST Node */
Boolean *ast_boolean_create(BooleanValue value) {
    Boolean *boolean = malloc(sizeof(Boolean));
    boolean->value = value;
    return boolean;
}

void ast_boolean_cleanup(Boolean *boolean) {
    free(boolean);
}

/* Number Literal AST Node */
Number *ast_number_create(double value) {
    Number *number = malloc(sizeof(Number));
    number->value = value;

    return number;
}

void ast_number_cleanup(Number *number) {
    free(number);
}
