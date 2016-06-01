#ifndef TRAYLANG_FFI_H
#define TRAYLANG_FFI_H

#include "traylang.h"
#include "interpreter.h"
#include "object.h"

#include "bclib/bstrlib.h"
#include "bclib/list.h"

typedef Interpreter TrayLangState;

typedef Object TrayObject;

typedef c_func TrayCFunction;

TrayLang *traylang_add_function(TrayLang *tray, char *name, TrayCFunction function);

TrayObject *traylang_get_obj(TrayLangState *state);

TrayObject *traylang_new_cdata(TrayLangState *state, void *cdata);

TrayObject *traylang_new_number(TrayLangState *state, double number);

TrayObject *traylang_new_string(TrayLangState *state, bstring string);

TrayObject *traylang_new_list(TrayLangState *state, List *list);

TrayObject *traylang_new_nothing(TrayLangState *state);

#endif
