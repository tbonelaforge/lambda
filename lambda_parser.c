#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "lambda_expr.h"
#include "lambda_parser.h"
#include "hash.h"
#include "list.h"

#define SHIFT 1
#define REDUCE 2
#define ACCEPT 3

char * INPUT;            // The string which is being parsed.
struct trie_node * TRIE; // Will be used in tokenizing the string.
struct hash * NODE_HASH; // Will be used to keep track of unique nodes during parsing.

double SIZEOF_VOID_POINTER = sizeof( void * ); // Used when making hash keys for nodes.

char * PRODUCTIONS[11][6] = {
    { "PHI", "EXPR", "end" }, // # 0
    { "EXPR", "BETA" }, // # 1
    { "EXPR", "LAMBDA" }, // # 2
    { "EXPR", "VAR" }, // # 3
    { "VAR", "TEXT" }, // # 4
    { "BETA", "lparen", "EXPR", "comma", "EXPR", "rparen" }, // # 5
    { "LAMBDA", "backslash", "VAR", "BETA" }, // # 6
    { "LAMBDA", "backslash", "VAR", "LAMBDA" }, // # 7
    { "LAMBDA", "backslash", "VAR", "period", "EXPR" }, // # 8
    { "TEXT", "TEXT", "text" }, // #9
    { "TEXT", "text" } // # 10
};

typedef struct lambda_expr * (*pt2function)( int, struct list * );
pt2function REDUCTIONS[11] = {
    NULL,              // reduction 0
    &transitive_down,  // reduction 1
    &transitive_down,  // reduction 2
    &transitive_down,  // reduction 3
    &compress_VAR,     // reduction 4
    &compress_BETA,    // reduction 5
    &compress_LAMBDA1, // reduction 6
    &compress_LAMBDA1, // reduction 7
    &compress_LAMBDA2, // reduction 8
    &compress_TEXT2,   // reduction 9
    &compress_TEXT1,   // reduction 10
};

struct hash * TABLE[20];
struct hash * GOTO[20];

struct transition s8 = { SHIFT, 8 };
struct transition s7 = { SHIFT, 7 };
struct transition s6 = { SHIFT, 6 };
struct transition r1 = { REDUCE, 1 };
struct transition acc = { ACCEPT, 0 };
struct transition r2 = { REDUCE, 2 };
struct transition s10 = { SHIFT, 10 };
struct transition r4 = { REDUCE, 4 };
struct transition r3 = { REDUCE, 3 };
struct transition r10 = { REDUCE, 10 };
struct transition r9 = { REDUCE, 9 };
struct transition s15 = { SHIFT, 15 };
struct transition s16 = { SHIFT, 16 };
struct transition r6 = { REDUCE, 6 };
struct transition r7 = { REDUCE, 7 };
struct transition r8 = { REDUCE, 8 };
struct transition s19 = { SHIFT, 19 };
struct transition r5 = { REDUCE, 5 };


