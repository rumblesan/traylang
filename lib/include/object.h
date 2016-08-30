#ifndef TRAYLANG_OBJECT_H
#define TRAYLANG_OBJECT_H

#include "interpreter.h"
#include "ast.h"
#include "bclib/list.h"
#include "bclib/bstrlib.h"

typedef enum {
    CFUNCTION,
    LAMBDA,
    LIST,
    NOTHING,
    NUMBER,
    BOOLEAN,
    STRING,
    CDATA
} ObjectType;

typedef struct LambdaObject {
    List *arg_names;

    List *body;
} LambdaObject;

typedef struct Object *(*c_func)(Interpreter *interpreter, int arg_count);

typedef struct Object {

    ObjectType type;

    unsigned char marked;

    union {
        c_func cfunction;
        LambdaObject *lambda;
        List *list;
        double number;
        int boolean;
        bstring string;
        void *cdata;
    };

} Object;

#endif
