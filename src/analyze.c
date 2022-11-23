#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"
#include "analyze.h"
#include "aux.h"

//Global variables:
/* scope variable of studied line */
static int   scope_id = 0;

//Static function declarations:
static void free_row_entry(SYM_row_entry **ptr_r);
static void semantic_error(int lineno, char * message);
static void traverse_to_init_symbol_table(G_tree_node *t);
static void traverse_to_define_scope(G_tree_node *t, char *scope);
static void traverse_to_check_for_scope_errors(G_tree_node *t);
static void traverse_to_check_for_type_errors(G_tree_node *t);
static char *append_to_scope(char *scope, char* name); 
static int try_to_evaluate_const_expression(G_tree_node *t);
static SYM_row_entry *create_row_entry(G_tree_node *t);


//Function definitions:
void ANA_init_symbol_table(G_tree_node *root)
{ 
    SYM_init_symbol_table();
    traverse_to_init_symbol_table(root);
    traverse_to_check_for_scope_errors(root);
    traverse_to_check_for_type_errors(root);
    if(!SYM_there_is_function_name_conflict("main"))
    {
        //semantic error - main function not declared
        semantic_error(0, "Main function not declared.");
        exit(EXIT_FAILURE);
    }
    if (G_trace_analyze)
    { 
        fprintf(G_listing,"\nSymbol table:\n\n");
        SYM_print_symbol_table(G_listing);
    }
}

char *ANA_create_key(char *name, char *scope)
{
    return A_append(scope, "@", name);
}

char *ANA_extract_name(char *id)
{
    char *result;
    size_t i, id_len = strlen(id);
    for(i = id_len - 1; i >= 0; i--)
    {
        if (id[i] == '@') break;
    }
    if(i < 0)
    {
        fprintf(G_listing,"Invalid id at line %d\n",G_lineno);
        exit(EXIT_FAILURE);
    }
    size_t sz = id_len - i;
    result = calloc(sz, sizeof *result);
    if (!result)
    {
        fprintf(G_listing,"(ANA_extract_name)Out of memory error at line %d\n",G_lineno);
        exit(EXIT_FAILURE);
    }

    result[0] = '\0';
    strncpy(result, id + i + 1, sz - 1);
    result[sz] = '\0';
    return result;
}

char *ANA_remove_scope_layer(char* scope) 
{
    char *result;
    int i;
    for(i = strlen(scope) - 1; i >= 0; i--)
    {
        if (scope[i] == '$') break;
    }
    if(i == -1) i = 0; //no '$' found, equivalent to '$' in position 0.
    size_t sz = i + 1;
    result = calloc(sz, sizeof *result);
    if (!result)
    {
        fprintf(G_listing,"(ANA_remove_scope_layer)Out of memory error at line %d\n",G_lineno);
        exit(EXIT_FAILURE);
    }

    result[0] = '\0';
    strncpy(result, scope, i);
    result[i] = '\0';
    return result;
}

void ANA_set_syntax_tree_scope(G_tree_node *root)
{
    char *t = A_copy_string(GLOBAL_PREFIX);
    scope_id = 0;
    traverse_to_define_scope(root, t);
}


