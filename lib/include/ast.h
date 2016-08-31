#ifndef AST_H
#define AST_H

#include "bclib/list.h"
#include "bclib/bstrlib.h"

/* Identifier is just a bstring */
typedef struct tagbstring Identifier;

/* Number Literal AST Node */
typedef struct Number {

    double value;

} Number;

Number *ast_number_create(double value);

void ast_number_cleanup(Number *number);

/* Boolean Node */
typedef enum {
  BOOLEANTRUE,
  BOOLEANFALSE
} BooleanValue;

typedef struct Boolean {

    BooleanValue value;

} Boolean;

Boolean *ast_boolean_create(BooleanValue value);

void ast_boolean_cleanup(Boolean *boolean);

/* String Literal AST Node */
typedef struct String {

    bstring value;

} String;

String *ast_string_create(bstring value);

void ast_string_cleanup(String *string);

/* Variable AST Node */
typedef struct Variable {

    bstring name;

} Variable;

Variable *ast_variable_create(bstring name);

void ast_variable_cleanup(Variable *variable);

/* Lambda AST Node */
typedef struct Lambda {

    List *arg_names;

    List *body;

} Lambda;

Lambda *ast_lambda_create(List *arg_names, List *body);

void ast_lambda_cleanup(Lambda *lambda);

/* Application AST Node */
typedef struct Application {

    struct Expression *expr;

    List *args_expressions;

} Application;

Application *ast_application_create(struct Expression *expr, List *args_expressions);

void ast_application_cleanup(Application *application);

/* Let AST Node */
typedef struct Let {

    List *bindings;

    List *expressions;

} Let;

Let *ast_let_create(List *variable_expressions, List *expressions);

void ast_let_cleanup(Let *let);

/* Let Binding AST Node */
typedef struct LetBinding {

    bstring name;

    struct Expression *expression;

} LetBinding;

LetBinding *ast_let_binding_create(bstring name, struct Expression *expr);

void ast_let_binding_cleanup(LetBinding *let_binding);

/* If AST Node */
typedef struct If {

    struct Expression *predicate;

    struct Expression *ifBlock;

    struct Expression *elseBlock;

} If;

If *ast_if_create(
    struct Expression *predicate,
    struct Expression *ifBlock,
    struct Expression *elseBlock
);

void ast_if_cleanup(If *ifExpression);

/* Expression AST Node */
typedef enum {
  NUMBEREXPR,
  BOOLEANEXPR,
  STRINGEXPR,
  VARIABLEEXPR,
  LAMBDAEXPR,
  APPLICATIONEXPR,
  LETEXPR,
  IFEXPR
} ExpressionType;

typedef struct Expression {

    ExpressionType expressionType;

    union {
        Number *number;
        Boolean *boolean;
        String *string;
        Variable *variable;
        Lambda *lambda;
        Application *application;
        Let *let;
        If *ifExpr;
    };

} Expression;

Expression *ast_expression_create();

void ast_expression_cleanup(Expression *expression);

Expression *ast_number_expression(Number *number);

Expression *ast_boolean_expression(Boolean *boolean);

Expression *ast_string_expression(String *string);

Expression *ast_variable_expression(Variable *variable);

Expression *ast_lambda_expression(Lambda *lambda);

Expression *ast_application_expression(Application *application);

Expression *ast_let_expression(Let *let);

Expression *ast_if_expression(If *ifExpression);


/* Variable Definition AST Node */
typedef struct VarDefinition {

    bstring name;

    Expression *expression;

} VarDefinition;

VarDefinition *ast_vardef_create(bstring name, Expression *expression);

void ast_vardef_cleanup(VarDefinition *vardef);

/* Definition AST Node */
typedef enum {VARIABLEDEFINITION} DefinitionType;

typedef struct Definition {

    DefinitionType definitionType;

    union {
        VarDefinition *varDefinition;
    };

} Definition;

Definition *ast_defitinion_create();

void ast_definition_cleanup(Definition *definition);

Definition *ast_variable_definition(VarDefinition *variableDefinition);

/* Form AST Node */
typedef enum {
  DEFINITIONFORM,
  EXPRESSIONFORM
} FormType;

typedef struct Form {

    FormType formType;

    union {
        Definition *definition;
        Expression *expression;
    };

} Form;

Form *ast_form_create();

void ast_form_cleanup(Form *form);

Form *ast_definition_form(Definition *definition);

Form *ast_expression_form(Expression *expression);


/* Block AST Node */
typedef struct Program {

    List *forms;

} Program;

Program *ast_program_create(List *forms);

void ast_program_cleanup(Program *program);


#endif
