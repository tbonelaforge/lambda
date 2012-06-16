#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>

#include "charhash.h"
#include "hash.h"


#define COMMA_LEN 1;
#define BRACE_LEN 1;

/*====================================================================================

Function: hashinfo

Input: A pointer to a hash structure, 
       and a prefix string to be used as a header for this output.

Output: A string of HTML, representing the info for this hash structure,
        Taking into account the decision char 
        used by the previous node to get to this point.

=======================================================================================*/

char * hashinfo( struct hash * current, char * prefixstr ) {

    // Get the maximum number of characters that may be needed to display a pointer.
    double sizeof_voidpointer = sizeof( void* );
    double biggest_pointer = pow( 2, sizeof_voidpointer * BYTESIZE );
    int howmany_digits = ceil( log10(biggest_pointer) );

    // Allocate a string large enough to hold the three lines of output below.
    int formatlen = 56; // The number 56 is larger than the length of the template.
    char * hashinfostr = malloc( ( 1 + howmany_digits + formatlen + strlen(current->sigstr) + 1 ) * sizeof(char) );
    if( ! hashinfostr ) {
        return "Out of memory!";
    }

    char headerstr[8];
    sprintf( headerstr, "%s<br />", prefixstr );
    strcpy( hashinfostr, headerstr );
    char datastr[43 + howmany_digits];
    sprintf( datastr, "address: %p<br />data: %ld<br />", current, (long int) current->data );
    strcat( hashinfostr, datastr );
    char siginfostr[9 + strlen(current->sigstr)];
    sprintf( siginfostr, "sigstr: %s", current->sigstr );
    strcat( hashinfostr, siginfostr );
    //    char num_childrenstr[9];
    //    sprintf( num_childrenstr, "<br />%d", current->num_children );
    //    strcat( hashinfostr, num_childrenstr );
    return hashinfostr;

} // End function hashinfo


/*********************************************************************************

Function: hashtreeprint

Input: A pointer to a hash structure 
       and a prefix string used to get to 
       that node in the tree.

Output: HTML describing the tree structure of this hash.

*********************************************************************************/


char * hashtreeprint( struct hash * current, char * prefixstr) {

    char * nextprefixstr;
    int num_children, childinfolen, childformatchars, treeformatchars;
    struct hash * nexthash;

    // Get the info string for the current node.
    char * currentinfostr = hashinfo( current, prefixstr);

    // Get a pointer to an array containing the characters in the child charhash.
    char * c = chars_in( current->child, 0, 0 );

    // Record the number of children.
    num_children = strlen(c);

    // Allocate an array of strings ( one for each child ).
    char ** childstrings = malloc( num_children * sizeof(char*) );

    if( ! childstrings ) {
        return("Out of memory!");
    }

    // Go down each branch, creating childstrings.
    int i;
    childinfolen = 0;
    nextprefixstr = malloc( 2 * sizeof(char) );
    if( ! nextprefixstr ) {
        return("Out of memory!");
    }

    childformatchars = 9; // For the <td></td>.
    nextprefixstr[1] = '\0'; // Only the first character will change.

    for( i = 0; i < num_children; i++ ) { // Loop children.
        nexthash = ( struct hash * ) charhashlookup( current->child, c[i] )->data;
        nextprefixstr[0] = c[i];

        // Recurse to the next hash.
        childstrings[i] = hashtreeprint( nexthash, nextprefixstr );

        // Increment the info length.
	childinfolen += ( strlen(childstrings[i]) + childformatchars );
    } // End children loop.

    // Prepare child info string.
    char * childinfostr = malloc((childinfolen + 1) * sizeof(char));
    if( ! childinfostr ) {
        return("Out of memory!");
    }

    *childinfostr = '\0'; // Prepare to strcat onto this.

    // Surround each child with <td> tags.
    for( i = 0; i < num_children; i++ ) { // Loop children again.
        strcat( childinfostr, "<td>" );
        strcat( childinfostr, childstrings[i] );
        strcat( childinfostr, "</td>" );
    } // End children again loop.

    // Allocate a result string, with correct length.
    treeformatchars = 149; // All table formatting.

    char * resultstr = malloc(
	(strlen(childinfostr) +
         strlen(currentinfostr) +
         treeformatchars + 1) * sizeof(char) );

    if( ! resultstr ) {
        return "Out of memory!";
    }

    // Now fill out the template.
    *resultstr = '\0'; // For strcat.
    strcat(resultstr, "<table><tr align=center><td>" );
    strcat(resultstr, currentinfostr );
    strcat(resultstr, "</td></tr><tr align=center><td>" );
    strcat(resultstr, "<table cellpadding=5><tr align=center valign=top>" );
    strcat(resultstr, childinfostr );
    strcat(resultstr, "</tr></table></td></tr></table>" );

    // Free up resources allocated.
    free(childinfostr);
    free(nextprefixstr);
    free(childstrings);
    return resultstr;

} // End function hashtreeprint.



