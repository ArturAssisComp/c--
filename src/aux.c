#include <stdlib.h>
#include <string.h>
#include "aux.h"
#include "globals.h"
#include "parser.h"
#include "parser.tab.h"

#define INDENT indentno+=4
#define UNINDENT indentno-=4

//Type definition:
typedef enum {ROOT, CHILD, SIBLING}state;

//Global variables:
static int indentno = 0;
static int child_no = 0;
static state current_state = ROOT;

//Static function declarations:
static void print_spaces(void);
static char *get_basic_token_str( G_token_type token);

//Function definitions:
void A_print_tree( G_tree_node * root)
{ 
    bool reset_root = false;
    int i;
    if (current_state == ROOT) reset_root = true;
    INDENT;
    while (root != NULL) 
    {
        print_spaces();
        if (root->node_type == G_STMT)
        { 
            switch (root->node_subtype.stmt) 
            {
                case G_VAR_DCL:
                  if (root->array_sz == -1) fprintf(G_listing,"DECLARE VAR \"%s\" OF TYPE %s\n", root->attr.name, A_semantic_type_to_str(root->semantic_type));
                  else if (root->array_sz > 0) fprintf(G_listing,"DECLARE ARRAY VAR \"%s\" OF TYPE %s AND SIZE %d\n", root->attr.name, A_semantic_type_to_str(root->semantic_type), root->array_sz); 
                  else fprintf(G_listing, "Array with invalid size");
                  break;
                case G_FUNC_DCL:
                  fprintf(G_listing,"DECLARE FUNC \"%s\" OF TYPE %s\n", root->attr.name, A_semantic_type_to_str(root->semantic_type));
                  break;
                case G_BLOCK:
                  fprintf(G_listing,"BLOCK\n");
                  break;
                case G_IF:
                  fprintf(G_listing,"IF\n");
                  break;
                case G_WHILE:
                  fprintf(G_listing,"WHILE\n");
                  break;
                case G_RETURN:
                  fprintf(G_listing,"RETURN\n");
                  break;
                case G_ASSIGNMENT:
                  fprintf(G_listing,"ASSIGNMENT\n");
                  break;
                case G_PARAM:
                  if (root->array_sz == -1) fprintf(G_listing,"PARAM: \"%s\" OF TYPE %s\n",root->attr.name, A_semantic_type_to_str(root->semantic_type));
                  else if (root->array_sz == 0) fprintf(G_listing,"PARAM: \"%s\" OF TYPE %s[]\n",root->attr.name, A_semantic_type_to_str(root->semantic_type));
                  else 
                  {
                      fprintf(G_listing,"Node corrupted\n");
                      exit(EXIT_FAILURE);
                  }
                  break;
                default:
                  fprintf(G_listing,"Unknown ExpNode kind\n");
                  break;
            }
        }
        else if (root->node_type == G_EXP)
        { 
            switch (root->node_subtype.exp) {
                case G_COMP:
                  fprintf(G_listing,"COMPARISON OP: \"%s\"\n", get_basic_token_str(root->attr.op));
                  break;
                case G_OP:
                  fprintf(G_listing,"ARITHMETIC OP: \"%s\"\n", get_basic_token_str(root->attr.op));
                  break;
                case G_FUNC_ACTV:
                  fprintf(G_listing,"CALLING FUNC: \"%s\"\n", root->attr.name);
                  break;
                case G_CONST:
                  fprintf(G_listing,"CONST: %d\n",root->attr.val);
                  break;
                case G_ID:
                  fprintf(G_listing,"ID: \"%s\"\n",root->attr.name);
                  break;
                case G_ARRAY_ID:
                  fprintf(G_listing,"ARRAY ID: \"%s\"\n",root->attr.name);
                  break;
                default:
                  fprintf(G_listing,"Unknown ExpNode kind\n");
                  break;
          }
        }
        else fprintf(G_listing,"INVALID NODE\n");
        for (i=0;i<MAXCHILDREN;i++)
        {
            child_no = i;
            current_state = CHILD;
            A_print_tree(root->child[i]);
        }
        current_state = SIBLING;
        root = root->sibling;
    }

    UNINDENT;
    if (reset_root) 
    {
        current_state = ROOT;
        indentno = 0;
    }
}

