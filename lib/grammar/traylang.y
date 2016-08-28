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

void yyerror(yyscan_t scanner, Block **ast, const char *str) {
    fprintf(stderr, "error: %s\n", str);
}

int parse(Block **ast, FILE *fp) {

    yyscan_t scanner;

    yylex_init(&scanner);
    yyset_in(fp, scanner);

    int parseResult = yyparse(scanner, ast);

    yylex_destroy(scanner);

    return parseResult;
}

int parse_string(Block **ast, bstring program) {

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
%parse-param {void *scanner} {Block **ast}

%union {
    Block         *blockNode;
    List          *elementsNode;
    Element       *elementNode;
    Application   *applicationNode;
    List          *listNode;
    Lambda        *lambdaNode;
    VarDefinition *varDefNode;
    Expression    *expressionNode;
    Let           *letNode;
    LetVariable   *letVariableNode;
    Number        *numberNode;
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
%token<number> NUMBER
%token<string> STRING
%token<identifier> IDENTIFIER
%type<blockNode> body
%type<elementsNode> elements
%type<elementNode> element
%type<applicationNode> application
%type<listNode> argList
%type<listNode> argNamesList
%type<listNode> letVarList
%type<letVariableNode> letVar
%type<lambdaNode> lambda
%type<varDefNode> vardefinition
%type<letNode> let
%type<expressionNode> expression
%type<variableNode> variable

%%
body: elements
    {
        Block *block = ast_block_create($1);
        *ast = block;
        $$ = block;
    }
    ;

elements: %empty
        {
            $$ = list_create();
        }
        | elements element
        {
            $$ = list_push($1, $2);
        }
        ;

element: application
       {
           $$ = ast_application_element($1);
       }
       | vardefinition
       {
           $$ = ast_vardefinition_element($1);
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

lambda: OPAREN LAMBDA OPAREN argNamesList CPAREN OPAREN body CPAREN CPAREN
       {
           $$ = ast_lambda_create($4, $7);
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
          | application
          {
              $$ = ast_application_expression($1);
          }
          | let
          {
              $$ = ast_let_expression($1);
          }
          ;

variable: IDENTIFIER
        {
            $$ = ast_variable_create($1);
        }
        ;

let: OPAREN LET OPAREN letVarList CPAREN expression CPAREN
          {
               $$ = ast_let_create($4, $6);
          }
          ;

letVarList: %empty
          {
              $$ = list_create();
          }
          | letVarList letVar
          {
              $$ = list_push($1, $2);
          }
          ;

letVar: OPAREN IDENTIFIER expression CPAREN
      {
          $$ = ast_let_variable_create($2, $3);
      }
      ;

%%
