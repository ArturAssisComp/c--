/*Main for the compiler C--*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "aux.h"
#include "globals.h"

/* set NO_PARSE to true to get a scanner-only compiler */
#define NO_PARSE false 
/* set NO_ANALYZE to true to get a parser-only compiler */
#define NO_ANALYZE false 

/* set NO_CODE to true to get a compiler that does not
 * generate code
 */
#define NO_CODE false

#if NO_PARSE
#include "scanner.h"
#else
#include "parser.h"
#if !NO_ANALYZE
#include "analyze.h"
#if !NO_CODE
#include "code_generator.h"
#endif
#endif
#endif

#define MAX_FILENAME 120


/*Define the global variables declared in the globals.h*/
FILE* G_source;  /* source code text file */
FILE* G_listing; /* listing output text file */
FILE* G_code;    /* code text file */
bool G_echo_source = true;
bool G_trace_scan  = true;
bool G_trace_parse = true;
bool G_trace_analyze = true;
bool G_trace_code = true;
bool G_error       = true;



int main(int argc, char *argv[])
{
    G_tree_node *root;
    char source_filename[MAX_FILENAME]; 
    char code_filename[MAX_FILENAME] = "out.ic"; 
    if(argc == 1) 
    {
        G_source = stdin;
        G_code = stdout;
    }
    else if (argc == 2) 
    {

        strncpy(source_filename, argv[1], MAX_FILENAME) ;
        G_source = fopen(source_filename, "r");
        if(!G_source)
        {
            perror("fopen");
            exit(EXIT_FAILURE);        
        }
        G_code = stdout;
    }
    else if(argc == 3)
    {
        strncpy(source_filename, argv[1], MAX_FILENAME) ;
        strncpy(code_filename, argv[2], MAX_FILENAME) ;
        G_source = fopen(source_filename, "r");
        if(!G_source)
        {
            perror("fopen");
            exit(EXIT_FAILURE);        
        }
        G_code = fopen(code_filename, "w");
        if(!G_code)
        {
            perror("fopen");
            exit(EXIT_FAILURE);        
        }
    }
    else 
    {
        fprintf(stderr, "Usage: %s [<source_code_file>] [<intermediate_code_file>]", argv[0]);
        exit(EXIT_FAILURE);
    }

    G_listing = stdout;
#if NO_PARSE
    while(S_get_token() != ENDFILE);
#else
  root = P_parse();
  if (G_trace_parse) {
    fprintf(G_listing,"\nSyntax tree: \n");
    if (root == NULL) fprintf(G_listing, "<the tree is NULL>\n");
    else A_print_tree(root);
    fprintf(G_listing,"\n################################\n");
  }
#if !NO_ANALYZE
    ANA_set_syntax_tree_scope(root);
    if (G_trace_analyze && root) {
      fprintf(G_listing,"\nSyntax tree (with scope and type): \n");
      if (root == NULL) fprintf(G_listing, "<the tree is NULL>\n");
      A_print_complete_tree(root);
      fprintf(G_listing,"\n################################\n");
    }
    if (root) ANA_init_symbol_table(root);
    if (!G_error)
    { 
        ;
    }
#if !NO_CODE
    if(root) CG_generate_code(root, code_filename);
    if (! G_error)
    { 
        ;
    }
#endif
#endif
#endif

    fclose(G_source);
    return 0;
}