/*============================================================

Function: longest_common_prefix

Input: Two strings, and a pointer to a character.

Output: The length of the longest common prefix between the two
        strings.  Furthermore, 
	the pointed-to character hash been set to a value
	which reflects the state of the string comparison.
	
	EXAMPLES:
	str    => abc
	sigstr => abc
	configuration => 'a' ( exact match )

	str    => abc
        sigstr => abcghi
	configuration => 'b' ( leftover chars in sigstr )

	str    => abcdef
	sigstr => abc
	configuration => 'c' ( leftover chars in str )

	str    => abcdef
	sigstr => abcghi
	configuration => 'd' ( leftover chars in str and sigstr )

============================================================*/


int longest_common_prefix( char * str, char * sigstr, char * config ) {
    char c1 = str[0];
    char c2 = sigstr[0];
    int i = 0;
    while ( c1 && c2 && ( c1 == c2 ) ) {
	i++;
	c1 = str[i];
	c2 = sigstr[i];
    }
    if ( !c1 ) { // Reached the end of str
	if ( !c2 ) { // Reached the end of sigstr at same time
	    *config = 'a'; // Perfect match!
	}
	else { // Leftover chars in sigstr
	    *config = 'b';
	}
    }
    else { // leftover chars in str
	if ( !c2 ) { // Reached the end of sigstr
	    *config = 'c';
	}
	else {
	    *config = 'd';
	}
    }
    return i;
}



/*============================================================

Function: hashlookup

Input: A pointer to a hash structure, and a string (key) to lookup.

Output: Either a pointer to a hash structure which contains data
        associated with the key, or NULL, 
        depending on whether the key was found in the hash or not.

============================================================*/


struct hash * hashlookup( struct hash * root, char * key ) {
    struct hash_search_state search_state = hash_search( root, key );
    if ( search_state.config != 'a' ) { // Not found.
	return NULL;
    }
    return search_state.node;
}



/*============================================================

Function: hash_search

Input: A pointer to a hash, 
       and a key string to search for in the hash.

Output: A hash_search_state structure,
        representing the state of affairs after searching
        for the key in the hash.

============================================================*/


struct hash_search_state hash_search( struct hash * root, char * key ) {
    struct hash * parent = NULL;
    struct hash * node = root;
    struct hash * child;
    char * str = key;
    int num_matched;
    char config;
    char parent_choice = '\0';
    char next_char = '\0';
    struct charhash * child_charhash;
    while ( node ) {
	num_matched = longest_common_prefix( str, node->sigstr, &config );
	if ( config != 'c' ) {
	    break;
	}
	next_char = *( str + num_matched );
	if ( !( child_charhash = charhashlookup( node->child, next_char ) ) ) {
	    break; // No child corresponding to this character.
	}
	str += ( num_matched + 1 );
	parent = node;
	node = child_charhash->data;
	parent_choice = next_char;
    }
    struct hash_search_state search_state = {
	parent,
	node,
	parent_choice,
	str,
	num_matched,
	config
    };
    return search_state;
}





/*============================================================

Function: add_to_hash

Input: A pointer to a hash to be added to, 
       a key ( string ) to added, 
       and a value to be stored under that key.

Output: A pointer to a hash containing the desired value.

============================================================*/


struct hash * add_to_hash( struct hash * root, char * key, void * value ) {
    struct hash_search_state search_state = hash_search( root, key );
    while ( 1 ) {
	if ( search_state.config == 'a' ) { // Found it.
	    search_state.node->data = value;
	    return search_state.node;
	}
	if ( search_state.config == 'c' ) { // Leftover chars in str.
	    return hash_leaf_node( search_state, value );
	}

	// Configuration is either 'b' or 'd'.
	search_state.node = hash_branch_node(search_state);
	search_state.config -= 1; // 'b' goes to 'a', 'd' goes to 'c'.
    }
}



