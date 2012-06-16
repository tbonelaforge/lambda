#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "hash_default.h"


/*************************************************************

Function: new_hash_default

Input: Nothing.

Output: A pointer to a newly allocated hash_default structure,
        with everything empty, or set to zero.

*************************************************************/

struct hash_default * new_hash_default() {

    struct hash_default * result = malloc(sizeof(struct hash_default));
    if ( !result ) {
	printf("Out of memory!\n");
	return NULL;
    }
    result->main_hash = new_hash();
    result->default_value = 0;
    
    return result;

} // End function new_hash_default.


/*************************************************************

Function: hash_default_lookup

Input: A pointer to a hash_default structure, 
       and a key ( string ) to lookup in the hash_default structure.

Output: Either the value associated with the key in the given
        structure's main hash, or the given structure's main hash,
        depending on whether the key was found in the main hash, or not.

************************************************************/

void * hash_default_lookup( struct hash_default * current, char * key ) {

    void * result;

    struct hash * main = current->main_hash;
    struct hash * looked_up = hashlookup( main, key );
    if ( !looked_up || !looked_up->data ) {
	return current->default_value;
    }
    return looked_up->data;

} // End function hash_default_lookup.



/*============================================================

Function: reverse

Input: A string

Output: Nothing, but the string has been reversed in place.

============================================================*/

void reverse( char * string ) {

    int i, j;
    char c;

    for ( i = 0, j = strlen(string) - 1; i < j; i++, j-- ) {
	c = string[i];
	string[i] = string[j];
	string[j] = c;
    }

    return;

} // End function reverse.


/*============================================================

Function: num_decimal_digits

Input: An integer greater than or equal to 0.

Output: The number of characters in the 
        decimal representation of the number.

============================================================*/

int num_decimal_digits( int num ) {

    if ( num == 0 ) {
	return 1;
    }

    return floor( log(num) / log(10) ) + 1;

} // End functin num_decimal-digits.



/*============================================================

Function: num_to_string

Input: An integer

Output: A newly allocated string showing the
        decimal representation of the number.

============================================================*/


char * num_to_string( int num ) {

    int is_negative = ( num < 0 ) ? 1 : 0;
    int abs_num = abs(num);
    int string_length = num_decimal_digits(abs_num) + is_negative;
    char * result = malloc( ( string_length + 1 ) * sizeof(char) );
    if ( ! result ) {
	return "Out of memory!";
    }
    int i = 0;
    int r;
    do {
	r = abs_num % 10;
	result[i] = r + '0';

	abs_num /= 10;
	i++;
    } while ( abs_num );
    if ( is_negative ) { 
	result[i++] = '-';
    }
    result[i] = '\0';
    reverse(result);

    return result;

} // End function num_to_string.
