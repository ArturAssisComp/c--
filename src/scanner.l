 /* Scanner for toy pascal-like language - This will be 
used as template for the scanner for the C-- */

 /*Definitions*/
%{
#include <math.h>
#include <stdbool.h>
#include "aux.h"
#include "globals.h"
#include "scanner.h"
#include "parser.h"
#include "parser.tab.h"


char S_token_string[S_MAX_TOKEN_LEN];
int G_lineno = 0;
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
"int"           { yylval.semantic_type = G_INT; return INT; }
"return"        {return RETURN;}
"void"          { yylval.semantic_type = G_VOID; return VOID; }
"while"         {return WHILE;}
"=="            { yylval.tkn_type = EQ; return EQ;}
"="             { yylval.tkn_type = ASSIGN; return ASSIGN;}
"!="            { yylval.tkn_type = NEQ; return NEQ;}
"<"             { yylval.tkn_type = LT; return LT;}
"<="            { yylval.tkn_type = LET; return LET;}
">"             { yylval.tkn_type = GT; return GT;}
"=>"            { yylval.tkn_type = GET; return GET;}
"+"             { yylval.tkn_type = PLUS; return PLUS;}
"-"             { yylval.tkn_type = MINUS; return MINUS;}
"*"             { yylval.tkn_type = TIMES; return TIMES;}
"/"             { yylval.tkn_type = OVER; return OVER;}
"("             {return LPAREN;}
")"             {return RPAREN;}
";"             {return SEMI;}
","             {return COMMA;}
"["             {return LSQ_BRACKET;}
"]"             {return RSQ_BRACKET;}
"{"             {return LCU_BRACKET;}
"}"             {return RCU_BRACKET;}
{NUMBER}        { yylval.num_type.int_val = atoi(yytext); return NUM;}
{ID}            { yylval.id_type.name = A_copy_string(yytext); yylval.id_type.lineno = yylineno; return ID;}
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

