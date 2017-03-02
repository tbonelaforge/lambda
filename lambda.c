#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
#include <unistd.h>

#include "lambda_parser.h"
#include "lambda_expr.h"
#include "charhash.h"
#include "hash.h"

#include "list.h"

struct hash * NAMESPACE = NULL; // Will index all the expressions, by name.


////////////////////////////////////////////////////////////
// Reduction Functions /////////////////////////////////////
////////////////////////////////////////////////////////////

/*============================================================

Function: compress_HASH2

Input: An integer ( prod_num ) and a pointer to 
       the parsing stack which just recognized
       the production HASH -> HASH newline STMT.

Output: A node of type HASH, having no children.

============================================================*/

struct lambda_expr * compress_HASH2( int prod_num, struct list * stack ) {
    pop_from_list( stack ); // Old state number.
    pop_from_list( stack ); // STMT node ( already referenced in NAMESPACE. )
    pop_from_list( stack ); // Old state number.
    free( pop_from_list( stack ) ); // Unneeded token ( newline ).
    pop_from_list( stack ); // Old state number.
    struct lambda_expr * hash_place_holder = pop_from_list( stack );
    return hash_place_holder;
}

/*============================================================

Function: compress_HASH1

Input: an integer ( prod_num ) and a pointer to a list ( stack )

Output: A node of type HASH, with no children

============================================================*/

struct lambda_expr * compress_HASH1( int prod_num, struct list * stack ) {
    pop_from_list( stack ); // Old state number.
    pop_from_list( stack ); // STMT node ( already referenced in NAMESPACE. )
    struct lambda_expr * hash_place_holder = new_lambda_expr();
    strcpy( hash_place_holder->type, "HASH" );
    return hash_place_holder;
}

/*============================================================

Function: install_name

Input: An integer ( prod_num ), and a pointer to a list ( stack ),
       which has just recognized the production STMT -> LHS RHS.

Output: A node of type LAMBDA, BETA, or VAR, 
        representing the RHS of the recognized production.
        Furthermore, the RHS expression has been installed
        in NAMESPACE, under the key specified by LHS.
        

============================================================*/


struct lambda_expr * install_name( int prod_num, struct list * stack ) {
    struct lambda_expr * rhs_expr;
    struct lambda_expr * lhs_text;
    int name_length;
    char * name;
    struct hash * var_hash;
    pop_from_list( stack ); // Old state number.
    rhs_expr = pop_from_list( stack );
    pop_from_list( stack ); // Old state number.
    lhs_text = pop_from_list( stack );
    name_length = lhs_text->last - lhs_text->first + 1;
    name = malloc( ( name_length + 1 ) * sizeof( char ) );
    strncpy( name, get_INPUT() + lhs_text->first, name_length );
    name[name_length] = '\0';
    try_to_delete( lhs_text );
    var_hash = hashlookup( get_NODE_HASH(), "VAR" )->data;
    struct hash * looked_up = hashlookup( var_hash, name );
    if ( looked_up ) { // A VAR like this already exists.
        add_to_hash( NAMESPACE, name, (void *) rhs_expr );
        free( name );
    }
    else { // Create a VAR to avoid covering this name.
        struct lambda_expr * var_placeholder = new_lambda_expr();
        strcpy( var_placeholder->type, "VAR" );
        var_placeholder->data = name;
        add_to_hash( var_hash, name, (void *) var_placeholder );
        add_to_hash( NAMESPACE, name, (void *) rhs_expr );
    }
    rhs_expr->parents++;
    update_variables( rhs_expr );
    return rhs_expr;
}


/*============================================================

Function: compress_LHS

Input: An integer ( prod_num ), and a pointer to a list ( stack ),
       which has just recognized the production LHS -> TEXT colon.

Output: A pointer to a lambda_expr of type TEXT.
        

============================================================*/

struct lambda_expr * compress_LHS( int prod_num, struct list * stack ) {
    pop_from_list( stack ); // Old state number.
    free( pop_from_list( stack ) ); // Unneeded token ( colon ).
    pop_from_list( stack ); // Old state number.
    return pop_from_list( stack ); // TEXT node.
}


/*============================================================

Function: compress_RHS

Input: An integer ( prod_num ), and a pointer to a list ( stack ),
       which has just recognized the production RHS -> EXPR semicolon.

Output: A pointer to a node of type LAMBDA, BETA, or VAR, 
        depending on what the EXPR was.

============================================================*/

