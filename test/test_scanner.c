#include "../ext/ctest_lib/include/ctest.h"
#include <stdio.h>
#include <stdlib.h>
#include "../src/globals.h"
#include "../src/scanner.h"
#include "../src/parser.tab.h"

#define MSG_SIZE 100



#define INIT_SCANNER(str) \
    tmp_file = fopen(tmp_filename, "w");\
    if(!tmp_file)\
    {\
        perror("fopen");\
        exit(EXIT_FAILURE);\
    }\
    fprintf(tmp_file, (str));\
    fclose(tmp_file);\
    G_source = fopen(tmp_filename, "r");\
    yyrestart(G_source);\
    yylineno = 1;\
    if(!G_source)\
    {\
        perror("fopen");\
        exit(EXIT_FAILURE);\
    }\
    do{;}while(0)

#define END_SCANNER() \
    fclose(G_source);\
    remove(tmp_filename);\
    do{;}while(0)




/*Define the global variables declared in the globals.h*/
FILE* G_source;  /* source code text file */
FILE* G_listing; /* listing output text file */
FILE* G_code;    /* code text file */
bool G_echo_source = false;
bool G_trace_scan  = false;
bool G_error       = false;

int main(void)
{
	char suite_name[] = "Test S_get_token function";
	char suite_description[] = "This test suite tests the scanner function S_get_token.";
	char *functions_tested[] = {
        "S_get_token",
		NULL
	};
	char error_msg[MSG_SIZE] = "";
	char msg[MSG_SIZE]; 
	bool ignore_case = false;
    char *expected_token_string;
    char tmp_filename[] = "./build/test/tmp_src_file.txt";
    int expected_lineno;
    FILE *tmp_file;
    G_token_type next_token;
    G_token_type expected_token_type;


	//------------------------------------------------------------------------------
	start_suite(suite_name, suite_description, functions_tested);
		//------------------------------------------------------------------------------
		start_module("EDGE CASES", "Module for testing edge cases.", functions_tested);
            /* Empty file */
            //Create temporary file:
            INIT_SCANNER("");
                //First token
                next_token = S_get_token();
                /*check token*/
                expected_token_type = ENDFILE;
                snprintf(msg, MSG_SIZE + 1, "next_token (%d) != endfile (%d)", next_token, expected_token_type);
                assert_integer_equal(next_token, expected_token_type, __LINE__, msg);
                /*check token_string*/
                expected_token_string = "";
                snprintf(msg, MSG_SIZE + 1, "Expects the string \"%s\"", expected_token_string);
                assert_string_equal(S_token_string, expected_token_string, ignore_case, __LINE__, msg);
                /*check line number*/
                expected_lineno = 1;
                snprintf(msg, MSG_SIZE + 1, "Expecter line number %d.", expected_lineno);
                assert_integer_equal(yylineno, expected_lineno, __LINE__, msg);
            END_SCANNER();
                //-----------
            /*------*/

            /* "\n" */
            //Create temporary file:
            INIT_SCANNER("\n");
                //First token
                next_token = S_get_token();
                /*check token*/
                expected_token_type = ENDFILE;
                snprintf(msg, MSG_SIZE + 1, "next_token (%d) != endfile (%d)", next_token, expected_token_type);
                assert_integer_equal(next_token, expected_token_type, __LINE__, msg);
                /*check token_string*/
                expected_token_string = "";
                snprintf(msg, MSG_SIZE + 1, "Expects the string \"%s\"", expected_token_string);
                assert_string_equal(S_token_string, expected_token_string, ignore_case, __LINE__, msg);
                /*check line number*/
                expected_lineno = 2;
                snprintf(msg, MSG_SIZE + 1, "Expecter line number %d.", expected_lineno);
                assert_integer_equal(yylineno, expected_lineno, __LINE__, msg);
            END_SCANNER();
                //-----------
            /*------*/


            /* "123\n" */
            //Create temporary file:
            INIT_SCANNER("123\n");
                //First token: "123"
                next_token = S_get_token();
                /*check token*/
                expected_token_type = NUM;
                snprintf(msg, MSG_SIZE + 1, "next_token (%d) != id (%d)", next_token, expected_token_type);
                assert_integer_equal(next_token, expected_token_type, __LINE__, msg);
                /*check token_string*/
                expected_token_string = "123";
                snprintf(msg, MSG_SIZE + 1, "Expects the string \"%s\"", expected_token_string);
                assert_string_equal(S_token_string, expected_token_string, ignore_case, __LINE__, msg);
                /*check line number*/
                expected_lineno = 1;
                snprintf(msg, MSG_SIZE + 1, "Expecter line number %d.", expected_lineno);
                assert_integer_equal(yylineno, expected_lineno, __LINE__, msg);
                //-----------

                //Second token:
                next_token = S_get_token();
                /*check token*/
                expected_token_type = ENDFILE;
                snprintf(msg, MSG_SIZE + 1, "next_token (%d) != id (%d)", next_token, expected_token_type);
                assert_integer_equal(next_token, expected_token_type, __LINE__, msg);
                /*check token_string*/
                expected_token_string = "";
                snprintf(msg, MSG_SIZE + 1, "Expects the string \"%s\"", expected_token_string);
                assert_string_equal(S_token_string, expected_token_string, ignore_case, __LINE__, msg);
                /*check line number*/
                expected_lineno = 2;
                snprintf(msg, MSG_SIZE + 1, "Expecter line number %d.", expected_lineno);
                assert_integer_equal(yylineno, expected_lineno, __LINE__, msg);
            END_SCANNER();
                //-----------
            /*------*/
            /*------*/
		end_module();
		//------------------------------------------------------------------------------
        
		start_module("DOMAIN CASES", "Module for testing domain cases.", functions_tested);
            /* Hello World */
            //Create temporary file:
            INIT_SCANNER("Hello World");
                //First token: "Hello"
                next_token = S_get_token();
                /*check token*/
                expected_token_type = ID;
                snprintf(msg, MSG_SIZE + 1, "next_token (%d) != id (%d)", next_token, expected_token_type);
                assert_integer_equal(next_token, expected_token_type, __LINE__, msg);
                /*check token_string*/
                expected_token_string = "Hello";
                snprintf(msg, MSG_SIZE + 1, "Expects the string \"%s\"", expected_token_string);
                assert_string_equal(S_token_string, expected_token_string, ignore_case, __LINE__, msg);
                /*check line number*/
                expected_lineno = 1;
                snprintf(msg, MSG_SIZE + 1, "Expecter line number %d.", expected_lineno);
                assert_integer_equal(yylineno, expected_lineno, __LINE__, msg);
                //-----------

                //Second token: "World"
                next_token = S_get_token();
                /*check token*/
                expected_token_type = ID;
                snprintf(msg, MSG_SIZE + 1, "next_token (%d) != id (%d)", next_token, expected_token_type);
                assert_integer_equal(next_token, expected_token_type, __LINE__, msg);
                /*check token_string*/
                expected_token_string = "World";
                snprintf(msg, MSG_SIZE + 1, "Expects the string \"%s\"", expected_token_string);
                assert_string_equal(S_token_string, expected_token_string, ignore_case, __LINE__, msg);
                /*check line number*/
                expected_lineno = 1;
                snprintf(msg, MSG_SIZE + 1, "Expecter line number %d.", expected_lineno);
                assert_integer_equal(yylineno, expected_lineno, __LINE__, msg);
                //-----------
                
                //Third token:
                next_token = S_get_token();
                /*check token*/
                expected_token_type = ENDFILE;
                snprintf(msg, MSG_SIZE + 1, "next_token (%d) != id (%d)", next_token, expected_token_type);
                assert_integer_equal(next_token, expected_token_type, __LINE__, msg);
                /*check token_string*/
                expected_token_string = "";
                snprintf(msg, MSG_SIZE + 1, "Expects the string \"%s\"", expected_token_string);
                assert_string_equal(S_token_string, expected_token_string, ignore_case, __LINE__, msg);
                /*check line number*/
                expected_lineno = 1;
                snprintf(msg, MSG_SIZE + 1, "Expecter line number %d.", expected_lineno);
                assert_integer_equal(yylineno, expected_lineno, __LINE__, msg);
            END_SCANNER();
                //-----------
		end_module();
	end_suite();
	//------------------------------------------------------------------------------
}