//Static function definitions:
static void traverse_to_define_scope(G_tree_node *t, char *scope)
{
    char *aux_scope = NULL, *tmp = NULL;
    if (t)
    {
        if (t->node_type == G_STMT)
        { 
            switch (t->node_subtype.stmt) 
            {
                case G_VAR_DCL:
                    t->scope = A_copy_string(scope);                    
                    break;
                case G_FUNC_DCL:
                    tmp = A_copy_string(scope);
                    aux_scope = append_to_scope(scope, t->attr.name);
                    free(tmp);
                    t->scope = aux_scope;                    
                    aux_scope = NULL;
                    tmp = NULL;
                    traverse_to_define_scope(t->child[0], t->scope);
                    traverse_to_define_scope(t->child[1], t->scope);
                    break;
                case G_BLOCK:
                    tmp = A_copy_string(scope);
                    aux_scope = append_to_scope(scope, "BLOCK");
                    free(tmp);
                    t->scope = aux_scope;                    
                    aux_scope = NULL;
                    tmp = NULL;
                    traverse_to_define_scope(t->child[0], t->scope);
                    break;
                case G_IF:
                    tmp = A_copy_string(scope);
                    aux_scope = append_to_scope(scope, "IF");
                    free(tmp);
                    t->scope = aux_scope;                    
                    aux_scope = NULL;
                    tmp = NULL;
                    traverse_to_define_scope(t->child[0], scope);
                    traverse_to_define_scope(t->child[1], t->scope);
                    traverse_to_define_scope(t->child[2], t->scope);
                    break;
                case G_WHILE:
                    tmp = A_copy_string(scope);
                    aux_scope = append_to_scope(scope, "WHILE");
                    free(tmp);
                    t->scope = aux_scope;                    
                    aux_scope = NULL;
                    tmp = NULL;
                    traverse_to_define_scope(t->child[0], scope);
                    traverse_to_define_scope(t->child[1], t->scope);
                    break;
                case G_RETURN:
                    t->scope = A_copy_string(scope);                    
                    traverse_to_define_scope(t->child[0], scope);
                    break;
                case G_ASSIGNMENT:
                    t->scope = A_copy_string(scope);                    
                    traverse_to_define_scope(t->child[0], scope);
                    traverse_to_define_scope(t->child[1], scope);
                    break;
                case G_PARAM:
                    t->scope = A_copy_string(scope);                    
                    break;
                default:
                    fprintf(G_listing,"Unknown ExpNode kind\n");
                    break;
            }
        }
        else if (t->node_type == G_EXP)
        { 
            switch (t->node_subtype.exp) {
                case G_COMP:
                    t->scope = A_copy_string(scope);                    
                    traverse_to_define_scope(t->child[0], scope);
                    traverse_to_define_scope(t->child[1], scope);
                    break;
                case G_OP:
                    t->scope = A_copy_string(scope);                    
                    traverse_to_define_scope(t->child[0], scope);
                    traverse_to_define_scope(t->child[1], scope);
                    break;
                case G_FUNC_ACTV:
                    t->scope = A_copy_string(scope);                    
                    traverse_to_define_scope(t->child[0], scope);
                    break;
                case G_CONST:
                    t->scope = A_copy_string(scope);                    
                    break;
                case G_ID:
                    t->scope = A_copy_string(scope);                    
                    break;
                case G_ARRAY_ID:
                    t->scope = A_copy_string(scope);                    
                    traverse_to_define_scope(t->child[0], t->scope);
                    break;
                default:
                    fprintf(G_listing,"Unknown ExpNode kind\n");
                    break;
          }
        }
        else fprintf(G_listing,"INVALID NODE\n");

        traverse_to_define_scope(t->sibling, scope);
    }
}