struct lambda_expr * compress_RHS( int prod_num, struct list * stack ) {
    pop_from_list( stack ); // Old state number.
    free( pop_from_list( stack ) ); // Unneeded token ( semicolon ).
    pop_from_list( stack ); // Old state number.
    return pop_from_list( stack );
}


/*============================================================

Function: transitive_down

Input: A production number ( int ), and a pointer to a stack.

Output: A pointer to a finished lambda_expr,
        based on the one on top of the stack, 
	which has been removed from the stack.

============================================================*/

struct lambda_expr * transitive_down( int prod_num, struct list * stack ) {
    pop_from_list( stack ); // Throw away old state.
    struct lambda_expr * node = pop_from_list( stack );
    return node;
}


/*============================================================

Function: compress_BETA

Input: A production number ( int ), and a pointer to a stack.

Output: A pointer to a finished node, 
        representing the compressed tree for the specified production,
	realized with the child nodes on top of the stack,
	which have been removed from the stack.

============================================================*/

struct lambda_expr * compress_BETA( int prod_num, struct list * stack ) {
    pop_from_list( stack ); // Throw away an old state.
    free( pop_from_list( stack ) ); // Throw away the "rparen" token.
    pop_from_list( stack ); // Throw away an old state.
    struct lambda_expr * right = pop_from_list( stack ); // Get the second expr.
    pop_from_list( stack ); // Throw away an old state.
    free( pop_from_list( stack ) ); // Throw away the "comma" token.
    pop_from_list( stack ); // Throw away an old state.
    struct lambda_expr * left = pop_from_list( stack ); // Get first expr.
    pop_from_list( stack ); // Throw away an old state.
    free( pop_from_list( stack ) ); // Throw away the "lparen" token.
    char * data = NULL;
    char * key = hash_key( left, right, data );
    struct lambda_expr * node;
    struct hash * hash_node;
    struct hash * BETAS = hashlookup( get_NODE_HASH(), "BETA" )->data;
    if ( (hash_node = hashlookup( BETAS, key )) ) {
	free( key );
	return hash_node->data;
    }
    node = new_lambda_expr();
    strcpy( node->type, "BETA" );
    node->child[0] = left;
    node->child[1] = right;
    left->parents += 1;
    right->parents += 1;
    add_to_hash( BETAS, key, (void *) node );
    return node;
}

/*============================================================

Function: compress_LAMBDA1

Input: A production number ( int ), and a pointer to a stack.

Output: A pointer to a finished node, 
        representing the compressed tree for the specified production,
	realized with the child nodes on top of the stack,
	which have been removed from the stack.

============================================================*/

struct lambda_expr * compress_LAMBDA1( int prod_num, struct list * stack ) {
    pop_from_list( stack ); // Throw away an old state.
    struct lambda_expr * body = pop_from_list( stack ); // Get the lambda body.
    pop_from_list( stack ); // Throw away an old state.
    struct lambda_expr * var = pop_from_list( stack ); // Get the VAR node.
    pop_from_list( stack ); // Throw away an old state.
    free( pop_from_list( stack ) ); // Throw away the "backslash" token.
    char * data = NULL;
    char * key = hash_key( var, body, data );
    struct lambda_expr * node;
    struct hash * hash_node;
    struct hash * LAMBDAS = hashlookup( get_NODE_HASH(), "LAMBDA" )->data;
    if ( (hash_node = hashlookup( LAMBDAS, key )) ) {
	free( key );
	return hash_node->data;
    }
    node = new_lambda_expr();
    strcpy( node->type, "LAMBDA" );
    node->child[0] = var;
    node->child[1] = body;
    var->parents += 1;
    body->parents += 1;
    add_to_hash( LAMBDAS, key, (void *) node );
    return node;
}


/*============================================================

Function: compress_LAMBDA2

Input: A production number ( int ), and a pointer to a stack.

Output: A pointer to a finished node, 
        representing the compressed tree for the specified production,
	realized with the child nodes on top of the stack,
	which have been removed from the stack.

============================================================*/

