#ifndef _HASH_H_
#define _HASH_H_

#include "list.h"

struct list; // Forward declaration.


/*====================================================================================

Definition: hash


====================================================================================*/


struct hash {

    void * data;
    char * sigstr;
    int num_children;
    struct charhash * child; // N-ary tree.

}; // End hash definition


/*====================================================================================

Definition: hash_search_state

====================================================================================*/

struct hash_search_state {
    struct hash * parent;
    struct hash * node;
    char parent_choice;
    char * str;
    int num_matched;
    char config;
};



char * hashinfo( struct hash *, char *);
char * hashtreeprint( struct hash *, char *);
int longest_common_prefix( char *, char *, char * );
struct hash * hashlookup( struct hash *, char * );
struct hash * new_hash();
char * sub_strcpy( char *, char *, int );
struct hash * add_to_hash( struct hash *, char *, void * );
struct hash_search_state hash_search( struct hash*, char * );
struct hash * hash_leaf_node( struct hash_search_state, void* );
struct hash * hash_branch_node( struct hash_search_state );
struct hash * add_pairs_to_hash( struct hash * my_hash, int num_pairs, ... );
char * join_strings_with_char( char, char *, char * );
void delete_from_hash( struct hash *, char * );
void remove_hash_leaf_node( struct hash_search_state );
void absorb_single_hash_child( struct hash_search_state );
void list_keys_in_hash( struct hash *, struct list *, char * );
void destroy_hash( struct hash * );
void destroy_key_list( struct list * );
char * print_hash_key_set( struct hash * );
struct hash * copy_shallow_hash( struct hash * );

#endif // #ifndef _HASH_H_