/*============================================================

Function: hash_branch_node

Input: A hash_search_state structure.

Output: A pointer to a newly-created branch node,
        having the matched portion as its sig, 
        which has been inserted into the existing data structure,
        in the appropriate way.

============================================================*/


struct hash * hash_branch_node( struct hash_search_state search_state ) {

    // Construct a new branch node with the matched portion as its sigstr.
    char * new_sigstr = malloc( ( search_state.num_matched + 1 ) * sizeof(char) );
    sub_strcpy( new_sigstr, search_state.node->sigstr, search_state.num_matched );
    struct hash * new_node = new_hash();
    new_node->sigstr = new_sigstr;
    char * unmatched = search_state.node->sigstr + search_state.num_matched;
    char decision_char = *(unmatched++); // Unmatched portion starts after decision char.

    // Change the search_state node's sigstr to the unmatched portion.
    new_sigstr = malloc( ( strlen(unmatched) + 1 ) * sizeof(char) );
    strcpy( new_sigstr, unmatched );
    free(search_state.node->sigstr);
    search_state.node->sigstr = new_sigstr;

    // Insert new branch node between search_state.parent and search_state.node.
    add_to_charhash( search_state.parent->child, search_state.parent_choice, (void*) new_node );
    add_to_charhash( new_node->child, decision_char, search_state.node );
    new_node->num_children++;
    return new_node;
}



/*============================================================

Function: hash_leaf_node

Input: A search_state structure, 
       corresponding to a search that should end with the creation
       of a leaf node, 
       and the desired new_data to be stored in that leaf node.

Output: A pointer to a newly-constructed hash node
        which contains the desired data, 
	and has the unmatched portion of the search_state's str
	as it's sigstr, and is connected to the existing data
        structure as specified by the given search state.

============================================================*/


struct hash * hash_leaf_node( struct hash_search_state search_state, void * value ) {
    struct hash * new_node = new_hash();
    char * unmatched = search_state.str + search_state.num_matched;
    char decision_char = *(unmatched++); // Unmatched portion starts after decision char.
    char * new_sigstr = malloc( ( strlen(unmatched) + 1 ) * sizeof(char) );
    strcpy( new_sigstr, unmatched );
    new_node->sigstr = new_sigstr;
    new_node->data = value;
    add_to_charhash( search_state.node->child, decision_char, new_node );
    search_state.node->num_children++;
    return new_node;
}




/*============================================================

Function: add_pairs_to_hash

Input: A pointer to a hash, the number of pairs to add, 
       and a sequence of arguments of the form
       key, value, key, value, ...

Output: A pointer to the given hash, but all the key-value
        pairs have been stored in the hash.

============================================================*/


struct hash * add_pairs_to_hash( struct hash * my_hash, int num_pairs, ... ) {

    va_list arg_pointer;
    char * key;
    void * value;

    va_start( arg_pointer, num_pairs );

    while ( num_pairs-- ) {
	key = va_arg( arg_pointer, char * );
	value = va_arg( arg_pointer, void * );
	add_to_hash( my_hash, key, value );
    }

    return my_hash;

}




/************************************************************

Function: join_strings_with_char

Input: A char, and two strings.

Output: A single string, 
        containing the result of joining the two strings,
        with the character in between.

************************************************************/

char * join_strings_with_char( char c, char * string1, char * string2 ) {

    char * joined_string;
    int string1_len = strlen(string1);

    joined_string = malloc( ( string1_len + strlen(string2) + 2 ) * sizeof(char) );
    if ( !joined_string ) {
	return "Out of memory!";
    }

    strcpy( joined_string, string1 );
    joined_string[string1_len] = c;
    joined_string[string1_len + 1] = '\0';
    strcat( joined_string, string2 );
    return joined_string;

} // End function join_strings_with_char



/*============================================================

Function: delete_from_hash

Input: A pointer to a hash to use as a starting point for a search,
       and a key ( string ) to be deleted from the hash, if found.

Output: Nothing, but the given key is removed from the hash,
        and the associated memory is freed.

============================================================*/


