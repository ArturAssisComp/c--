%{
#define YYPARSER /* distinguishes Yacc output from other code files */

#include <stdbool.h>
#include <stdlib.h>
#include "globals.h"
#include "scanner.h"
#include "parser.h"
#include "aux.h"

#define BUFFER_SZ 150
static char * saved_name[BUFFER_SZ]; /* for use in assignments */
static int current_saved_name_index = 0;
static int saved_lineno[BUFFER_SZ];  /* ditto */
static int current_saved_lineno_index = 0;
static G_tree_node * saved_tree; /* stores syntax tree for later return */
static int yylex(void);
int yyerror(char *s);

#define DEBUG printf("debug l.%d -> %s\n", __LINE__, S_token_string);

%}

%union
{
    G_tree_node *t_node;
    G_type semantic_type;
    G_token_type tkn_type;
    G_id           id_type;
    G_num          num_type;
}
 
%nterm <t_node> arg_list exp args activation factor term sum_exp simple_exp var 
%nterm <t_node> return_decl restrict_stmt stmt iter_decl select_decl exp_decl stmt_list local_decl
%nterm <t_node> var_decl compound_decl param params_list params fun_decl decl decl_list
%nterm <tkn_type> mult sum relational
%nterm <semantic_type> spec_type
%token ERROR 
%token IF ELSE WHILE
%token <semantic_type> VOID INT
%token <id_type> ID 
%token <num_type> NUM
%token <tkn_type>  ASSIGN EQ NEQ LT LET GT GET PLUS MINUS TIMES OVER RETURN
%token LPAREN RPAREN SEMI COMMA LSQ_BRACKET RSQ_BRACKET LCU_BRACKET RCU_BRACKET


%% /* Grammar for C-- */

program         :   decl_list
                    {saved_tree = $1; }
                ;

decl_list        :   decl_list decl
                    { 
                        G_tree_node *t = $1;
                        if (t != NULL)
                        { 
                            while (t->sibling != NULL) t = t->sibling;

                            t->sibling = $2;
                            $$ = $1; 
                        }
                        else $$ = $2;
                    }
                |   decl 
                    {}
                ;

decl            :   var_decl
                    {} 
                |   fun_decl
                    {}
                ;

var_decl        :   spec_type ID  SEMI
                    {
                        $$ = A_new_stmt_node(G_VAR_DCL);
                        $$->semantic_type = $1;
                        $$->attr.name     = $2.name;   
                        $$->lineno        = $2.lineno; 
                    }
                |   spec_type ID  LSQ_BRACKET NUM  RSQ_BRACKET SEMI
                    {
                        $$ = A_new_stmt_node(G_VAR_DCL);
                        $$->semantic_type = $1;
                        $$->attr.name     = $2.name;   
                        $$->lineno        = $2.lineno; 
                        $$->array_sz = $4.int_val;
                    }
                ;

spec_type       :   INT 
                    { }
                |   VOID
                    { }
                ;

fun_decl        :   spec_type ID  LPAREN params RPAREN compound_decl
                    {
                        
                        $$ = A_new_stmt_node(G_FUNC_DCL);
                        $$->semantic_type = $1;
                        $$->attr.name     = $2.name;   
                        $$->lineno        = $2.lineno; 
                        $$->child[0]      = $4;
                        $$->child[1]      = $6;
                    }
                ;

params          :   params_list
                    {}
                |   VOID
                    { $$ = NULL; }
                ;

params_list     :   params_list COMMA  param
                    { 
                        G_tree_node *t = $1;
                        if (t != NULL)
                        { 
                            while (t->sibling != NULL) t = t->sibling;

                            t->sibling = $3;
                            $$ = $1; 
                        }
                        else $$ = $3;
                    }
                |   param
                    {}
                ;

param           :   spec_type ID
                    {
                        $$ = A_new_stmt_node(G_PARAM);
                        $$->semantic_type = $1;
                        $$->attr.name     = $2.name;   
                        $$->lineno        = $2.lineno; 
                        $$->array_sz      = -1;
                    }
                |   spec_type ID  LSQ_BRACKET RSQ_BRACKET
                    {
                        $$ = A_new_stmt_node(G_PARAM);
                        $$->semantic_type = $1;
                        $$->attr.name     = $2.name;   
                        $$->lineno        = $2.lineno; 
                        $$->array_sz      = 0;
                    }
                ;

compound_decl   :   LCU_BRACKET local_decl stmt_list RCU_BRACKET
                    {
                        G_tree_node *t = $2;
                        if (t != NULL)
                        {
                            while (t->sibling != NULL) t = t->sibling;
                            t->sibling = $3;
                            $$ = $2;
                        }
                        else $$ = $3;
                    }
                ;


