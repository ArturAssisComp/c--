/****************************************************/
/* File: globals.h                                  */
/* Global types and vars for C-- compiler           */
/****************************************************/

#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdbool.h>
#include <stdio.h>

#ifndef YYPARSER

/* ENDFILE is implicitly defined by Yacc/Bison,
 * and not included in the tab.h file
 */
#define ENDFILE 0

/*Define the maximum number of children that an
 * abstract syntax tree may have.
 */

#endif

#define MAXCHILDREN 3
/*Types definition*/

typedef int G_token_type;

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

/* trace_parse = true causes parsing information to be
 * printed to the listing file. 
 */
extern bool G_trace_parse;

/* trace_analyze = true causes semantic analysis information to be
 * printed to the listing file. 
 */
extern bool G_trace_analyze;

/* Error = true prevents further passes if an error occurs */
extern bool G_error;




//Syntax tree:
typedef struct {                                                                                                                              
    char *name;
    int lineno;
}G_id;
typedef union {
    int int_val;
}G_num; 
typedef enum {G_STMT,G_EXP} G_node_type;
typedef enum {G_VAR_DCL, G_FUNC_DCL, G_BLOCK, G_IF, G_WHILE, G_RETURN, G_ASSIGNMENT, G_PARAM} G_stmt_type;
typedef enum {G_COMP, G_OP, G_FUNC_ACTV, G_CONST, G_ID, G_ARRAY_ID} G_exp_type;

/* ExpType is used for type checking */
typedef enum {G_VOID, G_INT} G_type;


typedef struct G_tree_node
   { 
       struct G_tree_node * child[MAXCHILDREN];
       struct G_tree_node * sibling;
       int lineno;
       G_node_type node_type;
       union { G_stmt_type stmt; G_exp_type exp;} node_subtype;
       union { G_token_type op;
               int val;
               char * name; } attr;
       G_type semantic_type; /* for type checking of exps */
       int array_sz; // -1 is default, 0 if [] and >0 otherwise.
       char *scope; //This member will be used during semantic analysis
   } G_tree_node;

#endif

