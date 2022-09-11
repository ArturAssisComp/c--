/****************************************************/
/* File: globals.h                                  */
/* Global types and vars for C-- compiler           */
/****************************************************/

#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdbool.h>
#include <stdio.h>

/*Types definition*/
typedef enum
/* book-keeping tokens */
{
    ENDFILE,ERROR,
    /* reserved words */
    IF, ELSE, RETURN, VOID, WHILE, INT,
    /*...*/
    /* multicharacter tokens */
    ID,NUM,
    /* special symbols */
    /*...*/
    ASSIGN, EQ, NEQ, LT, LET, GT, GET, PLUS, MINUS, TIMES, OVER, 
    LPAREN, RPAREN, SEMI, COMMA, LSQ_BRACKET, RSQ_BRACKET, LCU_BRACKET, RCU_BRACKET
} G_token_type;


/*Global Variables*/
extern FILE* G_source;  /* source code text file */
extern FILE* G_listing; /* listing output text file */
extern FILE* G_code;    /* code text file */

extern int G_lineno;    /* source line number for listing */

/** Tracing flags **/

/* echo_source = true causes the source program to
 * be echoed to the listing file with line numbers
 * during parsing
 */
extern bool G_echo_source;

/* trace_scan = true causes token information to be
 * printed to the listing file as each token is
 * recognized by the scanner
 */
extern bool G_trace_scan;


/* Error = true prevents further passes if an error occurs */
extern bool G_error;


#endif

