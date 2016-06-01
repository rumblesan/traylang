
#include "traylang_ffi.h"

#include "traylang.h"
#include "interpreter.h"
#include "interpreter_funcs.h"
#include "bclib/bstrlib.h"
#include "object.h"

TrayLang *traylang_add_function(TrayLang *tray, char *name, c_func function) {
    TrayObject *obj = interpreter_set_global(
        tray->interpreter,
        bfromcstr(name),
        object_c_function(tray->interpreter, function)
    );
    check(obj, "Could not add Foreign Function");
    return tray;
error:
    return NULL;
}

TrayObject *traylang_get_obj(TrayLangState *state) {
    return get_obj(state);
}

TrayObject *traylang_new_cdata(TrayLangState *state, void *cdata) {
    return object_cdata(state, cdata);
}

TrayObject *traylang_new_number(TrayLangState *state, double number) {
    return object_number(state, number);
}

TrayObject *traylang_new_string(TrayLangState *state, bstring string) {
    return object_string(state, string);
}

TrayObject *traylang_new_list(TrayLangState *state, List *list) {
    return object_list(state, list);
}

TrayObject *traylang_new_nothing(TrayLangState *state) {
    return object_nothing(state);
}