struct lambda_expr * compress_LAMBDA2( int prod_num, struct list * stack ) {
    pop_from_list( stack ); // Throw away an old state.
    struct lambda_expr * body = pop_from_list( stack ); // Get the lambda body.
    pop_from_list( stack ); // Throw away an old state.
    free( pop_from_list( stack ) ); // Throw away the "period" token.
    pop_from_list( stack ); // Throw away an old state.
    struct lambda_expr * var = pop_from_list( stack ); // Get the VAR node.
    pop_from_list( stack ); // Throw away an old state.
    free( pop_from_list( stack ) ); // Throw away the "backslash" token.
    char * data = NULL;
    char * key = hash_key( var, body, data );
    struct lambda_expr * node;
    struct hash * hash_node;
    struct hash * LAMBDAS = hashlookup( get_NODE_HASH(), "LAMBDA" )->data;
    if ( (hash_node = hashlookup( LAMBDAS, key )) ) {
	free( key );
	return hash_node->data;
    }
    node = new_lambda_expr();
    strcpy( node->type, "LAMBDA" );
    node->child[0] = var;
    node->child[1] = body;
    var->parents += 1;
    body->parents += 1;
    add_to_hash( LAMBDAS, key, (void *) node );
    return node;
}


/*============================================================

Function: compress_VAR

Input: A production number ( int ), and a pointer to a stack.

Output: A pointer to a finished node, 
        representing the compressed tree for the specified production,
        realized with the child nodes on top of the stack,
	which have been removed from the stack.

============================================================*/


struct lambda_expr * compress_VAR( int prod_num, struct list * stack ) {
    int old_state = (long int) pop_from_list( stack ); // Throw away an old state.
    struct lambda_expr * text = pop_from_list( stack ); // Turn TEXT to VAR.
    int varname_length = text->last - text->first + 1;
    char * varname = malloc( ( varname_length + 1 ) * sizeof( char ) );
    strncpy( varname, get_INPUT() + text->first, varname_length ); // Substring.
    varname[varname_length] = '\0';
    try_to_delete( text );
    struct hash * VARS = hashlookup( get_NODE_HASH(), "VAR" )->data;
    struct lambda_expr * node;
    struct hash * hash_node;
    if ( (hash_node = hashlookup( VARS, varname )) ) {
	free( varname );
	return hash_node->data;
    }
    node = new_lambda_expr();
    strcpy( node->type, "VAR" );
    node->data = varname;
    add_to_hash( VARS, varname, (void *) node );
    return node;
}


/*============================================================

Function: compress_TEXT1

Input: A production number ( int ), and a pointer to a stack.

Output: A pointer to a lambda_expr, 
        based on the token on top of the stack, 
	which has been removed from the stack.

============================================================*/

struct lambda_expr * compress_TEXT1( int prod_num, struct list * stack ) {
    pop_from_list( stack ); // Throw away an old state.
    struct token * t = pop_from_list( stack ); // Get the token
    struct lambda_expr * text = new_lambda_expr();
    strcpy( text->type, "TEXT" );
    text->first = t->first;
    text->last = t->last;
    free(t);
    return text;
}


/*============================================================

Function: compress_TEXT2

Input: A production number ( int ), and a pointer to a stack.

Output: A pointer to a lambda_expr, 
        representing the concatenation of two substrings of text.
	The token on top of the stack, 
	and  the TEXT node directly underneath it have been removed
	from the stack.

============================================================*/

struct lambda_expr * compress_TEXT2( int prod_num, struct list * stack ) {
    pop_from_list( stack ); // Throw away an old state.
    struct token * right = pop_from_list( stack ); // Get extending token.
    pop_from_list( stack ); // Throw away an old state.
    struct lambda_expr * left = pop_from_list( stack ); // Get extendable TEXT.
    left->last = right->last;
    free( right );
    return left;
}




////////////////////////////////////////////////////////////
// Helper Functions ////////////////////////////////////////
////////////////////////////////////////////////////////////


void print_NAMESPACE() {
    struct list * key_list = new_list();
    list_keys_in_hash( NAMESPACE, key_list, "" );
    int i;
    for ( i = 0; i < key_list->next_index; i++ ) {
        char * key = listlookup( key_list, i );
        struct lambda_expr * value = hashlookup( NAMESPACE, key )->data;
        printf( "%s =><br />\n", key );
        printf( "%s<br />\n", print_lambda_expr( value ) );
    }
    destroy_key_list( key_list );
}