static void _traverse_to_check_for_scope_errors_special_for_args(G_tree_node *t)
{
    char *tmp;
    G_tree_node *aux_node;
    if (t)
    {
        if (t->node_type == G_STMT)
        { 
            switch (t->node_subtype.stmt) 
            {
                case G_ASSIGNMENT:
                    //Traverse the left and right sides of the assignment:
                    traverse_to_check_for_scope_errors(t->child[0]); 
                    traverse_to_check_for_scope_errors(t->child[1]); 
                    break;
                case G_VAR_DCL:
                case G_FUNC_DCL:
                case G_BLOCK:
                case G_IF:
                case G_WHILE:
                case G_RETURN:
                case G_PARAM:
                    break;
                default:
                    fprintf(G_listing,"Unknown ExpNode kind\n");
                    break;
            }
        }
        else if (t->node_type == G_EXP)
        { 
            switch (t->node_subtype.exp) {
                case G_COMP:
                    //Traverse the left and right sides of the comparison:
                    traverse_to_check_for_scope_errors(t->child[0]); 
                    traverse_to_check_for_scope_errors(t->child[1]); 
                    break;
                case G_OP:
                    //Traverse the left and right sides of the arithmetic operation:
                    traverse_to_check_for_scope_errors(t->child[0]); 
                    traverse_to_check_for_scope_errors(t->child[1]); 
                    break;
                case G_FUNC_ACTV:
                    //Check scope
                    tmp = SYM_get_function_declaration_id(t->attr.name);
                    if (!tmp)
                    {
                        //semantic error - function not declared
                        semantic_error(t->lineno, A_append("Function \"", t->attr.name, "\" was not declared."));
                        exit(EXIT_FAILURE);
                    }
                    //count lineno occurrence
                    t->declaration_id = tmp;
                    SYM_add_occurrence_to_id(tmp, t->lineno); 
                    tmp = NULL;
                    //Traverse the arguments:
                    aux_node = t->child[0];
                    while(aux_node)
                    {
                        _traverse_to_check_for_scope_errors_special_for_args(aux_node);
                        aux_node = aux_node->sibling;
                    }
                    break;
                case G_CONST:
                    break;
                case G_ID:
                    if (SYM_there_is_function_name_conflict(t->attr.name))
                    {
                        //semantic error - invalid call for function
                        semantic_error(t->lineno, A_append("Invalid call for function \"", t->attr.name, "\"."));
                        exit(EXIT_FAILURE);
                    }
                    //Check scope
                    tmp = SYM_get_declaration_id(t->attr.name, t->scope, SYM_ANY);
                    if (!tmp)
                    {
                        //semantic error - variable not declared
                        semantic_error(t->lineno, A_append("Variable \"", t->attr.name, "\" was not declared within the scope."));
                        exit(EXIT_FAILURE);
                    }
                    //count lineno occurrence
                    t->declaration_id = tmp;
                    SYM_add_occurrence_to_id(tmp, t->lineno); 
                    tmp = NULL;
                    break;
                case G_ARRAY_ID:
                    if (SYM_there_is_function_name_conflict(t->attr.name))
                    {
                        //semantic error - invalid call for function
                        semantic_error(t->lineno, A_append("Invalid call for function \"", t->attr.name, "\"."));
                        exit(EXIT_FAILURE);
                    }
                    //Check scope
                    tmp = SYM_get_declaration_id(t->attr.name, t->scope, SYM_ARRAY_VAR);
                    if (!tmp)
                    {
                        //semantic error - variable not declared
                        semantic_error(t->lineno, A_append("Array \"", t->attr.name, "\" was not declared within the scope."));
                        exit(EXIT_FAILURE);
                    }
                    //count lineno occurrence
                    t->declaration_id = tmp;
                    SYM_add_occurrence_to_id(tmp, t->lineno); 
                    tmp = NULL;
                    //Traverse the expression of the offset:
                    traverse_to_check_for_scope_errors(t->child[0]); 
                    break;
                default:
                    fprintf(G_listing,"Unknown ExpNode kind\n");
                    break;
          }
        }
        else fprintf(G_listing,"INVALID NODE\n");
    }
}


