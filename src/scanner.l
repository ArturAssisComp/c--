 /* Scanner for toy pascal-like language - This will be 
used as template for the scanner for the C-- */

 /*Definitions*/
%{
#include <math.h>
#include <stdbool.h>
#include "aux.h"
#include "globals.h"
#include "scanner.h"

char S_token_string[S_MAX_TOKEN_LEN];
%}

 /* It is necessary to have any rule with \n to make flex handle yylineno automatically */
%option   yylineno 

NON_ZERO_DIGIT [1-9]
DIGIT [0-9]
NUMBER {NON_ZERO_DIGIT}{DIGIT}*
ID    [A-Za-z_][A-Za-z0-9_]*

%%
 /*Rules*/
{NUMBER}  {return NUM;}
{ID}      {return ID;}
[ \t\n]+    /* Eat up white space */
.         {return ERROR;}
%%
 /*User Code*/

G_token_type S_get_token(void)
{ 
    static bool first_time = true;
    G_token_type current_token;
    if (first_time)
    { 
        first_time = false;
        yyin = G_source;
        yyout = G_listing;
    }
    current_token = yylex();
    strncpy(S_token_string,yytext,S_MAX_TOKEN_LEN);
    if (G_trace_scan) {
        fprintf(G_listing,"\t%d: ",yylineno);
        A_print_token(current_token,S_token_string);
    }
    return current_token;
}

