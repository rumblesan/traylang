#ifndef __tray_lang_parser_h__
#define __tray_lang_parser_h__

#include <stdio.h>

#include "ast.h"
#include "bclib/bstrlib.h"

int parse(Block **block, FILE *fp);

int parse_string(Block **block, bstring program);

#endif
