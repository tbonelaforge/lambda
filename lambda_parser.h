#ifndef _LAMBDA_PARSER2_H_
#define _LAMBDA_PARSER2_H_

#include "charhash.h"
#include "hash.h"
#include "list.h"
#include "lambda_expr.h"

/*============================================================

Definition: ptr2function

A pointer to a function of ( production_number, children )

============================================================*/

typedef struct lambda_expr * (*ptr2function)( int, struct list * );


/*============================================================

Definition: grammar_entry

============================================================*/

struct grammar_entry {
    struct list * production;
    ptr2function reduction;    
};


/*============================================================

Definition: item

============================================================*/

struct item {
    int prod_num;
    int dot;
};

/*============================================================

Definition: itemset

============================================================*/

struct itemset {
    struct hash * items;
    struct hash * ready_for;
    struct list * complete;
};


/*============================================================

Definition: parser_generator

============================================================*/

struct parser_generator {
    struct list * itemsets;
    struct hash * itemsets_by_key;
    struct list * TABLE;
    struct list * GOTO;
};


/*============================================================

Definition: transition

============================================================*/

struct transition {
    char * action;
    int arg;
};


/*============================================================

Definition: trie_node

============================================================*/

struct trie_node {
    char * accepting_type;
    struct charhash * child;
};


/*============================================================

Definition: token

============================================================*/

struct token {
    char * type;
    int first;
    int last;
};



void set_GRAMMAR( struct grammar_entry *, int );
int get_NUM_PRODUCTIONS();
int get_GSIZE();
struct grammar_entry * get_GRAMMAR();
struct hash * get_IS_TERMINAL();
struct hash * get_PRODUCTIONS_FOR();
struct hash * get_FIRST();
struct hash * get_FOLLOW();
struct list * get_TABLE();
struct list * get_GOTO();
struct trie_node * get_TRIE();
struct hash * get_NODE_HASH();
char * get_INPUT();
void analyze_productions();
void fill_FIRST();
void fill_FOLLOW();
void fill_TABLES();
void fill_TRIE();
void add_initial_itemset( struct parser_generator * );
void install_incomplete_transitions( int, struct itemset *, struct parser_generator * );
void install_complete_transitions( int, struct itemset *, struct parser_generator * );
void install_incomplete_transition( int, char *, int, struct parser_generator * );
void init_NODE_HASH();
struct lambda_expr * parse( char *, char ** );
struct token * get_next_token( char *, int );
void add_to_trie( struct trie_node *, char *, char * );
char * create_item_key( struct item * );
char * create_closure_key( struct itemset * );
char * join_key_list( struct list *, char * );
struct item * new_item();
struct itemset * new_itemset();
struct parser_generator * new_parser_generator();
struct transition * new_transition();
struct trie_node * new_trie_node();
struct token * new_token();
void destroy_itemset( struct itemset * );
char * hash_key( struct lambda_expr *, struct lambda_expr *, char * );
#endif // #ifndef _LAMBDA_PARSER2_H_
