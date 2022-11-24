#include <stdlib.h>
#include "code_generator.h"
#include "globals.h"
#include "aux.h"
#include "parser.tab.h"
#include "symbol_table.h"

//Global variables:
static int label_count = 0;
static int aux_var_count = 0;

//static function declarations:
void generate_code(G_tree_node *t);
static char *get_operation_string( G_token_type op);
char *create_next_aux_variable(void);
char *create_next_label(void);
int get_size(G_type type);


//Function definitions:
void CG_generate_code(G_tree_node *t, char *filename)
{
    label_count = 0;
    aux_var_count = 0;
    fprintf(G_code, "# File: %s\n", filename);
    fprintf(G_code, "####################\n");
    fprintf(G_code, "# Intermediate code \n");
    generate_code(t);
    fprintf(G_code, "####################\n");
}

//Static function definitions


void _generate_code_depth_only(G_tree_node *t)
{
    char *tmp, *tmp1;
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
                    fprintf(G_code, "%s:", t->attr.name);
                    if(t->child[1]) generate_code(t->child[1]);
                    else fprintf(G_code, "\n");
                    break;
                case G_BLOCK:
                    generate_code(t->child[0]);
                    break;
                case G_IF:
                    generate_code(t->child[0]);
                    tmp = create_next_label();
                    tmp1 = create_next_label();
                    fprintf(G_code, "if %s goto %s\n", t->child[0]->var_name, tmp);
                    generate_code(t->child[2]);
                    fprintf(G_code, "goto %s", tmp1);
                    fprintf(G_code, "%s: ", tmp);
                    if(t->child[1]) generate_code(t->child[1]);
                    else fprintf(G_code, "\n");
                    fprintf(G_code, "%s: \n", tmp1);
                    free(tmp);
                    free(tmp1);
                    break;
                case G_WHILE:
                    tmp = create_next_label();
                    tmp1 = create_next_label();
                    fprintf(G_code, "goto %s\n", tmp1);
                    fprintf(G_code, "%s:", tmp);
                    if(t->child[1]) generate_code(t->child[1]);
                    else fprintf(G_code, "\n");
                    fprintf(G_code, "%s:", tmp1);
                    generate_code(t->child[0]);
                    fprintf(G_code, "if %s goto %s\n", t->child[0]->var_name, tmp);
                    free(tmp);
                    free(tmp1);
                    break;
                case G_RETURN:
                    generate_code(t->child[0]);
                    if(t->child[0]) fprintf(G_code, "%s %s\n", get_operation_string(t->attr.op), t->child[0]->var_name);
                    else fprintf(G_code, "%s\n", get_operation_string(t->attr.op));
                    break;
                case G_ASSIGNMENT:
                    generate_code(t->child[0]);
                    generate_code(t->child[1]);
                    t->var_name = A_copy_string(t->child[0]->attr.name);
                    fprintf(G_code, "%s %s %s\n", t->child[0]->var_name, get_operation_string(t->attr.op), t->child[1]->var_name);
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
                case G_OP:
                    generate_code(t->child[0]);
                    generate_code(t->child[1]);
                    t->var_name = create_next_aux_variable();
                    fprintf(G_code, "%s = %s %s %s\n", t->var_name, t->child[0]->var_name, get_operation_string(t->attr.op), t->child[1]->var_name);
                    break;
                case G_FUNC_ACTV:
                    tmp = SYM_get_function_declaration_id(t->attr.name);
                    if (!tmp)
                    {
                        fprintf(G_code, "Bug: function not declared");
                        exit(EXIT_FAILURE);
                    }
                    tmp1 = A_append("call ", t->attr.name, " "); 
                    t->var_name = A_append(tmp1, A_int_to_string(SYM_get_num_of_args(tmp)), "");
                    free(tmp1);
                    aux_node = t->child[0];
                    while(aux_node)
                    {
                        _generate_code_depth_only(aux_node);
                        fprintf(G_code, "param %s\n", aux_node->var_name);
                        aux_node = aux_node->sibling;
                    }
                    if(SYM_get_semantic_type(tmp) == G_VOID) fprintf(G_code, "%s\n", t->var_name);
                    free(tmp);
                    break;
                case G_CONST:
                    t->var_name = A_int_to_string(t->attr.val);
                    break;
                case G_ID:
                    t->var_name = A_copy_string(t->attr.name);
                    break;
                case G_ARRAY_ID:
                    generate_code(t->child[0]);
                    tmp = create_next_aux_variable();
                    fprintf(G_code, "%s = %s %s %d\n", tmp, t->child[0]->var_name, get_operation_string(TIMES), get_size(t->semantic_type));
                    t->var_name = A_append(t->attr.name, "[", tmp);
                    free(tmp);
                    tmp = t->var_name;
                    t->var_name = A_append(tmp, "]", "");
                    free(tmp);
                    break;
                default:
                    fprintf(G_listing,"Unknown ExpNode kind\n");
                    break;
          }
        }
        else fprintf(G_listing,"INVALID NODE\n");
    }
}



