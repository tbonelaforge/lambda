#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "lambda_parser.h"
#include "lambda_expr.h"
#include "hash.h"
#include "hash_default.h"
#include "list.h"


int NUM_PRODUCTIONS = 0;
int GSIZE = 0;
struct grammar_entry * GRAMMAR = NULL; // Array of grammar entries, to be filled by user.
struct hash * IS_TERMINAL = NULL;
struct hash * PRODUCTIONS_FOR = NULL;
struct hash * FIRST = NULL;
struct hash * FOLLOW = NULL;
struct list * TABLE = NULL;
struct list * GOTO = NULL;
struct trie_node * TRIE = NULL;
struct hash * NODE_HASH = NULL;
char * INPUT = NULL;

int TRUE = 1;
int FALSE = 0;

double SIZEOF_VOID_POINTER = sizeof( void * ); // Used to make keys for nodes.

int get_NUM_PRODUCTIONS() {
    return NUM_PRODUCTIONS;
}

int get_GSIZE() {
    return GSIZE;
}

struct grammar_entry * get_GRAMMAR() {
    return GRAMMAR;
}

void set_GRAMMAR( struct grammar_entry * my_grammar, int num_productions ) {
    GRAMMAR = my_grammar;
    NUM_PRODUCTIONS = num_productions;

}

struct hash * get_IS_TERMINAL() {
    return IS_TERMINAL;
}

struct hash * get_PRODUCTIONS_FOR() {
    return PRODUCTIONS_FOR;
}


struct hash * get_FIRST() {
    return FIRST;
}

struct hash * get_FOLLOW() {
    return FOLLOW;
}

struct list * get_TABLE() {
    return TABLE;
}

struct list * get_GOTO() {
    return GOTO;
}

struct trie_node * get_TRIE() {
    return TRIE;
}

struct hash * get_NODE_HASH() {
    return NODE_HASH;
}

char * get_INPUT() {
    return INPUT;
}


/*============================================================

Function: analyze_productions

Input: Nothing

Output: Nothing, but the global variables 
        GSIZE, IS_TERMINAL, and PRODUCTIONS_FOR will be filled out.

============================================================*/

void analyze_productions() {
    GSIZE = NUM_PRODUCTIONS;
    int i;
    int j;
    struct list * production;
    struct list * productions_for_symbol;
    char * symbol;
    if ( ! IS_TERMINAL ) {
        IS_TERMINAL = new_hash();
    }
    if ( ! PRODUCTIONS_FOR ) {
        PRODUCTIONS_FOR = new_hash();
    }
    for ( i = 0; i < NUM_PRODUCTIONS; i++ ) {
        production = GRAMMAR[i].production;
        if ( production->next_index > GSIZE ) {
            GSIZE = production->next_index;
        }
        for ( j = 0; j < production->next_index; j++ ) {
            symbol = listlookup( production, j );
            struct hash * looked_up = hashlookup( IS_TERMINAL, symbol );
            if ( ! looked_up ) { // Assume each new symbol is a terminal.
                add_to_hash( IS_TERMINAL, symbol, (void *) &TRUE );
            }
            if ( j == 0 ) { // Symbols on the left side of a productions are nonterminal.
                add_to_hash( IS_TERMINAL, symbol, (void *) &FALSE );
                looked_up = hashlookup( PRODUCTIONS_FOR, symbol );
                if ( ! looked_up ) {
                    productions_for_symbol = new_list();
                    add_to_hash( PRODUCTIONS_FOR, symbol, (void *) productions_for_symbol );
                }
                else {
                    productions_for_symbol = looked_up->data;
                }

                append_to_list( productions_for_symbol, (void *) (long int) i );
            }
        }
    }
}


/*============================================================

Function: fill_FIRST

Input: Nothing.

Output: Nothing, but the FIRST hash has been filled appropriately.

============================================================*/

