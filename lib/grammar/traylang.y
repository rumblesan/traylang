%code requires {
#include "ast.h"
#include "bclib/list.h"
}

%{

#include <stdio.h>
#include <string.h>

#include "parser.h"
#include "y.tab.h"
#include "lex.yy.h"

#include "bclib/bstrlib.h"

void yyerror(yyscan_t scanner, Program **ast, const char *str) {
    fprintf(stderr, "error: %s\n", str);
}

int parse(Program **ast, FILE *fp) {

    yyscan_t scanner;

    yylex_init(&scanner);
    yyset_in(fp, scanner);

    int parseResult = yyparse(scanner, ast);

    yylex_destroy(scanner);

    return parseResult;
}

int parse_string(Program **ast, bstring program) {

    yyscan_t scanner;

    yylex_init(&scanner);
    yy_scan_string(bdata(program), scanner);

    int parseResult = yyparse(scanner, ast);

    yylex_destroy(scanner);

    return parseResult;
}

%}

%define parse.error verbose
%define api.pure full
%lex-param {void *scanner}
%parse-param {void *scanner} {Program **ast}

%union {
    Program       *programNode;
    List          *formsNode;
    Form          *formNode;
    Definition    *definitionNode;
    VarDefinition *varDefNode;
    Expression    *expressionNode;
    Application   *applicationNode;
    List          *listNode;
    Lambda        *lambdaNode;
    Let           *letNode;
    LetBinding    *letBindingNode;
    Number        *numberNode;
    Boolean       *booleanNode;
    Variable      *variableNode;
    Identifier    *identifier;
    bstring       string;
    double        number;
}

%token DEFINE
%token LAMBDA
%token LET
%token OPAREN
%token CPAREN
%token DQUOTE
%token SQUOTE
%token HASH
%token<number> NUMBER
%token<string> STRING
%token<identifier> IDENTIFIER
%type<programNode> program
%type<booleanNode> boolean
%type<formNode> form
%type<listNode> forms
%type<definitionNode> definition
%type<varDefNode> vardefinition
%type<expressionNode> expression
%type<listNode> expressionList
%type<applicationNode> application
%type<listNode> argList
%type<listNode> argNamesList
%type<listNode> letBindingList
%type<letBindingNode> letBinding
%type<lambdaNode> lambda
%type<letNode> let
%type<variableNode> variable

%%
program: forms
    {
        Program *program = ast_program_create($1);
        *ast = program;
        $$ = program;
    }
    ;

forms: %empty
     {
         $$ = list_create();
     }
     | forms form
     {
         $$ = list_push($1, $2);
     }
     ;

form: definition
    {
        $$ = ast_definition_form($1);
    }
    | expression
    {
        $$ = ast_expression_form($1);
    }
    ;

definition: vardefinition
          {
              $$ = ast_variable_definition($1);
          }
          ;

vardefinition: OPAREN DEFINE IDENTIFIER expression CPAREN
          {
               $$ = ast_vardef_create($3, $4);
          }
          ;


expression: variable
          {
              $$ = ast_variable_expression($1);
          }
          | lambda
          {
              $$ = ast_lambda_expression($1);
          }
          | STRING
          {
              bstring noquotes = bmidstr($1, 1, blength($1) - 2);
              $$ = ast_string_expression(
                  ast_string_create(noquotes)
              );
          }
          | NUMBER
          {
              $$ = ast_number_expression(
                  ast_number_create($1)
              );
          }
          | boolean
          {
              $$ = ast_boolean_expression($1);
          }
          | application
          {
              $$ = ast_application_expression($1);
          }
          | let
          {
              $$ = ast_let_expression($1);
          }
          ;

expressionList: %empty
              {
                  $$ = list_create();
              }
              | expressionList expression
              {
                  $$ = list_push($1, $2);
              }
              ;

application: OPAREN expression argList CPAREN
           {
               $$ = ast_application_create($2, $3);
           }
           ;

argList: %empty
       {
           $$ = list_create();
       }
       | argList expression
       {
           $$ = list_push($1, $2);
       }
       ;

argNamesList: %empty
       {
           $$ = list_create();
       }
       | argNamesList IDENTIFIER
       {
           $$ = list_push($1, $2);
       }
       ;

lambda: OPAREN LAMBDA OPAREN argNamesList CPAREN OPAREN forms CPAREN CPAREN
       {
           $$ = ast_lambda_create($4, $7);
       }
       ;

variable: IDENTIFIER
        {
            $$ = ast_variable_create($1);
        }
        ;

boolean: HASH "t"
       {
           $$ = ast_boolean_create(BOOLEANTRUE);
       }
       | HASH "f"
       {
           $$ = ast_boolean_create(BOOLEANFALSE);
       }
       ;

let: OPAREN LET OPAREN letBindingList CPAREN expressionList CPAREN
          {
               $$ = ast_let_create($4, $6);
          }
          ;

letBindingList: %empty
              {
                  $$ = list_create();
              }
              | letBindingList letBinding
              {
                  $$ = list_push($1, $2);
              }
              ;

letBinding: OPAREN IDENTIFIER expression CPAREN
          {
              $$ = ast_let_binding_create($2, $3);
          }
          ;

%%
