#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hash.h"
#include "list.h"

#include "lambda_expr.h"

#define TYPE_LEN 10;
#define TABLE_TAG_LEN 30;
#define TR_TAG_LEN 35
#define BR_TAG_LEN 7;
#define TD_TAG_LEN 20;
#define COMMA_LEN 1;

struct hash * V = NULL;
struct lambda_expr * VAR = NULL;
struct lambda_expr * EXPR = NULL;
struct lambda_expr * TO_BE_RENAMED = NULL;
struct lambda_expr * FRESH_VAR = NULL;


/*============================================================

Function: new_lambda_expr

Input: Nothing

Output: A reference to a newly-allocated lambda_expr,
        with everything empty.

============================================================*/

struct lambda_expr * new_lambda_expr() {
    struct lambda_expr * new_expr =  malloc(sizeof(struct lambda_expr));
    if ( !new_expr ) {
	return NULL;
    }
    strcpy( new_expr->type, "" );
    new_expr->data = NULL;
    new_expr->bound = new_hash();
    if ( !new_expr->bound ) {
	return NULL;
    }
    new_expr->free = new_hash();
    if ( !new_expr->free ) {
	return NULL;
    }
    new_expr->child[0] = NULL;
    new_expr->child[1] = NULL;
    new_expr->parents = 0;
    return new_expr;
}



/*============================================================

Function: new_lambda_search_state

Input: Nothing

Output: A reference to a newly-allocated lambda_search_state,
        with everything empty.

============================================================*/

struct lambda_search_state * new_lambda_search_state() {
    struct lambda_search_state * new_lambda_search_state = 
	malloc( 1 * sizeof( struct lambda_search_state ) );
    new_lambda_search_state->parent = NULL;
    new_lambda_search_state->current = NULL;
    new_lambda_search_state->parent_choice = 0;
    new_lambda_search_state->progress = 0;
    return new_lambda_search_state;
}



/*============================================================

Function: update_variables

Input: A reference to a lambda_expr.

Output: Nothing, but the free and bound variables of the
        given expression have been updated.


============================================================*/

void update_variables( struct lambda_expr * expr ) {
    struct lambda_expr * child;
    if ( child = expr->child[0] ) {
	update_variables(child);
    }
    if ( child = expr->child[1] ) {
	update_variables(child);
    }
    if ( !strcmp( expr->type, "LAMBDA" ) ) {
	update_lambda_variables(expr);
	return;
    }
    if ( !strcmp( expr->type, "BETA" ) ) {
	update_beta_variables(expr);
	return;
    }
    if ( !strcmp( expr->type, "VAR" ) ) {
	update_var_variables(expr);
	return;
    }
    return;
}



/*============================================================

Function: update_lambda_variables

Input: A reference to a lambda_expr of type "LAMBDA".

Output: Nothing, but the free and bound variables of the 
        given expression are updated.

============================================================*/

void update_lambda_variables(struct lambda_expr * current) {
    struct lambda_expr * left = current->child[0];
    struct lambda_expr * right = current->child[1];

    // The bound variables of this lambda are
    // the bound variables of the right expr, plus left_var.
    expand_variables( current->bound, left->free );
    expand_variables( current->bound, right->bound );
    refine_variables( current->bound, left->free, right->bound );

    // The free vars of this lambda are right->free - { left_var }
    expand_variables( current->free, right->free );
    delete_from_hash( current->free, left->data );
    refine_variables( current->free, left->bound, right->free );
    return;
}


/*============================================================

Function: update_beta_variables

Input: A reference to a lambda_expr of type 'BETA'.

Output: Nothing, but the free and bound variables 
        of the given expressionhave been updated.

============================================================*/

void update_beta_variables(struct lambda_expr * current) {
    struct lambda_expr * left = current->child[0];
    struct lambda_expr * right = current->child[1];

    // The bound variables of this beta come from both children.
    expand_variables( current->bound, left->bound );
    expand_variables( current->bound, right->bound );
    refine_variables( current->bound, left->bound, right->bound );

    // The free variables of this beta come from both children.
    expand_variables( current->free, left->free );
    expand_variables( current->free, right->free );
    refine_variables( current->free, left->free, right->free );
    return;
}


