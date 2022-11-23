#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "symbol_table.h"
#include "analyze.h"
#include "globals.h"
#include "aux.h"


#define BASE_ARRAY_SZ  511
#define MAX_OCCURRENCES 2048

//Types:
typedef struct row
{
    char *id;
    char *name;
    char *scope;
    bool is_parameter;
    int definition_lineno;
    G_type semantic_type;
    SYM_id_type row_type;
    //for functions:
    int num_of_args;
    char *args_keys[ARGS_BUFFER_LIMIT];
    //for arrays:
    size_t size;
    // occurrences
    size_t num_of_occurrences;
    int lineno_occurrence[MAX_OCCURRENCES];
    // link to the next row
    struct row *next;

}row;

typedef struct
{
    row *base_array[BASE_ARRAY_SZ];
    int num_of_elements;
}symbol_table;

//Global Variables:
static symbol_table *current_table = NULL;

//Static function declarations:
static void free_row(row **p_row);
static void print_row(row *r, FILE *listing);
static void free_table(void);
static char *get_occurrences_list(int *occurrences, size_t sz);
static char *semantic_type_to_string(G_type t);
static int calculate_hash(char *r);
static bool add_row_to_symbol_table(row *target_row);
static row *create_row(SYM_row_entry *row_entry);
static row *get_row(char *id);


//Function definitions:
void SYM_init_symbol_table()
{
    int i;
    if (current_table) free_table();
    current_table = NULL;
    current_table = malloc(sizeof *current_table);
    if(!current_table)
    {
        fprintf(G_listing, "Out of memory while creating the symbol table");
        exit(EXIT_FAILURE);
    }
    for (i = 0; i < BASE_ARRAY_SZ; i++) current_table->base_array[i] = NULL;

    current_table->num_of_elements = 0;
}

void SYM_add_occurrence_to_id(char *id, int lineno) 
{
    row *r = get_row(id);
    if (r)
    {
        if(r->num_of_occurrences >= MAX_OCCURRENCES)
        {
            fprintf(G_listing, "Occurrences of the id \"%s\" exceded the limit of %d occurrences.", id, MAX_OCCURRENCES);
            exit(EXIT_FAILURE);
        }
        r->lineno_occurrence[r->num_of_occurrences++] = lineno;
    }
}

void SYM_print_symbol_table(FILE * listing)
{
    int i;
    int remaining_elements = current_table->num_of_elements;
    row *aux;
    //calculate the max padding for each column:
    fprintf(listing, "|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|\n");
    print_row(NULL, listing); //print the header
    fprintf(listing, "|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|\n");
    for(i = 0; i < BASE_ARRAY_SZ && remaining_elements > 0; i++)
    {
        aux = current_table->base_array[i];
        while(aux)
        {
            print_row(aux, listing);
            remaining_elements--;
            aux = aux->next;
        }
    }
    fprintf(listing, "|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|\n");
}

bool SYM_add_definition_to_symbol_table( SYM_row_entry *row_entry)
{
    row *new_row = create_row(row_entry);
    if (!new_row)
    {
        fprintf(G_listing, "Out of memory while creating a row for the symbol table");
        exit(EXIT_FAILURE);
    }
    return add_row_to_symbol_table(new_row);
}

bool SYM_there_is_function_name_conflict(char *name)
{
    char *func_id = SYM_get_function_declaration_id(name);
    if (func_id)
    {
        free(func_id);
        return true;
    }
    return false;
}

bool SYM_there_is_name_conflict(char *name)
{
    int i;
    int remaining_elements = current_table->num_of_elements;
    row *aux;
    for(i = 0; i < BASE_ARRAY_SZ && remaining_elements > 0; i++)
    {
        aux = current_table->base_array[i];
        while(aux)
        {
            if(strcmp(name, aux->name) == 0) return true;
            remaining_elements--;
            aux = aux->next;
        }
    }
    return false;
}

bool SYM_is_in_table(char *id)
{
    if(get_row(id)) return true;
    return false;
}

char *SYM_get_function_declaration_id(char *name)
{
    int i;
    int remaining_elements = current_table->num_of_elements;
    row *aux;
    for(i = 0; i < BASE_ARRAY_SZ && remaining_elements > 0; i++)
    {
        aux = current_table->base_array[i];
        while(aux)
        {
            if(aux->row_type == SYM_FUNC && strcmp(name, aux->name) == 0) return A_copy_string(aux->id);
            remaining_elements--;
            aux = aux->next;
        }
    }
    return NULL;
}

char *SYM_get_declaration_id(char *name, char *scope, SYM_id_type type)
{
    char *current_scope, *current_id, *tmp;
    row *current_row;
    current_scope = A_copy_string(scope);
    while(strcmp(current_scope, ""))
    {
        current_id = ANA_create_key(name, current_scope);
        current_row = get_row(current_id);
        if(current_row) 
        {
            if(type == SYM_ANY || current_row->row_type == type)
            {
                free(current_scope);
                return current_id;
            }
        }
        tmp = ANA_remove_scope_layer(current_scope);
        free(current_id);
        free(current_scope);
        current_scope = tmp;
    }
    return NULL;
}

G_type SYM_get_semantic_type(char *id)
{
    row *r = get_row(id);
    if (!r)
    {
        fprintf(G_listing, "(SYM_get_semantic_type)ID not found.");
        exit(EXIT_FAILURE);
    }
    return r->semantic_type;
}

