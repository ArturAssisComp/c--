#include <stdlib.h>
#include "aux.h"
#include "globals.h"
#include "parser.tab.h"


/* A_print_token: prints 'token'
 * and its lexeme to the G_listing file
 */
void A_print_token( G_token_type token, const char* lexeme)
{ 
    switch (token)
    { 
        case ENDFILE: 
            fprintf(G_listing,"ENDFILE\n"); 
            break;
        case ERROR:
            fprintf(G_listing,"ERROR: %s\n",lexeme);
            break;
        case IF:
        case ELSE:
        case RETURN:
        case VOID:
        case WHILE:
        case INT:
            fprintf(G_listing,"reserved word: %s\n",lexeme);
            break;
        case ID:
            fprintf(G_listing,"ID, name= %s\n",lexeme);
            break;
        case NUM:
            fprintf(G_listing,"NUM, val= %s\n",lexeme);
            break;
        case ASSIGN:
        case EQ:
        case NEQ:
        case LT:
        case LET:
        case GT:
        case GET:
        case PLUS:
        case MINUS: 
        case TIMES:
        case OVER:
        case LPAREN:
        case RPAREN:
        case SEMI:
        case COMMA:
        case LSQ_BRACKET:
        case RSQ_BRACKET:
        case LCU_BRACKET:
        case RCU_BRACKET:
            fprintf(G_listing,"special symbol: %s\n",lexeme);
            break;
        default: /* should never happen */
            fprintf(G_listing,"Unknown token: %d\n",token);
            break;
    }
}

G_tree_node * A_new_stmt_node(G_stmt_type stmt_type)
{
    int i;
    G_tree_node *t = malloc(sizeof *t);
    if (t==NULL)
    {
        fprintf(G_listing,"Out of memory error at line %d\n",G_lineno);
        exit(EXIT_FAILURE);
    }

    for (i=0; i<MAXCHILDREN; i++) t->child[i] = NULL;

    t->sibling = NULL;
    t->node_type = G_STMT;
    t->node_subtype.stmt = stmt_type;
    t->lineno = G_lineno;
    return t;
}

G_tree_node * A_new_exp_node(G_exp_type exp_type)
{
    int i;
    G_tree_node *t = malloc(sizeof *t);
    if (t==NULL)
    {
        fprintf(G_listing,"Out of memory error at line %d\n",G_lineno);
        exit(EXIT_FAILURE);
    }

    for (i=0; i<MAXCHILDREN; i++) t->child[i] = NULL;

    t->sibling = NULL;
    t->node_type = G_EXP;
    t->node_subtype.exp= exp_type;
    t->semantic_type = G_VOID;
    t->lineno = G_lineno;
    return t;
}