void fill_FIRST() {
    FIRST = new_hash();
    int i;
    int j;
    char * symbol;
    char * lhs;
    char * left_corner;
    struct list * key_list = new_list();
    list_keys_in_hash( IS_TERMINAL, key_list, "" );
    for ( i = 0; i < key_list->next_index; i++ ) {
        symbol = listlookup( key_list, i );
        struct hash * symbol_firsts = new_hash();
         if ( ( (int *) hashlookup( IS_TERMINAL, symbol )->data ) == &TRUE ) { // Terminals self-derive.
            add_to_hash( symbol_firsts, symbol, (void *) (long int) 1 );
        }
        add_to_hash( FIRST, symbol, (void *) symbol_firsts );
    }
    destroy_key_list( key_list );
    int change = 1;
    while ( change ) {
        change = 0;
        for ( i = 0; i < NUM_PRODUCTIONS; i++ ) {
            struct list * production = GRAMMAR[i].production;
            lhs = listlookup( production, 0 );
            left_corner = listlookup( production, 1 );
            key_list = new_list();
            list_keys_in_hash( hashlookup( FIRST, left_corner )->data, key_list, "" );
            for ( j = 0; j <  key_list->next_index; j++ ) { // Go through all the firsts of left_corner.
                symbol = listlookup( key_list, j );
                struct hash * lhs_firsts = hashlookup( FIRST, lhs )->data;
                struct hash * looked_up = hashlookup( lhs_firsts, symbol );
                if ( ! looked_up ) {
                    add_to_hash( lhs_firsts, symbol, (void *) &TRUE ); // Add them to firsts of lhs.
                    change = 1;
                }
            }
            destroy_key_list( key_list );
        }
    }
}


/*============================================================

Function : fill_FOLLOW

Input : Nothing.

Output : Nothing, but the FOLLOW hash has been filled out.

============================================================*/

void fill_FOLLOW() {
    FOLLOW = new_hash();
    char * symbol;
    char * next_symbol;
    char * lhs;
    int i;
    int j;
    struct list * production;
    int change;
    struct hash * symbol_followers;

    struct list * key_list = new_list();
    list_keys_in_hash( IS_TERMINAL, key_list, "" );
    for ( i = 0; i < key_list->next_index; i++ ) {
        symbol = listlookup( key_list, i );
        if ( ( (int *) hashlookup( IS_TERMINAL, symbol )->data ) == &FALSE ) {
            symbol_followers = new_hash();
            add_to_hash( FOLLOW, symbol, (void *) symbol_followers );
        }
    }
    destroy_key_list( key_list );
    change = 1;
    while ( change ) {
        change = 0;
        for ( i = 0; i < NUM_PRODUCTIONS; i++ ) {
            production = GRAMMAR[i].production;
            lhs = listlookup( production, 0 );
            for ( j = 1; j < production->next_index; j++ ) { // Loop RHS symbols.
                symbol = listlookup( production, j );
                if ( ( (int *) hashlookup( IS_TERMINAL, symbol )->data ) == &TRUE ) { // No FOLLOW for terminals.
                    continue;
                }
                symbol_followers = hashlookup( FOLLOW, symbol )->data;
                if ( j + 1 < production->next_index ) { // There is an adjacent symbol.
                    next_symbol = listlookup( production, j + 1 );
                    struct hash * next_symbol_firsts = hashlookup( FIRST, next_symbol )->data;

                    // Everything in FIRST{next_symbol} should be in FOLLOW{symbol}.
                    change = expand_hash( symbol_followers, next_symbol_firsts ) || change;
                    continue;
                }
                else { // Last symbol in production is nonterminal.
                    struct hash * lhs_followers = hashlookup( FOLLOW, lhs )->data;

                    // Everything in FOLLOW{lhs} should be in FOLLOW{symbol}.
                    change = expand_hash( symbol_followers, lhs_followers ) || change;
                    continue;
                }
            }
        }
    }
}


/*============================================================

Function: fill_TABLES

Input: Nothing.

Output: Nothing, but the package-level TABLE and GOTO lists
        have been filled out appropriately.

============================================================*/

void fill_TABLES() {
    struct hash * looked_up;
    struct hash * row;
    struct parser_generator * parser_generator = new_parser_generator();
    append_to_list( parser_generator->TABLE, new_hash() );
    append_to_list( parser_generator->GOTO, new_hash() );
    add_initial_itemset( parser_generator );
    struct list * itemsets = parser_generator->itemsets;
    int i = 0;
    while ( i < parser_generator->itemsets->next_index ) {
        struct itemset * current_itemset = listlookup( itemsets, i );
        looked_up = listlookup( parser_generator->TABLE, i );
        if ( ! looked_up ) {
            row = new_hash();
            append_to_list( parser_generator->TABLE, row );
        }
        looked_up = listlookup( parser_generator->GOTO, i );
        if ( ! looked_up ) {
            row = new_hash();
            append_to_list( parser_generator->GOTO, row );
        }
        install_incomplete_transitions( i, current_itemset, parser_generator );
        install_complete_transitions( i, current_itemset, parser_generator );
        i++;
    }
    TABLE = parser_generator->TABLE;
    GOTO = parser_generator->GOTO;
    
}

