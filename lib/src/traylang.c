#include <stdlib.h>

#include "dbg.h"

#include "traylang.h"

#include "interpreter.h"
#include "interpreter_funcs.h"
#include "interpreter_interpret.h"
#include "object_funcs.h"
#include "parser.h"
#include "ast.h"

#include "stdlib/print.h"
#include "stdlib/math.h"
#include "stdlib/list.h"

TrayLang *traylang_stdlib(TrayLang *tray) {

    interpreter_set_global(
        tray->interpreter,
        bfromcstr("print"), object_c_function(tray->interpreter, print));
    interpreter_set_global(
        tray->interpreter,
        bfromcstr("add"), object_c_function(tray->interpreter, add));
    interpreter_set_global(
        tray->interpreter,
        bfromcstr("sub"), object_c_function(tray->interpreter, sub));

    interpreter_set_global(
        tray->interpreter,
        bfromcstr("list"), object_c_function(tray->interpreter, list));
    interpreter_set_global(
        tray->interpreter,
        bfromcstr("append"), object_c_function(tray->interpreter, append));

    return tray;
}

TrayLang *traylang_init() {
    Interpreter *interpreter = NULL;

    TrayLang *tray = malloc(sizeof(TrayLang));
    check_mem(tray);

    interpreter = interpreter_create();
    check(interpreter, "Could not create interpreter");

    tray->interpreter = interpreter;

    traylang_stdlib(tray);

    return tray;
error:
    if (interpreter) {
        interpreter_destroy(interpreter);
    }
    if (tray) {
        free(tray);
    }
    return NULL;
}

int traylang_interpret_file(TrayLang *tray, FILE *fp) {

    Program *ast;

    int parseResult = parse(&ast, fp);

    check(parseResult == 0, "Error during parsing");

    interpret(tray->interpreter, ast);

    ast_program_cleanup(ast);

    check(
        tray->interpreter->error == 0,
        "Error whilst interpreting: %s", tray->interpreter->err_message->data
    );

    return 0;
error:
    return 1;
}

int traylang_interpret_string(TrayLang *tray, bstring text) {

    Program *ast;

    int parseResult = parse_string(&ast, text);

    check(parseResult == 0, "Error during parsing");

    interpret(tray->interpreter, ast);

    ast_program_cleanup(ast);

    check(
        tray->interpreter->error == 0,
        "Error whilst interpreting: %s", tray->interpreter->err_message->data
    );

    return 0;
error:
    return 1;
}

void traylang_cleanup(TrayLang *tray) {
    interpreter_destroy(tray->interpreter);
    free(tray);
}