/*============================================================

Function: update_var_variables

Input: A reference to a lambda_expr of type 'VAR'.

Output: Nothing, but the free and bound variables are updated.

============================================================*/

void update_var_variables( struct lambda_expr * current ) {
    add_to_hash( current->free, current->data, (void *) 1 );
    struct list * var_list = new_list();
    list_keys_in_hash( current->free, var_list, "" );
    int var_num = var_list->next_index;
    while ( var_num-- ) {
	char * var = listlookup( var_list, var_num );
	if ( strcmp( var, current->data ) ) {
	    delete_from_hash( current->free, var );
	}
    }
    destroy_key_list(var_list);
    return;
}



/*============================================================

Function: expand_variables

Input: A reference to a target hash to expand, 
       and a reference to a source hash to use when expanding.

Output: Nothing, but the target hash contains all the variables
        contained in the source hash.

============================================================*/

void expand_variables( struct hash * target, struct hash * source ) {
    struct list * var_list = new_list();
    list_keys_in_hash( source, var_list, "" );
    int var_num = var_list->next_index;
    while ( var_num-- ) {
	char * var = listlookup( var_list, var_num );
	add_to_hash( target, var, (void *) 1 );
    }
    destroy_key_list(var_list);
    return;
}


/*============================================================

Function: refine_variables

Input: A reference to a target hash to refine, 
       and references to two source hashes to use when refining.

Output: Nothing, but the given source hash doesn't contain
        any variables that aren't in either source hash.

============================================================*/

void refine_variables( struct hash * target, struct hash * source_a, struct hash * source_b ) {
    struct list * var_list = new_list();
    list_keys_in_hash( target, var_list, "" );
    int var_num = var_list->next_index;
    while ( var_num-- >= 0 ) {
	char * var = listlookup( var_list, var_num );
	if ( ! var ) {
	    continue;
	}
	if ( hashlookup( source_a, var ) ) {
	    continue;
	}
	if ( hashlookup( source_b, var ) ) {
	    continue;
	}
	delete_from_hash( target, var );
    }
    destroy_key_list(var_list);
    return;
}



/*============================================================

Function: print_lambda_expr

Input: A reference to a lambda_expr.

Output: A string of html representing the 
        structure of the given lambda_expr.

============================================================*/

char * print_lambda_expr( struct lambda_expr * current ) {
    if ( ! current ) {
	char * temp = malloc( 1*sizeof(char) );
	*temp = '\0';
	return temp;
    }
    char * address_str = print_lambda_expr_address( current );
    char * type_str = print_lambda_expr_type(current);
    char * data_str = print_lambda_expr_data(current);
    char * variables_str = print_lambda_expr_variables(current);
    char *  left_recursion = print_lambda_expr(current->child[0]);
    char * right_recursion = print_lambda_expr(current->child[1]);
    int table_tag_len = TABLE_TAG_LEN;
    int tr_tag_len = TR_TAG_LEN;
    int br_tag_len = BR_TAG_LEN;
    int td_tag_len = TD_TAG_LEN;
    char * lambda_expr_str = malloc(
    ( 
     2 * table_tag_len
     + 4 * tr_tag_len
     + 2 * td_tag_len
     + strlen(address_str) + br_tag_len
     + strlen(type_str) + br_tag_len
     + strlen(data_str) + br_tag_len
     + strlen(variables_str) + br_tag_len
     + 4 * td_tag_len
     + strlen(left_recursion) + strlen(right_recursion)
     + 1 
     ) * sizeof(char)
				    );

    *lambda_expr_str = '\0';
    sprintf( lambda_expr_str,
	     "<table cellpadding=\"5\">"
	       "<tr align=\"center\">"
	         "<td colspan=\"2\">"
	           "%s<br />"
	           "%s<br />"
	           "%s<br />"
	           "%s<br />"
	         "</td>"
	       "</tr>"
	       "<tr align=\"center\" valign=\"top\">"
	         "<td>%s</td><td>%s</td>"
	       "</tr>"
	     "</table>",
	     address_str,
	     type_str,
	     data_str,
	     variables_str,
	     left_recursion,
	     right_recursion
	     );
    free(address_str);
    free(type_str);
    free(data_str);
    free(variables_str);
    free(left_recursion);
    free(right_recursion);
    return lambda_expr_str;
}