/*============================================================

Function: add_initial_itemset

Input: A pointer to a parser_generator structure.

Output: Nothing, but the parser generator's itemsets list
        now contains the initial itemset, 
        and the parser generator's ititemsets_by_key hash
        indexes the initial itemset's index, by its closure key.

 ============================================================*/

void add_initial_itemset( struct parser_generator * self ) {
    struct item * initial_item = new_item();
    char * initial_item_key = create_item_key( initial_item );
    struct itemset * initial_itemset = new_itemset();
    add_to_hash( initial_itemset->items, initial_item_key, (void *) initial_item );
    free( initial_item_key );
    char * initial_itemset_key = create_closure_key( initial_itemset );
    append_to_list( self->itemsets, (void *) initial_itemset );
    int initial_state = self->itemsets->next_index - 1;
    add_to_hash( self->itemsets_by_key, initial_itemset_key, (void *) (long int) initial_state );
    free( initial_itemset_key );
    return;
}


/*============================================================

Function: install_incomplete_transitions

Input: An integer 'i', a pointer to the current itemset structure,
       and a pointer to the parser generator currently in use.

Output: Nothing, but any transitions implied by incomplete items
        in the given itemset have been installed in the TABLE
        or GOTO lists of the given parser generator, 
        depending on whether they were transitions on 
        terminals or nonterminals, respectively.

============================================================*/

void install_incomplete_transitions( int i, struct itemset * current_itemset, struct parser_generator * self ) {
    struct list * itemsets = self->itemsets;
    struct hash * itemsets_by_key = self->itemsets_by_key;
    struct hash * ready_for = current_itemset->ready_for;

    // Prepare to loop through the keys of current_itemset->ready_for.
    struct list * key_list = new_list();
    list_keys_in_hash( ready_for, key_list, "" );
    int j;
    int k;
    for ( j = 0; j < key_list->next_index; j++ ) { // Loop ready-for symbols.
        char * symbol = listlookup( key_list, j );
        struct list * ready_for_symbol = hashlookup( ready_for, symbol )->data;
        struct itemset * proposed_itemset = new_itemset();
        for ( k = 0; k < ready_for_symbol->next_index; k++ ) { // Loop symbol items.
            struct item * ready_item = listlookup( ready_for_symbol, k );
            struct item * proposed_item = new_item();
            proposed_item->prod_num = ready_item->prod_num;
            proposed_item->dot = ready_item->dot + 1;
            char * proposed_item_key = create_item_key( proposed_item );
            add_to_hash( proposed_itemset->items, proposed_item_key, (void *) proposed_item );
            free( proposed_item_key ); // It's now in the hash.
        } // End symbol items loop.
        char * proposed_itemset_key = create_closure_key( proposed_itemset );
        struct hash * looked_up = hashlookup( itemsets_by_key, proposed_itemset_key );
        if ( ! looked_up ) {
            append_to_list( itemsets, (void *) proposed_itemset );
            int state_num = itemsets->next_index - 1;
            looked_up = add_to_hash( itemsets_by_key, proposed_itemset_key, (void *) (long int) state_num );
        }
        else {
            destroy_itemset( proposed_itemset );
        }
        int destination = (long int) looked_up->data;
        install_incomplete_transition( i, symbol, destination, self );
    } // End ready-for symbols loop.
    return;
}


/*============================================================

Function: install_complete_transitions

Input: An integer ('i'), a pointer to the current itemset, 
       and a pointer to the parser generator currently in use.

Output: Nothing, but any transitions implied by complete items
        in the given itemset have been installed in the TABLE
        list of the given parser generator. 

============================================================*/