static void traverse_to_check_for_scope_errors(G_tree_node *t)
{
    char *tmp;
    G_tree_node *aux_node;
    if (t)
    {
        if (t->node_type == G_STMT)
        { 
            switch (t->node_subtype.stmt) 
            {
                case G_VAR_DCL:
                    break;
                case G_FUNC_DCL:
                    //Traverse the body of the function:
                    traverse_to_check_for_scope_errors(t->child[1]); 
                    break;
                case G_BLOCK:
                    //Traverse the body of the block:
                    traverse_to_check_for_scope_errors(t->child[0]); 
                    break;
                case G_IF:
                    //Traverse the condition expression and the bodies of the if/else:
                    traverse_to_check_for_scope_errors(t->child[0]); 
                    traverse_to_check_for_scope_errors(t->child[1]); 
                    traverse_to_check_for_scope_errors(t->child[2]); 
                    break;
                case G_WHILE:
                    //Traverse the condition expression and the body of the while:
                    traverse_to_check_for_scope_errors(t->child[0]); 
                    traverse_to_check_for_scope_errors(t->child[1]); 
                    break;
                case G_RETURN:
                    //Traverse the expression to be returned:
                    traverse_to_check_for_scope_errors(t->child[0]); 
                    break;
                case G_ASSIGNMENT:
                    //Traverse the left and right sides of the assignment:
                    traverse_to_check_for_scope_errors(t->child[0]); 
                    traverse_to_check_for_scope_errors(t->child[1]); 
                    break;
                case G_PARAM:
                    break;
                default:
                    fprintf(G_listing,"Unknown ExpNode kind\n");
                    break;
            }
        }
        else if (t->node_type == G_EXP)
        { 
            switch (t->node_subtype.exp) {
                case G_COMP:
                    //Traverse the left and right sides of the comparison:
                    traverse_to_check_for_scope_errors(t->child[0]); 
                    traverse_to_check_for_scope_errors(t->child[1]); 
                    break;
                case G_OP:
                    //Traverse the left and right sides of the arithmetic operation:
                    traverse_to_check_for_scope_errors(t->child[0]); 
                    traverse_to_check_for_scope_errors(t->child[1]); 
                    break;
                case G_FUNC_ACTV:
                    //Check scope
                    tmp = SYM_get_function_declaration_id(t->attr.name);
                    if (!tmp)
                    {
                        //semantic error - function not declared
                        semantic_error(t->lineno, A_append("Function \"", t->attr.name, "\" was not declared."));
                        exit(EXIT_FAILURE);
                    }
                    //count lineno occurrence
                    t->declaration_id = tmp;
                    SYM_add_occurrence_to_id(tmp, t->lineno); 
                    tmp = NULL;
                    //Traverse the arguments:
                    aux_node = t->child[0];
                    while(aux_node)
                    {
                        _traverse_to_check_for_scope_errors_special_for_args(aux_node);
                        aux_node = aux_node->sibling;
                    }
                    break;
                case G_CONST:
                    break;
                case G_ID:
                    if (SYM_there_is_function_name_conflict(t->attr.name))
                    {
                        //semantic error - invalid call for function
                        semantic_error(t->lineno, A_append("Invalid call for function \"", t->attr.name, "\"."));
                        exit(EXIT_FAILURE);
                    }
                    //Check scope
                    tmp = SYM_get_declaration_id(t->attr.name, t->scope, SYM_VAR);
                    if (!tmp)
                    {
                        //semantic error - variable not declared
                        semantic_error(t->lineno, A_append("Variable \"", t->attr.name, "\" was not declared within the scope."));
                        exit(EXIT_FAILURE);
                    }
                    //count lineno occurrence
                    t->declaration_id = tmp;
                    SYM_add_occurrence_to_id(tmp, t->lineno); 
                    tmp = NULL;
                    break;
                case G_ARRAY_ID:
                    if (SYM_there_is_function_name_conflict(t->attr.name))
                    {
                        //semantic error - invalid call for function
                        semantic_error(t->lineno, A_append("Invalid call for function \"", t->attr.name, "\"."));
                        exit(EXIT_FAILURE);
                    }
                    //Check scope
                    tmp = SYM_get_declaration_id(t->attr.name, t->scope, SYM_ARRAY_VAR);
                    if (!tmp)
                    {
                        //semantic error - variable not declared
                        semantic_error(t->lineno, A_append("Array \"", t->attr.name, "\" was not declared within the scope."));
                        exit(EXIT_FAILURE);
                    }
                    //count lineno occurrence
                    t->declaration_id = tmp;
                    SYM_add_occurrence_to_id(tmp, t->lineno); 
                    tmp = NULL;
                    //Traverse the expression of the offset:
                    traverse_to_check_for_scope_errors(t->child[0]); 
                    break;
                default:
                    fprintf(G_listing,"Unknown ExpNode kind\n");
                    break;
          }
        }
        else fprintf(G_listing,"INVALID NODE\n");

        traverse_to_check_for_scope_errors(t->sibling);
    }
}