/*============================================================

Function: print_lambda_expr_address

Input: A reference to a lambda_expr.

Output: A reference to a newly-allocated string
        showing the address of that node.

============================================================*/


char * print_lambda_expr_address( struct lambda_expr * current ) {
    char * address_str = malloc( 50 * sizeof(char) );
    sprintf( address_str, "%p", current );
    return address_str;
}



/*============================================================

Function: print_lambda_expr_type

Input: A reference to a lambda_expr.

Output: A reference to a newly-allocated string 
        representing the type of the given node.

============================================================*/

char * print_lambda_expr_type( struct lambda_expr * current ) {
    int type_len = TYPE_LEN;
    char * type_str = malloc( ( type_len + 1 ) * sizeof(char) );
    *type_str = '\0';
    if ( !strcmp( current->type, "LAMBDA" ) ) {
	sprintf( type_str, "LAMBDA" );
	return type_str;
    }
    if ( !strcmp( current->type, "BETA" ) ) {
	sprintf( type_str, "BETA" );
	return type_str;
    }
    if ( !strcmp( current->type, "ROOT" ) ) {
	sprintf( type_str, "ROOT" );
	return type_str;
    }
    sprintf( type_str, "VAR" );
    return type_str;
}



/*============================================================

Function: print_lambda_expr_data

Input: A reference to a lambda_expr.

Output: A reference to a newly-allocated string,
        representing the type of the given lambda_expr.

============================================================*/

char * print_lambda_expr_data( struct lambda_expr * current ) {
    char * data_str;
    if ( !current->data ) {
	data_str = malloc( 1 * sizeof(char) );
	*data_str = '\0';
	return data_str;
    }
    data_str = malloc( ( 1 + strlen(current->data) ) * sizeof(char) );
    *data_str = '\0';
    sprintf( data_str, "%s", current->data );
    return data_str;
}


/*============================================================

Function: print_lambda_expr_variables

Input: A reference to a lambda_expr.

Output: A reference to a newly-allocated string
        representing the variables ( both bound and free )
	of the given lambda_expr.

============================================================*/

char * print_lambda_expr_variables( struct lambda_expr * current ) {
    char * bound_str = print_hash_key_set(current->bound);
    char * free_str = print_hash_key_set(current->free);
    int comma_len = COMMA_LEN;
    char * variables_str = malloc(
				  ( strlen(bound_str) 
				    + comma_len 
				    + strlen(free_str)
				    + 1 
				  ) 
				  * sizeof(char)
				  );
    *variables_str = '\0';
    sprintf( variables_str, "%s,%s", bound_str, free_str );
    free(bound_str);
    free(free_str);
    return variables_str;
}



/*============================================================

Function: fresh_variable

Input: a string (x), 
          and a pointer to a hash of globally-used variable
	  names.

Output: A pointer to a newly-allocated string such that
        the new string is not a key in the given hash.

============================================================*/

char * fresh_variable( char * x, struct hash * V ) {
    char * new_x = NULL;
    do {
	if ( new_x ) {
	    free(new_x);
	}
	new_x = malloc( ( strlen(x) + 2 ) * sizeof(char) );
	strcpy( new_x, x );
	strcat( new_x, "'" );
	x = new_x;
    } while ( hashlookup( V, new_x ) );
    add_to_hash( V, new_x, (void *) 1 );
    return new_x;
}


/*============================================================

Function: change_child

Input: An integer ( 0 or 1 ), 
       a pointer to a list ( stack ), of lambda search states,
       an index i into the stack, corresponding to the node to whose child needs changed,
       and a pointer to a new node to attach at the specified position.

Output: Nothing, but the child ( and possibly other children )
        has been changed and memory possibly freed or allocated.

============================================================*/