void install_complete_transitions( int i, struct itemset * current_itemset, struct parser_generator * self ) {
    struct list * table = self->TABLE;
    struct list * complete_items = current_itemset->complete;
    char * lhs;
    struct item * complete_item;
    struct list * complete_production;
    int j; int k;
    struct hash * table_row = listlookup( self->TABLE, i );
    for ( j = 0; j < complete_items->next_index; j++ ) {
        complete_item = listlookup( complete_items, j );
        int prod_num = complete_item->prod_num;
        complete_production = GRAMMAR[prod_num].production;
        lhs = listlookup( complete_production, 0 );

        // Loop all terminals which can follow this lhs.
        struct list * key_list = new_list();
        list_keys_in_hash( hashlookup( FOLLOW, lhs )->data, key_list, "" );
        for ( k = 0; k < key_list->next_index; k++ ) {
            struct list * trans_list;
            char * follower = listlookup( key_list, k );

            // Add a reduce transition on each follower token.
            struct hash * looked_up = hashlookup( table_row, follower );
            if ( ! looked_up ) {
                trans_list = new_list();
                add_to_hash( table_row, follower, (void *) trans_list );
            }
            else {
                trans_list = looked_up->data;
            }
            struct transition * reduce_transition = new_transition();
            reduce_transition->action = "reduce";
            reduce_transition->arg = prod_num;
            append_to_list( trans_list, reduce_transition );
        }
        destroy_key_list( key_list );
    }
}



/*============================================================

Function: install_incomplete_transition

Input: An integer 'i', a symbol, a destination, 
       and a pointer to the parser generator currently in use.

Output: Nothing, but a transition has been installed in the i'th
        row of either the parser generator's TABLE or GOTO list,
        depending if the symbol was terminal or not.

============================================================*/

void install_incomplete_transition( int i, char * symbol, int destination, struct parser_generator * self ) {
    struct hash * table_row = listlookup( self->TABLE, i );
    struct hash * goto_row = listlookup( self->GOTO, i );
    struct list * trans_list;
    struct hash * looked_up;
    if ( ( (int *) hashlookup( IS_TERMINAL, symbol )->data ) == &TRUE ) {

        // Transitions on terminals go in TABLE.
        looked_up = hashlookup( table_row, symbol );
        if  ( ! looked_up ) {
            trans_list = new_list();
            add_to_hash( table_row, symbol, (void *) trans_list );
        }
        else {
            trans_list = looked_up->data;
        }
        struct transition * terminal_transition = new_transition();
        terminal_transition->action = "shift";
        terminal_transition->arg = destination;
        if ( ! strcmp( symbol, "end" ) ) {
            terminal_transition->action = "accept";
        }
        append_to_list( trans_list, (void *) terminal_transition );
    }
    else { // Transitions on nonterminals go in GOTO.
        looked_up = hashlookup( goto_row, symbol );
        if ( ! looked_up ) {
            trans_list = new_list();
            add_to_hash( goto_row, symbol, (void *) trans_list );
        }
        else {
            trans_list = looked_up->data;
        }
        append_to_list( trans_list, (void *) (long int) destination );
    }
}


/*============================================================

Function: fill_TRIE

Input: A hash of string literal => accepting types 
       ( string -> string ) defining the tokens 
       to be recognized by TRIE.

Output: Nothing, but the global TRIE has been set up
        to recognize the given tokens.

============================================================*/

void fill_TRIE( struct hash * token_types ) {
    int i;
    char * path;
    char * accepting_type;
    TRIE = new_trie_node();
    struct list * key_list = new_list();
    list_keys_in_hash( token_types, key_list, "" );
    for ( i = 0; i < key_list->next_index; i++ ) {
        path = listlookup( key_list, i );
        accepting_type = hashlookup( token_types, path )->data;
        add_to_trie( TRIE, path, accepting_type );
    }
    destroy_key_list( key_list );
    return;
}


/*============================================================

Function: init_NODE_HASH

Input: Nothing.

Output: Nothing, but the global NODE_HASH has been initialized
        with slots for the various node types involved in
        lambda expressions.

============================================================*/

void init_NODE_HASH() {
    NODE_HASH = new_hash();
    add_to_hash( NODE_HASH, "BETA", (void *) new_hash() );
    add_to_hash( NODE_HASH, "LAMBDA", (void *) new_hash() );
    add_to_hash( NODE_HASH, "VAR", (void *) new_hash() );
    return;
}



/*============================================================

Function: parse

Input: A string to parse, and a pointer to another string
       to be used for recording an error message.

Output: A pointer to a lambda_expr, 
        representing the parse tree for the given string
        according to the language defined by the current
	parsing structures. 
        If the given string contained errors, 
        the given error string will point to the errors.

============================================================*/