local_decl      :   local_decl var_decl
                    { 
                        G_tree_node *t = $1;
                        if (t != NULL)
                        { 
                            while (t->sibling != NULL) t = t->sibling;

                            t->sibling = $2;
                            $$ = $1; 
                        }
                        else $$ = $2;
                    }
                |   %empty
                    { $$ = NULL; }
                ;

stmt_list       :   stmt_list stmt
                    { 
                        G_tree_node *t = $1;
                        if (t != NULL)
                        { 
                            while (t->sibling != NULL) t = t->sibling;

                            t->sibling = $2;
                            $$ = $1; 
                        }
                        else $$ = $2;
                    }
                |   %empty
                    { $$ = NULL; }
                ;

stmt            :   exp_decl
                    {}
                |   compound_decl
                    {
                        $$ = A_new_stmt_node(G_BLOCK);
                        $$->child[0]     = $1;
                    }
                |   select_decl
                    {}
                |   iter_decl
                    {}
                |   return_decl
                    {}
                ;

restrict_stmt   :   exp_decl
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
                    { $$ = NULL;}
                ;

select_decl     :   IF LPAREN exp RPAREN restrict_stmt
                    {
                        $$ = A_new_stmt_node(G_IF);
                        $$->child[0]     = $3;
                        $$->child[1]     = $5;
                    }
                |   IF  LPAREN exp RPAREN restrict_stmt ELSE restrict_stmt
                    {
                        $$ = A_new_stmt_node(G_IF);
                        $$->child[0]     = $3;
                        $$->child[1]     = $5;
                        $$->child[2]     = $7;
                    }
                ;

iter_decl       :   WHILE LPAREN exp RPAREN restrict_stmt
                    {
                        $$ = A_new_stmt_node(G_WHILE);
                        $$->child[0]     = $3;
                        $$->child[1]     = $5;
                    }
                ;

return_decl     :   RETURN SEMI
                    {
                        $$ = A_new_stmt_node(G_RETURN);
                        $$->attr.op  = RETURN;
                    }
                |   RETURN exp SEMI
                    {
                        $$ = A_new_stmt_node(G_RETURN);
                        $$->child[0] = $2;
                        $$->attr.op  = RETURN;
                    }
                ;

exp             :   var ASSIGN exp
                    {
                        $$ = A_new_stmt_node(G_ASSIGNMENT);
                        $$->child[0] = $1;
                        $$->child[1] = $3;
                        $$->attr.op  = $2;
                    }
                |   simple_exp
                    {}
                ;

var             :   ID
                    {
                        $$ = A_new_exp_node(G_ID);
                        $$->attr.name     = $1.name;   
                        $$->lineno        = $1.lineno; 
                    }
                |   ID  LSQ_BRACKET exp RSQ_BRACKET
                    {
                        $$ = A_new_exp_node(G_ARRAY_ID);
                        $$->attr.name     = $1.name;   
                        $$->lineno        = $1.lineno; 
                        $$->child[0] = $3;
                    }
                ;

simple_exp      :   sum_exp relational sum_exp
                    {
                        $$ = A_new_exp_node(G_COMP);
                        $$->child[0] = $1;
                        $$->child[1] = $3;
                        $$->attr.op  = $2;
                    }
                |   sum_exp
                    {}
                ;

relational      :   LET
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
                    {
                        $$ = A_new_exp_node(G_OP);
                        $$->child[0] = $1;
                        $$->child[1] = $3;
                        $$->attr.op  =  $2;
                    }
                |   term
                    {}
                ;

sum             :   PLUS
                    { }
                |   MINUS
                    { }
                ;

term            :   term mult factor
                    {
                        $$ = A_new_exp_node(G_OP);
                        $$->child[0] = $1;
                        $$->child[1] = $3;
                        $$->attr.op  = $2;
                    }
                |   factor
                    {}
                ;

mult            :   TIMES
                    {}
                |   OVER
                    {}
                ;

factor          :   LPAREN exp RPAREN
                    { $$ = $2; }
                |   var
                    {}
                |   activation
                    {}
                |   NUM
                    { 
                        $$ = A_new_exp_node(G_CONST);
                        $$->attr.val = $1.int_val;
                    }
                ;
                
activation      :   ID  LPAREN args RPAREN
                    {
                        $$ = A_new_exp_node(G_FUNC_ACTV);
                        $$->attr.name     = $1.name;   
                        $$->lineno        = $1.lineno; 
                        $$->child[0] = $3;
                    }
                ;

args            :   arg_list
                    {}
                |   %empty
                    { $$ = NULL; }
                ;

arg_list        :   arg_list COMMA exp
                    { 
                        G_tree_node * t = $1;
                        while (t->sibling != NULL) t = t->sibling;

                        t->sibling = $3;
                        $$ = $1; 
                    }
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