void change_child( int choice, struct list * stack, int i, struct lambda_expr * new_child ) {
    struct lambda_search_state * current_state = listlookup( stack, i );
    struct lambda_expr * current_node = current_state->current;
    struct lambda_expr * old_child = current_node->child[choice];
    if ( new_child == old_child ) {
	return;
    }
    struct lambda_expr * static_child = current_node->child[( choice + 1 ) % 2 ];

    if ( current_node->parents == 0 ) { // Safe to change this one.
	old_child->parents -= 1;
	current_node->child[choice] = new_child;
	new_child->parents += 1;
        if ( i + 1 < stack->next_index ) {
	    struct lambda_search_state * later_state = listlookup( stack, i + 1 );
	    if ( later_state->parent_choice == choice ) { // Change stack.
		later_state->current = new_child;
	    }
	}
	try_to_delete( old_child );
	return;
    }

    // Otherwise, make a copy of the node in question, and change THAT.
    struct lambda_expr * node_copy = copy_lambda_expr( current_node );
    static_child->parents += 1; // The copy now references the same static child.
    node_copy->child[choice] = new_child;
    new_child->parents += 1;
    current_state->current = node_copy;

    // Check if any other later search states need changed.
    if ( i + 1 < stack->next_index ) {
	struct lambda_search_state * later_state = listlookup( stack, i + 1 );
	later_state->parent = node_copy;
	if ( later_state->parent_choice == choice ) { // Change stack.
	    later_state->current = new_child;
	}
    }

    // Connect the new copy into the old path.
    change_child( current_state->parent_choice, stack, i - 1, node_copy );
}

/*============================================================

Function: process_beta_reduction

Input: A pointer to a search state representing a beta-reduction candidate, 
       and a list ( stack ) of search states, 
       such that the given state is on top.

Output: 1 or 0, depending on whether the node represented by the state
        on top of the stack was a beta-reduction, and has been reduced,
	or not.

============================================================*/

int process_beta_reduction( struct lambda_search_state * current_state, struct list * stack ) {
    int original_i = stack->next_index - 1;
    struct lambda_expr * beta = current_state->current;
    if ( strcmp( beta->type, "BETA" ) ) {
	return 0; // Must be a BETA to be a beta-reduction.
    }
    struct lambda_expr * lambda = beta->child[0];
    struct lambda_expr * expr = beta->child[1];
    if ( strcmp( lambda->type, "LAMBDA" ) ) {
	return 0; // Must have a LAMBDA as left child to be reduced.
    }
    struct lambda_expr * parent = current_state->parent;
    int parent_choice = current_state->parent_choice;
    struct lambda_search_state * new_search_state = new_lambda_search_state();
    new_search_state->current = lambda;
    new_search_state->parent = beta;
    new_search_state->parent_choice = 0;
    current_state->progress = 2;
    append_to_list( stack, (void*) new_search_state ); // Push the "lambda" search state.
    struct lambda_expr * var = lambda->child[0];
    struct lambda_expr * source = lambda->child[1];
    new_search_state->progress = 3;
    new_search_state = new_lambda_search_state();
    new_search_state->current = source;
    new_search_state->parent = lambda;
    new_search_state->parent_choice = 1; // Source is right child of lambda.
    append_to_list( stack, (void*) new_search_state ); // Push the "source" search state.
    struct lambda_expr * result_of_substitution = substitute( expr, var, stack );
    free( pop_from_list( stack ) ); // Free the "source" search state.
    free( pop_from_list( stack ) ); // Free the "lambda" search state.

    // Now use change_child to actually perform the reduction, and clean up memory.
    change_child( parent_choice, stack, original_i - 1, result_of_substitution );
    return 1;
    
}

/*============================================================

Function: pre_order_traverse

Input: A stack, with the starting node represented by the search
       state at the top of the stack, 
       and a function (returning 0 or 1) to be run on each search state encountered
       during the traversal.

Output: 1 or 0, depending on whether the traversal ended prematurely ( done processing = 1 )
        or not ( the whole tree was traversed ).

============================================================*/

