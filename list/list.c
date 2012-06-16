#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "hash_default.h"

#include "list.h"



/*************************************************************

Function: new_list

Input: nothing

Output: A pointer to a newly allocated list structure,
        complete with an empty hash_default structure, 
        and next_index of 0.

************************************************************/

struct list * new_list() {

    struct hash_default * my_hash_default = new_hash_default();

    if ( ! my_hash_default ) { 
	printf( "Out of memory!\n" );
	return NULL;
    }

    struct list * my_list = malloc(sizeof(struct list));
    if ( ! my_list ) {
	printf( "Out of memory!\n" );
	return NULL;
    }
    my_list->main_hash = my_hash_default;
    my_list->next_index = 0;

    return my_list;

}


/*************************************************************

Function: listlookup

Input: A pointer to a list data structure, and an integer
       to look up in the list.

Output: A void* representing the data contained at the given
        index of the list, or NULL, if not found.

*************************************************************/

void * listlookup( struct list * current, int index ) {
    char * index_string = num_to_string(index);
    void * result = hash_default_lookup( current->main_hash, index_string );
    free(index_string);

    return result;

}






/*************************************************************

Function: append_to_list

Input: A pointer to a list structure, 
       and some data to push onto the end of the list.

Output: The address of the given list, 
        but the new data has been appended onto the list.

************************************************************/


struct list * append_to_list( struct list * current, void * data ) {
    char * next_index_string = num_to_string(current->next_index);
    add_to_hash( current->main_hash->main_hash, next_index_string, data );
    free(next_index_string);
    current->next_index++;

    return current;

}


/*************************************************************

Function: append_items_to_list

Input: A pointer to a list, the number of items to add,
       and a sequence of (void*) arguments.

Output: A pointer to the given list, 
        but all the items have been appended.
        

************************************************************/

struct list * append_items_to_list( struct list * my_list, int num_items, ... ) {
    va_list arg_pointer;
    void * item;
    va_start( arg_pointer, num_items );
    while ( num_items-- ) {
	item = va_arg( arg_pointer, void * );
	append_to_list( my_list, item );
    }
    return my_list;
}


/*************************************************************

Function: pop_from_list

Input: A pointer to a list

Output: Either the data being stored at the last index of the list,
        or NULL, depending on whether there is anything in the list or not.
        If there was something in the list, 
        the last element has been removed.

************************************************************/

void * pop_from_list( struct list * list ) {
    if ( ! list->next_index ) {
	return NULL;
    }
    int index = list->next_index - 1;
    void * value = listlookup( list, index );
    char * index_str = num_to_string(index);
    delete_from_hash( list->main_hash->main_hash, index_str );
    free(index_str);
    list->next_index = index;
    return value;
}



/*************************************************************

Function: extend_list

Input: A pointer to a list structure to extend, 
       and a pointer to a list structure to use 
       as an extension.

Output: The address of the first list, 
        but it has been extended with the elements of the 
        second list.

************************************************************/

struct list * extend_list( struct list * extendee, struct list * extension ) {

    int i;
    for ( i = 0; i < extension->next_index; i++ ) {
	void * data = listlookup( extension, i );
	append_to_list( extendee, data );
    }

    return extendee;

} // End function extend_list.


/*************************************************************

Function: copy_to_list

Input: A pointer to a destination list, 
       and a pointer to a source list.

Output: Nothing, but the first list contains
        at least all the info from the second, 
        with the correct next_index.

************************************************************/

void copy_to_list( struct list * destination, struct list * source ) {

    int i;

    destination->next_index = 0;
    for ( i = 0; i < source->next_index; i++ ) {
	append_to_list( destination, listlookup( source, i ) );
    }

    return;

} // End function copy_to_list.



/*************************************************************

Function: get_line_as_list

Input: A pointer to a list structure.

Output: The number of characters in the next line
        read from standard input.  
        Also, the line is stored in the given list,
        by indexing each character by the order in which it was read.
        

************************************************************/

int get_line_as_list( struct list * buffer ) {

    int c;
    buffer->next_index = 0;

    while ( ( c = getchar() ) != '\n' && c != EOF ) {
	append_to_list( buffer, (void*)(long int) c );
    }
    if ( c == '\n' ) {
	append_to_list( buffer, (void*)(long int) c );
    }

    return buffer->next_index;

} // End function get_line_as_list.


/*************************************************************

Function: print_list_as_chars

Input: A pointer to a list structure.

Output: Nothing, but the contents of the list have been
        interpreted as characters, and put on the input stream.

************************************************************/

void print_list_as_chars( struct list * string ) {

    int i;
    char c;
    for ( i = 0; i < string->next_index; i++ ) {
	c = (long int) listlookup( string, i );
	putchar(c);
    }
    
    return;

} // End function print_list_as_chars.


/*************************************************************

Function: destroy_empty_list

Input: A pointer to a list which is assumed to be empty.

Output: Nothing, but the memory used by that list is freed.

************************************************************/

void destroy_empty_list( struct list * l ) {
    destroy_hash( l->main_hash->main_hash );
    free(l);
}

/*============================================================

Function: destroy_list

Input: A reference to a list.

Output: Nothing, but the memory used by that list ( and all subhashes ) is freed.
        If the data fields of any subhashes point to allocated memoryf,
        that memory is expected to be freed by some other function.

============================================================*/


void destroy_list( struct list * this_list ) {
    while ( this_list->next_index ) {
        pop_from_list( this_list );
    }
    destroy_empty_list( this_list );
    return;
}
