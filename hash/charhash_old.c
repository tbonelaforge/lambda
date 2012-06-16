#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <math.h>

#include "charhash.h" // Structure definition is in here.

#define BYTESIZE 8



/*============================================================

Function: num_bits_in_char

Input: A character

Output: A character whose value is the number of significant bits
        of the given character.

============================================================*/


char num_bits_in_char( char c ) {

    return floor( log(c) / log(2) ) + 1;

}




/*============================================================


Function: reverse_bits

Input: The number of bits (n) to reverse, 
       and a character whose n least significant bits are to be reversed.

Outpu: A character consisting of the n least significant bits of the input,
       only reversed.


============================================================*/

char reverse_bits( int n, char c ) {

    char result = 0;
    int i;

    for ( i = 0; i < n; i++ ) {
	result <<= 1;
	result += c & 1;
	c >>= 1;
    }

    return result;

}






/*=============================================================================================

Function: binarystring

Input: a character (byte) to be converted to binary representation.

Ouput: a pointer to an array of 8 characters which hold the ascii codes for '1' or '0'.
       When read as a string, this represents the binary notation of the input character.
       This function uses malloc, to create the return array in memory, and returns a constant
       error string if the requested memory is not available.

================================================================================================*/

char * binarystring( char c ) {

    // Set up an array to hold the bitpattern.
    char * bitpattern = malloc( BYTESIZE + 1 );

    // Always beware of memory limitations.
    if( ! bitpattern ) return "Out of memory!\n";

    char r, i; // Remainder mod 2, and index, respectively.

    // Write the characters of the string in reverse order.
    bitpattern[BYTESIZE] = '\0'; 

    for( i = BYTESIZE - 1; i >= 0; i-- ) { // Loop bits of the input.
        r = c % 2;
        bitpattern[i] = ( r ) ? '1' : '0';
        c = ( c - r ) / 2;
    } // End input bits loop.

    return bitpattern;

} // End function binarystring


/*====================================================================================================

Function: blankmask

Input: a pointer to an array of characters, 
       and a siglen ( char ) telling how many characters from the right of the string
       actually "count".

Output: the same string, but the unimportant characters have been changed to blanks.

======================================================================================================*/

char* blankmask( char* binarystring, char siglen ) {

    char i;
    char* blankmasked = (char *) malloc((strlen(binarystring)+1)*sizeof(char));

    if( ! blankmasked ) return "Out of memory!\n";

    strcpy( blankmasked, binarystring );

    for( i = 0; i < BYTESIZE; i++ ) {
        if( i < BYTESIZE - siglen ) {
            blankmasked[i] = '-';
        }
    }

    return blankmasked;

} // End function blankmask


/*====================================================================================================

Function: info

Input: a pointer to a charhash structure.

Output: a string of HTML (using <br />'s) that describes all the data contained within the structure.
        This includes printing out the memory address of the main "data" pointer, the sig
        in binary notation, and the siglen in base 10 notation.

======================================================================================================*/

char * info( struct charhash * self ) {

    /* Find out how big a pointer can be, in order to find max length of infostring. */
    double sizeof_voidpointer = sizeof( void * );

    double biggest_pointer = pow( 2, sizeof_voidpointer * BYTESIZE );
    int howmany_digits = ceil( log10( biggest_pointer ) );

    /* Allocate a string that is large enough to hold all three lines of output below. */
    char * infostring = malloc( ( howmany_digits + BYTESIZE + 1 + 3 * 14 ) * sizeof(char) );

    if( ! infostring ) return "Out of memory!\n";

    char sigstring[ BYTESIZE + 14 ];
    char siglenstring[ 1 + 14 ];

    sprintf( infostring, "data:  %ld<br />", (long int) self->data );
    sprintf( sigstring, "sig:   %s<br />", blankmask(binarystring( self->sig ), self->siglen ) );
    sprintf( siglenstring, "siglen:%d<br />", self->siglen );
    strcat( infostring, sigstring );
    strcat( infostring, siglenstring );

    return infostring;

} // End function info.



