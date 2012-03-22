#ifndef _LAMBDA_PARSER_H_
#define _LAMBDA_PARSER_H_

#include "charhash.h"
#include "hash.h"

/*============================================================

Definition: token

============================================================*/

struct token {
    char * type;
    int first;
    int last;
};

/*============================================================

Definition: trie_node

============================================================*/

struct trie_node {
    char * accepting_type;
    struct charhash * child;
};


/*============================================================

Definition: transition

============================================================*/

struct transition {
    int action;
    int arg;
};

struct lambda_expr * parse( char *, char ** );
void fill_TABLE();
void fill_GOTO();
void init_NODE_HASH();
void fill_TRIE();
char ** get_production( int );
struct hash * get_NODE_HASH();
struct lambda_expr * compress_BETA( int, struct list * );
struct lambda_expr * compress_VAR( int, struct list * );
struct lambda_expr * compress_TEXT1( int, struct list * );
struct lambda_expr * compress_TEXT2( int, struct list * );
struct lambda_expr * transitive_down( int, struct list * );
struct lambda_expr * compress_LAMBDA1( int, struct list * );
struct lambda_expr * compress_LAMBDA2( int, struct list * );
struct trie_node * new_trie_node();
struct token * get_next_token( char *, int, struct trie_node * );
struct token * new_token();
char * hash_key( struct lambda_expr * left, struct lambda_expr * right, char * data, struct list * );

#endif // #ifndef _LAMBDA_PARSER_H_
