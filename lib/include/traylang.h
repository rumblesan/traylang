#ifndef TRAYLANG_H
#define TRAYLANG_H

#include <stdlib.h>

#include "interpreter.h"

typedef struct {

    Interpreter *interpreter;

} TrayLang;

TrayLang *traylang_init();

int traylang_interpret_file(TrayLang *tray, FILE *fp);

void traylang_cleanup(TrayLang *tray);

#endif