static void traverse_to_check_for_type_errors(G_tree_node *t)
{
    G_type s_type;
    G_tree_node *aux_node;
    int i;
    int num_of_args;
    char *tmp, *parent_function_id, *param_id ;

    if (t)
    {
        if (t->node_type == G_STMT)
        { 
            switch (t->node_subtype.stmt) 
            {
                case G_VAR_DCL:
                    switch(t->semantic_type)
                    {
                        case G_INT:
                            break;
                        case G_VOID:
                            semantic_error(t->lineno, A_append("Variable \"", t->attr.name, "\" declared as void."));
                            exit(EXIT_FAILURE);
                            break;
                        default:
                            semantic_error(t->lineno, A_append("Variable \"", t->attr.name, "\" has invalid type."));
                            exit(EXIT_FAILURE);
                            break;
                    }
                    break;
                case G_FUNC_DCL:
                    traverse_to_check_for_type_errors(t->child[0]);
                    traverse_to_check_for_type_errors(t->child[1]);
                    break;
                case G_BLOCK:
                    traverse_to_check_for_type_errors(t->child[0]);
                    break;
                case G_IF:
                    traverse_to_check_for_type_errors(t->child[0]);
                    s_type = t->child[0]->semantic_type; 
                    switch(s_type)
                    {
                        case G_INT:
                            break;
                        case G_VOID:
                        default:
                            semantic_error(t->lineno, A_append("Invalid IF expression type: ", A_semantic_type_to_str(s_type), ""));
                            exit(EXIT_FAILURE);
                            break;
                    }
                    traverse_to_check_for_type_errors(t->child[1]);
                    traverse_to_check_for_type_errors(t->child[2]);
                    break;
                case G_WHILE:
                    traverse_to_check_for_type_errors(t->child[0]);
                    s_type = t->child[0]->semantic_type; 
                    switch(s_type)
                    {
                        case G_INT:
                            break;
                        case G_VOID:
                        default:
                            semantic_error(t->lineno, A_append("Invalid WHILE expression type: ", A_semantic_type_to_str(s_type), ""));
                            exit(EXIT_FAILURE);
                            break;
                    }
                    traverse_to_check_for_type_errors(t->child[1]);
                    break;
                case G_RETURN:
                    traverse_to_check_for_type_errors(t->child[0]);
                    if(t->child[0]) t->semantic_type = t->child[0]->semantic_type;
                    else t->semantic_type = G_VOID;

                    parent_function_id = SYM_get_parent_function_id(t->scope);
                    s_type = SYM_get_semantic_type(parent_function_id);
                    if(t->semantic_type != s_type)
                    {
                        semantic_error(t->lineno, A_append("Invalid return type, \"", A_semantic_type_to_str(s_type) ,"\" was expected"));
                        exit(EXIT_FAILURE);
                    }
                    free(parent_function_id);
                    break;
                case G_ASSIGNMENT:
                    traverse_to_check_for_type_errors(t->child[0]);
                    traverse_to_check_for_type_errors(t->child[1]);
                    if(t->child[0]->semantic_type != t->child[1]->semantic_type)
                    {
                        semantic_error(t->lineno, "Assignment of a type to a variable of different type .");
                        exit(EXIT_FAILURE);
                    }
                    t->semantic_type = t->child[0]->semantic_type;
                    break;
                case G_PARAM:
                    switch(t->semantic_type)
                    {
                        case G_INT:
                            break;
                        case G_VOID:
                            semantic_error(t->lineno, A_append("Parameter \"", t->attr.name, "\" declared as void."));
                            exit(EXIT_FAILURE);
                            break;
                        default:
                            semantic_error(t->lineno, A_append("Parameter \"", t->attr.name, "\" has invalid type."));
                            exit(EXIT_FAILURE);
                            break;
                    }
                    break;
                default:
                    fprintf(G_listing,"Unknown ExpNode kind\n");
                    break;
            }
        }
        else if (t->node_type == G_EXP)
        { 
            switch (t->node_subtype.exp) {
                case G_COMP:
                case G_OP:
                    traverse_to_check_for_type_errors(t->child[0]);
                    traverse_to_check_for_type_errors(t->child[1]);
                    if(t->child[0]->semantic_type != t->child[1]->semantic_type)
                    {
                        semantic_error(t->lineno, "Left and right sides have different types.");
                        exit(EXIT_FAILURE);
                    }
                    t->semantic_type = t->child[0]->semantic_type;
                    break;
                case G_FUNC_ACTV:
                    //get the type from symbol table
                    tmp = SYM_get_function_declaration_id(t->attr.name);
                    if (!tmp)
                    {
                        //semantic error - function not declared
                        semantic_error(t->lineno, A_append("Function \"", t->attr.name, "\" was not declared."));
                        exit(EXIT_FAILURE);
                    }
                    t->semantic_type = SYM_get_semantic_type(tmp);

                    traverse_to_check_for_type_errors(t->child[0]);
                    aux_node = t->child[0];
                    num_of_args = SYM_get_num_of_args(tmp);
                    i = 0;
                    while(aux_node)
                    {
                        if (i + 1 > num_of_args) 
                        {
                            semantic_error(aux_node->lineno, A_append("More arguments than expected for function ", t->attr.name, "."));
                            exit(EXIT_FAILURE);
                        }
                        param_id = SYM_get_function_parameter_id(tmp, i);

                        if(aux_node->semantic_type != SYM_get_semantic_type(param_id))
                        {
                            semantic_error(aux_node->lineno, "Argument type does not match the parameter expected type.");
                            exit(EXIT_FAILURE);
                        }
                        free(param_id);
                        aux_node = aux_node->sibling;
                        i++;
                    }
                    if (i < num_of_args) 
                    {
                        semantic_error(t->lineno, A_append("Less arguments than expected for function ", t->attr.name, "."));
                        exit(EXIT_FAILURE);
                    }
                    free(tmp);
                    break;
                case G_CONST:
                    t->semantic_type = G_INT;
                    break;
                case G_ID:
                    //get the type from symbol table
                    tmp = SYM_get_declaration_id(t->attr.name, t->scope, SYM_ANY);
                    if (!tmp)
                    {
                        //semantic error - variable not declared
                        semantic_error(t->lineno, A_append("Variable \"", t->attr.name, "\" was not declared within the scope."));
                        exit(EXIT_FAILURE);
                    }
                    t->semantic_type = SYM_get_semantic_type(tmp);
                    free(tmp);
                    break;
                case G_ARRAY_ID:
                    //get the type from symbol table
                    tmp = SYM_get_declaration_id(t->attr.name, t->scope, SYM_ARRAY_VAR);
                    if (!tmp)
                    {
                        //semantic error - variable not declared
                        semantic_error(t->lineno, A_append("Variable \"", t->attr.name, "\" was not declared within the scope."));
                        exit(EXIT_FAILURE);
                    }
                    t->semantic_type = SYM_get_semantic_type(tmp);
                    free(tmp);

                    traverse_to_check_for_type_errors(t->child[0]);
                    if(t->child[0]->semantic_type != G_INT)
                    {
                        semantic_error(t->lineno, "Index of an array must be an int");
                        exit(EXIT_FAILURE);
                    }
                    break;
                default:
                    fprintf(G_listing,"Unknown ExpNode kind\n");
                    break;
          }
        }
        else fprintf(G_listing,"INVALID NODE\n");

        traverse_to_check_for_type_errors(t->sibling);
    }
}