int pre_order_traverse( struct list * stack, int (*process_and_stop)( struct lambda_search_state *, struct list * ), int lazy ) {
    int changed = 0;
    int current_progress = 0;
    int original_i = stack->next_index - 1;
    struct lambda_search_state * stack_top = listlookup( stack, original_i );
    struct lambda_expr * current = stack_top->current;
    int original_progress = stack_top->progress;
    stack_top->progress = 0;
    while ( 1 ) {
	if ( ! stack_top ) {
	    stack_top = listlookup( stack, stack->next_index - 1 );
	}
	current = stack_top->current;
	current_progress = stack_top->progress;
	stack_top->progress += 1;
	if ( !current_progress && ( !changed || !lazy ) ) {
	    int processed = (*process_and_stop)( stack_top, stack );
	    if ( processed ) {
		stack_top->progress = 3; // Don't process kids.
	    }
	    changed = changed || processed;
	}
	else if ( current_progress < 3 
            && ( !changed || !lazy )
	    && current->child[current_progress - 1] ) {
	    struct lambda_search_state * new_search_state = new_lambda_search_state();
	    new_search_state->parent = current;
	    new_search_state->current = current->child[current_progress - 1];
	    new_search_state->parent_choice = current_progress - 1;
	    append_to_list( stack, (void*) new_search_state );
	    stack_top = NULL;
	}
	else { // Node has finished processing.
	    if ( stack->next_index - 1 == original_i ) {
		stack_top->progress = original_progress; // Restore.
		return changed;
	    }
	    free( pop_from_list(stack) );
	    stack_top = NULL;
	}
    }
}


/*============================================================

Function: substitute

Input: A pointer to a lambda expression (expr) to be substituted,
       a pointer to a lambda expression (var) to be search for,
       and a pointer to a list ( stack ) such that the source expression
       is represented by the search state on top of the stack.

Output: A pointer to the lambda expression which is the result of
        substituting expr for occurrences of var in source.

============================================================*/

struct lambda_expr * substitute( struct lambda_expr * expr, struct lambda_expr * var, struct list * stack ) {

    // Set global vars to be accessed during the traversal.
    EXPR = expr;
    VAR = var;

    // Perform a non-lazy traversal, 
    // making sure that all substitution candidates are processed.
    pre_order_traverse( stack, &process_substitution_candidate, 0 );
    struct lambda_search_state * stack_top = listlookup( stack, stack->next_index - 1 );
    return stack_top->current;
}



/*============================================================

Function: process_substitution_candidate

Input: A search state to process, 
       and stack of lambda search states
       ( having the given state at the top ).

Output: 0 or 1, 
        depending on whether the node represented by the state at the top
        of the stack has been processed as a sustitution 
	candidate or not, according to the global VAR and EXPR constants.
	The stack has been changed if necessary.

============================================================*/

int process_substitution_candidate( struct lambda_search_state * current_state, struct list * stack ) {
    int original_i = stack->next_index - 1;
    if ( current_state->current == VAR ) { // This node matches previously set global VAR.

	// Use change_child to substitute EXPR for VAR.
	change_child( current_state->parent_choice, stack, original_i - 1, EXPR );
	return 1;
    }
    else if ( !strcmp( current_state->current->type, "LAMBDA" ) ) {
	struct lambda_expr * left_child = current_state->current->child[0];
	if ( left_child == VAR ) { // This lambda's bound var is the same as VAR.
	    return 1; // Don't proceed any further down this path.
	}
	char * varname = left_child->data;
	if ( hashlookup( EXPR->free, varname ) ) {
	    rename_var( left_child, stack );
	}
    }
    return 0;
}

/*============================================================

Function: rename_var

Input: A pointer to a lambda expression of type VAR, 
       and a pointer to stack, such that the expression to
       search through is on the top of the stack.

Output: Nothing, but all occurrences of the given VAR node
        have been renamed to a fresh variable.

============================================================*/