void generate_code(G_tree_node *t)
{
    char *tmp, *tmp1;
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
                    fprintf(G_code, "%s:", t->attr.name);
                    if(t->child[1]) generate_code(t->child[1]);
                    else fprintf(G_code, "\n");
                    break;
                case G_BLOCK:
                    generate_code(t->child[0]);
                    break;
                case G_IF:
                    generate_code(t->child[0]);
                    tmp = create_next_label();
                    tmp1 = create_next_label();
                    fprintf(G_code, "if %s goto %s\n", t->child[0]->var_name, tmp);
                    generate_code(t->child[2]);
                    fprintf(G_code, "goto %s\n", tmp1);
                    fprintf(G_code, "%s: ", tmp);
                    if(t->child[1]) generate_code(t->child[1]);
                    else fprintf(G_code, "\n");
                    fprintf(G_code, "%s: \n", tmp1);
                    free(tmp);
                    free(tmp1);
                    break;
                case G_WHILE:
                    tmp = create_next_label();
                    tmp1 = create_next_label();
                    fprintf(G_code, "goto %s\n", tmp1);
                    fprintf(G_code, "%s:", tmp);
                    if(t->child[1]) generate_code(t->child[1]);
                    else fprintf(G_code, "\n");
                    fprintf(G_code, "%s:", tmp1);
                    generate_code(t->child[0]);
                    fprintf(G_code, "if %s goto %s\n", t->child[0]->var_name, tmp);
                    free(tmp);
                    free(tmp1);
                    break;
                case G_RETURN:
                    generate_code(t->child[0]);
                    if(t->child[0]) fprintf(G_code, "%s %s\n", get_operation_string(t->attr.op), t->child[0]->var_name);
                    else fprintf(G_code, "%s\n", get_operation_string(t->attr.op));
                    break;
                case G_ASSIGNMENT:
                    generate_code(t->child[0]);
                    generate_code(t->child[1]);
                    t->var_name = A_copy_string(t->child[0]->attr.name);
                    fprintf(G_code, "%s %s %s\n", t->child[0]->var_name, get_operation_string(t->attr.op), t->child[1]->var_name);
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
                case G_OP:
                    generate_code(t->child[0]);
                    generate_code(t->child[1]);
                    t->var_name = create_next_aux_variable();
                    fprintf(G_code, "%s = %s %s %s\n", t->var_name, t->child[0]->var_name, get_operation_string(t->attr.op), t->child[1]->var_name);
                    break;
                case G_FUNC_ACTV:
                    tmp = SYM_get_function_declaration_id(t->attr.name);
                    if (!tmp)
                    {
                        fprintf(G_code, "Bug: function not declared");
                        exit(EXIT_FAILURE);
                    }
                    if(SYM_get_semantic_type(tmp) == G_VOID) {
                        tmp1 = A_append("call ", t->attr.name, " "); 
                        tmp1 = A_append(tmp1, A_int_to_string(SYM_get_num_of_args(tmp)), "");
                        t->var_name = tmp1;
                    }
                    else {
                        t->var_name = create_next_aux_variable();
                        tmp1 = A_append(t->var_name, " = call", " ");
                        tmp1 = A_append(tmp1, t->attr.name, " ");
                        tmp1 = A_append(tmp1, A_int_to_string(SYM_get_num_of_args(tmp)), "");
                    }
                    aux_node = t->child[0];
                    while(aux_node)
                    {
                        _generate_code_depth_only(aux_node);
                        fprintf(G_code, "param %s\n", aux_node->var_name);
                        aux_node = aux_node->sibling;
                    }
                    fprintf(G_code, "%s\n", tmp1);
                    free(tmp1);
                    free(tmp);
                    break;
                case G_CONST:
                    t->var_name = A_int_to_string(t->attr.val);
                    break;
                case G_ID:
                    t->var_name = A_copy_string(t->attr.name);
                    break;
                case G_ARRAY_ID:
                    generate_code(t->child[0]);
                    tmp = create_next_aux_variable();
                    fprintf(G_code, "%s = %s %s %d\n", tmp, t->child[0]->var_name, get_operation_string(TIMES), get_size(t->semantic_type));
                    t->var_name = A_append(t->attr.name, "[", tmp);
                    free(tmp);
                    tmp = t->var_name;
                    t->var_name = A_append(tmp, "]", "");
                    free(tmp);
                    break;
                default:
                    fprintf(G_listing,"Unknown ExpNode kind\n");
                    break;
          }
        }
        else fprintf(G_listing,"INVALID NODE\n");
        generate_code(t->sibling);
    }
}

static char *get_operation_string( G_token_type token)
{ 
    switch (token)
    { 
        case RETURN:
            return "return";
            break;
        case ASSIGN:
            return "=";
            break;
        case EQ:
            return "=";
            break;
        case NEQ:
            return "!=";
            break;
        case LT:
            return "<";
            break;
        case LET:
            return "<=";
            break;
        case GT:
            return ">";
            break;
        case GET:
            return ">=";
            break;
        case PLUS:
            return "+";
            break;
        case MINUS: 
            return "-";
            break;
        case TIMES:
            return "*";
            break;
        case OVER:
            return "/";
            break;
        case VOID:
        case WHILE:
        case INT:
        case ID:
        case NUM:
        case LPAREN:
        case RPAREN:
        case SEMI:
        case COMMA:
        case LSQ_BRACKET:
        case RSQ_BRACKET:
        case LCU_BRACKET:
        case RCU_BRACKET:
        case ENDFILE: 
        case ERROR:
        case IF:
        case ELSE:
            return "INVALID OPERATION";
            break;
        default: /* should never happen */
            return "UNKNOWN";
            break;
    }
}

char *create_next_aux_variable()
{
    char *tmp, *result;
    tmp = A_int_to_string(aux_var_count++);
    result = A_append("t", tmp, "");
    free(tmp);
    return result;
}

char *create_next_label(void)
{
    char *tmp, *result;
    tmp = A_int_to_string(label_count++);
    result = A_append("L", tmp, "");
    free(tmp);
    return result;
}

int get_size(G_type type)
{
    switch(type)
    {
        case G_INT:
            return 4;
            break;
        case G_VOID:
            return 0;
            break;
        default:
            return 0;
            break;
    }
}
