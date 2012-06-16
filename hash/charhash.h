#ifndef _CHARHASH_H_
#define _CHARHASH_H_

#define BYTESIZE 8


/*============================================================================================

Definition: charhash

============================================================================================*/

struct charhash {

    void * data;
    char sig; // Hold a substring of a key.
    char siglen; // The number of significant bits (from the right) in the sig.
    struct charhash * child[2]; // The children are indexed by 0 and 1.

};

/*============================================================================================

Definition: char_search_state

============================================================================================*/

struct char_search_state {
    struct charhash * parent;
    struct charhash * node;
    char parent_choice;
    char sig;
    char siglen;
    char num_matched;
};

char * binarystring( char );
char * blankmask( char *, char );
char reverse_bits( int, char );
char * info( struct charhash * );
char * charhashtreeprint( struct charhash * );
char longest_common_suffix( char, char, char, char );
struct charhash * new_charhash();
struct charhash * add_to_charhash( struct charhash *, char, void * );
struct charhash * create_branch_node(struct char_search_state);
struct charhash * create_leaf_node( struct char_search_state, void * );
char * chars_in( struct charhash *, char, char );
struct charhash * charhashlookup( struct charhash *, char );
char num_bits( char );
void delete_from_charhash( struct charhash *, char );
void destroy_charhash( struct charhash * );
void absorb_single_charhash_child( struct char_search_state );
void remove_leaf_node( struct char_search_state );
char is_successful_charsearch( struct char_search_state );
char num_charhash_children( struct charhash * );
struct char_search_state char_search( struct charhash *, char );

#endif // #ifndef _CHARHASH_H
