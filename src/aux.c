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
            fprintf(G_listing,"EOF\n"); 
            break;
        case ERROR:
            fprintf(G_listing,"ERROR: %s\n",lexeme);
            break;
        case ID:
            fprintf(G_listing,"ID, name= %s\n",lexeme);
            break;
        case NUM:
            fprintf(G_listing,"NUM, val= %s\n",lexeme);
            break;
        default: /* should never happen */
            fprintf(G_listing,"Unknown token: %d\n",token);
            break;
    }
}