void A_print_complete_tree( G_tree_node * root)
{ 
    bool reset_root = false;
    int i;
    if (current_state == ROOT) reset_root = true;
    INDENT;
    while (root != NULL) 
    {
        print_spaces();
        if (root->node_type == G_STMT)
        { 
            switch (root->node_subtype.stmt) 
            {
                case G_VAR_DCL:
                  if (root->array_sz == -1) fprintf(G_listing,"DECLARE VAR \"%s\" OF TYPE %s (scope: %s)\n", root->attr.name, A_semantic_type_to_str(root->semantic_type), root->scope);
                  else if (root->array_sz > 0) fprintf(G_listing,"DECLARE ARRAY VAR \"%s\" OF TYPE %s AND SIZE %d (scope: %s)\n", root->attr.name, A_semantic_type_to_str(root->semantic_type), root->array_sz, root->scope); 
                  else fprintf(G_listing, "Array with invalid size");
                  break;
                case G_FUNC_DCL:
                  fprintf(G_listing,"DECLARE FUNC \"%s\" OF TYPE %s (scope: %s)\n", root->attr.name, A_semantic_type_to_str(root->semantic_type), root->scope);
                  break;
                case G_BLOCK:
                  fprintf(G_listing,"BLOCK (scope: %s)\n", root->scope);
                  break;
                case G_IF:
                  fprintf(G_listing,"IF (scope: %s)\n", root->scope);
                  break;
                case G_WHILE:
                  fprintf(G_listing,"WHILE (scope: %s)\n", root->scope);
                  break;
                case G_RETURN:
                  fprintf(G_listing,"RETURN\n");
                  break;
                case G_ASSIGNMENT:
                  fprintf(G_listing,"ASSIGNMENT\n");
                  break;
                case G_PARAM:
                  if (root->array_sz == -1) fprintf(G_listing,"PARAM: \"%s\" OF TYPE %s (scope: %s)\n",root->attr.name, A_semantic_type_to_str(root->semantic_type), root->scope);
                  else if (root->array_sz == 0) fprintf(G_listing,"PARAM: \"%s\" OF TYPE %s[] (scope: %s)\n",root->attr.name, A_semantic_type_to_str(root->semantic_type), root->scope);
                  else 
                  {
                      fprintf(G_listing,"Node corrupted\n");
                      exit(EXIT_FAILURE);
                  }
                  break;
                default:
                  fprintf(G_listing,"Unknown ExpNode kind\n");
                  break;
            }
        }
        else if (root->node_type == G_EXP)
        { 
            switch (root->node_subtype.exp) {
                case G_COMP:
                  fprintf(G_listing,"COMPARISON OP: \"%s\"\n", get_basic_token_str(root->attr.op));
                  break;
                case G_OP:
                  fprintf(G_listing,"ARITHMETIC OP: \"%s\"\n", get_basic_token_str(root->attr.op));
                  break;
                case G_FUNC_ACTV:
                  fprintf(G_listing,"CALLING FUNC: \"%s\" (scope: %s)\n", root->attr.name, root->scope);
                  break;
                case G_CONST:
                  fprintf(G_listing,"CONST: %d\n",root->attr.val);
                  break;
                case G_ID:
                  fprintf(G_listing,"ID: \"%s\" (scope: %s)\n",root->attr.name, root->scope);
                  break;
                case G_ARRAY_ID:
                  fprintf(G_listing,"ARRAY ID: \"%s\" (scope: %s)\n",root->attr.name, root->scope);
                  break;
                default:
                  fprintf(G_listing,"Unknown ExpNode kind\n");
                  break;
          }
        }
        else fprintf(G_listing,"INVALID NODE\n");
        for (i=0;i<MAXCHILDREN;i++)
        {
            child_no = i;
            current_state = CHILD;
            A_print_complete_tree(root->child[i]);
        }
        current_state = SIBLING;
        root = root->sibling;
    }

    UNINDENT;
    if (reset_root) 
    {
        current_state = ROOT;
        indentno = 0;
    }
}

void A_print_token( G_token_type token, const char* lexeme)
{ 
    switch (token)
    { 
        case ENDFILE: 
            fprintf(G_listing,"ENDFILE\n"); 
            break;
        case ERROR:
            fprintf(G_listing,"ERROR: %s\n",lexeme);
            break;
        case IF:
        case ELSE:
        case RETURN:
        case VOID:
        case WHILE:
        case INT:
            fprintf(G_listing,"reserved word: %s\n",lexeme);
            break;
        case ID:
            fprintf(G_listing,"ID, name= %s\n",lexeme);
            break;
        case NUM:
            fprintf(G_listing,"NUM, val= %s\n",lexeme);
            break;
        case ASSIGN:
        case EQ:
        case NEQ:
        case LT:
        case LET:
        case GT:
        case GET:
        case PLUS:
        case MINUS: 
        case TIMES:
        case OVER:
        case LPAREN:
        case RPAREN:
        case SEMI:
        case COMMA:
        case LSQ_BRACKET:
        case RSQ_BRACKET:
        case LCU_BRACKET:
        case RCU_BRACKET:
            fprintf(G_listing,"special symbol: %s\n",lexeme);
            break;
        default: /* should never happen */
            fprintf(G_listing,"Unknown token: %d\n",token);
            break;
    }
}