/*============================================================

Function: charhashtreeprint

Input: A pointer to a charhash structure, NULL or otherwise.

Output: A newly allocated string containing HTML code.
        When viewed in a browser, 
        the output shows the structure of the hash tree.

=============================================================*/

char * charhashtreeprint( struct charhash * self ) {

    char * output;

    if( self == NULL ) { // There is nothing to see here.
        output = malloc( 1 * sizeof(char) );
        output[0] = '\0'; // Prepare to return the empty string.
        return output;
    }

    // If we made it this far, there is at least some data in this node.
    // There may also be children of this node to output.
    char *self_output, *left_output, *right_output;

    // Get the three constituent strings of data, and calculate their length.
    // Use non-breaking spaces for empty nodes, to force proper table layout.
    int num_blanks = 20;
    char blank_output[ 6*num_blanks + 1 ];
    blank_output[0] = '\0';
    int i;
    for(i = 0; i < num_blanks; i++ ) {
        strcat(blank_output,"&nbsp;");   
    }

    self_output = info(self);
    left_output = charhashtreeprint(self->child[0]);
    right_output = charhashtreeprint(self->child[1]);

    // Replace empty nodes with strings of blanks, where appropriate.
    if( strlen(left_output) && !strlen(right_output) 
        || strlen(right_output) && !strlen(left_output) ) {
        if( !strlen(left_output) ) {
            free(left_output);
            left_output = malloc( ( strlen(blank_output) + 1 ) * sizeof(char) );
            sprintf( left_output, "%s", blank_output );
        }
        if( !strlen(right_output) ) {
            free(right_output);
            right_output = malloc( ( strlen(blank_output) + 1 ) * sizeof(char) );
            sprintf( right_output, "%s", blank_output );
        }
    }

    size_t data_length = strlen(self_output) + strlen(left_output) + strlen(right_output);

    // Prepare the output array.
    // The output template is less than 100 chars.
    output = malloc( ( 140 + data_length ) * sizeof(char) ); 

    if( !output ) return "Out of memory!\n";

    // Fill in the output template.
     sprintf( output,
        "<table><tr align = center><td colspan = 2>%s</td></tr><tr valign = top><td>%s</td><td>%s</td></tr></table>",
        self_output, left_output, right_output );

    // Finally, free the constituent data strings.
    free(self_output); // Created by info(), using malloc.
    free(left_output); // Created by charhashtreeprint(), using malloc.
    free(right_output); // Created by charhashtreeprint(), using malloc.

    return output;

} // End function charhashtreeprint.


/*============================================================

Function: longest_common_suffix

Input: A new bit pattern ( character ),
       the number of bits in that new bit pattern, 
       an old bit pattern, and the length of the old bit pattern.

Output: The length of the longest common suffix 
        between the old and new bit patterns.

=============================================================*/

char longest_common_suffix( char oldsig, char oldsiglen, char newsig, char newsiglen ) {

    char num_matched = 0;
    char mask;

    while( (num_matched < oldsiglen) && (num_matched < newsiglen) ) { // Count matched bits.
        mask = 1 << num_matched;
        if( (oldsig & mask) != (newsig & mask) ) {
            break;
         }
        num_matched++;
    } // End count matched bits loop.

    return num_matched;

} // End function longest_common_suffix


/*============================================================

Function: insert

Input: A parent node ( charhash* ), 
       a decision bit to use when inserting under the parent,
       a new node to be inserted ( charhash* ), 
       and a decision bit to use when connecting the new node to the old child.

Output: A pointer to the newly inserted node.

=============================================================*/

struct charhash* insert( struct charhash* parent, char parentchoice, struct charhash* newchild, char childchoice ) {

    struct charhash* oldchild = parent->child[parentchoice];

    parent->child[parentchoice] = newchild;
    newchild->child[childchoice] = oldchild;
    return newchild;

} // End function insert.

/*============================================================

Function: new_charhash

Input: nothing

Output: A pointer to a newly allocated charhash structure, 
        with all fields set to zero, or NULL, if there is 
        not enough memory available.

=============================================================*/

struct charhash* new_charhash() {

    struct charhash* newnode = ( struct charhash* ) malloc(sizeof(struct charhash));

    if( ! newnode ) return NULL;