void rename_var( struct lambda_expr * var, struct list * stack ) {
    TO_BE_RENAMED = var;
    char * fresh_varname = fresh_variable( var->data, V );
    struct lambda_expr * new_var = new_lambda_expr();
    strcpy( new_var->type, "VAR" );
    new_var->data = fresh_varname;
    FRESH_VAR = new_var;
    add_to_global_V( fresh_varname, new_var );

    // Perform a non-lazy traversal,
    // making sure that all rename candidates are processed.
    pre_order_traverse( stack, &process_rename_candidate, 0 );
    return;
}



/*============================================================

Function: process_rename_candidate

Input: A pointer to a search state representing a rename candidate, 
       and a pointer to a list ( stack ) of search states, 
       such that the given state is on top.

Output: 1 or 0, 
        depending on if the node represented by the search state has been renamed,
        using the global vars TO_BE_RENAMED, and FRESH_VAR.

============================================================*/

int process_rename_candidate( struct lambda_search_state * current_state, struct list * stack ) {
    if ( current_state->current == TO_BE_RENAMED ) {
	change_child( 
	    current_state->parent_choice,
	    stack,
	    stack->next_index - 2,
	    FRESH_VAR
        );
	return 1;
    }
    return 0;
}

/*============================================================

Function: try_to_delete

Input: A pointer to a lambda expression.

Output: 1 or 0, depending on whether the memory used in the given
        lambda expression was succcessfully freed or not.  
	If so, the memory used in any sub-expressions has
        been freed as well, if possible.

============================================================*/


int try_to_delete(struct lambda_expr * current) {
    if ( current->parents ) {
	return 0;
    }
    if ( current->child[0] ) {
	current->child[0]->parents -= 1;
	try_to_delete(current->child[0]);
    }
    if ( current->child[1] ) {
	current->child[1]->parents -= 1;
	try_to_delete(current->child[1]);
    }
    destroy_hash(current->bound);
    destroy_hash(current->free);
    if ( current->data ) {
	delete_from_global_V( current->data );
	free(current->data);
    }
    free(current);
    return 1;
}


/*============================================================

Function: copy_lambda_expr

Input: A pointer to a lambda_expr

Output: A pointer to a newly-allocated lambda_expr structure,
        having the same type, data, and children.
	It's parents counter is set to 0,
	and its bound and free hashes are copies 
	of the given node's bound and free variables.

============================================================*/


struct lambda_expr * copy_lambda_expr( struct lambda_expr * original ) {
    struct lambda_expr * copy = malloc( sizeof( struct lambda_expr ) );
    copy->parents = 0;
    strcpy( copy->type, original->type );
    copy->data = original->data; // Should only ever be NULL.
    copy->child[0] = original->child[0];
    copy->child[1] = original->child[1];
    copy->bound = copy_shallow_hash( original->bound );
    copy->free = copy_shallow_hash( original->free );
    return copy;
}

/*============================================================

Function: add_to_global_V

Input: A string, and a pointer to a VAR node containing that string.

Output: Nothing, but the given key, value pair has been added to the global hash V.

============================================================*/

void add_to_global_V( char * key, struct lambda_expr * value ) {
    if ( !V ) {
	V = new_hash();
    }
    add_to_hash( V, key, (void*) value );
    return;
}


/*============================================================

Function: set_global_V

Input: A pointer to a hash to use as V.

Output: Nothing, but this module's V is set to the given hash.

============================================================*/

void set_global_V( struct hash * varhash ) {
    V = varhash;
}

struct hash * get_global_V() {
    return V;
}


/*============================================================

Function: delete_from_global_V

Input: A string.

Output: Nothing, but the given key has been deleted from the global hash V.

============================================================*/

void delete_from_global_V( char * key ) {
    delete_from_hash( V, key );
    return;
}


/*============================================================

Function: evaluate

Input: A pointer to a lambda_expr.

Output: A pointer to an evaluated version of the given lambda_expr.

============================================================*/