void delete_from_hash( struct hash * root, char * key ) {
    struct hash_search_state search_state = hash_search( root, key );
    if ( search_state.config != 'a' ) {
	return; // Nothing to do if key not found in hash.
    }
    search_state.node->data = NULL;
    while ( 1 ) {
	if ( !search_state.parent ) {
	    return; // Don't remove root node.
	}
	int num_children = search_state.node->num_children;
	if ( num_children >= 2 ) {
	    return; // Don't delete branch nodes.
	}
	if ( num_children == 1 ) {
	    absorb_single_hash_child(search_state);
	    return;
	}

	// Assert: num_children == 0
	remove_hash_leaf_node(search_state);
	search_state.parent->num_children -= 1;
	if ( search_state.parent->data ) { 
	    return; // Can't consolidate any further.
	}
	search_state.node = search_state.parent;
	search_state.parent = NULL; // Should never have to ascend more than 1x.
    }    
}

/*============================================================

Function: remove_hash_leaf_node

Input: A hash_search_state structure, 
       corresponding to a search where the found node is an only child.

Output: Nothing, but the search_state node hash been removed from the hash,
        and its memory freed.

============================================================*/


void remove_hash_leaf_node( struct hash_search_state search_state ) {
    struct charhash * parent_charhash = search_state.parent->child;
    delete_from_charhash( parent_charhash, search_state.parent_choice );

    // Free the memory used by the leaf node.
    destroy_charhash(search_state.node->child);
    free(search_state.node);
    return;
}


/*============================================================

Function: absorb_single_child

Input: A hash_search_state structure,
       corresponding to a search where the found node
       has a single child.

Output: Nothing, 
        but the key corresponding to the search_state node
	is removed from the hash, 
	and a node's worth of memory is freed.

============================================================*/


void absorb_single_hash_child( struct hash_search_state search_state ) {

    // Figure out what character the single child is listed under.
    char * decision_char = chars_in( search_state.node->child, '\0', 0 ); // 1-char string
    struct hash * child = charhashlookup( search_state.node->child, *decision_char )->data;

    // Concatenate the decision_char and the child's sigstr onto search_state node's sigstr.
    int extension_length = strlen(child->sigstr);
    int sigstr_length = strlen(search_state.node->sigstr);
    char * new_sigstr = malloc( ( sigstr_length + 1 + extension_length ) * sizeof(char) );
    strcpy( new_sigstr, search_state.node->sigstr );
    strcat( new_sigstr, decision_char );
    strcat( new_sigstr, child->sigstr );
    free(search_state.node->sigstr);
    search_state.node->sigstr = new_sigstr;

    // Copy the child node's info.
    destroy_charhash(search_state.node->child);
    search_state.node->child = child->child;
    search_state.node->data = child->data;
    search_state.node->num_children = child->num_children;
    free(child);
    return;
}



/*============================================================

Function: new_hash

Input: Nothing.

Output: A pointer to a newly allocated hash structure,
        with all fields set to zero, or 
        NULL, if there is not enough memory available.


============================================================*/


struct hash * new_hash() {

    struct hash * newnode = ( struct hash * ) malloc(sizeof(struct hash));

    if( ! newnode ) return NULL;

    newnode->data = 0;
    newnode->sigstr = malloc( 1 *sizeof(char) );
    if( ! newnode->sigstr ) {
        return 0;
    }
    *newnode->sigstr = '\0';
    newnode->child = new_charhash();
    if( ! newnode->child ) {
        return 0;
    }
    newnode->num_children = 0;
    return newnode;

} // End function new_hash.



/*============================================================

Function: sub_strcpy

Input: Two strings (destination, source), 
       and a number of chars to copy from the second string.
       The allocated memory pointed to by destination
       must be large enough to hold the specified number of chars.

Output: destination.


============================================================*/

char * sub_strcpy( char * destination, char * source, int num_to_copy ) {

    int i;
    for( i = 0; i < num_to_copy; i++ ) {
        destination[i] = source[i];
    }
    destination[i] = '\0';
    return destination;

} // End function sub_strcpy.




/*============================================================

Function: list_keys_in_hash

Input: A pointer to a hash, 
       a pointer to a list to append to, 
       and a prefix string representing 
       the path used to get to the given hash.

Output: Nothing, but the given list has been filled 
        with strings representing all the keys stored in the
        hash.


============================================================*/

