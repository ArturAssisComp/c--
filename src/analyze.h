#ifndef ANALYZE_H
#define ANALYZE_H

#include "globals.h"

/* Function buildSymtab constructs the symbol 
 * table by preorder traversal of the syntax tree
 */
void ANA_build_symtab(G_tree_node *);

/* Procedure typeCheck performs type checking 
 * by a postorder syntax tree traversal
 */
void ANA_type_check(G_tree_node *);

#endif