#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>

#include "lambda_expr.h"
#include "lambda_parser.h"

int main( int argc, char * argv[] ) {
    long file_size;
    FILE * fp = fopen( argv[1], "r" );
    fseek( fp, 0, SEEK_END );
    file_size = ftell( fp );
    char * lambda_code = malloc( ( file_size + 1 ) * sizeof(char) );
    int fd = open( argv[1], O_RDONLY, 0 );
    read( fd, lambda_code, file_size * sizeof(char) );
    lambda_code[file_size] = '\0';

    char * error = NULL;
    fill_TABLE();
    fill_GOTO();
    fill_TRIE();
    init_NODE_HASH();
    struct lambda_expr * parsed_result = parse( lambda_code, &error );

    set_global_V( hashlookup( get_NODE_HASH(), "VAR" )->data );
    
    printf( "Before evaluating the parsed result, it looks like: <br />\n" );
    printf( "%s<br />\n", print_lambda_expr( parsed_result ) );

    struct lambda_expr * evaluated_result = evaluate( parsed_result );

    printf( "After evaluating the result, it looks like: <br />\n" );
    printf( "%s<br />\n", print_lambda_expr( evaluated_result ) );
    
    /*

    struct lambda_expr * root = new_lambda_expr();
    strcpy( root->type, "ROOT" );
    root->child[0] = parsed_result;
    parsed_result->parents += 1;

    struct lambda_search_state * state1 = new_lambda_search_state();
    state1->parent = NULL;
    state1->current = root;
    state1->parent_choice = 0;
    state1->progress = 0;

    struct lambda_search_state * state2 = new_lambda_search_state();
    state2->parent = root;
    state2->current = parsed_result;
    state2->parent_choice = 0;
    state2->progress = 0;

    struct list * stack = new_list();
    append_to_list( stack, state1 );
    append_to_list( stack, state2 );

    update_variables( root );
    printf( "%s<br />\n", print_lambda_expr( root ) );
    int change = 0;

    // Repeatedly perform a lazy traversal, 
    // stopping each time a beta reduction is processed to update variables.
printf("About to perform the first pre_order_traversal.\n");
    while ( change = pre_order_traverse( stack, &process_beta_reduction, 1 ) ) {
	update_variables( root );
	printf( "%s<br />\n", print_lambda_expr( root ) );
    }
    */
}
