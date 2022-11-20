#include <stdlib.h>
#include <string.h>
//#include "symtab.h"
#include "analyze.h"
#include "aux.h"

/* scope variable of studied line */
static int   scope_id = 0;

static char *append_to_scope(char *scope, char* name) {
    char *result;
    size_t sz = strlen(scope) + strlen(name) + snprintf(NULL, 0, "%d", scope_id + 1) + 3;
    result = calloc(sz, sizeof *result);
    if (!result)
    {
        fprintf(G_listing,"Out of memory error at line %d\n",G_lineno);
        exit(EXIT_FAILURE);
    }

    result[0] = '\0';
    snprintf(result, sz, "%s$%s%%%d", scope, name, ++scope_id);
    return result;
}

static char *remove_scope(char* scope) {
    char *result;
    size_t i;
    for(i = strlen(scope) - 1; i >= 0; i--)
    {
        if (scope[i] == '$') break;
    }
    size_t sz = i + 1;
    result = calloc(sz, sizeof *result);
    if (!result)
    {
        fprintf(G_listing,"Out of memory error at line %d\n",G_lineno);
        exit(EXIT_FAILURE);
    }

    result[0] = '\0';
    strncpy(result, scope, i);
    result[i] = '\0';
    return result;
}


static void traverse_to_define_scope(G_tree_node *t, char *scope)
{
    int i;
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
                    traverse_to_define_scope(t->child[0], scope);
                    break;
                case G_ASSIGNMENT:
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
                    traverse_to_define_scope(t->child[0], scope);
                    traverse_to_define_scope(t->child[1], scope);
                    break;
                case G_OP:
                    traverse_to_define_scope(t->child[0], scope);
                    traverse_to_define_scope(t->child[1], scope);
                    break;
                case G_FUNC_ACTV:
                    t->scope = A_copy_string(scope);                    
                    traverse_to_define_scope(t->child[0], scope);
                    break;
                case G_CONST:
                    break;
                case G_ID:
                    t->scope = A_copy_string(scope);                    
                    break;
                case G_ARRAY_ID:
                    t->scope = A_copy_string(scope);                    
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



/* Procedure traverse is a generic recursive 
 * syntax tree traversal routine:
 * it applies preProc in preorder and postProc 
 * in postorder to tree pointed to by t
 */
/*
static void traverse( G_tree_node * t,
               void (* pre_proc) (G_tree_node *),
               void (* post_proc) (G_tree_node *) )
{ if (t != NULL)
  { pre_proc(t);
    { int i;
      for (i=0; i < MAXCHILDREN; i++)
        traverse(t->child[i],pre_proc,post_proc);
    }
    post_proc(t);
    traverse(t->sibling,pre_proc,post_proc);
  }
}
*/

/* nullProc is a do-nothing procedure to 
 * generate preorder-only or postorder-only
 * traversals from traverse
 */
/*
static void null_proc(G_tree_node * t)
{ if (t==NULL) return;
  else return;
}
*/

/* Procedure insertNode inserts 
 * identifiers stored in t into 
 * the symbol table 
 */
/*************************************************************
static void insert_node( G_tree_node * t)
{ 
    switch (t->node_type)
    { 
        case G_STMT:
            switch (t->node_subtype)
            { 
                case G_ASSIGNMENT:
                    if !(SYM_var_isin_within(t->child[0], scope)){
                      type_error(t, t->child[0]->attr.name);
                  }
                  break;
              case G_VAR_DCL:
                  if (SYM_var_isin_exact(t, scope)){
                      type_error(t, t->attr.name);
                  } else if (SYM_fun_isin(t, scope)) {
                      type_error(t, t->attr.name);
                  } else {
                      SYM_row_entry e = SYM_create_entry(t, scope);
                      if( !SYM_add_definition_to_symbol_table(e) )
                          // Nothing for instance
                  }
                  break;
              case G_FUNC_DCL:
                  if (strcmp(scope, '') || SYM_var_isin_exact(t, scope))
                      type_error(t, t->attr.name);
                  SYM_row_entry e = SYM_create_entry(t, scope);
                  SYM_add_definition_to_symbol_table(e);
                  append_to_scope(t->attr.name);
                  break;
              case G_IF:
                  // Em teoria não pode porque tem a expressão que não está no scope do if
                  append_to_scope(t->attr.name);
                  break;
              case G_WHILE:
                  append_to_scope(t->attr.name);
                  break;
              case G_RETURN:
                  break;
              case G_PARAM:
                  if (SYM_var_isin_exact(t, scope)){
                      type_error(t, t->attr.name);
                  } else if (SYM_fun_isin(t, scope)) {
                      type_error(t, t->attr.name);
                  } else {
                      SYM_row_entry e = SYM_create_entry(t, scope);
                      SYM_add_definition_to_symbol_table(e);
                  }
                  break;
              default:
                break;
            }
      break;
    case G_EXP:
      switch (t->node_subtype)
      { //case IdK:
    //       if (st_lookup(t->attr.name) == -1)
    //         st_insert(t->attr.name,t->lineno,location++);
    //       else
    //         st_insert(t->attr.name,t->lineno,0);
    //       break;
        case G_FUNC_ACTV:
            if !(SYM_fun_isin(t, scope)) {
                type_error(t, t->attr.name);
            else {
                SYM_insert_line_occurrence_into_symbol_table(t->attr.name, scope, )
            }
            break;
        case G_ID:
        case G_ARRAY_ID:
            if (SYM_fun_isin(t, scope)) {
                type_error(t, t->attr.name);
            else if !(SYM_var_isin_within(t, scope)) {
                type_error(t, t->attr.name);
            } else {
                SYM_insert_line_occurrence_into_symbol_table(t->attr.name, scope, )
            }
            break;
        case G_COMP: // Ok
        case G_OP: // Ok
        case G_CONST: // OK
        default:
            break;
      }
        break;
    default:
        break;
  }
}
*************************************************************/

void ANA_set_syntax_tree_scope(G_tree_node *root)
{
    char *t = A_copy_string("GLOBAL%0");
    scope_id = 0;
    traverse_to_define_scope(root, t);
}

/* Function buildSymtab constructs the symbol 
 * table by preorder traversal of the syntax tree
 */
/*
void build_symtab(G_tree_node * syntaxTree)
{ traverse(syntaxTree,insert_node,null_proc);
  if (TraceAnalyze)
  { fprintf(listing,"\nSymbol table:\n\n");
    printSymTab(listing);
  }
}
*/

/*
static void type_error(G_tree_node * t, char * message)
{ fprintf(G_listing,"Type error at line %d: %s\n",t->lineno,message);
  G_error = true;
}
*/

/* Procedure checkNode performs
 * type checking at a single tree node
 */
/*
static void check_node(G_tree_node * t)
{ switch (t->node_type)
  { case G_EXP:
      switch (t->node_subtype)
      { // case OpK:
        //   if ((t->child[0]->type != Integer) ||
        //       (t->child[1]->type != Integer))
        //     typeError(t,"Op applied to non-integer");
        //   if ((t->attr.op == EQ) || (t->attr.op == LT))
        //     t->type = Boolean;
        //   else
        //     t->type = Integer;
        //   break;
        // case ConstK:
        // case IdK:
        //   t->type = Integer;
        //   break;
        case G_COMP:
        case G_OP:
        case G_FUNC_ACTV:
        case G_CONST:
        case G_ID:
        case G_ARRAY_ID:
        default:
          break;
      }
      break;
    case G_STMT:
      switch (t->node_subtype)
      { // case IfK:
        //   if (t->child[0]->type == Integer)
        //     typeError(t->child[0],"if test is not Boolean");
        //   break;
        // case AssignK:
        //   if (t->child[0]->type != Integer)
        //     typeError(t->child[0],"assignment of non-integer value");
        //   break;
        // case WriteK:
        //   if (t->child[0]->type != Integer)
        //     typeError(t->child[0],"write of non-integer value");
        //   break;
        // case RepeatK:
        //   if (t->child[1]->type == Integer)
        //     typeError(t->child[1],"repeat test is not Boolean");
        //   break;
        case G_ASSIGNMENT:
        case G_VAR_DCL: 
        case G_FUNC_DCL:
        case G_IF:
        case G_WHILE:
        case G_RETURN: 
        case G_PARAM:
        default:
          break;
      }
      break;
    default:
      break;

  }
}
*/

/* Procedure typeCheck performs type checking 
 * by a postorder syntax tree traversal
 */
/*
void type_check(G_tree_node * syntax_tree)
{ traverse(syntax_tree,null_proc,check_node);
}
*/