struct lambda_expr * parse( char * string, char ** error ) {
    INPUT = string;
    struct lambda_expr * result;
    struct token * t = NULL;
    struct lambda_expr * (*action_function)( int, struct list * );
    int current_state = 0;
    struct list * stack = new_list();
    append_to_list( stack, (void *) (long int) 0 ); // Start stack with initial state.
    int i = 0;
    struct hash * row;
    struct hash * looked_up;
    struct list * trans_list;
    struct transition * trans;
    char * action;
    int arg;
    
    // Loop tokens of input.
    while ( 1 ) {
        if ( ! t ) {
            t = get_next_token( string, i );
            if ( strcmp( t->type, "end" ) ) {
                i += ( t->last - t->first + 1 );
            }
        }
        row = listlookup( TABLE, current_state );
        looked_up = hashlookup( row, t->type );
        if ( ! looked_up ) {
            *error = "No transition";
            return NULL;
        }
        trans_list = looked_up->data;
        if ( trans_list->next_index == 0 ) {
            *error = "No transition";
            return NULL;
        }
        if ( trans_list->next_index > 1 ) {
            *error = "Ambiguous transition.";
            return NULL;
        }
        trans = listlookup( trans_list, 0 );
        action = trans->action;
        if ( !strcmp( action, "accept" ) ) {
            pop_from_list( stack ); // Throw away old state
            result = pop_from_list( stack );
            pop_from_list( stack ); // Throw away initial state number
            destroy_empty_list( stack );
            free( t );
            return result;
        }
        arg = trans->arg;
        if ( !strcmp( action, "shift" ) ) {
            append_to_list( stack, (void *) t );
            append_to_list( stack, (void *) (long int) arg );
            current_state = arg; // Iterate state.
            t = NULL; // Force reading of another token,.
            continue;
        }

        // The action is "reduce"
        action_function = GRAMMAR[arg].reduction;
        result = (*action_function)( arg, stack ); // Perform action, change stack.
        current_state = (int) (long int) listlookup( stack, stack->next_index - 1  );
        append_to_list( stack, (void *) result );
        struct list * production = GRAMMAR[arg].production;
        char * lhs = listlookup( production, 0 );
        row = listlookup( GOTO, current_state );
        struct list * trans_list = hashlookup( row, lhs )->data;
        if ( !trans_list || trans_list->next_index == 0 ) {
            *error = "No transition.";
            return NULL;
        }
        else if ( trans_list->next_index >= 2 ) {
            *error = "Ambiguous transition.";
            return NULL;
        }
        current_state = (int) (long int) listlookup( trans_list, 0 );
        append_to_list( stack, (void *) (long int) current_state );
    } // End tokens of input loop.
    // Should never get here!
}


/*============================================================

Function: get_next_token

Input: A pointer to a string, an integer
       representing the position in the string to start
       looking for the next token, using the global TRIE
       structure as a recognizer.

Output: A pointer to a newly-allocated token structure, 
        giving the type of the next token in the string, 
	starting from "start",
        and the substring of input the token covers.

============================================================*/

