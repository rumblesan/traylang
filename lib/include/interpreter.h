#ifndef TRAYLANG_INTERPRETER_H
#define TRAYLANG_INTERPRETER_H

#include "bclib/hashmap.h"
#include "bclib/stack.h"
#include "bclib/list.h"
#include "bclib/bstrlib.h"

#define INITIAL_GC_THRESHOLD 20

typedef struct Interpreter {

    int error;

    bstring err_message;

    Stack *call_stack;

    Stack *scopes;

    List *objects;

    int max_objects;

    Hashmap *globals;

} Interpreter;

#endif
