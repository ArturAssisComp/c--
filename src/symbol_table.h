#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdbool.h>
#include "globals.h"


/* Procedure SYM_insert_into_symbol_table inserts line number 'lineno' into
 * the symbol table entry with key 'key' inside the scope 'scope'. If the entry 
 * does not exist, it is created.
 */
void SYM_insert_into_symbol_table( char * name, char * scope, int lineno);

/* Function SYM_lookup returns true if the entry with name 'name' is into the table
 * in the scope 'scope' or its supersets and false otherwise. 
 */
bool SYM_lookup ( char * name, char *scope );

/* Procedure SYM_print_symbol_table prints a formatted
 * listing of the symbol table contents
 * to the listing file
 */
void SYM_print_symbol_table(FILE * listing);



#endif