void list_keys_in_hash( struct hash * current, struct list * result, char * prefix ) {
    int siglen = strlen(current->sigstr);
    int prefixlen = strlen(prefix);
    char * path = malloc( ( prefixlen + siglen + 2 ) * sizeof(char) ); // Extra child char.
    strcpy( path, prefix );
    strcat( path, current->sigstr );
    path[ prefixlen + siglen + 1 ] = path[ prefixlen + siglen ] = '\0';
    char * chars_in_child = chars_in( current->child, 0, 0 );
    char child_char = '\0';
    struct hash * child_hash = NULL;
    int i = current->num_children;
    while ( i-- ) { // Recurse through all children.
	child_char = chars_in_child[i];
	if ( child_char == '\0' ) { // No keys to recurse through.
	    continue;
	}
	path[ prefixlen + siglen ] = child_char; // Get the path prefix for this child.
	child_hash = charhashlookup( current->child, child_char )->data;
	list_keys_in_hash( child_hash, result, path );
    }

    // Decide if the path to this node needs to be included in the list of keys.
    if ( current->data ) {
	path[ prefixlen + siglen ] = '\0';
	append_to_list( result, (void *) path );
    }
    else {
	free(path);
    }
    free(chars_in_child);
    return;
}



/*============================================================

Function: destroy_hash

Input: A reference to a hash.

Output: Nothing, but the memory used by that hash ( and all subhashes ) is freed.
        If the data fields of any subhashes point to allocated memory,
        that memory is expected to be freed by some other function.

============================================================*/

void destroy_hash( struct hash * current ) {
    char * child_chars = chars_in( current->child, 0, 0 );
    int i = current->num_children;
    struct hash * child_hash;
    while ( i-- ) {
	child_hash = charhashlookup( current->child, child_chars[i] )->data;
	destroy_hash( child_hash );
    }
    free(child_chars);
    free(current->sigstr);
    destroy_charhash(current->child);
    return;
}


/*============================================================

Function: destroy_key_list

Input: A reference to a list, 
       whose entries are strings created by calls to malloc
       ( such as that produced by list_keys_in_hash ).

Output: Nothing, but the memory used by the list 
        ( and all the strings ) has been freed.

============================================================*/


void destroy_key_list( struct list * key_list ) {
    int key_num = key_list->next_index;
    while ( key_num-- ) {
	free( listlookup( key_list, key_num ) );
    }
    destroy_hash(key_list->main_hash->main_hash);
    free(key_list->main_hash);
    free(key_list);
    return;
}


/*============================================================

Function: print_hash_key_set

Input: A reference to a hash.

Output: A string representing the set of keys stored in the hash.

============================================================*/

char * print_hash_key_set( struct hash * current ) {
    struct list * var_list = new_list();
    list_keys_in_hash( current, var_list, "" );
    int var_list_len = 0;
    int i = var_list->next_index;
    while ( i-- ) {
	var_list_len += strlen(listlookup( var_list, i ));
    }
    int brace_len = BRACE_LEN;
    int comma_len = COMMA_LEN;
    char * key_set_str = malloc( 
				( var_list->next_index  * comma_len
				 + var_list_len
				 + 2 * brace_len
				  + 1
				  ) * sizeof(char)
				 );

    *key_set_str = '\0';
    sprintf( key_set_str, "{" );
    i = var_list->next_index;
    while ( i-- ) {
	strcat( key_set_str, listlookup( var_list, i ));
	if ( i ) {
	    strcat( key_set_str, "," );
	}
    }
    strcat( key_set_str, "}" );
    destroy_key_list(var_list);
    return key_set_str;
}


/*============================================================

Function: copy_shallow_hash

Input: A pointer to a hash to copy,
       assuming that it's ok to copy
       all of the data fields directly.
       ( If any of the data fields point to allocated
       memory, this functieon should not be used. )

Output: A pointer to a newly-allocated hash containing
        the exact same data as the original,
	under the exact same keys.

============================================================*/


struct hash * copy_shallow_hash( struct hash * original ) {
    struct hash * copy = new_hash();
    struct list * key_list = new_list();
    list_keys_in_hash( original, key_list, "" );
    char * key = NULL;
    int i = key_list->next_index;
    while ( i-- ) {
	key = listlookup( key_list, i );
	add_to_hash( copy, key, hashlookup( original, key ) );
    }
    destroy_key_list( key_list );
    return copy;
}
