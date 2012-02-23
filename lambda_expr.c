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

Function: remove_lambda_expr_node

Input: A reference to a lambda_expr

Output: Nothing, but the memory used by that lambda_expr node,
        and it's variable hashes, has been destroyed.

============================================================*/

void remove_lambda_expr_node( struct lambda_expr * current ) {
    destroy_hash(current->bound);
    destroy_hash(current->free);
    free(current);
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

Function: search_lambda_expr

Input: A pointer to the root of an expression to be evaluated,
       and and empty stack.

Output: A lambda_search_state structure describing the state
        of affairs after searching for the first beta-reduction
        ( pre-order traversal ) in the given lambda_expr.
        If not found, the search state will have it's
        current field set to NULL. 
	Furthermore, the given stack will contain previous search
	states encountered in the path taken to get to the beta-reduction.

============================================================*/

/*
struct lambda_search_state search_lambda_expr( struct lambda_expr * root, struct list * stack ) {
    struct lambda_search_state * stack_top = malloc( sizeof( struct lambda_search_state ) );
    stack_top->parent = NULL;
    stack_top->current = root;
    stack_top->parent_choice = 0;
    stack_top->progress = 0;
    append_to_list( stack, (void *) stack_top );
    while ( stack->next_index ) {
	if ( ! stack_top ) {
	    stack_top = listlookup( stack, stack->next_index - 1 );
	}
	switch ( stack_top->progress ) {
	case 0:
	    stack_top->progress = 1;
	    if ( is_beta_reduction( stack_top->current ) ) { // Found.
		pop_from_list(stack);
		struct lambda_search_state positive_result = {
		    stack_top->parent,
		    stack_top->current,
		    stack_top->parent_choice,
		    stack_top->progress,
		};
		return positive_result;
	    }

	    // Fall through to next case.
	case 1:
	    stack_top->progress = 2;
	    if ( stack_top->current->child[0] ) {
		stack_top = push_child( 0, stack_top, stack );
	    }
	    break;
	case 2:
	    stack_top->progress = 3;
	    if ( stack_top->current->child[1] ) {
		stack_top = push_child( 1, stack_top, stack );
	    }
	    break;
	case 3:
	    free( pop_from_list( stack ) );
	    stack_top = NULL;  // Force re-lookup of stack_top.
	    break;
	default:
	    break;
	} // End switch on progress.
    } // End while loop.

    struct lambda_search_state negative_result = {
	NULL,
	NULL,
	0,
	0
    };
    return negative_result;
}
*/



/*
struct lambda_search_state search_lambda_expr( struct lambda_expr * root ) {
    struct list * stack = new_list();
    struct lambda_expr * parent = NULL;
    struct lambda_expr * current = NULL;
    struct lambda_expr * found = NULL;
    int progress = 0;
    int parent_choice = 0;
    struct lambda_search_state * stack_top = malloc(sizeof(struct lambda_search_state));
    stack_top->parent = NULL;
    stack_top->current = root;
    stack_top->parent_choice = 0;
    stack_top->progress = 0;
    append_to_list( stack, (void*) stack_top );
    while ( stack->next_index ) {
	if ( !stack_top ) {
	    stack_top = listlookup( stack, stack->next_index - 1 );
	}
	current = stack_top->current;
	switch ( stack_top->progress ) {
	case 0:
	    stack_top->progress = 1;
	    if ( !found ) {
		if ( is_beta_reduction(current) ) {
		    parent = stack_top->parent;
		    found = current;
		    progress = stack_top->progress;
		    parent_choice = stack_top->parent_choice;
		}
	    }

	    // Fall through to the next case.
	case 1:
	    stack_top->progress = 2;
	    if ( !found ) {
		if ( current->child[0] ) {
		    stack_top = push_child( 0, stack_top, stack );
		}
	    }
	    break;
	case 2:
	    stack_top->progress = 3;
	    if ( !found ) {
		if ( current->child[1] ) {
		    stack_top = push_child( 1, stack_top, stack );
		}
	    }
	    break;
	case 3:
	    free(pop_from_list(stack));
	    stack_top = NULL;
	    break;
	default:
	    break;
	} // End switch.
    } // End while
    destroy_hash(stack->main_hash->main_hash);
    free(stack->main_hash);
    free(stack);
    struct lambda_search_state result = {
	parent,
	found,
	parent_choice,
	progress
    };
    return result;
}
*/

/*============================================================

Function: is_beta_reduction

Input: A pointer to a lambda_expr

Output: 1 or 0, depending on whether the given node is a 
        candidate for beta-reduction or not.

============================================================*/

/*
int is_beta_reduction( struct lambda_expr * current ) {
    if ( strcmp( current->type, "BETA" ) ) {
	return 0;
    }
    if ( strcmp( current->child[0]->type, "LAMBDA" ) ) {
	return 0;
    }
    return 1;
}
*/

/*============================================================

Function: push_child

Input: A character ( 'l' or 'r' ), a pointer to a lambda_search_state
       structure, and a pointer to a stack.

Output: A pointer to a newly-allocated lambda_search_state structure,
        representing the state of affairs after moving from the current node
	in the given search state, to either the left or the right child,
	depending on whether the first argument was 'l' or 'r', respectively.
	Furthermore, the new search state has been pushed onto the given stack.

============================================================*/

/*
struct lambda_search_state * push_child( int requested_child, struct lambda_search_state * frame, struct list * stack ) {
    struct lambda_search_state * new_frame = malloc(sizeof(struct lambda_search_state));
    struct lambda_expr * current = frame->current;
    new_frame->parent = current;
    new_frame->progress = 0;
    new_frame->current = current->child[requested_child];
    new_frame->parent_choice = requested_child;
    append_to_list( stack, (void *) new_frame );
    return new_frame;
}
*/

/*============================================================

Function: reduce

Input: A lambda_search_state 
       ( consisting of parent, current, parent_choice, and progress ),
       representing a found beta-reduction,
       a pointer to a stack of previous search states encountered
       in the path to the beta-reduction,
       and a global hash V of used variable names.

Output: Nothing, but the beta-reduction has been reduced, 
        and the tree ( DAG ) has been altered accordingy.
	As a result, some memory may have been freed,
	and some new memory may have been allocated.

============================================================*/

/*
void reduce( struct lambda_search_state search_state, struct list * stack, struct hash * V ) {
printf("Inside reduce, got called.\n");
    struct lambda_expr * beta = search_state.current;
    struct lambda_expr * lambda = beta->child[0];
    struct lambda_expr * expr = beta->child[1];
    struct lambda_expr * var = lambda->child[0];
    struct lambda_expr * source = lambda->child[1];

    // Prepare to replace beta with source, 
    // by disconnecting source, and substituting expr into it.
    lambda->child[1] = NULL;
    source->parents -= 1;
printf("Inside reduce, about to call substitute.\n");
    source = substitute( expr, var, source, V );
printf("Inside reduce, made it back from the substitute call.\n");
    struct lambda_expr * parent = search_state.parent;
printf("Inside reduce, the parent ( whose child needs changed ) has address:<br />\n" );
printf( "%p<br />\n", parent );
    int parent_choice = search_state.parent_choice;
printf("Inside reduce, about to call change_child.\n");
    change_child( parent_choice, parent, source, stack );
printf("Inside reduce, made it back from the change_child call.\n");
    return;
}
*/

/*
void reduce ( struct lambda_search_state search_state, struct hash * V ) {
    struct lambda_expr * beta = search_state.current;
    struct lambda_expr * lambda = beta->child[0];
    struct lambda_expr * expr = beta->child[1];
    struct lambda_expr * var = lambda->child[0];
    struct lambda_expr * source = lambda->child[1];

    lambda->child[1] = NULL;
    source->parents -= 1; //Disconnect source and subsitute.
    source = substitute( expr, var, source, V );

    // Check if a copy needs to be made.
    if ( search_state.parent->parents > 1 ) {
	struct lambda_expr new_parent = new_lambda_expr();
	new_parent->bound = copy_hash( search_state.parent->bound );
	new_parent->free = copy_hash( search_state.parent->free );
	new_parent->child[search_state.parent_choice] = source;
	int static_child = (search_state.parent_choice + 1) % 2;
	new_parent->child[static_child] = search_state.parent->child[static_child];
	search_state.grandparent->child[search_state.grandparent_choice] = new_parent;
	new_parent->parents += 1;
	search_state.parent->parents -= 1;
    }
    search_state.parent->child[search_state.parent_choice] = source;
    source->parents += 1;
    try_to_delete(beta);
    return;
}
*/


/*============================================================

Function: substitute

Input: A pointer to a lambda expression to substitute ( expr ),
       a pointer to a VAR expression to look for ( var ),
       a pointer to a lambda expression to search through ( current ),
       and a global hash of used variable names ( V ).

Output: A pointer to the result of substituting 
        expr for occurrences of var in current, 
	after possibly renaming things in current according to V.

============================================================*/
/*
struct lambda_expr * substitute( struct lambda_expr * expr, struct lambda_expr * var, struct lambda_expr * current, struct hash * V ) {
    if ( !strcmp( current->type, "VAR" ) ){
	if ( current == var ) { //Replace with expr.
	    try_to_delete(current);
	    return expr;
	}
	return current;
    }
    if ( !strcmp( current->type, "LAMBDA" ) ) {
	if ( current->child[0] == var ) {
	    return current;
	}
	if ( hashlookup( expr->free, current->child[0]->data ) ) {
	    rename_bound_var( current, V );
	}
    }
    int i;
    for ( i = 0; i < 2; i++ ) {
	current->child[i]->parents -= 1; // Temporarily break link.
	current->child[i] = substitute( expr, var, current->child[i], V );
	current->child[i]->parents += 1; // Re-attach link.
    }
    return current;

}
*/

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
    struct lambda_expr * static_child = current_node->child[( choice + 1 ) % 2 ];
    if ( current_node->parents <= 1 ) { // Safe to change this one.
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

Output: A poiner to the node represented by the last search state encountered
        during the traversal.

============================================================*/

struct lambda_expr * pre_order_traverse( struct list * stack, int (*process_and_stop)( struct lambda_search_state *, struct list * ) ) {
    int done_processing = 0;
    int current_progress = 0;
    int original_i = stack->next_index - 1;
    struct lambda_search_state * stack_top = listlookup( stack, original_i );
    struct lambda_expr * current = stack_top->current;
    int original_progress = stack_top->progress;
    stack_top->progress = 0;
    while ( 1 ) {
	if ( ! stack_top ) {
	    stack_top = listlookup( stack, stack->next_index - 1 );
	    current = stack_top->current;
	}
	else {
	    // The variable current  may no longer point to valid memory,
	    // If, for example, the last processing was a beta reduction,
	    // but if so, then we are done processing, 
	    // and won't have to check current->child
	}
	current_progress = stack_top->progress;
	stack_top->progress += 1;
	if ( !current_progress && !done_processing ) {
	    done_processing = (*process_and_stop)( stack_top, stack );
	}
	else if ( current_progress < 3 
	    && !done_processing 
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
		return stack_top->current; // Get the latest changes.
	    }
	    free( pop_from_list(stack) );
	    stack_top = NULL;
	}
    }
}



/*============================================================

Function: change_child

Input: An integer ( 0 or 1 ), representing a child pointer to change,
       a pointer to a lambda_expr whose child pointer needs to change,
       a pointer to a lambda_expr to change the child pointer to,
       and a stack of search states such that 
       the lambda_expr to change is stack_top->current.

Output: Nothing, but the tree has been changed appropriately.
        As a result, some memory may have been freed,
	or some new memory will have been allocated.

============================================================*/

/*
void change_child( int choice, struct lambda_expr * node, struct lambda_expr * new_child, struct list * stack ) {
printf("Inside change_child, got called with choice = %d.\n", choice );
printf("The node whose child is to be changed has address:<br />\n");
printf( "%p<br />\n", node );
    if ( node->parents <= 1 ) {
printf("Inside change_child, realized that the given node has only one parent.\n");
	struct lambda_expr * old_child = node->child[choice];
	old_child->parents -= 1;
printf("Inside change_child, about to call try_to_delete.\n");
	try_to_delete(old_child);
printf("Inside change_child, made it back from try_to_delete.\n");
	node->child[choice] = new_child;
	new_child->parents += 1;
	return;
    }
    
    // Otherwise, more than one node is depending on the given node to be the way it is.
    // In order to effectively change the child, 
    // make a copy and connect it to the same point in the stack's path.
    int static_choice = ( choice + 1 ) % 2;
    struct lambda_expr * copy = copy_lambda_expr( node ); // The copy has 0 parents. 
    node->child[static_choice]->parents += 1; // The copy will also use static child.
    copy->child[choice] = new_child;
    new_child->parents += 1;
    struct lambda_search_state * stack_top = pop_from_list( stack );
    change_child( stack_top->parent_choice, stack_top->parent, copy, stack );
    return;
}
*/

/*


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
    return pre_order_traverse( stack, &process_substitution_candidate );
}



/*============================================================

Function: process_substitution_candidate

Input: A search state to process, 
       and stack of lambda search states
       ( having the given state at the top ).

Output: 0, but the node represented by the state at the top
        of the stack has been processed as a substitution 
	candidate, according to the global VAR and EXPR constants.
	The stack has been changed if necessary.

============================================================*/

int process_substitution_candidate( struct lambda_search_state * current_state, struct list * stack ) {
    int original_i = stack->next_index - 1;
    if ( current_state->current == VAR ) { // This node matches previously set global VAR.
	
	// Use change_child to substitute EXPR for VAR.
	change_child( current_state->parent_choice, stack, original_i - 1, EXPR );
    }
    else if ( !strcmp( current_state->current->type, "LAMBDA" ) ) {
	struct lambda_expr * left_child = current_state->current->child[0];
	if ( left_child == VAR ) { // This lambda's bound var is the same as VAR.
	    return 1; // Don't proceed any further down this path.
	}
	char * varname = left_child->data;
	if ( hashlookup( EXPR->free, varname ) ) {
//	    rename_var( left_child, current_state->current );
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
    pre_order_traverse( stack, &process_rename_candidate );
    return;
}



/*============================================================

Function: process_rename_candidate

Input: A pointer to a search state representing a rename candidate, 
       and a pointer to a list ( stack ) of search states, 
       such that the given state is on top.

Output: 0, but the node represented by the search state has been renamed,
        if applicable, using the global vars TO_BE_RENAMED, and FRESH_VAR.

============================================================*/

int process_rename_candidate( struct lambda_search_state * current_state, struct list * stack ) {
    if ( current_state->current == TO_BE_RENAMED ) {
	change_child( 
	    current_state->parent_choice,
	    stack,
	    stack->next_index - 2,
	    FRESH_VAR
        );	     
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

Function: rename_bound_var

Input: A pointer to a lambda expression of type "LAMBDA",
       whose variable ( left child ) needs to be renamed,
       and a global hash V of used variable names.

Output: Nothing, but all instances of the original bound variable
        have been replaced by a fresh variable.

============================================================*/

/*
void rename_bound_var( struct lambda_expr * current, struct hash * V ) {
    struct lambda_expr * old_var = current->child[0];
    struct lambda_expr * body = current->child[1];
    char * fresh_varname = fresh_variable( old_var->data, V );
    struct lambda_expr * new_var = new_lambda_expr();
    new_var->data = fresh_varname;
    strcpy( new_var->type, "VAR" );
    old_var->parents -= 1;
    current->child[0] = new_var;
    new_var->parents += 1;
    if ( try_to_delete(old_var) ) {
	return; // No more instances to replace.
    }
    body->parents -= 1;
    body = substitute( new_var, old_var, body, V );
    body->parents += 1;
    current->child[1] = body;
    return;
}
*/

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

Function: delete_from_global_V

Input: A string.

Output: Nothing, but the given key has been deleted from the global hash V.

============================================================*/

void delete_from_global_V( char * key ) {
    delete_from_hash( V, key );
    return;
}
