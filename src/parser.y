%{
#define YYPARSER /* distinguishes Yacc output from other code files */

#include "globals.h"
#include "util.h"
#include "scanner.h"
#include "parser.h"
#include "stdbool.h"

#define YYSTYPE G_tree_node *
static char * saved_name; /* for use in assignments */
static int saved_line_no;  /* ditto */
static G_tree_node * saved_tree; /* stores syntax tree for later return */
static int yylex(void);
int yyerror(char *s);

%}

%token ERROR 
%token IF ELSE RETURN VOID WHILE INT
%token ID NUM
%token ASSIGN EQ NEQ LT LET GT GET PLUS MINUS TIMES OVER
%token LPAREN RPAREN SEMI COMMA LSQ_BRACKET RSQ_BRACKET LCU_BRACKET RCU_BRACKET

%% /* Grammar for C-- */

abc : ;

%%

int yyerror(char * message)
{ 
    fprintf(listing,"Syntax error at line %d: %s\n",lineno,message);
    fprintf(listing,"Current token: ");
    A_print_token(yychar,S_token_string);
    G_error = true;
    return 0;
}

/* yylex calls S_get_token to make Yacc/Bison output
 * compatible with ealier versions of the C-- scanner
 */
static int yylex(void)
{ 
    return S_get_token(); 
}

G_tree_node * P_parse(void)
{ 
    yyparse();
    return saved_tree;
}