int main( int argc, char * argv[] ) {

    // Read input file.
    long file_size;
    FILE * fp = fopen( argv[1], "r" );
    fseek( fp, 0, SEEK_END );
    file_size = ftell( fp );
    char * lambda_code = malloc( ( file_size + 1 ) * sizeof(char) );
    int fd = open( argv[1], O_RDONLY, 0 );
    read( fd, lambda_code, file_size * sizeof(char) );
    lambda_code[file_size] = '\0';

    // Set up grammar.
    struct list * production0 = new_list();
    append_items_to_list( production0, 3, "PHI", "HASH", "end" );

    struct list * production1 = new_list();
    append_items_to_list( production1, 4, "HASH", "HASH", "newline", "STMT" );

    struct list * production2 = new_list();
    append_items_to_list( production2, 2, "HASH", "STMT" );

    struct list * production3 = new_list();
    append_items_to_list( production3, 3, "STMT", "LHS", "RHS" );

    struct list * production4 = new_list();
    append_items_to_list( production4, 3, "LHS", "TEXT", "colon" );

    struct list * production5 = new_list();
    append_items_to_list( production5, 3, "RHS", "EXPR", "semicolon" );

    struct list * production6 = new_list();
    append_items_to_list( production6, 2, "EXPR", "BETA" );

    struct list * production7 = new_list();
    append_items_to_list( production7, 2, "EXPR", "LAMBDA" );

    struct list * production8 = new_list();
    append_items_to_list( production8, 2, "EXPR", "VAR" );

    struct list * production9 = new_list();
    append_items_to_list( production9, 6, "BETA", "lparen", "EXPR", "comma", "EXPR", "rparen" );

    struct list * production10 = new_list();
    append_items_to_list( production10, 4, "LAMBDA", "backslash", "VAR", "BETA" );

    struct list * production11 = new_list();
    append_items_to_list( production11, 4, "LAMBDA", "backslash", "VAR", "LAMBDA" );

    struct list * production12 = new_list();
    append_items_to_list( production12, 5, "LAMBDA", "backslash", "VAR", "period", "EXPR" );

    struct list * production13 = new_list();
    append_items_to_list( production13, 2, "VAR", "TEXT" );

    struct list * production14 = new_list();
    append_items_to_list( production14, 3, "TEXT", "TEXT", "text" );

    struct list * production15 = new_list();
    append_items_to_list( production15, 2, "TEXT", "text" );

    struct grammar_entry my_grammar[] = {
        { production0, NULL },
        { production1, &compress_HASH2 },
        { production2, &compress_HASH1 },
        { production3, &install_name },
        { production4, &compress_LHS },
        { production5, &compress_RHS },
        { production6, &transitive_down },
        { production7, &transitive_down },
        { production8, &transitive_down },
        { production9, &compress_BETA },
        { production10, &compress_LAMBDA1 },
        { production11, &compress_LAMBDA1 },
        { production12, &compress_LAMBDA2 },
        { production13, &compress_VAR },
        { production14, &compress_TEXT2 },
        { production15, &compress_TEXT1 }
    };

    // Set up tokens.
    struct hash * token_types = new_hash();
    add_pairs_to_hash( token_types, 8,
                       "(", (void *) "lparen",
                       ")", (void *) "rparen",
                       ".", (void *) "period",
                       ",", (void *) "comma",
                       "\\", (void *) "backslash",
                       ":" , (void *) "colon",
                       ";" , (void *) "semicolon",
                       "\n", (void *) "newline"
                       );

    set_GRAMMAR( my_grammar, ( sizeof my_grammar ) / ( sizeof my_grammar[0] ) );

    analyze_productions();

    fill_FIRST();

    fill_FOLLOW();

    fill_TABLES();

    fill_TRIE( token_types );

    init_NODE_HASH();

    NAMESPACE = new_hash();

    char * error =  "";

    struct lambda_expr * parse_tree = parse( lambda_code, &error );
    printf( "The result of the parse is: <br />\n" );
    print_NAMESPACE();
    if ( strlen( error ) ) {
        printf( "The error is:<br />\n" );
        printf( "%s<br />\n", error );
    }

    set_global_V( hashlookup( get_NODE_HASH(), "VAR" )->data );
    evaluate_namespace( NAMESPACE );

    printf( "After evaluating the namespace, it looks like:<br />\n" );
    print_NAMESPACE();

}