struct lambda_expr * evaluate( struct lambda_expr * expr ) {
    struct lambda_expr * root = new_lambda_expr();
    strcpy( root->type, "ROOT" );
    root->child[0] = expr;
    expr->parents += 1;

    struct lambda_search_state * state1 = new_lambda_search_state();
    state1->parent = NULL;
    state1->current = root;
    state1->parent_choice = 0;
    state1->progress = 0;

    struct lambda_search_state * state2 = new_lambda_search_state();
    state2->parent = root;
    state2->current = expr;
    state2->parent_choice = 0;
    state2->progress = 0;

    struct list * stack = new_list();
    append_to_list( stack, state1 );
    append_to_list( stack, state2 );

    int change = 0;
    update_variables( root );

    while ( change = pre_order_traverse( stack, &process_beta_reduction, 1 ) ) {
	update_variables( root );
    }
    expr = root->child[0];
    root->child[0] = NULL;
    expr->parents -= 1;
    try_to_delete( root );
    free( state2 );
    free( state1 );
    return expr;
}


/*============================================================

Function: bind_free_variable

Input: A pointer to an expression, 
       and a pointer to a namespace hash 
       ( varnames -> expresssions ), to use when binding.

Output: Either a new expression, 
        resulting from substituting one expression from the namespace
	in for one free variable of the expression, or
	NULL, if not possible.

============================================================*/

struct lambda_expr * bind_free_variable( struct lambda_expr * source, struct hash * namespace ) {
    struct list * free_vars = new_list();
    list_keys_in_hash( source->free, free_vars, "" );
    int i;
    struct lambda_expr * result = NULL;
    for ( i = 0; i < free_vars->next_index; i++ ) {
	char * free_var = listlookup( free_vars, i );
	struct hash * looked_up = hashlookup( namespace, free_var );
	if ( ! looked_up ) {
	    continue;
	}
	struct lambda_expr * expr = looked_up->data;
	if ( hashlookup( expr->free, free_var ) ) {
	    continue; // Don't substitute self-referential structures.
	}

	struct lambda_expr * temp_lambda = new_lambda_expr();
	strcpy( temp_lambda->type, "LAMBDA" );
	struct lambda_expr * bound_var = hashlookup( V, free_var )->data;
	temp_lambda->child[0] = bound_var;
	bound_var->parents += 1;
	temp_lambda->child[1] = source;
	source->parents += 1;

	struct lambda_expr * temp_beta = new_lambda_expr();
	strcpy( temp_beta->type, "BETA" );
	temp_beta->child[0] = temp_lambda;
	temp_lambda->parents += 1;
	temp_beta->child[1] = expr;
	expr->parents += 1;
	result = evaluate( temp_beta );
	break;
    }
    destroy_key_list( free_vars );
    return result;
}



/*============================================================

Function: evaluate_namespace

Input: A pointer to a hash of evaluated lambda_expressions, 
       indexed by name.

Output: Nothing, but any cross-references in the namespace have been
        "filled-out", such that the original names now point to 
	the fully-evaluated expressions.

============================================================*/

void evaluate_namespace( struct hash * namespace ) {
    struct list * names = new_list();
    list_keys_in_hash( namespace, names, "" );
    int changed = 1;
    int i;
    struct lambda_expr * named_expr;
    struct lambda_expr * changed_expr;
    char * name;
    while ( changed ) {
	changed = 0;
	for ( i = 0; i < names->next_index; i++ ) {
	    name = listlookup( names, i );
	    named_expr = hashlookup( namespace, name )->data;
	    if ( hashlookup( named_expr->free, name ) ) {
		continue; // Don't mess with recursive definitions.
	    }
	    
	    // Uninstall expression from namespace while trying to evaluate.
	    named_expr->parents -= 1;
	    changed_expr = bind_free_variable( named_expr, namespace );
	    if ( changed_expr ) {
		add_to_hash( namespace, name, changed_expr );
		changed_expr->parents += 1; // Install the new expression in the namespace.
		changed = 1;
		break;
	    }
	    named_expr->parents += 1; // Install the expression back into the namespace.
	}
    }
}