    newnode->data = 0;
    newnode->sig = newnode->siglen = 0;
    newnode->child[0] = newnode->child[1] = NULL;

} // End function new_charhash

/*============================================================

Function: add_to_charhash

Input: A pointer to the head node of a hashtree, 
       a new key ( character ), and a new value ( void * )
       to be added to the hash tree.

Output: A pointer to a node which contains the requested new value, or NULL,
        Depending on whether there was enough memory left to install the new
        value in the hashtree, or not.
        If a node already existed for the given key, 
        then that node is changed, and it's pointer is returned.

=============================================================*/


struct charhash* add_to_charhash( struct charhash* current, char newsig, void* newvalue ) {

    struct charhash* parent;
    struct charhash* newnode;
    char nextbit, currentchoice, matchedportion, num_matched, childchoice;
    char newsiglen = num_bits_in_char(newsig);
    newsig = reverse_bits( newsiglen, newsig );

    while( current ) { // Loop to find insertion point.
        char currentsig = current->sig;
        char currentsiglen = current->siglen;
        if( currentsig == newsig && currentsiglen == newsiglen ) { // Key already exists.
            current->data = newvalue;
            return 0; // This return value is used by add_to_hash
        }

        // Otherwise, see how much of a common suffix this key has with the current node.
        num_matched = longest_common_suffix( currentsig, currentsiglen, newsig, newsiglen );

        if( currentsiglen > num_matched ) { // There are left over bits of the key.

            // Try to allocate storage for the new node.
            newnode = new_charhash();
            if( !newnode ) return NULL;
            currentsig >>= num_matched; // Shift off the matched portion of current

            // Take the very next bit as a decision bit, and adjust siglen accordingly.
            nextbit = currentsig & 1; 
            current->sig = currentsig >>= 1;
            current->siglen -= ( num_matched + 1 );

            // Mask off the matched portion ( common suffix ).
            matchedportion = newsig & ~(~0 << num_matched );

            // Partially fill out the newnode, and enter the loop again.
            newnode->sig = matchedportion;
            newnode->siglen = num_matched;
            current = insert( parent, currentchoice, newnode, nextbit );

        } // End there are leftover bits of the key if.
        else { // The entire key is matched, and there are leftover bits in newsig.

            // Shift off the matched portion of newsig.
            newsig >>= num_matched;

            // Take the very next bit as a decision bit, and adjust the length accordingly.
            nextbit = newsig & 1;
            newsig >>= 1;
            newsiglen -= ( num_matched + 1 );

            // Use the decision bit to iterate parent and current.
            parent = current;
            current = current->child[nextbit];
            currentchoice = nextbit;
        } // End leftover bits in newsig else.
    } // End loop to find insertion point.

    // If we made it here, we went down a NULL child path.
    newnode = new_charhash();
    if( !newnode ) return NULL;
    newnode->sig = newsig;
    newnode->siglen = newsiglen;
    newnode->data = newvalue;

    return insert( parent, currentchoice, newnode, 0 ); // The newnode->child[0] is NULL anyway.

} // End function add_to_charhash

/*============================================================

Function: chars_in

Input: A pointer to a charhash structure, 
       a suffix ( char ), and a suffix length ( char ).

Output: A string such that each key character in the input charhash
        is represented by a character in the string.

=============================================================*/

