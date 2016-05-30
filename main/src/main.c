#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "dbg.h"

#include "parser.h"
#include "interpreter.h"
#include "interpreter_funcs.h"

#include "stdlib/print.h"
#include "stdlib/math.h"
#include "stdlib/list.h"

int main(int argc, char *argv[]) {

    char *input_file = NULL;
    int debug_mode = 0;
    int c;

    while ((c = getopt(argc, argv, "di:")) != -1) {
        switch(c) {
            case 'i':
                input_file = optarg;
                break;
            case 'd':
                debug_mode = 1;
                break;
        }
    }

    check(input_file, "Need to supply input file path");

    FILE *fp = fopen(input_file, "r");
    check(fp, "Could not open file %s", input_file);

    Block *ast;

    int parseResult = parse(&ast, fp);

    fclose(fp);

    check(parseResult == 0, "Error during parsing");

    printf("Parsed input file: %s\n", input_file);

    Interpreter *interpreter = interpreter_create();
    check(interpreter, "Could not create interpreter");

    interpreter_set_global(interpreter, bfromcstr("print"), object_c_function(interpreter, print));
    interpreter_set_global(interpreter, bfromcstr("add"), object_c_function(interpreter, add));
    interpreter_set_global(interpreter, bfromcstr("sub"), object_c_function(interpreter, sub));

    interpreter_set_global(interpreter, bfromcstr("list"), object_c_function(interpreter, list));
    interpreter_set_global(interpreter, bfromcstr("append"), object_c_function(interpreter, append));

    interpreter_set_debug(interpreter, debug_mode);

    interpret(interpreter, ast);

    check(
        interpreter->error == 0,
        "Error whilst interpreting: %s", interpreter->err_message->data
    );


    interpreter_destroy(interpreter);
    ast_block_cleanup(ast);

    return 0;
error:
    return 1;
}

