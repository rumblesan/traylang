#ifndef TRAYLANG_FFI_H
#define TRAYLANG_FFI_H

#include "traylang.h"
#include "interpreter.h"
#include "bclib/bstrlib.h"
#include "bclib/list.h"
#include "object.h"

TrayLang *traylang_add_function(TrayLang *tray, char *name, c_func function);

Object *get_obj(Interpreter *interpreter);

#endif
