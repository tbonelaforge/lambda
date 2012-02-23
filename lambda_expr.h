#ifndef _LAMBDA_EXPR_H
#define _LAMBDA_EXPR_H

#include "hash.h"
#include "list.h"

/*============================================================

Definition: lambda_expr

============================================================*/

struct lambda_expr {
    char type[8]; // Can be 'LAMBDA', 'BETA', or 'VAR'
    char * data; // Should only be a variable name.
    struct hash * bound;
    struct hash * free;
    int parents;
    struct lambda_expr * child[2];
};



/*============================================================

Definition: lambda_search_state

============================================================*/

struct lambda_search_state {
    struct lambda_expr * parent;
    struct lambda_expr * current;
    int parent_choice;
    int progress;
};


struct lambda_expr * new_lambda_expr();
struct lambda_search_state * new_lambda_search_state();
void update_variables( struct lambda_expr * );
void update_lambda_variables( struct lambda_expr * );
void update_beta_variables( struct lambda_expr * );
void update_var_variables( struct lambda_expr * );
void expand_variables( struct hash *, struct hash * );
void refine_variables( struct hash *, struct hash *, struct hash * );
char * print_lambda_expr( struct lambda_expr * ); 
char * print_lambda_expr_address( struct lambda_expr * );
char * print_lambda_expr_type( struct lambda_expr * );
char * print_lambda_expr_data( struct lambda_expr * );
char * print_lambda_expr_variables( struct lambda_expr * );
void remove_lambda_expr_node( struct lambda_expr * );
//struct lambda_search_state search_lambda_expr( struct lambda_expr *, struct list * );
//int is_beta_reduction( struct lambda_expr * );
//struct lambda_search_state * push_child( int, struct lambda_search_state *, struct list * );
//void reduce( struct lambda_search_state, struct list *, struct hash * );
//struct lambda_expr * substitute( struct lambda_expr *, struct lambda_expr *, struct lambda_expr *, struct hash * );
struct lambda_expr * substitute( struct lambda_expr *, struct lambda_expr *, struct list * );
int process_substitution_candidate( struct lambda_search_state *, struct list * );
void rename_var( struct lambda_expr *, struct list * );
int process_rename_candidate( struct lambda_search_state *, struct list * );
int try_to_delete(struct lambda_expr *);
//void rename_bound_var( struct lambda_expr *, struct hash * );
char * fresh_variable( char *, struct hash * );
void change_child( int, struct list *, int, struct lambda_expr * );
int process_beta_reduction( struct lambda_search_state *, struct list * );
struct lambda_expr * pre_order_traverse( struct list *, int (*process_and_stop)( struct lambda_search_state *, struct list * ) );
struct lambda_expr * copy_lambda_expr( struct lambda_expr * );
void add_to_global_V( char *, struct lambda_expr * );
void delete_from_global_V( char * );

#endif // #ifndef _LAMBDA_EXPR_H
