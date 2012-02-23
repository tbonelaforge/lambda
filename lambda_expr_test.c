#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "hash.h"
#include "list.h"
#include "lambda_expr.h"

int main() {

    struct lambda_expr * node1, * node2, * node3, * node4, * node5, * node6, * node7, * node8, * node9, * node10, * node11, * node12, * root;

/***** // Testing the print function.

    struct lambda_expr * node1 = new_lambda_expr();
    strcpy( node1->type, "VAR" );
    node1->data = "x";
    add_to_hash( node1->free, "x", (void *) 1 );

    struct lambda_expr * node2 = new_lambda_expr();
    strcpy( node2->type, "VAR" );
    node2->data = "y";
    add_to_hash( node2->free, "y", (void *) 1 );

    struct lambda_expr * node3 = new_lambda_expr();
    strcpy( node3->type, "VAR" );
    node3->data = "y";
    add_to_hash( node3->free, "y", (void *) 1 );

    struct lambda_expr * node4 = new_lambda_expr();
    strcpy( node4->type, "VAR" );
    node4->data = "z";
    add_to_hash( node4->free, "z", (void *) 1 );

    struct lambda_expr * node5 = new_lambda_expr();
    strcpy( node5->type, "VAR" );
    node5->data = "y";
    add_to_hash( node5->free, "y", (void *) 1 );

    struct lambda_expr * node6 = new_lambda_expr();
    strcpy( node6->type, "BETA" );
    add_pairs_to_hash( node6->free, 2,
		       "x", (void *) 1,
		       "y", (void *) 1
		       );
    node6->child[0] = node1;
    node6->child[1] = node2;

    struct lambda_expr * node7 = new_lambda_expr();
    strcpy( node7->type, "VAR" );
    node7->data = "z";
    add_to_hash( node7->free, "z", (void *) 1 );

    struct lambda_expr * node8 = new_lambda_expr();
    strcpy( node8->type, "LAMBDA" );
    add_to_hash( node8->bound, "y", (void *) 1 );
    add_to_hash( node8->free, "z", (void *) 1 );
    node8->child[0] = node3;
    node8->child[1] = node4;

    struct lambda_expr * node9 = new_lambda_expr();
    strcpy( node9->type, "LAMBDA" );
    add_to_hash( node9->bound, "y", (void *) 1 );
    add_to_hash( node9->free, "x", (void *) 1 );
    node9->child[0] = node5;
    node9->child[1] = node6;

    struct lambda_expr * node10 = new_lambda_expr();
    strcpy( node10->type, "LAMBDA" );
    add_pairs_to_hash( node10->bound, 2,
		       "y", (void *) 1, 
		       "z", (void *) 1
		       );
    node10->child[0] = node7;
    node10->child[1] = node8;

    struct lambda_expr * node11 = new_lambda_expr();
    strcpy( node11->type, "VAR" );
    node11->data = "x";
    add_to_hash( node11->free, "x", (void *) 1 );
    
    struct lambda_expr * node12 = new_lambda_expr();
    strcpy( node12->type, "BETA" );
    add_pairs_to_hash( node12->bound, 2,
		       "y", (void *) 1 ,
		       "z", (void *) 1
		       );
    add_to_hash( node12->free, "x", (void *) 1 );
    node12->child[0] = node9;
    node12->child[1] = node10;

    struct lambda_expr * root = new_lambda_expr();
    strcpy( root->type, "LAMBDA" );
    add_pairs_to_hash( root->bound, 3,
		       "x", (void *) 1,
		       "y", (void *) 1,
		       "z", (void *) 1
		       );
    root->child[0] = node11;
    root->child[1] = node12;

    printf( "The expression looks like:\n<br />" );
    printf( "%s", print_lambda_expr(root) );

    // End Testing the print function 
****/

/**** // Begin Testing the update_variables function


    struct lambda_expr * node1 = new_lambda_expr();
    strcpy( node1->type, "VAR" );
    node1->data = "x";

    struct lambda_expr * node2 = new_lambda_expr();
    strcpy( node2->type, "VAR" );
    node2->data = "y";

    struct lambda_expr * node3 = new_lambda_expr();
    strcpy( node3->type, "VAR" );
    node3->data = "y";

    struct lambda_expr * node4 = new_lambda_expr();
    strcpy( node4->type, "VAR" );
    node4->data = "z";

    struct lambda_expr * node5 = new_lambda_expr();
    strcpy( node5->type, "VAR" );
    node5->data = "y";

    struct lambda_expr * node6 = new_lambda_expr();
    strcpy( node6->type, "BETA" );

    node6->child[0] = node1;
    node6->child[1] = node2;

    struct lambda_expr * node7 = new_lambda_expr();
    strcpy( node7->type, "VAR" );
    node7->data = "z";

    struct lambda_expr * node8 = new_lambda_expr();
    strcpy( node8->type, "LAMBDA" );
    node8->child[0] = node3;
    node8->child[1] = node4;

    struct lambda_expr * node9 = new_lambda_expr();
    strcpy( node9->type, "LAMBDA" );
    node9->child[0] = node5;
    node9->child[1] = node6;

    struct lambda_expr * node10 = new_lambda_expr();
    strcpy( node10->type, "LAMBDA" );
    node10->child[0] = node7;
    node10->child[1] = node8;

    struct lambda_expr * node11 = new_lambda_expr();
    strcpy( node11->type, "VAR" );
    node11->data = "x";
    
    struct lambda_expr * node12 = new_lambda_expr();
    strcpy( node12->type, "BETA" );
    node12->child[0] = node9;
    node12->child[1] = node10;

    struct lambda_expr * root = new_lambda_expr();
    strcpy( root->type, "LAMBDA" );
    root->child[0] = node11;
    root->child[1] = node12;


    printf( "Before running update_varialbes,  expression looks like:\n<br />" );
    printf( "%s", print_lambda_expr(root) );

    update_variables(root);
    printf( "AFter updating variables, the expression looks like:\n<br />" );
    printf( "%s", print_lambda_expr(root) );

    // Simulate substituting node10 for occurrences of 'y' ( node2 ) in node6.
    remove_lambda_expr_node(node2);
    node6->child[1] = node10;
    remove_lambda_expr_node(node5);
    remove_lambda_expr_node(node9);
    remove_lambda_expr_node(node12);
    root->child[1] = node6;

    // Now print out the intermediate step...
    printf( "After simulating a substitution, the expression looks like:\n<br />" );
    printf( "%s\n<br />", print_lambda_expr(root) );

    // Update the variables, and print the result again.
    update_variables(root);
    printf( "AFter updating the variables, the expression looks like:\n<br />" );
    printf( "%s\n<br />", print_lambda_expr(root) );


    /****/ // End testing update_variables function.

    /**** // Begin testing fresh_variable function.
    struct hash * test_V = new_hash();
    add_pairs_to_hash( test_V, 3,
		       "x", (void *) 1,
		       "x'", (void *) 1,
		       "x'y", (void *) 1
		       );
    char * fresh_variable_result = fresh_variable( "x", test_V );
    printf( "The result of fresh_variable is: <br />\n" );
    printf( "%s<br />\n", fresh_variable_result );
    /****/ // End testing fresh_variable function.

    /**** // Begin testing rename function.
    char * fstring = malloc( 2*sizeof(char));
    strcpy( fstring, "f" );
    char * xstring = malloc( 2*sizeof(char));
    strcpy( xstring, "x" );
    node1 = new_lambda_expr();
    strcpy( node1->type, "VAR" );
    node1->data = fstring;
    node2 = new_lambda_expr();
    strcpy( node2->type, "VAR" );
    node2->data = xstring;
    node3 = new_lambda_expr();
    strcpy( node3->type, "VAR" );
    node3->data = fstring;
    node4 = new_lambda_expr();
    strcpy( node4->type, "BETA" );
    node4->child[0] = node1;
    node4->child[1] = node2;
    node5 = new_lambda_expr();
    strcpy( node5->type, "VAR" );
    node5->data = xstring;
    node6 = new_lambda_expr();
    strcpy( node6->type, "BETA" );
    node6->child[0] = node3;
    node6->child[1] = node4;
    node7 = new_lambda_expr();
    strcpy( node7->type, "VAR" );
    node7->data = fstring;
    node8 = new_lambda_expr();
    strcpy( node8->type, "LAMBDA" );
    node8->child[0] = node5;
    node8->child[1] = node6;
    root = new_lambda_expr();
    strcpy( root->type, "LAMBDA" );
    root->child[0] = node7;
    root->child[1] = node8;

    update_variables(root);
    printf( "The node about to have rename_var run on it  looks like: \n" );
    printf( "%s<br />\n", print_lambda_expr(root) );

    char * fprime_string = malloc( 3*sizeof(char) );
    strcpy( fprime_string, "f'" );
    rename_var( fstring, fprime_string, root );
    printf( "AFter renaming f to f', the expression looks like: <br />\n" );
    printf( "%s<br />\n", print_lambda_expr(root) );
    update_variables(root);
    printf( "After updating variables, the expression looks like: <br />\n" );
    printf( "%s<br />\n", print_lambda_expr(root) );
    
    /****/ // End testing rename function.


    /**** // Begin testing substitute function.

    // Use centralized strings.
    char * xstring = "x";
    char * ystring = "y";
    char * fstring = "f";

    // Build source expression.
    node1 = new_lambda_expr();
    strcpy( node1->type, "VAR" );
    node1->data = fstring;

    node2 = new_lambda_expr();
    strcpy( node2->type, "VAR" );
    node2->data = xstring;

    node3 = new_lambda_expr();
    strcpy( node3->type, "VAR" );
    node3->data = xstring;
    
    node4 = new_lambda_expr();
    strcpy( node4->type, "BETA" );
    node4->child[0] = node1;
    node4->child[1] = node2;

    struct lambda_expr * source = new_lambda_expr();
    strcpy( source->type, "LAMBDA" );
    source->child[0] = node3;
    source->child[1] = node4;
    update_variables(source);

    // Build expr
    node1 = new_lambda_expr();
    strcpy( node1->type, "VAR" );
    node1->data = xstring;
    
    node2 = new_lambda_expr();
    strcpy( node2->type, "VAR" );
    node2->data = ystring;

    node3 = new_lambda_expr();
    strcpy( node3->type, "VAR" );
    node3->data = ystring;

    node4 = new_lambda_expr();
    strcpy( node4->type, "BETA" );
    node4->child[0] = node1;
    node4->child[1] = node2;

    struct lambda_expr * expr = new_lambda_expr();
    strcpy( expr->type, "LAMBDA" );
    expr->child[0] = node3;
    expr->child[1] = node4;

    update_variables(expr);

    struct hash * V = new_hash();
    add_pairs_to_hash( V, 3,
		       xstring, (void *) 1,
		       ystring, (void *) 1,
		       fstring, (void *) 1
		       );

    printf( "The source expression looks like: <br />\n" );
    printf( "%s<br />\n", print_lambda_expr(source) );

    printf( "The expr looks like: <br />\n" );
    printf( "%s<br />\n", print_lambda_expr(expr) );

    struct lambda_expr * result = substitute( expr, fstring, source, V );

    update_variables(result);

    printf( "After substituting expr for occurrences of f in source, the result looks like: <br />\n" );
    printf( "%s<br />\n", print_lambda_expr(result) );
    

    /****/ // End testing substitute function.
    

    /**** // Begin testing search_lambda_expr.

    // Build lambda_expr.
    char * xstring = "x";
    char * ystring = "y";
    char * wstring = "w";
    char * zstring = "z";
    node1 = new_lambda_expr();
    strcpy( node1->type, "VAR" );
    node1->data = ystring;

    node2 = new_lambda_expr();
    strcpy( node2->type, "VAR" );
    node2->data = wstring;

    node3 = new_lambda_expr();
    strcpy( node3->type, "VAR" );
    node3->data = wstring;

    node4 = new_lambda_expr();
    strcpy( node4->type, "VAR" );
    node4->data = zstring;

    node5 = new_lambda_expr();
    strcpy( node5->type, "VAR" );
    node5->data = xstring;

    node6 = new_lambda_expr();
    strcpy( node6->type, "VAR" );
    node6->data = ystring;

    node7 = new_lambda_expr();
    strcpy( node7->type, "LAMBDA" );
    node7->child[0] = node1;
    node7->child[1] = node2;

    node8 = new_lambda_expr();
    strcpy( node8->type, "BETA" );
    node8->child[0] = node3;
    node8->child[1] = node4;

    node9 = new_lambda_expr();
    strcpy( node9->type, "LAMBDA" );
    node9->child[0] = node5;
    node9->child[1] = node6;

    node10 = new_lambda_expr();
    strcpy( node10->type, "BETA" );
    node10->child[0] = node7;
    node10->child[1] = node8;

    root = new_lambda_expr();
    strcpy( root->type, "LAMBDA" );
    root->child[0] = node9;
    root->child[1] = node10;

    update_variables(root);

    printf( "Before searching for the next beta-reduction, the expression looks like: <br />\n" );
    printf( "%s<br />\n", print_lambda_expr(root) );
    
    struct list * stack = new_list();
    struct lambda_search_state result = search_lambda_expr( root, stack );
    printf( "The first candidate for beta-reduction looks like: <br />\n" );
    printf( "%s<br />\n", print_lambda_expr(result.current) );

    struct hash * V = new_hash();
    expand_variables( V, result.current->bound );
    expand_variables( V, result.current->free );
    reduce( result, stack, V );

    printf( "After beta-reduction, the entire tree looks like:<br />\n" );
    printf( "%s<br />\n", print_lambda_expr(root) );

    /****/ // End testing search_lambda_expr.

    /**** // Begin testing reduce function.

    char * fstring = "f";
    char * xstring = "x";

    node1 = new_lambda_expr();
    strcpy( node1->type, "VAR" );
    node1->data = fstring;

    node2 = new_lambda_expr();
    strcpy( node2->type, "VAR" );
    node2->data = xstring;

    node3 = new_lambda_expr();
    strcpy( node3->type, "BETA" );
    node3->child[0] = node1;
    node3->child[1] = node2;

    node4 = new_lambda_expr();
    strcpy( node4->type, "LAMBDA" );
    node4->child[0] = node2;
    node4->child[1] = node3;

    node5 = new_lambda_expr();
    strcpy( node5->type, "BETA" );
    node5->child[0] = node4;
    node5->child[1] = node3;

    node6 = new_lambda_expr();
    strcpy( node6->type, "LAMBDA" );
    node6->child[0] = node1;
    node6->child[1] = node5;

    update_variables( node6 );
    printf("Before searching for a valid beta-reduction, node6 looks like:<br />\n");
    printf( "%s<br />\n", print_lambda_expr(node6) );

    struct list * stack = new_list();
    struct lambda_search_state search_state = search_lambda_expr( node6, stack );
    printf( "The first candidate for beta-reduction looks like:<br />\n" );
    printf( "%s<br />\n", print_lambda_expr( search_state.current ) );

    struct hash * V = new_hash();
    expand_variables( V, node6->bound );
    expand_variables( V, node6->free );

    reduce( search_state, stack, V );
    update_variables( node6 );
    printf( "After reduction, the entire expression looks like:<br />\n" );
    printf( "%s<br />\n", print_lambda_expr( node6 ) );


    /****/ // End testing 

    // Begin testing a single beta-reduction.
    /*
    char * xstring = malloc( 2 * sizeof( char ) );
    sprintf( xstring, "x" );
printf( "just allocated xstring: %p<br />\n", xstring );
    char * fstring = malloc( 2 * sizeof( char ) );
    sprintf( fstring, "f" );
    char * ystring = malloc( 2 * sizeof( char ) );
    sprintf( ystring, "y" );
    node1 = new_lambda_expr();
    strcpy( node1->type, "VAR" );
    node1->data = xstring;
    add_to_global_V( xstring, node1 );

    node2 = new_lambda_expr();
    strcpy( node2->type, "VAR" );
    node2->data = ystring;
    add_to_global_V( ystring, node2 );
    
    node3 = new_lambda_expr();
    strcpy( node3->type, "VAR" );
    node3->data = fstring;
    add_to_global_V( fstring, node3 );

    node4 = new_lambda_expr();
printf( "node4 is: %p<br />\n", node4 );
    strcpy( node4->type, "BETA" );
    node4->child[0] = node1; node1->parents += 1;
    node4->child[1] = node2; node2->parents += 1;

    node5 = new_lambda_expr();
    strcpy( node5->type, "LAMBDA" );
    node5->child[0] = node1; node1->parents += 1;
    node5->child[1] = node4; node4->parents += 1;

    node6 = new_lambda_expr();
printf( "node6 is: %p<br />\n", node6 );
    strcpy( node6->type, "BETA" );
    node6->child[0] = node3; node3->parents += 1;
    node6->child[1] = node2; node2->parents += 1;

    node7 = new_lambda_expr();
    strcpy( node7->type, "BETA" );
printf( "node7 is: %p<br />\n", node7 );
    node7->child[0] = node5; node5->parents += 1;
    node7->child[1] = node6; node6->parents += 1;

    node8 = new_lambda_expr();
    strcpy( node8->type, "ROOT" );
    node8->child[0] = node7; node7->parents += 1;

    update_variables( node8 );
printf( "The whole expression looks like:<br />\n" );
printf( "%s<br />\n", print_lambda_expr( node8 ) );

    struct lambda_search_state * state1 = new_lambda_search_state();
    state1->parent = NULL;
    state1->current = node8;
    state1->parent_choice = 0;
    state1->progress = 0;

    struct lambda_search_state * state2 = new_lambda_search_state();
    state2->parent = node8;
    state2->current = node7;
    state2->parent_choice = 0;
    state2->progress = 0;

    struct list * stack = new_list();
    append_to_list( stack, (void *) state1 );
    append_to_list( stack, (void *) state2 );

    printf( "about to try and perform a beta-reduction.<br />\n" );
    struct lambda_expr * reduced = pre_order_traverse( stack, &process_beta_reduction );
    update_variables( node8 );
printf( "after reduction, the whole thing looks like:<br />\n" );
printf( "%s<br />\n", print_lambda_expr( node8 ) );
    printf( "The result of the beta-reduction (%p) is:<br />\n", reduced );
    printf( "%s<br />\n", print_lambda_expr( reduced ) );
    */

    /*
    char * xstring = malloc( 2 * sizeof( char ) );
    sprintf( xstring, "x" );
    char * ystring = malloc( 2 * sizeof( char ) );
    sprintf( ystring, "y" );

    node1 = new_lambda_expr();
    strcpy( node1->type, "VAR" );
    node1->data = xstring;
    add_to_global_V( xstring, node1 );

    node2 = new_lambda_expr();
    strcpy( node2->type, "VAR" );
    node2->data = ystring;
    add_to_global_V( ystring, node2 );

    node3 = new_lambda_expr();
    strcpy( node3->type, "LAMBDA" );
    node3->child[0] = node1; node1->parents += 1;
    node3->child[1] = node1; node1->parents += 1;

    node4 = new_lambda_expr();
    strcpy( node4->type, "LAMBDA" );
    node4->child[0] = node1; node1->parents += 1;
    node4->child[1] = node3; node3->parents += 1;

    node5 = new_lambda_expr();
    strcpy( node5->type, "BETA" );
    node5->child[0] = node4; node4->parents += 1;
    node5->child[1] = node2; node2->parents += 1;

    node6 = new_lambda_expr();
    strcpy( node6->type, "ROOT" );
    node6->child[0] = node5; node5->parents += 1;

    update_variables( node6 );
printf( "The whole expression looks like:<br />\n" );
printf( "%s<br />\n", print_lambda_expr( node6 ) );

    struct lambda_search_state * state1 = new_lambda_search_state();
    state1->parent = NULL;
    state1->current = node6;
    state1->parent_choice = 0;
    state1->progress = 0;

    struct lambda_search_state * state2 = new_lambda_search_state();
    state2->parent = node6;
    state2->current = node5;
    state2->parent_choice = 0;
    state2->progress = 0;

    struct list * stack = new_list();
    append_to_list( stack, (void *) state1 );
    append_to_list( stack, (void *) state2 );

    printf( "about to try and perform a beta-reduction.<br />\n" );
    struct lambda_expr * reduced = pre_order_traverse( stack, &process_beta_reduction );
    update_variables( node6 );
printf( "after reduction, the whole thing looks like:<br />\n" );
printf( "%s<br />\n", print_lambda_expr( node6 ) );
    printf( "The result of the beta-reduction (%p) is:<br />\n", reduced );
    printf( "%s<br />\n", print_lambda_expr( reduced ) );
*/

    /*
    char * xstring = malloc( 2 * sizeof( char ) );
    sprintf( xstring, "x" );
    char * ystring = malloc( 2 * sizeof( char ) );
    sprintf( ystring, "y" );
    char * zstring = malloc( 2 * sizeof( char ) );
    sprintf( zstring, "z" );

    node1 = new_lambda_expr();
    strcpy( node1->type, "VAR" );
    node1->data = xstring;
    add_to_global_V( xstring, node1 );

    node2 = new_lambda_expr();
    strcpy( node2->type, "VAR" );
    node2->data = ystring;
    add_to_global_V( ystring, node2 );

    node3 = new_lambda_expr();
    strcpy( node3->type, "VAR" );
    node3->data = zstring;

    node4 = new_lambda_expr();
    strcpy( node4->type, "LAMBDA" );
    node4->child[0] = node1; node1->parents += 1;
    node4->child[1] = node1; node1->parents += 1;

    node5 = new_lambda_expr();
    strcpy( node5->type, "BETA" );
    node5->child[0] = node4; node4->parents += 1;
    node5->child[1] = node3; node3->parents += 1;

    node6 = new_lambda_expr();
    strcpy( node6->type, "LAMBDA" );
    node6->child[0] = node2; node3->parents += 1;
    node6->child[1] = node5; node5->parents += 1;

    node7 = new_lambda_expr();
    strcpy( node7->type, "ROOT" );
    node7->child[0] = node6; node6->parents += 1;

    update_variables( node7 );
printf( "The whole expression looks like:<br />\n" );
printf( "%s<br />\n", print_lambda_expr( node7 ) );

    struct lambda_search_state * state1 = new_lambda_search_state();
    state1->parent = NULL;
    state1->current = node7;
    state1->parent_choice = 0;
    state1->progress = 0;

    struct lambda_search_state * state2 = new_lambda_search_state();
    state2->parent = node7;
    state2->current = node6;
    state2->parent_choice = 0;
    state2->progress = 0;

    struct list * stack = new_list();
    append_to_list( stack, (void *) state1 );
    append_to_list( stack, (void *) state2 );

    printf( "about to try and perform a beta-reduction.<br />\n" );
    struct lambda_expr * reduced = pre_order_traverse( stack, &process_beta_reduction );
    update_variables( node7 );
printf( "after reduction, the whole thing looks like:<br />\n" );
printf( "%s<br />\n", print_lambda_expr( node7 ) );
    printf( "The result of the beta-reduction (%p) is:<br />\n", reduced );
    printf( "%s<br />\n", print_lambda_expr( reduced ) );
    */

    char * xstring = malloc( 2 * sizeof( char ) );
    sprintf( xstring, "x" );
    char * ystring = malloc( 2 * sizeof( char ) );
    sprintf( ystring, "y" );
    char * zstring = malloc( 2 * sizeof( char ) );
    sprintf( zstring, "z" );

    node1 = new_lambda_expr();
    strcpy( node1->type, "VAR" );
    node1->data = ystring;
    add_to_global_V( ystring, node1 );

    node2 = new_lambda_expr();
    strcpy( node2->type, "VAR" );
    node2->data = zstring;
    add_to_global_V( zstring, node2 );

    node3 = new_lambda_expr();
    strcpy( node3->type, "LAMBDA" );
    node3->child[0] = node2; node2->parents += 1;
    node3->child[1] = node1; node1->parents += 1;

    node4 = new_lambda_expr();
    strcpy( node4->type, "BETA" );
    node4->child[0] = node1; node1->parents += 1;
    node4->child[1] = node2; node2->parents += 1;

    node5 = new_lambda_expr();
    strcpy( node5->type, "LAMBDA" );
    node5->child[0] = node1; node1->parents += 1;
    node5->child[1] = node3; node3->parents += 1;

    node6 = new_lambda_expr();
    strcpy( node6->type, "BETA" );
    node6->child[0] = node5; node5->parents += 1;
    node6->child[1] = node4; node4->parents += 1;


    node7 = new_lambda_expr();
    strcpy( node7->type, "ROOT" );
    node7->child[0] = node6; node6->parents += 1;

    update_variables( node7 );
printf( "The whole expression looks like:<br />\n" );
printf( "%s<br />\n", print_lambda_expr( node7 ) );

    struct lambda_search_state * state1 = new_lambda_search_state();
    state1->parent = NULL;
    state1->current = node7;
    state1->parent_choice = 0;
    state1->progress = 0;

    struct lambda_search_state * state2 = new_lambda_search_state();
    state2->parent = node7;
    state2->current = node6;
    state2->parent_choice = 0;
    state2->progress = 0;

    struct list * stack = new_list();
    append_to_list( stack, (void *) state1 );
    append_to_list( stack, (void *) state2 );

    printf( "about to try and perform a beta-reduction.<br />\n" );
    struct lambda_expr * reduced = pre_order_traverse( stack, &process_beta_reduction );
printf( "Got back from the outer pre_order_traverse call.<br />\n" );
    update_variables( node7 );
printf( "after reduction, the whole thing looks like:<br />\n" );
printf( "%s<br />\n", print_lambda_expr( node7 ) );
    printf( "The result of the beta-reduction (%p) is:<br />\n", reduced );
    printf( "%s<br />\n", print_lambda_expr( reduced ) );
    


}