void fill_TABLE() {
    TABLE[0] = new_hash();
    add_pairs_to_hash( TABLE[0], 3,
		       "text", (void *) &s8,
		       "lparen", (void *) &s7,
		       "backslash", (void *) &s6
		       );

    TABLE[1] = new_hash();
    add_pairs_to_hash( TABLE[1], 3,
		       "rparen", (void *) &r1,
		       "comma", (void *) &r1,
		       "end", (void *) &r1
		       );

    TABLE[2] = new_hash();
    add_pairs_to_hash( TABLE[2], 1,
		       "end", (void *) &acc
		       );

    TABLE[3] = new_hash();
    add_pairs_to_hash( TABLE[3], 3,
		       "rparen", (void *) &r2,
		       "comma", (void *) &r2,
		       "end", (void *) &r2
		       );

    TABLE[4] = new_hash();
    add_pairs_to_hash( TABLE[4], 7,
		       "text", (void *) &s10,
		       "lparen", (void *) &r4,
		       "rparen", (void *) &r4,
		       "period", (void *) &r4,
		       "comma", (void *) &r4,
		       "backslash", (void *) &r4,
		       "end", (void *) &r4
		       );

    TABLE[5] = new_hash();
    add_pairs_to_hash( TABLE[5], 3,
		       "rparen", (void *) &r3,
		       "comma", (void *) &r3,
		       "end", (void *) &r3
		       );

    TABLE[6] = new_hash();
    add_pairs_to_hash( TABLE[6], 1,
		       "text", (void *) &s8
		       );

    TABLE[7] = new_hash();
    add_pairs_to_hash( TABLE[7], 3,
		       "text", (void *) &s8,
		       "lparen", (void *) &s7,
		       "backslash", (void *) &s6
		       );

    TABLE[8] = new_hash();
    add_pairs_to_hash( TABLE[8], 7,
		       "text", (void *) &r10,
		       "lparen", (void *) &r10,
		       "rparen", (void *) &r10,
		       "period", (void *) &r10,
		       "comma", (void *) &r10,
		       "backslash", (void *) &r10,
		       "end", (void *) &r10
		       );

    TABLE[9] = new_hash();
    
    TABLE[10] = new_hash();
    add_pairs_to_hash( TABLE[10], 7,
		       "text", (void *) &r9,
		       "lparen", (void *) &r9,
		       "rparen", (void *) &r9,
		       "period", (void *) &r9,
		       "comma", (void *) &r9,
		       "backslash", (void *) &r9,
		       "end", (void *) &r9
		       );

    TABLE[11] = new_hash();
    add_pairs_to_hash( TABLE[11], 3,
		       "lparen", (void *) &s7,
		       "period", (void *) &s15,
		       "backslash", (void *) &s6
		       );

    TABLE[12] = new_hash();
    add_pairs_to_hash( TABLE[12], 1,
		       "comma", (void *) &s16
		       );
    
    TABLE[13] = new_hash();
    add_pairs_to_hash( TABLE[13], 3,
		       "rparen", (void *) &r6,
		       "comma", (void *) &r6,
		       "end", (void *) &r6
		       );

    TABLE[14] = new_hash();
    add_pairs_to_hash( TABLE[14], 3,
		       "rparen", (void *) &r7,
		       "comma", (void *) &r7,
		       "end", (void *) &r7
		       );

    TABLE[15] = new_hash();
    add_pairs_to_hash( TABLE[15], 3,
		       "text", (void *) &s8,
		       "lparen", (void *) &s7,
		       "backslash", (void *) &s6
		       );

    TABLE[16] = new_hash();
    add_pairs_to_hash( TABLE[16], 3,
		       "text", (void *) &s8,
		       "lparen", (void *) &s7,
		       "backslash", (void *) &s6
		       );

    TABLE[17] = new_hash();
    add_pairs_to_hash( TABLE[17], 3,
		       "rparen", (void *) &r8,
		       "comma", (void *) &r8,
		       "end", (void *) &r8
		       );

    TABLE[18] = new_hash();
    add_pairs_to_hash( TABLE[18], 1,
		       "rparen", (void *) &s19
		       );

    TABLE[19] = new_hash();
    add_pairs_to_hash( TABLE[19], 3,
		       "rparen", (void *) &r5,
		       "comma", (void *) &r5,
		       "end", (void *) &r5
		       );

}


void fill_GOTO() {
    GOTO[0] = new_hash();
    add_pairs_to_hash( GOTO[0], 5,
		       "EXPR", (void *) 2,
		       "BETA", (void *) 1,
		       "LAMBDA", (void *) 3,
		       "VAR", (void *) 5,
		       "TEXT", (void *) 4
		       );

    GOTO[1] = GOTO[2] = GOTO[3] = GOTO[4] = GOTO[5] = new_hash();

    GOTO[6] = new_hash();
    add_pairs_to_hash( GOTO[6], 2,
		       "VAR", (void *) 11,
		       "TEXT", (void *) 4
		       );

    GOTO[7] = new_hash();
    add_pairs_to_hash( GOTO[7], 5,
		       "EXPR", (void *) 12,
		       "BETA", (void *) 1,
		       "LAMBDA", (void *) 3,
		       "VAR", (void *) 5,
		       "TEXT", (void *) 4
		       );

    GOTO[8] = GOTO[9] = GOTO[10] = new_hash();

    GOTO[11] = new_hash();
    add_pairs_to_hash( GOTO[11], 2,
		       "BETA", (void *) 13,
		       "LAMBDA", (void *) 14
		       );

    GOTO[12] = GOTO[13] = GOTO[14] = new_hash();
    
    GOTO[15] = new_hash();
    add_pairs_to_hash( GOTO[15], 5,
		       "EXPR", (void *) 17,
		       "BETA", (void *) 1,
		       "LAMBDA", (void *) 3,
		       "VAR", (void *) 5,
		       "TEXT", (void *) 4
		       );

    GOTO[16] = new_hash();
    add_pairs_to_hash( GOTO[16], 5,
		       "EXPR", (void *) 18,
		       "BETA", (void *) 1,
		       "LAMBDA", (void *) 3,
		       "VAR", (void *) 5,
		       "TEXT", (void *) 4
		       );
    GOTO[17] = GOTO[18] = GOTO[19] = new_hash();
}