static void traverse_to_init_symbol_table(G_tree_node *t)
{
    SYM_row_entry *aux_entry;
    if (t)
    {
        if (t->node_type == G_STMT)
        { 
            switch (t->node_subtype.stmt) 
            {
                case G_VAR_DCL:
                case G_PARAM:
                    aux_entry = create_row_entry(t);
                    if(SYM_there_is_function_name_conflict(aux_entry->name))
                    {
                        //semantic error - there is already a function with the same name
                        semantic_error(t->lineno, A_append("There is already a declaration of a function with name \"", t->attr.name, "\"."));
                        exit(EXIT_FAILURE);
                    }
                    if(!SYM_add_definition_to_symbol_table(aux_entry))
                    {
                        //semantic error - multiple declarations in the same scope
                        semantic_error(t->lineno, A_append("Multiple declarations of \"", t->attr.name, "\" in the same scope."));
                        exit(EXIT_FAILURE);
                    }
                    free_row_entry(&aux_entry);
                    break;
                case G_FUNC_DCL:
                    aux_entry = create_row_entry(t);
                    if(SYM_there_is_name_conflict(aux_entry->name))
                    {
                        //semantic error - there is already a function/variable with the same name
                        semantic_error(t->lineno, A_append("There is already a declaration with name \"", t->attr.name, "\"."));
                        exit(EXIT_FAILURE);
                    }
                    if(!SYM_add_definition_to_symbol_table(aux_entry))
                    {
                        //semantic error - multiple declarations in the same scope
                        semantic_error(t->lineno, A_append("Multiple declarations of \"", t->attr.name, "\" in the same scope."));
                        exit(EXIT_FAILURE);
                    }
                    free_row_entry(&aux_entry);
                    traverse_to_init_symbol_table(t->child[0]);
                    traverse_to_init_symbol_table(t->child[1]);
                    break;
                case G_BLOCK:
                    traverse_to_init_symbol_table(t->child[0]);
                    break;
                case G_IF:
                    traverse_to_init_symbol_table(t->child[1]);
                    traverse_to_init_symbol_table(t->child[2]);
                    break;
                case G_WHILE:
                    traverse_to_init_symbol_table(t->child[1]);
                    break;
                case G_RETURN:
                case G_ASSIGNMENT:
                    break;
                default:
                    fprintf(G_listing,"Unknown ExpNode kind\n");
                    break;
            }
        }
        else if (t->node_type == G_EXP)
        { 
            switch (t->node_subtype.exp) {
                case G_COMP:
                case G_OP:
                case G_FUNC_ACTV:
                case G_CONST:
                case G_ID:
                case G_ARRAY_ID:
                    break;
                default:
                    fprintf(G_listing,"Unknown ExpNode kind\n");
                    break;
          }
        }
        else fprintf(G_listing,"INVALID NODE\n");

        traverse_to_init_symbol_table(t->sibling);
    }
}