//Static function definitions:
static void free_row(row **p_row)
{
    int i;
    row *aux_row = *p_row;
    *p_row = NULL;
    free(aux_row->id);
    free(aux_row->name);
    free(aux_row->scope); 
    switch(aux_row->row_type)
    {
        case SYM_FUNC:
            for(i = 0; i < aux_row->num_of_args; i++) free(aux_row->args_keys[i]);
            break;
        case SYM_ARRAY_VAR:
        case SYM_VAR:
            break;
        default:
            fprintf(G_listing, "Invalid row type");
            exit(EXIT_FAILURE);
            break;
    }
}


static void print_row(row *r, FILE *listing)
{
    char *occurrences_list;
    if (r)
    {
        occurrences_list = get_occurrences_list(r->lineno_occurrence, r->num_of_occurrences);
        switch(r->row_type)
        {
            case SYM_FUNC:
                fprintf(listing, "|%10s|%30s|%13s|%20d|%5s|%10s|%12d|%13s|%18d|%50s|\n",r->name,r->scope, r->is_parameter?"true":"false",r->definition_lineno,semantic_type_to_string(r->semantic_type),"Function",r->num_of_args,"----",r->num_of_occurrences, occurrences_list);
                break;
            case SYM_VAR:
                fprintf(listing, "|%10s|%30s|%13s|%20d|%5s|%10s|%12s|%13s|%18d|%50s|\n",r->name,r->scope, r->is_parameter?"true":"false", r->definition_lineno,semantic_type_to_string(r->semantic_type),"Variable","----","----",r->num_of_occurrences, occurrences_list);
                break;
            case SYM_ARRAY_VAR:
                fprintf(listing, "|%10s|%30s|%13s|%20d|%5s|%10s|%12s|%13d|%18d|%50s|\n",r->name,r->scope, r->is_parameter?"true":"false", r->definition_lineno,semantic_type_to_string(r->semantic_type),"Array Var","----",r->size,r->num_of_occurrences, occurrences_list);
                break;
            defaul:
                break;
        }
    }
    else
    {
        //print the header:
        fprintf(listing, "|%10s|%30s|%13s|%20s|%5s|%10s|%12s|%13s|%18s|%50s|\n","Name","Scope", "Is Parameter","Definition lineno","Type","Row Type","Num of args","Size of Array","Num of Occurrences","Occurrences");
    }
}

static void free_table()
{
    //TODO: implement
    free(current_table);
}

static char *get_occurrences_list(int *occurrences, size_t sz)
{
    char *result, *tmp, *num;
    int i;
    result = A_int_to_string(occurrences[0]);
    for (i = 1; i < sz; i++)
    {
        num = A_int_to_string(occurrences[i]);
        tmp = A_append(result, ", ", num);
        free(num);
        free(result);
        result = tmp;
    }
    return result;
}

static char *semantic_type_to_string(G_type t)
{
    switch(t)
    {
        case G_INT:
            return "int";
            break;
        case G_VOID:
            return "void";
            break;
        default:
            break;
    }
}

static int calculate_hash(char *r)
{
    int result = 0;
    int limit = 257;
    int i;
    for (i = 0; i < strlen(r); i++) result += r[i] * (i+1) % limit;
    return result % BASE_ARRAY_SZ;
}

static bool add_row_to_symbol_table(row *target_row)
{
    int hash = calculate_hash(target_row->id);
    row *aux = current_table->base_array[hash];

    //Check for conflicts during decaration:
    if(SYM_is_in_table(target_row->id)) return false;


    if (!aux) 
    {
        current_table->base_array[hash] = target_row;
        current_table->num_of_elements++;
        return true;
    }
    while(aux->next) aux = aux->next;
    aux->next = target_row;
    current_table->num_of_elements++;
    return true;
}

static row *create_row(SYM_row_entry *row_entry)
{
    int i;
    row *new_row = malloc(sizeof *new_row);
    if (!new_row)
    {
        fprintf(G_listing, "Out of memory while creating the row for the symbol table");
        exit(EXIT_FAILURE);
    }
    new_row->id    = A_copy_string(row_entry->id);
    new_row->name  = A_copy_string(row_entry->name);
    new_row->scope = A_copy_string(row_entry->scope);
    new_row->is_parameter       = row_entry->is_parameter;
    new_row->definition_lineno  = row_entry->lineno;
    new_row->semantic_type      = row_entry->semantic_type;
    new_row->row_type           = row_entry->id_type;
    new_row->num_of_occurrences = 1;
    new_row->lineno_occurrence[0] = row_entry->lineno;
    new_row->next         = NULL;
    new_row->num_of_args  = 0;
    new_row->args_keys[0] = NULL;
    new_row->size         = 0;

    switch(row_entry->id_type)
    {
        case SYM_FUNC:
            new_row->num_of_args = row_entry->specific_attr.func.num_of_args;
            for(i = 0; i < new_row->num_of_args; i++)
            {
                new_row->args_keys[i] = A_copy_string(row_entry->specific_attr.func.args_keys[i]);
            }
            new_row->args_keys[i] = NULL;
            break;
        case SYM_ARRAY_VAR:
            new_row->size = row_entry->specific_attr.arr.size;
            break;
        case SYM_VAR:
            break;
        default:
            fprintf(G_listing, "Invalid row type");
            exit(EXIT_FAILURE);
            break;

    }
    return new_row;
}

static row *get_row(char *id)
{
    int hash = calculate_hash(id);
    row *aux = current_table->base_array[hash];

    while(aux)
    {
        if (strcmp(id, aux->id) == 0) return aux;
        aux = aux->next;
    }
    return NULL;
}
