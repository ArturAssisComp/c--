#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdbool.h>
#include "globals.h"

#define BUFFER_LIMIT 1024
#define ARGS_BUFFER_LIMIT 128

typedef enum {SYM_FUNC, SYM_VAR, SYM_ARRAY_VAR} SYM_id_type;

typedef struct
{
    char *id;
    char *name;
    char *scope;
    bool is_parameter;
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
            size_t size;
            int offset;
        }arr;
    } specific_attr;
}SYM_row_entry;

//Initiates the symbol table. It must be called before using the symbol table.
void SYM_init_symbol_table(void);

/* Procedure SYM_print_symbol_table prints a formatted
 * listing of the symbol table contents
 * to the listing file
 */
void SYM_print_symbol_table(FILE * listing);

/* Procedure SYM_add_definition_to_symbol_table inserts the row entry 'row_entry' into
 * the symbol table. If the entry already exists, it is not added and false is returned.
 * If the entry does not exist, it is added and true is returned.
 */
bool SYM_add_definition_to_symbol_table( SYM_row_entry *row_entr);

//This function returns true if there is already a function declared with the same name
//as 'name'.
bool SYM_there_is_function_name_conflict(char *name);

//This function returns true if there is already a function or variable declared with the same name
// 'name'.
bool SYM_there_is_name_conflict(char *name);

//Returns true if the input with id 'id' is in table. Otherwise, it returns
//false.
bool SYM_is_in_table(char *id);
#endif