static void free_row_entry(SYM_row_entry **ptr_r)
{
    int i;
    SYM_row_entry *t = *ptr_r;
    *ptr_r = NULL;
    free(t->id);
    free(t->name);
    free(t->scope);
    if (t->id_type == SYM_FUNC)
    {
        for (i = 0; i < t->specific_attr.func.num_of_args; i++) free(t->specific_attr.func.args_keys[i]); 
    }
}

static void semantic_error(int lineno, char * message)
{ 
    fprintf(G_listing,"Semantic error at line %d: %s\n",lineno,message);
    G_error = true;
}


static char *append_to_scope(char *scope, char* name) 
{
    char *result;
    size_t sz = strlen(scope) + strlen(name) + snprintf(NULL, 0, "%d", scope_id + 1) + 3;
    result = calloc(sz, sizeof *result);
    if (!result)
    {
        fprintf(G_listing,"(append_to_scope)Out of memory error at line %d\n",G_lineno);
        exit(EXIT_FAILURE);
    }

    result[0] = '\0';
    snprintf(result, sz, "%s$%s%%%d", scope, name, ++scope_id);
    return result;
}


static int try_to_evaluate_const_expression(G_tree_node *t)
{
    //TODO: Implement
    return -1;
}


static SYM_row_entry *create_row_entry(G_tree_node *t)
{
    SYM_row_entry *result = NULL;
    G_tree_node *tmp = NULL;
    int i;
    result = malloc(sizeof *result);
    if (!result)
    {
        fprintf(G_listing,"(create_row_entry)Out of memory error at line %d\n",G_lineno);
        exit(EXIT_FAILURE);
    }
    result->id = ANA_create_key(t->attr.name, t->scope);
    result->name = A_copy_string(t->attr.name);
    result->scope = A_copy_string(t->scope);
    result->is_parameter = false;
    result->lineno = t->lineno;
    result->semantic_type = t->semantic_type;
    if (t->node_type == G_STMT)
    { 
        switch (t->node_subtype.stmt) 
        {
            case G_VAR_DCL:
                if (t->array_sz > 0)
                {
                    result->specific_attr.arr.size = t->array_sz;
                    result->specific_attr.arr.offset = -1;
                    result->id_type = SYM_ARRAY_VAR;
                }
                else result->id_type = SYM_VAR;
                break;
            case G_PARAM:
                if (t->array_sz == 0)
                {
                    result->specific_attr.arr.size = t->array_sz;
                    result->specific_attr.arr.offset = -1;
                    result->id_type = SYM_ARRAY_VAR;
                }
                else result->id_type = SYM_VAR;

                result->is_parameter = true;
                break;
            case G_FUNC_DCL:
                result->id_type = SYM_FUNC;
                i = 0;
                tmp = t->child[0];
                while (tmp)
                {
                    result->specific_attr.func.args_keys[i++] = ANA_create_key(tmp->attr.name, tmp->scope); 
                    tmp = tmp->sibling;
                }
                result->specific_attr.func.num_of_args = i;
                break;
            default:
                fprintf(G_listing,"Invalid ExpNode kind\n");
                exit(EXIT_FAILURE);
                break;
        }
    }
    else if (t->node_type == G_EXP)
    { 
        switch (t->node_subtype.exp) {
            case G_COMP:
            case G_OP:
            case G_CONST:
            case G_FUNC_ACTV:
                result->id_type = SYM_FUNC;
                break;
            case G_ID:
                result->id_type = SYM_VAR;
                break;
            case G_ARRAY_ID:
                result->specific_attr.arr.offset = try_to_evaluate_const_expression(t->child[0]);
                result->id_type = SYM_ARRAY_VAR;
                break;
            default:
                fprintf(G_listing,"Unknown ExpNode kind\n");
                exit(EXIT_FAILURE);
                break;
      }
    }
    else 
    {
        fprintf(G_listing,"INVALID NODE\n");
        exit(EXIT_FAILURE);
    }
}

