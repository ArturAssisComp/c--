#ifndef AUX_H
#define AUX_H

#include "globals.h"


/*Auxiliary function declarations*/

/* procedure A_print_tree prints a syntax tree to the
 * listing file using indentation to indicate subtrees
 */
void A_print_tree( G_tree_node * root);

/* procedure A_print_complete_tree prints a syntax tree with some semantic 
 * information (scope and type) to the G_listing file using indentation to indicate 
 * subtrees
 */
void A_print_complete_tree( G_tree_node * root);

/* A_print_token: prints 'token'
 * and its lexeme to the G_listing file
 */
void A_print_token( G_token_type token, const char* lexeme);

/* Function A_new_stmt_node creates a new statement
 * node for syntax tree construction
 */
G_tree_node * A_new_stmt_node(G_stmt_type stmt_type);

/* Function A_new_exp_node creates a new expression
 * node for syntax tree construction
 */
G_tree_node * A_new_exp_node(G_exp_type exp_type);

/* Function A_copy_string allocates and makes a new
 * copy of an existing string
 */
char * A_copy_string( char * s);

//Returns a new allocated string that is the contatination of prefix + infix + postfix.
char *A_append(char *prefix, char *infix, char *postfix);

//Returns an allocated string of the integer 'n'.
char *A_int_to_string(int n);

#endif