char * chars_in( struct charhash * current, char suffix, char suffixlen ) {

    char * resultstr;

    if( ! current ) { // Base case: return empty string.
        resultstr = malloc( sizeof(char) );
        if( ! resultstr ) {
            return "Out of memory!";
        }
        *resultstr = '\0';
        return resultstr;
    }

    // Otherwise concatenate the suffix onto the current sig.
    char currentchar = ( current->sig << suffixlen ) + suffix;
    char currentcharlen = current->siglen + suffixlen;

    // Use recursion to build the list of chars to return for this charhash.
    char *leftstr,  *currentstr, *rightstr;

    // Recurse left.
    leftstr = chars_in( current->child[0], currentchar, currentcharlen + 1 );

    // Visit the current node.
    // It could represent an "accepting state" for a key, 
    // or an intermediate state for some other node's key.
    if( current->data ) { // The path to this node defines a character key.
        currentstr = (char *)malloc( 2 * sizeof(char) );
        if( ! currentstr ) {
            return "Out of memory!";
        }
        currentstr[0] = reverse_bits( currentcharlen, currentchar );
        currentstr[1] = '\0';
    } // End current path is character key if.
    else { // This is an intermediate path.
        currentstr = malloc( 2 * sizeof(char) );
        if( ! current ) {
            return "Out of memory!";
        }

        // Ignore the path to this node.
        *currentstr = '\0';
    } // End intermediate path else.

    // Recurse right.
    rightstr = chars_in( current->child[1], currentchar + ( 1 << currentcharlen ), currentcharlen + 1);
    long int resultlen = strlen(leftstr) + strlen(currentstr) + strlen(rightstr);

    // Prepare to construct the result string.
    resultstr = malloc( ( resultlen + 1 ) * sizeof(char) );
    if( ! resultstr ) {
        return "Out of memory!";
    }
    strcpy( resultstr, leftstr );
    strcat( resultstr, currentstr );
    strcat( resultstr, rightstr );

    // Clean up memory allocated by malloc.
    free(leftstr);
    free(currentstr);
    free(rightstr);

    return resultstr;

} // End function chars_in.

/*====================================================================================

Function: charhashlookup

Input: a charhash structure pointer, and a one-character key.

Output: a pointer to a charhash structure which contains data associated with the key.
        If the key is not found in the existing hash, return 0.

=======================================================================================*/


struct charhash * charhashlookup( struct charhash * current, char key ) {

    char lastbit, siglen, sig;
    char keylen = num_bits_in_char(key);
    key = reverse_bits( keylen, key );

     while ( current ) { // Loop bits of the key.
        siglen = current->siglen;
        sig = current->sig;

        // Check if key does not match sig to siglen places.
        if ( ( key & ~( ~0 << siglen ) ) ^ ( sig & ~( ~0 << siglen ) ) ) {
            return 0;
        }

        // Shift off the matching bits of the key.
        key = key >> siglen;
	keylen -= siglen;

        // Check to see if we've used up the whole key.
        if( !keylen ) {
            return current;
        }

        // If we made it this far, we need to go to another node.
        lastbit = key & 1;
        key = key >> 1;
	keylen -= 1;
        current = current->child[lastbit];
    } // End bits of the key loop.

    // If used up the whole key successfully, we're done.
    return current;

} // End function charhashlookup.




/*============================================================

Function: consolidate_charhash

Input: A pointer to a charhash node, 
       a pointer to the parent of that node,
       and a parent decision used to get there.

Output: Nothing, but an unnecessary node's memory hash been removed,
        if possible.

============================================================*/

void consolidate_charhash( struct charhash * current, struct charhash * parent, char parent_decision ) {

    struct charhash * child;
    char child_decision;
    char new_sig = 0;
    if ( current->data ) {
	return; // Can't consolidate nodes with data.
    }
    if ( current->child[0] && current->child[1] ) { // This node has two children.
	return; // Can't consolidate nodes with two children.
    }
    if ( !current->child[0] && !current->child[1] ) { // This node has no children.
	if ( !parent ) return; // No children and no parent = root node.
	parent->child[parent_decision] = NULL;
	free(current);
	consolidate_charhash( parent, NULL, 0 ); // The parent node either has data, or another child.
	return;
    }
    // Otherwise, this node has one child.
    if ( child = current->child[0] ) { // Left child.
	child_decision = 0;
    }
    else { // Right child.
	child = current->child[1];
	child_decision = 1;
    }

    // Copy the child's info into current.
    current->data = child->data;
    current->child[0] = child->child[0];
    current->child[1] = child->child[1];

    // Prepend the child's sig and decision char to current sig.
    new_sig = child->sig;
    new_sig = ( new_sig << 1 ) + child_decision;
    new_sig <<= current->siglen;
    current->sig += new_sig;
    current->siglen += ( child->siglen + 1 );
    free(child);
    return;
    

} // End function consolidate_charhash.



