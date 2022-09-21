#include "aux.h"
#include "globals.h"


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
    /* special symbols */
    /*...*/
    // 
    // 
