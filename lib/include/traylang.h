#ifndef TRAYLANG_H
#define TRAYLANG_H

#include <stdlib.h>

#include "interpreter.h"

typedef struct {

    Interpreter *interpreter;

} TrayLang;

TrayLang *traylang_init();

int traylang_interpret_file(TrayLang *tray, FILE *fp);

int traylang_interpret_string(TrayLang *tray, bstring program);

void traylang_cleanup(TrayLang *tray);

#endif