/*============================================================

Function: delete_charhash

Input: A pointer to a charhash node, 
       a pointer to the parent of that node, 
       and the decision bit ( char ) used to get to that node.

Output: Nothing, but the data, and possibly the memory, 
        corresponding to the given node is removed.

============================================================*/

void delete_charhash( struct charhash * current, struct charhash * parent, char parent_decision ) {

    if ( !current->data ) {
	return;
    }
    current->data = NULL;
    consolidate_charhash( current, parent, parent_decision );
    return;

} // End function delete_charhash.


/*====================================================================================

Function: delete_from_charhash

Input: A pointer to a charhash node, and a character to delete.

Output: Nothing, but the charhash no longer contains the given character.

=======================================================================================*/

void delete_from_charhash( struct charhash * current, char key ) {

    struct charhash * parent = NULL;
    char parent_decision = 0;
    char current_siglen, current_sig;
    char num_matched;
    char keylen = num_bits_in_char(key);
    key = reverse_bits( keylen, key );

    while ( current ) { // Loop to search tree.
	current_siglen = current->siglen;
	current_sig = current->sig;
	num_matched = longest_common_suffix( current_sig, current_siglen, key, keylen );
	if ( num_matched < current_siglen ) {
	    return; // Key doesn't exist.
	}
	// Check if we are already on the correct charhash node.
	key = key >> num_matched;
	keylen -= num_matched;
	if ( !keylen ) { // Found it.
	    delete_charhash( current, parent, parent_decision );
	    return;
	}
	// Otherwise, iterate on down the tree.
	parent_decision = key & 1;
	key = key >> 1;
	keylen -= 1;
	parent = current;
	current = current->child[parent_decision];
    } // End tree search loop.
    return;
}


/*============================================================

Function: destroy_charhash

Input: A pointer to a charhash node.

Output: Nothing, but all the memory used by that node is recursively freed.


============================================================*/

void destroy_charhash( struct charhash * current ) {

    if ( current->child[0] ) {
	destroy_charhash(current->child[0]);
    }
    if ( current->child[1] ) {
	destroy_charhash(current->child[1]);
    }
    free(current);

    return;

} // End function destroy_charhash.



