
#include "traylang.h"
#include "interpreter.h"
#include "interpreter_funcs.h"
#include "bclib/bstrlib.h"
#include "object.h"

TrayLang *traylang_add_function(TrayLang *tray, char *name, c_func function) {
    Object *obj = interpreter_set_global(
        tray->interpreter,
        bfromcstr(name),
        object_c_function(tray->interpreter, function)
    );
    check(obj, "Could not add Foreign Function");
    return tray;
error:
    return NULL;
}

Object *traylang_get_obj(Interpreter *interpreter) {
    return get_obj(interpreter);
}

