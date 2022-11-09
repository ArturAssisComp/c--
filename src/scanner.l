 /* Scanner for toy pascal-like language - This will be 
used as template for the scanner for the C-- */

 /*Definitions*/
%{
#include <math.h>
#include <stdbool.h>
#include "aux.h"
#include "globals.h"
#include "scanner.h"
#include "parser.tab.h"

char S_token_string[S_MAX_TOKEN_LEN];
%}

/* It is necessary to have any rule with \n to make flex handle yylineno automatically */
%option   yylineno 

NON_ZERO_DIGIT [1-9]
DIGIT [0-9]
NUMBER ({NON_ZERO_DIGIT}{DIGIT}*)|0
ID    [A-Za-z_][A-Za-z0-9_]*
NEW_LINE \n
WHITESPACE [ \t]+

%x INSIDE_COMMENT
%%
 /*Rules*/
"/*"            {BEGIN(INSIDE_COMMENT);}
"if"            {return IF;}
"else"          {return ELSE;}
"int"           {return INT;}
"return"        {return RETURN;}
"void"          {return VOID;}
"while"         {return WHILE;}
"=="            {return EQ;}
"="             {return ASSIGN;}
"!="            {return NEQ;}
"<"             {return LT;}
"<="            {return LET;}
">"             {return GT;}
"=>"            {return GET;}
"+"             {return PLUS;}
"-"             {return MINUS;}
"*"             {return TIMES;}
"/"             {return OVER;}
"("             {return LPAREN;}
")"             {return RPAREN;}
";"             {return SEMI;}
","             {return COMMA;}
"["             {return LSQ_BRACKET;}
"]"             {return RSQ_BRACKET;}
"{"             {return LCU_BRACKET;}
"}"             {return RCU_BRACKET;}
{NUMBER}        {return NUM;}
{ID}            {return ID;}
{NEW_LINE}      {}
{WHITESPACE}    {/* Skip whitespace */}
<INSIDE_COMMENT><<EOF>> {return ENDFILE;}
<<EOF>>         {return ENDFILE;}
.               {return ERROR;}
<INSIDE_COMMENT>"*/"        {BEGIN(INITIAL);}
<INSIDE_COMMENT>{NEW_LINE}  {}
<INSIDE_COMMENT>.           {}

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
    G_lineno = yylineno;
    if (G_trace_scan) {
        fprintf(G_listing,"\t%d: ",yylineno);
        A_print_token(current_token,S_token_string);
    }
    return current_token;
}

