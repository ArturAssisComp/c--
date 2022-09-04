#ifndef SCANNER_H
#define SCANNER_H

#define S_MAX_TOKEN_LEN 250
#include "globals.h"

extern char S_token_string[S_MAX_TOKEN_LEN];
extern int yylineno;

/* Function declaration */
G_token_type S_get_token(void);

#endif 