struct token * get_next_token( char * string, int start ) {
    struct token * next_token = new_token();
    if ( string[start] == '\0' ) {
	next_token->type = "end";
	next_token->first = start;
	next_token->last = start;
	return next_token;
    }
    struct trie_node * current_node = TRIE;
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

Function : expand_hash

Input : A pointer to a hash to expand ( target )
        and a pointer to a hash to expand by ( source )

Output : 1 or 0, depending on whether at least one of the keys
         in the source hash did not already exist in the target hash.
         Either way, the target hash will be changed to include 
          every ( key, value ) pair in the source hash.

============================================================*/

int expand_hash( struct hash * target, struct hash * source ) {
    int i;
    char * symbol;
    void * value;
    int change = 0;
    struct list * key_list = new_list();
    list_keys_in_hash( source, key_list, "" );
    for ( i = 0; i < key_list->next_index; i++ ) {
        symbol = listlookup( key_list, i );
        struct hash * looked_up = hashlookup( target, symbol );
        if ( ! looked_up ) {
            value = hashlookup( source, symbol )->data;
            add_to_hash( target, symbol, value ); // Use the same value or pointer.
            change = 1;
        }
    }
    return change;
}


/*============================================================

Function: create_closure_key

Input: A pointer to an itemset structure.

Output: A string representing all the items in the closure
        computed from the given itemset.

============================================================*/


char * create_closure_key( struct itemset * current_itemset ) {
    struct item * current_item;
    struct hash * looked_up;
    struct list * production;
    int p;
    int d;
    int i;

    // Push all the items in the itemset onto the unchecked stack.
    struct list * unchecked = new_list();
    struct hash * current_items = current_itemset->items;
    struct list * key_list = new_list();
    list_keys_in_hash( current_items, key_list, "" );
    for ( i = 0; i < key_list->next_index; i++ ) {
        char * key = listlookup( key_list, i );
        append_to_list( unchecked, (void *) hashlookup( current_items, key )->data );
    }
    destroy_key_list( key_list );

    // Now process unchecked items, possibly adding more along the way.
    while ( unchecked->next_index ) {
        current_item = pop_from_list( unchecked );
        p = current_item->prod_num;
        d = current_item->dot;
        production = GRAMMAR[p].production;
        if ( d == production->next_index - 1 ) { // This item is complete.
            append_to_list( current_itemset->complete, (void *) current_item );
            continue; // Nothing more to do here.
        }
        
        // Otherwise, this item is incomplete, 
        // so record it in the appropriate ready_for slot for this itemset.
        char * predicted = listlookup( production, d + 1 );
        struct list * items_ready_for = NULL;
        struct hash * looked_up = hashlookup( current_itemset->ready_for, predicted );
        if ( ! looked_up ) {
            items_ready_for = new_list();
            add_to_hash( current_itemset->ready_for, predicted, (void *) items_ready_for );
        }
        else {
            items_ready_for = looked_up->data;
        }
        append_to_list( items_ready_for, (void *) current_item );
        
        // If the predicted symbol is terminal, this item is done processing.
        if ( ( (int *) hashlookup( IS_TERMINAL, predicted )->data ) == &TRUE ) {
            continue; // Can't expand terminals.
        }
        
        // Otherwise, the predicted symbol is nonterminal, 
        // so expand the itemset using all applicable productions.
        struct list * predicted_productions = hashlookup( PRODUCTIONS_FOR, predicted )->data;
        for ( i = 0; i < predicted_productions->next_index; i++ ) {
            int predicted_p = (long int) listlookup( predicted_productions, i );
            struct item * predicted_item = new_item();
            predicted_item->prod_num = predicted_p;
            char * predicted_key = create_item_key( predicted_item );
            if ( looked_up = hashlookup( current_items, predicted_key ) ) { // Item unneeded.
                free( predicted_item );
                free( predicted_key );
                continue;
            }

            // Otherwise, add the new item to the itemset, and to the unchecked stack.
            add_to_hash( current_items, predicted_key, (void *) predicted_item );
            free( predicted_key );
            append_to_list( unchecked, (void *) predicted_item );
        }
    }

    // Now that the itemset has been filled out, create and return its identifying string.
    key_list = new_list();
    list_keys_in_hash( current_items, key_list, "" );
    char * closure_key = join_key_list( key_list, "_" );
    destroy_key_list( key_list );
    return closure_key;
}


/*============================================================

Function: add_to_trie

Input: A pointer to a trie node, a string ( "path" ) to add,
       and a string ( "accepting_type" ) giving the type of
       token to use for that path.

Output: Nothing, but the given trie may have been added to.

============================================================*/

void add_to_trie( struct trie_node * trie, char * path, char * accepting_type ) {
    struct trie_node * current_node = trie;
    struct trie_node * next_node = NULL;
    char c;
    int i;
    int path_len = strlen( path );
    for ( i = 0; i < path_len; i++ ) {
        c = path[i];
        struct  charhash * looked_up = charhashlookup( current_node->child, c );
        if ( ! looked_up ) {
            next_node = new_trie_node();
            add_to_charhash( current_node->child, c, (void *) next_node );
        }
        else {
            next_node = looked_up->data;
        }
        if ( i == path_len - 1 ) { // Last char gets accepting type.
            next_node->accepting_type = accepting_type;
        }
        current_node = next_node;
    }
    return;
}


/*============================================================

Function: join_key_list

Input: A pointer to a key list, 
       and a string to use to join the keys.

Output: A newly-allocated string containing all the strings
        in the given key list joined together with the given string.

============================================================*/

char * join_key_list( struct list * key_list, char * glue_string ) {
    int i;
    char * key;
    int num_chars = 0;
    int num_keys = key_list->next_index;
    for ( i = 0; i < num_keys; i++ ) {
        key = listlookup( key_list, i );
        num_chars += strlen( key );
    }
    num_chars += ( num_keys - 1 ) * strlen( glue_string );
    char * joined_string = malloc( ( num_chars + 1 ) * sizeof( char ) );
    *joined_string = '\0';
    for ( i = 0; i < num_keys; i++ ) {
        key = listlookup( key_list, i );
        strcat( joined_string, key );
        if ( i + 1 < num_keys ) {
            strcat( joined_string, glue_string );
        }
    }
    return joined_string;
}


/*============================================================

Function: new_item

Input: Nothing

Output: A pointer to a newly-allocated item structure,
        having both of its fields set to zero.

============================================================*/

struct item * new_item() {
    struct item * this_item = malloc( sizeof( struct item ) );
    this_item->prod_num = 0;
    this_item->dot = 0;
    return this_item;
}


/*============================================================

Function: new_itemset

Input: Nothing

Output: A pointer to a newly-allocated itemset structure,
        having all of its hashes and lists empty.

============================================================*/

struct itemset * new_itemset() {
    struct itemset * this_itemset = malloc( sizeof( struct itemset ) );
    this_itemset->items = new_hash();
    this_itemset->ready_for = new_hash();
    this_itemset->complete = new_list();
    return this_itemset;
}



/*============================================================

Function: create_item_key

Input: A pointer to an item structure.

Output: A newly-allocated string, 
        uniquely identifying the given structure,
        relative to the current GRAMMAR.

============================================================*/

char * create_item_key( struct item * this_item ) {
    int key_num = NUM_PRODUCTIONS * this_item->prod_num + this_item->dot;
    return num_to_string( key_num );
}



/*============================================================

Function: new_transition

Input: Nothing

Output: A pointer to a newly-allocated transition structure,
        having all of it's fields initialized to zero.

============================================================*/

struct transition * new_transition() {
    struct transition * this_transition = malloc( sizeof( struct transition ) );
    this_transition->action = NULL;
    this_transition->arg = 0;
    return this_transition;
}


/*============================================================

Function: new_parser_generator

Input: Nothing

Output: A pointer to a newly-allocated parser_generator structure,
        having all of it's fields initialized to empty lists/hashes.

============================================================*/

struct parser_generator * new_parser_generator() {
    struct list * my_itemsets = new_list();
    struct hash * my_itemsets_by_key = new_hash();
    struct list * my_TABLE = new_list();
    struct list * my_GOTO = new_list();
    struct parser_generator * this_parser_generator = malloc( sizeof( struct parser_generator ) );
    this_parser_generator->itemsets = my_itemsets;
    this_parser_generator->itemsets_by_key = my_itemsets_by_key;
    this_parser_generator->TABLE = my_TABLE;
    this_parser_generator->GOTO = my_GOTO;
    return this_parser_generator;
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

Function: destroy_itemset

Input: A pointer to a newly-allocated itemset structure.

Output: Nothing, but all the memory used by the itemset has been freed.

============================================================*/

void destroy_itemset( struct itemset * this_itemset ) {
    int i;
    char * key;
    struct list * key_list = new_list();
    list_keys_in_hash( this_itemset->items, key_list, "" );
    for ( i = 0; i < key_list->next_index; i++ ) {
        key = listlookup( key_list, i );
        struct item * data = hashlookup( this_itemset->items, key )->data;
        free( data );
    }
    destroy_key_list( key_list );
    destroy_hash( this_itemset->items );
    key_list = new_list();
    list_keys_in_hash( this_itemset->ready_for, key_list, "" );
    for ( i = 0; i < key_list->next_index; i++ ) {
        key = listlookup( key_list, i );
        struct list * ready_items = hashlookup( this_itemset->ready_for, key )->data;
        destroy_list( ready_items );
    }
    destroy_key_list( key_list );
    destroy_hash( this_itemset->ready_for );
    destroy_list( this_itemset->complete );
    free( this_itemset );
    return;
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

char * hash_key( struct lambda_expr * left, struct lambda_expr * right, char * data ) {
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
