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


char * binarystring( char );
char * blankmask( char *, char );
char * info( struct charhash * );
char * charhashtreeprint( struct charhash * );
char longest_common_suffix( char, char, char, char );
struct charhash * insert( struct charhash *, char, struct charhash *, char );
struct charhash * new_charhash();
struct charhash * add_to_charhash( struct charhash *, char, void * );
char * chars_in( struct charhash *, char, char );
struct charhash * charhashlookup( struct charhash *, char );
char num_bits_in_char( char );
void consolidate_charhash( struct charhash *, struct charhash *, char );
void delete_charhash( struct charhash *, struct charhash *, char );
void delete_from_charhash( struct charhash *, char );
void destroy_charhash( struct charhash * );

#endif // #ifndef _CHARHASH_H