G_tree_node * A_new_stmt_node(G_stmt_type stmt_type)
{
    int i;
    G_tree_node *t = malloc(sizeof *t);
    if (t==NULL)
    {
        fprintf(G_listing,"Out of memory error at line %d\n",G_lineno);
        exit(EXIT_FAILURE);
    }

    for (i=0; i<MAXCHILDREN; i++) t->child[i] = NULL;

    t->sibling = NULL;
    t->node_type = G_STMT;
    t->node_subtype.stmt = stmt_type;
    t->lineno = G_lineno;
    t->array_sz = -1;
    return t;
}

G_tree_node * A_new_exp_node(G_exp_type exp_type)
{
    int i;
    G_tree_node *t = malloc(sizeof *t);
    if (t==NULL)
    {
        fprintf(G_listing,"Out of memory error at line %d\n",G_lineno);
        exit(EXIT_FAILURE);
    }

    for (i=0; i<MAXCHILDREN; i++) t->child[i] = NULL;

    t->sibling = NULL;
    t->node_type = G_EXP;
    t->node_subtype.exp= exp_type;
    t->semantic_type = G_VOID;
    t->array_sz = -1;
    t->lineno = G_lineno;
    return t;
}

char *A_copy_string(char *s)
{
    int n;
    char * t;
    if (s==NULL) return NULL;
    n = strlen(s)+1;
    t = malloc(n);
    if (t==NULL)
    {
        fprintf(G_listing,"Out of memory error at line %d\n",G_lineno);
        exit(EXIT_FAILURE);
    }
    strncpy(t,s,n);
    return t;
}

char *A_append(char *prefix, char *infix, char *postfix)
{
    char *result;
    size_t sz = strlen(prefix) + strlen(infix) + strlen(postfix) + 1;
    result = calloc(sz, sizeof *result);
    if (!result)
    {
        fprintf(G_listing,"Out of memory error at line %d\n",G_lineno);
        exit(EXIT_FAILURE);
    }

    result[0] = '\0';
    snprintf(result, sz, "%s%s%s", prefix, infix, postfix);
    return result;
}

char *A_int_to_string(int n)
{
    char *result;
    size_t sz = snprintf(NULL, 0, "%d", n) + 1;
    result = calloc(sz, sizeof *result);
    if(!result)
    {
        fprintf(G_listing,"(A_int_to_string) Out of memory error at line %d\n",G_lineno);
        exit(EXIT_FAILURE);
    }
    snprintf(result, sz, "%d", n);
    return result;
}

char *A_semantic_type_to_str(G_type s_type)
{
    switch(s_type)
    {
        case G_VOID:
            return "void";
            break;
        case G_INT:
            return "int";
            break;
        default:
                return "INVALID SEMANTIC TYPE";
            break;
    }
}

//Static function definitions:
static void print_spaces(void)
{ 
    int i;
    for (i=0;i<indentno;i++) fprintf(G_listing," ");
    switch (current_state)
    {
        case ROOT:
            fprintf(G_listing, "(r )");
            break;
        case CHILD:
            fprintf(G_listing, "(c%1d)", child_no);
            break;
        case SIBLING:
            fprintf(G_listing, "(s )");
            break;
        default:
            break;
    }
}

static char *get_basic_token_str( G_token_type token)
{ 
    switch (token)
    { 
        case ENDFILE: 
            return "ENDFILE";
            break;
        case ERROR:
            return "ERROR";
            break;
        case IF:
            return "if";
            break;
        case ELSE:
            return "else";
            break;
        case RETURN:
            return "return";
            break;
        case VOID:
            return "void";
            break;
        case WHILE:
            return "while";
            break;
        case INT:
            return "int";
            break;
        case ID:
            return "id";
            break;
        case NUM:
            return "num";
            break;
        case ASSIGN:
            return "=";
            break;
        case EQ:
            return "==";
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
        case LPAREN:
            return "(";
            break;
        case RPAREN:
            return ")";
            break;
        case SEMI:
            return ";";
            break;
        case COMMA:
            return ",";
            break;
        case LSQ_BRACKET:
            return "[";
            break;
        case RSQ_BRACKET:
            return "]";
            break;
        case LCU_BRACKET:
            return "{";
            break;
        case RCU_BRACKET:
            return "}";
            break;
        default: /* should never happen */
            return "UNKNOWN";
            break;
    }
}


