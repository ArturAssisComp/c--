/*Main for the compiler C--*/
#include <stdio.h>
#include <stdlib.h>
#include "globals.h"
#include "scanner.h"

/*Define the global variables declared in the globals.h*/
FILE* G_source;  /* source code text file */
FILE* G_listing; /* listing output text file */
FILE* G_code;    /* code text file */
bool G_echo_source = true;
bool G_trace_scan  = true;
bool G_error       = true;

int main(int argc, char *argv[])
{
    if(argc == 1) G_source = stdin;
    else if (argc == 2) 
    {
        G_source = fopen(argv[1], "r");
        if(!G_source)
        {
            perror("fopen");
            exit(EXIT_FAILURE);        
        }
    }
    else 
    {
        fprintf(stderr, "Usage: %s <filename>", argv[0]);
        exit(EXIT_FAILURE);
    }

    G_listing = stdout;
    while(S_get_token() != ENDFILE);

    return 0;
}

