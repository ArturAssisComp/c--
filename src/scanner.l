 /* Scanner for toy pascal-like language - This will be 
used as template for the scanner for the C-- */

 /*Definitions*/
%{
#include <math.h>
%}

%option   yylineno

DIGIT [0-9]
ID    [a-z][a-z0-9]*

%%
 /*Rules*/
{DIGIT}+             {
                         printf("An integer: %s (%d)\n", yytext, atoi(yytext));
                     }

{DIGIT}+"."{DIGIT}*  {
                         printf( "A float: %s (%g)\n", yytext, atof( yytext ) );
                     }

if|then|begin|end|procedure|function {
                                        printf( "A keyword: %s\n", yytext );
                                     }

{ID}             printf( "An identifier: %s\n", yytext );

"+"|"-"|"*"|"/"  printf( "An operator: %s\n", yytext );

"{"[^{}\n]*"}"    /* eat up one-line comments */

[ \t\n]+          /* eat up whitespace */

.                printf( "Unrecognized character: %s\n", yytext );

%%
 /*User Code*/
int main( int argc, char **argv )
{
    ++argv, --argc;   /* skip over program name */

    if ( argc > 0 ) yyin = fopen( argv[0], "r" );
    else yyin = stdin;

    yylex();
}



