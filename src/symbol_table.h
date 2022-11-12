#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdbool.h>
#include "globals.h"

#define BUFFER_LIMIT 1024
#define ARGS_BUFFER_LIMIT 128

typedef enum {SYM_FUNC, SYM_VAR, SYM_ARRAY_VAR} SYM_id_type;

typedef struct
{
    char * name;
    char *scope;
    int lineno;
    G_type semantic_type;
    SYM_id_type id_type;
    union 
    {
        struct 
        {
            int num_of_args;
            char *args_keys[ARGS_BUFFER_LIMIT];
        }func;
        struct 
        {
            int offset;
        }arr;
    } specific_attr;
}SYM_row_entry;

/* Procedure SYM_insert_into_symbol_table inserts line number 'lineno' into
 * the symbol table entry with key 'key' inside the scope 'scope'. If the entry 
 * does not exist, it is created.
 */
void SYM_insert_line_occurrence_into_symbol_table( char * name, char * scope, int lineno);



/* Procedure SYM_add_definition_to_symbol_table inserts the row entry 'row_entry' into
 * the symbol table. If the entry already exists, it is not added and false is returned.
 * If the entry does not exist, it is added and true is returned.
 */
bool SYM_add_definition_to_symbol_table( SYM_row_entry row_entr);

/* Function SYM_is_into_table_within_scope returns true if the entry with name 'name' is into the table
 * within the scope 'scope' and false otherwise. 
 */
bool SYM_is_into_table_within_scope ( G_tree_node* name, char *scope );

/* Procedure SYM_print_symbol_table prints a formatted
 * listing of the symbol table contents
 * to the listing file
 */
void SYM_print_symbol_table(FILE * listing);

SYM_row_entry SYM_create_entry(G_tree_node* t, char* scope)
// SYM_row_entry e = {.name = A_copy_string(t->attr.name), 
//                                    .scope = A_copy_string(scope),
//                                    .lineno = t->lineno,
//                                    .};

#endif
