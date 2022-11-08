%{
#define YYPARSER /* distinguishes Yacc output from other code files */

#include <stdbool.h>
#include "globals.h"
#include "scanner.h"
#include "parser.h"
#include "aux.h"

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

program         :   decl_list
                    {}
                ;

decl_list        :   decl_list decl
                    {}
                |   decl 
                    {}
                ;

decl            :   var_decl
                    {} 
                |   fun_decl
                    {}
                ;

var_decl        :   spec_type ID  SEMI
                    {}
                |   spec_type ID LSQ_BRACKET NUM RSQ_BRACKET SEMI
                    {}
                ;

spec_type       :   INT 
                    {}
                |   VOID
                    {}
                ;

fun_decl        :   spec_type ID LPAREN params RPAREN compound_decl
                    {}
                ;

params          :   params_list
                    {}
                |   VOID
                    {}
                ;

params_list     :   params_list  param
                    {}
                |   param
                    {}
                ;

param           :   spec_type ID
                    {}
                |   spec_type ID LSQ_BRACKET RSQ_BRACKET
                    {}
                ;

compound_decl   :   LCU_BRACKET local_decl stmt_list RCU_BRACKET
                    {}
                ;


local_decl      :   local_decl var_decl
                    {}
                |   %empty
                    {}
                ;

stmt_list       :   stmt_list stmt
                    {}
                |   %empty
                    {}
                ;

stmt            :   exp_decl
                    {}
                |   compound_decl
                    {}
                |   select_decl
                    {}
                |   iter_decl
                    {}
                |   return_decl
                    {}
                ;


exp_decl        :   exp SEMI
                    {}
                |   SEMI
                    {}
                ;

select_decl     :   IF LPAREN exp RPAREN stmt
                    {}
                |   IF  LPAREN exp RPAREN stmt ELSE stmt
                    {}
                ;

iter_decl       :   WHILE LPAREN exp RPAREN stmt
                    {}
                ;

return_decl     :   RETURN SEMI
                    {}
                |   RETURN exp SEMI
                    {}
                ;

exp             :   var ASSIGN exp
                    {}
                |   simple_exp
                    {}
                ;

var             :   ID
                    {}
                |   ID LSQ_BRACKET exp RSQ_BRACKET
                    {}
                ;

simple_exp      :   sum_exp relacional sum_exp
                    {}
                |   sum_exp
                    {}
                ;

relacional      :   LET
                    {}
                |   LT
                    {}
                |   GT
                    {}
                |   GET
                    {}
                |   EQ
                    {}
                |   NEQ
                    {}
                ;

sum_exp         :   sum_exp sum term
                    {}
                |   term
                    {}
                ;

sum             :   PLUS
                    {}
                |   MINUS
                    {}
                ;

term            :   term mult factor
                    {}
                |   factor
                    {}
                ;

mult            :   TIMES
                    {}
                |   OVER
                    {}
                ;

factor          :   LPAREN exp RPAREN
                    {}
                |   var
                    {}
                |   activation
                    {}
                |   NUM
                    {}
                ;
                
activation      :   ID LPAREN args RPAREN
                    {}
                ;

args            :   arg_list
                    {}
                |   %empty
                    {}
                ;

arg_list        :   arg_list COMMA exp
                    {}
                |   exp
                    {}
                ;

%%

int yyerror(char * message)
{ 
    fprintf(G_listing,"Syntax error at line %d: %s\n",G_lineno,message);
    fprintf(G_listing,"Current token: ");
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
