#ifndef ANALYZE_H
#define ANALYZE_H

#include "globals.h"

/*It builds the symbol table, checking for scope errors, type
 * errors and semantic errors in general.
 */
void ANA_init_symbol_table(G_tree_node *);


//Returns the key created from the combination of the name 'name' and the 
//scope 'scope'.
char *ANA_create_key(char *name, char *scope);

//Returns an allocated string with the name
//extracted from id.
char *ANA_extract_name(char *id);

//Returns an allocated string with the scope 'scope' without the outter layer.
char *ANA_remove_scope_layer(char* scope);

//Set the scope for nodes of the syntax tree which is meaningful to have scope.
void ANA_set_syntax_tree_scope(G_tree_node *root);
#endif