char ** get_production( int prod_num ) {
    return PRODUCTIONS[prod_num];
}

struct hash * get_NODE_HASH() {
    return NODE_HASH;
}


void init_NODE_HASH() {
    NODE_HASH = new_hash();
    add_to_hash( NODE_HASH, "BETA", (void *) new_hash() );
    add_to_hash( NODE_HASH, "LAMBDA", (void *) new_hash() );
    add_to_hash( NODE_HASH, "VAR", (void *) new_hash() );
}


void fill_TRIE() {
    struct trie_node * trie_node1 = new_trie_node();
    trie_node1->accepting_type = "lparen";

    struct trie_node * trie_node2 = new_trie_node();
    trie_node2->accepting_type = "rparen";

    struct trie_node * trie_node3 = new_trie_node();
    trie_node3->accepting_type = "period";

    struct trie_node * trie_node4 = new_trie_node();
    trie_node4->accepting_type = "comma";

    struct trie_node * trie_node5 = new_trie_node();
    trie_node5->accepting_type = "backslash";

    struct trie_node * trie_node6 = new_trie_node();
    add_to_charhash( trie_node6->child, '(', (void*) trie_node1 );
    add_to_charhash( trie_node6->child, ')', (void *) trie_node2 );
    add_to_charhash( trie_node6->child, '.', (void *) trie_node3 );
    add_to_charhash( trie_node6->child, ',', (void *) trie_node4 );
    add_to_charhash( trie_node6->child, '\\', (void *) trie_node5 );
    TRIE = trie_node6;
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
    char * key = hash_key( left, right, data, stack );
    struct lambda_expr * node;
    struct hash * hash_node;
    struct hash * BETAS = hashlookup( NODE_HASH, "BETA" )->data;
    if ( hash_node = hashlookup( BETAS, key ) ) {
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
    char * key = hash_key( var, body, data, stack );
    struct lambda_expr * node;
    struct hash * hash_node;
    struct hash * LAMBDAS = hashlookup( NODE_HASH, "LAMBDA" )->data;
    if ( hash_node = hashlookup( LAMBDAS, key ) ) {
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
    char * key = hash_key( var, body, data, stack );
    struct lambda_expr * node;
    struct hash * hash_node;
    struct hash * LAMBDAS = hashlookup( NODE_HASH, "LAMBDA" )->data;
    if ( hash_node = hashlookup( LAMBDAS, key ) ) {
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
    pop_from_list( stack ); // Throw away an old state.
    struct lambda_expr * text = pop_from_list( stack ); // Turn TEXT to VAR.
    int varname_length = text->last - text->first + 1;
    char * varname = malloc( ( varname_length + 1 ) * sizeof( char ) );
    strncpy( varname, INPUT + text->first, varname_length ); // Substring.
    varname[varname_length] = '\0';
    try_to_delete( text );
    struct hash * VARS = hashlookup( NODE_HASH, "VAR" )->data;
    struct lambda_expr * node;
    struct hash * hash_node;
    if ( hash_node = hashlookup( VARS, varname ) ) {
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


/*============================================================

Function: parse

Input: A string, and a pointer to another string, 
       to be used for recording an error message.

Output: A pointer to a lambda_expr, 
        representing the parse tree for the given string
        according to the language defined by the current
	parsing structures.

============================================================*/

struct lambda_expr * parse( char * string, char ** error ) {
    INPUT = string;
    struct list * stack = new_list();
    append_to_list( stack, (void *) (long int) 0 );
    struct lambda_expr * result;
    struct token * t = NULL;
    struct lambda_expr * (*action_function)( int, struct list * );
    int current_state = 0;
    int i = 0;
    struct hash * row;
    struct hash * trans_node;
    struct transition * trans;
    int arg;
    int action;
    while ( 1 ) {
	if ( ! t ) {
	    t = get_next_token( string, i, TRIE );
            if ( strcmp( t->type, "end" ) ) {
	        i += ( t->last - t->first + 1 );
	    }
	}
	row = TABLE[current_state];
	trans_node = hashlookup( row, t->type );
	if ( ! trans_node ) {
	    *error = "error";
	    return NULL;
	}
	trans = trans_node->data;
	action = trans->action;
	if ( action == ACCEPT ) {
	    pop_from_list( stack ); // Throw away final state number.
	    result = pop_from_list( stack );
	    pop_from_list( stack ); // Throw away initial state number.
	    destroy_empty_list( stack );
	    free( t );
	    return result;
	}
	arg = trans->arg;
	if ( action == SHIFT ) {
	    append_to_list( stack, (void *) t );
	    append_to_list( stack, (void *) (long int) arg );
	    current_state = arg; // Iterate state.
	    t = NULL; // Trigger a call to get_next_token.
	    continue;
	}

	// The action is REDUCE.
	action_function = REDUCTIONS[arg];
	result = (*action_function)( arg, stack ); // Changes stack.
	current_state = (int) (long int) listlookup( stack, stack->next_index - 1 ); // Uncovered state.
	append_to_list( stack, (void *) result );
	char * lhs = PRODUCTIONS[arg][0];
	current_state = (int) (long int) hashlookup( GOTO[current_state], lhs )->data; // Iterate state.
	append_to_list( stack, (void *) (long int) current_state );
    } // End while loop.
    // Shouldn't ever get here!
}


/*============================================================

Function: new_trie_node

Input: Nothing.

Output: A pointer to a newly-allocated trie_node,
        having accepting_type = NULL, 
        and an empty child charhash.

============================================================*/

struct trie_node * new_trie_node() {
    struct trie_node * new_trie_node = ( struct trie_node * ) malloc( sizeof( struct trie_node ) );
    if ( ! new_trie_node ) {
	return NULL;
    }
    new_trie_node->accepting_type = NULL;
    new_trie_node->child = new_charhash();
    if ( ! new_trie_node->child ) {
	return NULL;
    }
    return new_trie_node;
}


/*============================================================

Function: get_next_token

Input: A pointer to a string, an integer ( start ),
       and a pointer to a trie structure,
       to use when tokenizing the string.

Output: A pointer to a newly-allocated token structure, 
        giving the type of the next token in the string, 
	starting from "start",
        and the substring of input the token covers.

============================================================*/

struct token * get_next_token( char * string, int start, struct trie_node * trie_root ) {
    struct token * next_token = new_token();
    if ( string[start] == '\0' ) {
	next_token->type = "end";
	next_token->first = start;
	next_token->last = start;
	return next_token;
    }
    struct trie_node * current_node = trie_root;
    struct charhash * next_node = NULL;
    int i = start;
    char input_char;
    while ( input_char = string[i] ) { // Loop characters of string.
	if ( next_node = charhashlookup( current_node->child, input_char ) ) {
	    current_node = next_node->data;
	    i = i + 1;
	}
	else { // No transition for this character.
	    if ( current_node->accepting_type ) {
		i = i - 1; // "Push" this character back on the input.
	    }
	    break;
	}
    } // End characters of string loop.

    if ( input_char == '\0' ) {
	i = i - 1;
    }
    next_token->first = start;
    next_token->last = i;
    next_token->type = current_node->accepting_type;
    if ( ! current_node->accepting_type ) {
	next_token->type = "text";
    }
    return next_token;
}


/*============================================================

Function: new_token

Input: Nothing.

Output: A pointer to a newly-allocated token structure,
        with all fields set to NULL or 0.

============================================================*/

struct token * new_token() {
    struct token * t = malloc( sizeof( struct token ) );
    if ( ! t ) {
	return NULL;
    }
    t->type = NULL;
    t->first = 0;
    t->last = 0;
}



/*============================================================

Function: hash_key

Input: A pointer to a lambda_expr ( left child ),
       another pointer to a lambda_expr ( right child ), 
       and a string.

Output: A newly-allocated string, 
        encoding the unique combination of ( left, right, data ),
	to be used as a key for a lambda_expr ( parent ) in a hash.

============================================================*/

char * hash_key( struct lambda_expr * left, struct lambda_expr * right, char * data, struct list * stack ) {

    int address_string_length = ceil( 8.0 * SIZEOF_VOID_POINTER / 4.0 ) + 2;
    int data_length = 0;
    if ( data ) {
	data_length = strlen( data );
    }

    char * key = malloc( ( 2 * address_string_length + data_length + 2 + 1 ) * sizeof(char) );

    if ( ! key ) {
	return NULL;
    }

    *key = '\0';

    sprintf( key, "%p_%p_%s", left, right, data );

    return key;
}