int charhash_test() {

    struct charhash * test = new_charhash();
    /*
    add_to_charhash( test, 'a', (void*)12 );
    add_to_charhash( test, 'b', (void*)13 );
    add_to_charhash( test, 'A', (void*)14 );

    printf( "After adding a -> 12, b -> 13, and A -> 14, the charhash looks like: \n<br />" );
    printf( "%s", charhashtreeprint(test) );

    add_to_charhash( test, 'Z', (void*)42 );

    printf( "After adding Z -> 42, the charhash looks like: \n<br />" );
    printf( "%s", charhashtreeprint(test) );

    add_to_charhash( test, 'Z', (void*)42 );



    delete_from_charhash( test, 'Z' );
    printf( "After deleting Z -> 42, the charhash looks like: \n<br />" );
    printf( "%s", charhashtreeprint(test) );
    */

    //
    // Test 1
    //
    /*    
    add_to_charhash( test, (char) 21, (void*)1 );
    add_to_charhash( test, (char) 29, (void*)2 );
    printf( "After adding 21 -> 1, and 29 -> 2, the hash looks like: \n<br />" );
    printf( "%s", charhashtreeprint(test) );

    delete_from_charhash( test, (char) 5 );
    printf( "After deleting a key of 5, the hash looks like: \n<br />" );
    printf( "%s", charhashtreeprint(test) );
    */

    //
    // Test 2
    //
    /*
    add_to_charhash( test, (char) 21, (void*)1 );
    add_to_charhash( test, ( char) 24, (void*)2 );
    printf( "After adding 21 -> 1, and 24 -> 2, the hash looks like: \n<br />" );
    printf( "%s", charhashtreeprint(test) );

    delete_from_charhash( test, (char) 8 );
    printf( "After deleteing a key of 8, the hash looks like: \n<br />" );
    printf( "%s", charhashtreeprint(test) );
    */

    //
    // Test 3
    //
    /*
    add_to_charhash( test, (char) 21, (void*)1 );
    add_to_charhash( test, (char) 29, (void*)2 );
    printf( "After adding 21 -> 1, and 29 -> 2, the hash looks like: \n<br />" );
    printf( "%s", charhashtreeprint(test) );

    delete_from_charhash( test, (char)53 );
    printf( "After deleteing a key of 53, the hash looks like:\n<br />" );
    printf( "%s", charhashtreeprint(test) );
    */

    //
    // Test 4
    //
    /*
    add_to_charhash( test, (char)21, (void*)1 );
    add_to_charhash( test, (char)29, (void*)2 );
    printf( "After adding 21 -> 1, and 29 -> 2, the hash looks like: \n<br />" );
    printf( "%s", charhashtreeprint(test) );

    delete_from_charhash( test, (char) 8 );
    printf( "After deleting a key of 8, the hash looks likd: \n<br />" );
    printf( "%s", charhashtreeprint(test) );
    */

    //
    // Test 5
    //
    /*
    add_to_charhash( test, (char) 21, (void*)1 );
    add_to_charhash( test, (char) 29, (void*)2 );
    add_to_charhash( test, (char) 5, (void*)3 );
    printf( "After adding 21 -> 1, 29 -> 2, and 5 -> 3, the hash looks like: \n<br />" );
    printf( "%s", charhashtreeprint(test) );

    delete_from_charhash( test, (char)5 );
    printf( "After deleting a key of 5, the hash looks like: \n<br />" );
    printf( "%s", charhashtreeprint(test) );
    */

    //
    // Test 6
    //
    /*
    add_to_charhash( test, (char) 10, (void*)1 );
    add_to_charhash( test, (char) 42, (void*)2 );
    printf( "After adding 10 -> 1, and 42 -> 2, the hash looks like: \n<br />" );
    printf( "%s", charhashtreeprint(test) );

    delete_from_charhash( test, (char) 10 );
    printf( "After deleting the key of 10, the hash looks like: \n<br />" );
    printf( "%s", charhashtreeprint(test) );
    */

    //
    // Test 7
    //
    /*
    add_to_charhash( test, (char) 10, (void*)1 );
    add_to_charhash( test, (char) 58, (void*)2 );
    printf( "After adding 10 -> 1, and 58 -> 2, the hash looks like: \n<br />" );
    printf( "%s", charhashtreeprint(test) );

    delete_from_charhash( test, (char) 10 );
    printf( "After deleting the key of 10, the hash looks like: \n<br />" );
    printf( "%s", charhashtreeprint(test) );
    */

    //
    // Test 8
    //
    /*
    add_to_charhash( test, (char) 10, (void*)1 );
    add_to_charhash( test, (char) 42, (void*)2 );
    printf( "After adding 10 -> 1, and 42 -> 2, the hash looks like: \n<br />" );
    printf( "%s", charhashtreeprint(test) );

    delete_from_charhash( test, (char)42 );
    printf( "After deleting the key of 42, the hash looks like: \n<br />" );
    printf( "%s", charhashtreeprint(test) );
    */

    //
    // Test 9
    //
    /*
    add_to_charhash( test, (char) 10, (void*)1 );
    add_to_charhash( test, (char) 58, (void*)2 );
    printf( "After adding 10 -> 1, and 58 -> 2, the hash looks like:\n<br />" );
    printf( "%s", charhashtreeprint(test) );

    delete_from_charhash( test, (char) 58 );
    printf( "After deleting the key of 58, the hash looks like: \n<br />" );
    printf( "%s", charhashtreeprint(test) );
    */

    //
    // Test 10
    //
    /*
    printf( "Before adding anything, the charhash looks like: \n<br />" );
    printf( "%s\n<br />", charhashtreeprint(test) );
    
    add_to_charhash( test, 'a', (void*)1 );
    printf( "After adding a key of 'a', the charhash looks like: \n<br />" );
    printf( "%s\n<br />", charhashtreeprint(test) );

    delete_from_charhash( test, 'a' );
    printf( "After deleting the key of 'a', the charhash looks like: \n<br />" );
    printf( "%s\n<br />", charhashtreeprint(test) );
    */

} // End function main.
