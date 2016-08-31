#include "minunit.h"

#include "ast.h"

static char * test_number() {
  double value = 3.0;
  Number *number = ast_number_create(value);
  mu_assert(number != NULL, "Failed to create Number");
  mu_assert(number->value == value, "Number had incorrect value");
  ast_number_cleanup(number);
  return 0;
}

static char * test_boolean() {
  BooleanValue value = BOOLEANTRUE;
  Boolean *boolean = ast_boolean_create(value);
  mu_assert(boolean != NULL, "Failed to create Boolean");
  mu_assert(boolean->value == BOOLEANTRUE, "Boolean had incorrect value");
  ast_boolean_cleanup(boolean);
  return 0;
}

static char * test_string() {
  bstring value = bfromcstr("string value");
  String *string = ast_string_create(value);
  mu_assert(string != NULL, "Failed to create String");
  mu_assert(bstricmp(string->value, value) == 0, "String had incorrect value");
  ast_string_cleanup(string);
  return 0;
}

static char * test_variable() {
  bstring name = bfromcstr("name");
  Variable *variable = ast_variable_create(name);
  mu_assert(variable != NULL, "Failed to create Variable");
  mu_assert(bstricmp(variable->name, name) == 0, "Variable had incorrect name");
  ast_variable_cleanup(variable);
  return 0;
}

static char * test_application() {
  Expression *name_expr = ast_variable_expression(ast_variable_create(bfromcstr("name")));

  List *args = list_create();
  list_push(args, ast_number_expression(ast_number_create(1)));
  list_push(args, ast_number_expression(ast_number_create(2)));

  Application *application = ast_application_create(name_expr, args);

  mu_assert(application != NULL, "Failed to create Application");
  ast_application_cleanup(application);
  return 0;
}

static char * test_lambda() {
  List *arg_names = list_create();
  list_push(arg_names, ast_string_expression(ast_string_create(bfromcstr("arg1"))));
  list_push(arg_names, ast_string_expression(ast_string_create(bfromcstr("arg2"))));

  Expression *name_expr = ast_variable_expression(ast_variable_create(bfromcstr("name")));

  List *args = list_create();
  list_push(args, ast_string_expression(ast_string_create(bfromcstr("arg1"))));
  list_push(args, ast_string_expression(ast_string_create(bfromcstr("arg2"))));

  Expression *application_expr = ast_application_expression(ast_application_create(name_expr, args));

  List *body = list_create();
  list_push(body, application_expr);

  Lambda *lambda = ast_lambda_create(arg_names, body);
  mu_assert(lambda != NULL, "Failed to create Lambda");
  ast_lambda_cleanup(lambda);
  return 0;
}

static char * test_let() {
  List *bindings = list_create();
  list_push(bindings, ast_let_binding_create((bfromcstr("binding1")), ast_number_expression(ast_number_create(1))));
  list_push(bindings, ast_let_binding_create((bfromcstr("binding2")), ast_number_expression(ast_number_create(2))));

  Expression *name_expr = ast_variable_expression(ast_variable_create(bfromcstr("name")));

  List *args = list_create();
  list_push(args, ast_string_expression(ast_string_create(bfromcstr("arg1"))));
  list_push(args, ast_string_expression(ast_string_create(bfromcstr("arg2"))));

  Expression *application_expr = ast_application_expression(ast_application_create(name_expr, args));

  List *expressions = list_create();
  list_push(expressions, application_expr);

  Let *let = ast_let_create(bindings, expressions);
  mu_assert(let != NULL, "Failed to create Let");
  ast_let_cleanup(let);
  return 0;
}

static char * test_if() {
  Expression *predicate = ast_boolean_expression(ast_boolean_create(BOOLEANTRUE));
  Expression *ifBlock = ast_number_expression(ast_number_create(4.0));
  Expression *elseBlock = ast_number_expression(ast_number_create(4.0));

  If *ifExpr = ast_if_create(predicate, ifBlock, elseBlock);
  mu_assert(ifExpr != NULL, "Failed to create If");
  ast_if_cleanup(ifExpr);
  return 0;
}

static char * test_expression() {
  Expression *number_expr = ast_number_expression(ast_number_create(3));
  mu_assert(number_expr != NULL, "Failed to create Number Expression");
  ast_expression_cleanup(number_expr);

  Expression *boolean_expr = ast_boolean_expression(ast_boolean_create(BOOLEANTRUE));
  mu_assert(boolean_expr != NULL, "Failed to create Boolean Expression");
  ast_expression_cleanup(boolean_expr);

  Expression *string_expr = ast_string_expression(ast_string_create(bfromcstr("string value")));
  mu_assert(string_expr != NULL, "Failed to create String Expression");
  ast_expression_cleanup(string_expr);

  Expression *variable_expr = ast_variable_expression(ast_variable_create(bfromcstr("name")));
  mu_assert(variable_expr != NULL, "Failed to create Variable Expression");
  ast_expression_cleanup(variable_expr);

  return 0;
}

static char * test_variable_definition() {
  Expression *value = ast_number_expression(ast_number_create(4.0));
  VarDefinition *varDefinition = ast_vardef_create(bfromcstr("varname"), value);
  mu_assert(varDefinition != NULL, "Failed to create Variable Definition");
  ast_vardef_cleanup(varDefinition);
  return 0;
}

static char * test_definition() {
  Expression *value = ast_number_expression(ast_number_create(4.0));
  VarDefinition *varDefinition = ast_vardef_create(bfromcstr("varname"), value);
  Definition *def = ast_variable_definition(varDefinition);
  mu_assert(def != NULL, "Failed to create Definition");
  ast_definition_cleanup(def);

  return 0;
}


char *all_tests() {
    mu_suite_start();

    mu_run_test(test_number);
    mu_run_test(test_boolean);
    mu_run_test(test_string);
    mu_run_test(test_variable);
    mu_run_test(test_expression);
    mu_run_test(test_lambda);
    mu_run_test(test_application);
    mu_run_test(test_let);
    mu_run_test(test_if);

    mu_run_test(test_variable_definition);
    mu_run_test(test_definition);

    return NULL;
}

RUN_TESTS(all_tests);
