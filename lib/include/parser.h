#ifndef __tray_lang_parser_h__
#define __tray_lang_parser_h__

#include <stdio.h>

#include "ast.h"
#include "bclib/bstrlib.h"

int parse(Program **program, FILE *fp);

int parse_string(Program **program, bstring text);

#endif
