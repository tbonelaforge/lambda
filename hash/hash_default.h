#ifndef _HASH_DEFAULT_H
#define _HASH_DEFAULT_H


#include "hash.h"

/*============================================================

Definition: hash_default -- A hash with a default value.

============================================================*/

struct hash_default {

    struct hash * main_hash;
    void * default_value;

};


struct hash_default * new_hash_default();
void * hash_default_lookup( struct hash_default *, char * );
void reverse( char * );
int num_decimal_digits( int );
char * num_to_string( int );

#endif // #ifndef _HASH_DEFAULT_H
