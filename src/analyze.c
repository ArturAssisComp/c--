#include "symtab.h"
#include "analyze.h"

/* counter for variable memory locations */
static int location = 0;

/* Procedure traverse is a generic recursive 
 * syntax tree traversal routine:
 * it applies preProc in preorder and postProc 
 * in postorder to tree pointed to by t
 */
static void traverse( G_tree_node * t,
               void (* pre_proc) (G_tree_node *),
               void (* post_proc) (G_tree_node *) )
{ if (t != NULL)
  { pre_proc(t);
    { int i;
      for (i=0; i < MAXCHILDREN; i++)
        traverse(t->child[i],preProc,postProc);
    }
    post_proc(t);
    traverse(t->sibling,preProc,postProc);
  }
}

/* nullProc is a do-nothing procedure to 
 * generate preorder-only or postorder-only
 * traversals from traverse
 */
static void null_proc(G_tree_node * t)
{ if (t==NULL) return;
  else return;
}

/* Procedure insertNode inserts 
 * identifiers stored in t into 
 * the symbol table 
 */
static void insertNode( G_tree_node * t)
{ switch (t->node_type)
  { case G_STMT:
      switch (t->node_subtype)
      { case G_ASSIGNMENT:
        case G_VAR_DCL: 
        case G_FUNC_DCL:
        case G_IF:
        case G_WHILE:
        case G_RETURN: 
        case G_ASSIGNMENT:
        case G_PARAM:
        default:
          break;
      }
      break;
    case G_EXP:
      switch (t->node_subtype)
      { //case IdK:
    //       if (st_lookup(t->attr.name) == -1)
    //       /* not yet in table, so treat as new definition */
    //         st_insert(t->attr.name,t->lineno,location++);
    //       else
    //       /* already in table, so ignore location, 
    //          add line number of use only */ 
    //         st_insert(t->attr.name,t->lineno,0);
    //       break;
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
    default:
      break;
  }
}

/* Function buildSymtab constructs the symbol 
 * table by preorder traversal of the syntax tree
 */
void build_symtab(G_tree_node * syntaxTree)
{ traverse(syntaxTree,insertNode,nullProc);
  if (TraceAnalyze)
  { fprintf(listing,"\nSymbol table:\n\n");
    printSymTab(listing);
  }
}

static void type_error(G_tree_node * t, char * message)
{ fprintf(listing,"Type error at line %d: %s\n",t->lineno,message);
  Error = TRUE;
}

/* Procedure checkNode performs
 * type checking at a single tree node
 */
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
        case G_ASSIGNMENT:
        case G_PARAM:
        default:
          break;
      }
      break;
    default:
      break;

  }
}

/* Procedure typeCheck performs type checking 
 * by a postorder syntax tree traversal
 */
void typeCheck(G_tree_node * syntax_tree)
{ traverse(syntax_tree,nullProc,checkNode);
}