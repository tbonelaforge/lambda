#include <stdio.h>
#include <string.h>

#include "list.h"

int main() {
    struct list * my_list = new_list();
    append_items_to_list( my_list, 3,
        (void *) "A", (void *) "B", (void *) "C" 
    );
    char * element;

    printf( "After appending items to list, the first element is: \n" );
    element = listlookup( my_list, 0 );
    printf( "%s\n", element );

    printf( "The second element is: \n" );
    element = listlookup( my_list, 1 );
    printf( "%s\n", element );

    printf( "The third element is: \n" );
    element = listlookup( my_list, 2 );
    printf( "%s\n", element );

}
