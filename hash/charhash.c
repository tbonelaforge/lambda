#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <math.h>

#include "charhash.h" // Structure definition is in here.

#define BYTESIZE 8



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
    char * infostring = malloc( ( 50 + howmany_digits + BYTESIZE + 1 + 3 * 14 ) * sizeof(char) );

    if( ! infostring ) return "Out of memory!\n";

    char sigstring[ BYTESIZE + 14 ];
    char siglenstring[ 1 + 14 ];

    sprintf( infostring, "address: %p<br />data:  %ld<br />", self, (long int) self->data );
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
    if( (strlen(left_output) && !strlen(right_output))
        || (strlen(right_output) && !strlen(left_output)) ) {
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

Function: new_charhash

Input: nothing

Output: A pointer to a newly allocated charhash structure, 
        with all fields set to zero, or NULL, if there is 
        not enough memory available.

=============================================================*/

struct charhash* new_charhash() {
    struct charhash* newnode = ( struct charhash* ) malloc(sizeof(struct charhash));
    if( ! newnode ) {
        return NULL;
    }
    newnode->data = 0;
    newnode->sig = newnode->siglen = 0;
    newnode->child[0] = newnode->child[1] = NULL;
    return newnode;

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


struct charhash * add_to_charhash( struct charhash * root, char key, void * value ) {
    struct charhash * new_node;
    char decision_bit;
    struct char_search_state search_state = char_search( root, key );
    while ( 1 ) { // Loop the following at most two times.
	if ( search_state.num_matched == search_state.node->siglen ) {

	    // Matched all of the search_state's sig.
	    if ( search_state.num_matched == search_state.siglen ) { // Found it.
		search_state.node->data = value;
		return search_state.node;
	    }
	    
	    // Create the desired leaf node with the leftover bits from the sig.
	    return create_leaf_node( search_state, value );
	}

        // Remove the matched portion of the sig,
        // make a branch node with the matched portion as it's sig,
        // insert the new node above the found node, and start again from there.
	search_state.node = create_branch_node(search_state);
    }
}


/*============================================================

Function: create_leaf_node

Input: The first argument is a char_search_state structure summarizing a search that
       ended with matching all the bits of a node's sig,
       and having leftover bits in the desired sig. 
       The second argument is the desired value to be stored in the new node.

Output: A reference to a newly-created charhash node, 
        containing the desired value, 
        and connected appropriately to the given search state node.

============================================================*/


struct charhash * create_leaf_node( struct char_search_state search_state, void * value ) {
    search_state.sig >>= search_state.num_matched;
    char decision_bit = search_state.sig & 1;
    search_state.sig >>= 1;
    search_state.siglen -= ( search_state.num_matched + 1 );
    struct charhash * new_node = new_charhash();
    new_node->sig = search_state.sig;
    new_node->siglen = search_state.siglen;
    new_node->data = value;
    search_state.node->child[decision_bit] = new_node;
    return new_node;
}



/*============================================================

Function: create_branch_node

Input: a char_search_state structure summarizing a search that
       ended with a partial match between the target sig, 
       and the sig of a node in the tree.

Output: A reference to a new branch node, 
        whose sig is the matched portion of the target sig,
	and which has been inserted between the parent and the
	node described by the given search state. Furthermore, 
	the given search state's node has had the matched portion
	removed from it's sig.

============================================================*/

struct charhash * create_branch_node( struct char_search_state search_state ) {
    struct charhash * new_node = new_charhash();
    new_node->sig = search_state.node->sig & ~(~0 << search_state.num_matched);
    new_node->siglen = search_state.num_matched;
    search_state.node->sig >>= search_state.num_matched;
    char decision_bit = search_state.node->sig & 1;
    search_state.node->sig >>= 1;
    search_state.node->siglen -= ( search_state.num_matched + 1 );
    search_state.parent->child[search_state.parent_choice] = new_node;
    new_node->child[decision_bit] = search_state.node;
    return new_node;
}



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
        currentstr[0] = currentchar;
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


struct charhash * charhashlookup( struct charhash * root, char key ) {
    struct char_search_state search_state = char_search( root, key );
    if ( is_successful_charsearch(search_state) ) {
	return search_state.node;
    }
    return NULL;
}



/*============================================================

Function: num_bits_in_char

Input: A character ( key )

Output: An integer representing the number of significant bits in the character.

============================================================*/

char num_bits_in_char( char key ) {

//    return floor( log(key) / log(2) ) + 1;
    char num_bits = 0;
    while ( key ) {
	key >>= 1;
	num_bits += 1;
    }
    return num_bits;


} // End function num_bits_in_char.

/*====================================================================================

Function: delete_from_charhash

Input: A pointer to a charhash node, and a character to delete.

Output: Nothing, but the charhash no longer contains the given character.

=======================================================================================*/


void delete_from_charhash( struct charhash * root, char key ) {
    struct char_search_state search_state = char_search( root, key );
    if ( ! is_successful_charsearch( search_state ) ) {
	return;
    }

    // Remove data from the found node, and consolidate memory if necessary.
    search_state.node->data = NULL;
    while ( 1 ) { // Assert: search_state.node has NULL data.
	if ( search_state.node == root ) {
	    return;
	}
        char num_children = num_charhash_children( search_state.node );
	if ( num_children >= 2 ) { // Don't delete branch nodes.
	    return;
	}
	if ( num_children == 1 ) {
	    absorb_single_charhash_child( search_state );
	    return;
	}

        // Assert: num_children == 0.
	if ( !search_state.parent ) { // Don't remove root node.
	    return;
	}
	remove_leaf_node(search_state);
	if ( search_state.parent->data ) { 
	    return; // Can't consolidate any further.
	}

	// Iterate to the parent.
        search_state.node = search_state.parent;
	search_state.parent = NULL;  // Won't loop more than twice.
    }
    return;
}



/*============================================================

Function: absorb_single_charhash_child

Input: A char_hash_search structure, 
       representing a search that ended on a node with NULL data, 
       and a single child.

Output: Nothing, 
        but the single child's info has been absorbed into the 
        search state node, it's link has been broken,
        and it's memory has been freed.

============================================================*/


void absorb_single_charhash_child( struct char_search_state search_state ) {
    
    // Determine which child the search_state node has.
    char child_decision;
    struct charhash * child;
    if ( (child = search_state.node->child[0]) ) {
	child_decision = 0;
    }
    else {
	child = search_state.node->child[1];
	child_decision = 1;
    }
    
    // Copy the child's data into the search_state node.
    search_state.node->data = child->data;
    search_state.node->child[0] = child->child[0];
    search_state.node->child[1] = child->child[1];

    // Prepend the child's sig and decision bit to the current sig.
    char new_sig = child->sig;
    new_sig = ( new_sig << 1 ) | child_decision;
    new_sig <<= search_state.node->siglen;
    search_state.node->sig |= new_sig;
    search_state.node->siglen += ( child->siglen + 1 );
    free(child);
    return;
}



/*============================================================

Function: remove_leaf_node

Input: A char_search_state structure, 
       representing a search that ended up pointing to a node
       with no children.

Output: Nothing, but the search_state node has been removed from 
        the tree, and its memory has been freed.

============================================================*/


void remove_leaf_node( struct char_search_state search_state ) {
    if ( ! search_state.parent ) {
	return;
    }
    search_state.parent->child[search_state.parent_choice] = NULL;
    free(search_state.node);
    return;
}


/*============================================================

Function: is_successful_charsearch

Input: A char_search_state structure, 
       representing the end of some search.

Output: 1 or 0, 
        depending on whether the key that was being searched
        for was found or not.

============================================================*/

char is_successful_charsearch( struct char_search_state search_state ) {
    if ( search_state.num_matched != search_state.siglen ) {
	return 0;
    }
    if ( search_state.num_matched != search_state.node->siglen ) {
	return 0;
    }
    if ( ! search_state.node->data ) {
	return 0;
    }
    return 1;
}



/*============================================================

Function: num_charhash_children

Input: A pointer to a charhash node

Ouput: The number of non-null children the node has.

============================================================*/

char num_charhash_children( struct charhash * node ) {
    char num_children = 0;
    if ( node->child[0] ) {
	num_children++;
    }
    if ( node->child[1] ) {
	num_children++;
    }
    return num_children;
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



/*============================================================

Function: char_search

Input: A pointer to a charhash to search, 
       and a key char to search for.

Output: A char_search_state structure describing the state of
        affairs at the point when iteration through the tree
        in pursuit of the key could not continue any further.

============================================================*/


struct char_search_state char_search( struct charhash * root, char sig ) {
    struct charhash * node = root;
    struct charhash * parent = NULL;
    struct charhash * child = NULL;
    char parent_choice;
    char num_matched;
    char siglen = num_bits_in_char(sig);
    while ( ( num_matched = longest_common_suffix( node->sig, node->siglen, sig, siglen ) ) == node->siglen ) {
	if ( siglen == num_matched ) { // Matched all the sig.
	    break;
	}

	// Otherwise, examine the unmatched portion of the sig.
        child = node->child[ ( sig >> num_matched ) & 1 ];
	if ( ! child ) { // There is nowhere to go from here.
	    break;
	}

	// Iterate search state.
	sig = sig >> num_matched;
	parent_choice = sig & 1;
	sig = sig >> 1;
	siglen -= ( num_matched + 1 );
	parent = node;
	node = child;
    };
    struct char_search_state search_state = {
	parent,
	node,
	parent_choice,
	sig,
	siglen,
	num_matched
    };
    return search_state;
}


//int main() {
int charhash_test() {
    struct charhash * test;

    //
    // Test 0
    //

    printf( "STARTING TEST 0<br />\n" );
    test = new_charhash();
    add_to_charhash( test, 'a', (void*)97 );
    add_to_charhash( test, 'b', (void*)98 );
    add_to_charhash( test, 'A', (void*)65 );
    printf( "After adding a -> 97, b -> 98, and A -> 65, the charhash looks like: <br />\n" );
    printf( "%s", charhashtreeprint(test) );

    add_to_charhash( test, 'Z', (void*)90 );
    printf( "After adding Z -> 90, the charhash looks like: <br />\n" );
    printf( "%s<br />\n", charhashtreeprint(test) );

    add_to_charhash( test, 'Z', (void*)90 );
    printf( "After adding Z -> 90 again, the charhash looks like: <br />\n" );
    printf( "%s<br />\n", charhashtreeprint(test) );

    delete_from_charhash( test, 'Z' );
    printf( "After deleting Z -> 90, the second charhash looks like: <br />\n" );
    printf( "%s", charhashtreeprint(test) );

    struct char_search_state my_state = char_search( test, '\x12' );
    printf( "The result of the search has:<br /> \n" );
    printf( "parent: %p<br />\n", my_state.parent );
    printf( "node: %p<br />\n", my_state.node );
    printf( "parent_choice: %d<br />\n", my_state.parent_choice );
    printf( "sig: %d<br />\n", my_state.sig );
    printf( "siglen: %d<br />\n", my_state.siglen );
    printf( "num_matched: %d<br />\n", my_state.num_matched );

    printf("The result of looking up 'a' in the charhash is: <br />\n");
    printf( "%p<br />\n", charhashlookup( test, 'a' ) );

    printf("The result of looking up 'b' in the charhash is: <br />\n");
    printf( "%p<br />\n", charhashlookup( test, 'b' ) );

    printf("The result of looking up 'c' in the charhash is: <br />\n");
    printf( "%p<br />\n", charhashlookup( test, 'c' ) );

    printf("The result of looking up 'A' in the charhash is: <br />\n");
    printf( "%p<br />\n", charhashlookup( test, 'A' ) );

    printf("The result of looking up 'B' in the charhash is: <br />\n");
    printf( "%p<br />\n", charhashlookup( test, 'B' ) );

    printf("The result of looking up 'Z' in the charhash is: <br />\n");
    printf( "%p<br />\n", charhashlookup( test, 'Z' ) );

    //
    // Test 1
    //

    printf( "STARTING TEST 1.<br />\n" );
    test = new_charhash();
    add_to_charhash( test, (char) 21, (void*)1 );
    add_to_charhash( test, (char) 29, (void*)2 );
    printf( "After adding 21 -> 1, and 29 -> 2, the hash looks like: \n<br />" );
    printf( "%s", charhashtreeprint(test) );

    delete_from_charhash( test, (char) 5 );
    printf( "After deleting a key of 5, the hash looks like: \n<br />" );
    printf( "%s", charhashtreeprint(test) );



    //
    // Test 2
    //

    printf("STARTING TEST 2<br />\n");
    test = new_charhash();
    add_to_charhash( test, (char) 21, (void*)1 );
    add_to_charhash( test, ( char) 24, (void*)2 );
    printf( "After adding 21 -> 1, and 24 -> 2, the hash looks like: \n<br />" );
    printf( "%s", charhashtreeprint(test) );

    delete_from_charhash( test, (char) 8 );
    printf( "After deleteing a key of 8, the hash looks like: \n<br />" );
    printf( "%s", charhashtreeprint(test) );

    //
    // Test 3
    //

    printf("STARTING TEST 3<br />\n");
    test = new_charhash();
    add_to_charhash( test, (char) 21, (void*)1 );
    add_to_charhash( test, (char) 29, (void*)2 );
    printf( "After adding 21 -> 1, and 29 -> 2, the hash looks like: \n<br />" );
    printf( "%s", charhashtreeprint(test) );

    delete_from_charhash( test, (char)53 );
    printf( "After deleteing a key of 53, the hash looks like:\n<br />" );
    printf( "%s", charhashtreeprint(test) );

    //
    // Test 4
    //

    printf("STARTING TEST 4<br />\n");
    test = new_charhash();
    add_to_charhash( test, (char)21, (void*)1 );
    add_to_charhash( test, (char)29, (void*)2 );
    printf( "After adding 21 -> 1, and 29 -> 2, the hash looks like: \n<br />" );
    printf( "%s", charhashtreeprint(test) );

    delete_from_charhash( test, (char) 8 );
    printf( "After deleting a key of 8, the hash looks like: \n<br />" );
    printf( "%s", charhashtreeprint(test) );

    //
    // Test 5
    //
    printf("STARTING TEST 5<br />\n");
    test = new_charhash();
    add_to_charhash( test, (char) 21, (void*)1 );
    add_to_charhash( test, (char) 29, (void*)2 );

    add_to_charhash( test, (char) 5, (void*)3 );
    printf( "After adding 21 -> 1, 29 -> 2, and 5 -> 3, the hash looks like: \n<br />" );
    printf( "%s", charhashtreeprint(test) );

    delete_from_charhash( test, (char)5 );
    printf( "After deleting a key of 5, the hash looks like: \n<br />" );
    printf( "%s", charhashtreeprint(test) );


    //
    // Test 6
    //

    printf("STARTING TEST 6<br />\n");
    test = new_charhash();
    add_to_charhash( test, (char) 10, (void*)1 );
    add_to_charhash( test, (char) 42, (void*)2 );
    printf( "After adding 10 -> 1, and 42 -> 2, the hash looks like: \n<br />" );
    printf( "%s", charhashtreeprint(test) );

    delete_from_charhash( test, (char) 10 );
    printf( "After deleting the key of 10, the hash looks like: \n<br />" );
    printf( "%s", charhashtreeprint(test) );

    //
    // Test 7
    //

    printf("STARTING TEST 7<br />\n");
    test = new_charhash();
    add_to_charhash( test, (char) 10, (void*)1 );
    add_to_charhash( test, (char) 58, (void*)2 );
    printf( "After adding 10 -> 1, and 58 -> 2, the hash looks like: \n<br />" );
    printf( "%s", charhashtreeprint(test) );

    delete_from_charhash( test, (char) 10 );
    printf( "After deleting the key of 10, the hash looks like: \n<br />" );
    printf( "%s", charhashtreeprint(test) );

    //
    // Test 8
    //

    printf("STARTING TEST 8<br />\n");
    test = new_charhash();
    add_to_charhash( test, (char) 10, (void*)1 );
    add_to_charhash( test, (char) 42, (void*)2 );
    printf( "After adding 10 -> 1, and 42 -> 2, the hash looks like: \n<br />" );
    printf( "%s", charhashtreeprint(test) );

    delete_from_charhash( test, (char)42 );
    printf( "After deleting the key of 42, the hash looks like: \n<br />" );
    printf( "%s", charhashtreeprint(test) );

    //
    // Test 9
    //

    printf("STARTING TEST 9.<br />\n");
    test = new_charhash();

    add_to_charhash( test, (char) 10, (void*)1 );
    add_to_charhash( test, (char) 58, (void*)2 );
    printf( "After adding 10 -> 1, and 58 -> 2, the hash looks like:\n<br />" );
    printf( "%s", charhashtreeprint(test) );

    delete_from_charhash( test, (char) 58 );
    printf( "After deleting the key of 58, the hash looks like: \n<br />" );
    printf( "%s", charhashtreeprint(test) );

    //
    // Test 10
    //

    printf("STARTING TEST 10<br />\n");
    test = new_charhash();
    printf( "Before adding anything, the charhash looks like: \n<br />" );
    printf( "%s\n<br />", charhashtreeprint(test) );

    add_to_charhash( test, 'a', (void*)1 );
    printf( "After adding a key of 'a', the charhash looks like: \n<br />" );
    printf( "%s\n<br />", charhashtreeprint(test) );

    delete_from_charhash( test, 'a' );
    printf( "After deleting the key of 'a', the charhash looks like: \n<br />" );
    printf( "%s\n<br />", charhashtreeprint(test) );
    return 0;

} // End function main.


