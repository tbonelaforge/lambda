#include <stdio.h>

#include "charhash.h"


int main() {

    /*
    char test_char = 37;
    printf( "Before reversing, the test_char has bit pattern: \n" );
    printf( "%s\n", binarystring(test_char) );
    char reversed_test_char = reverse_bits( 6, test_char );
    printf( "The reversed test_char has bit pattern: \n" );
    printf( "%s\n", binarystring(reversed_test_char) );
    */

    struct charhash * test = new_charhash();
    
    add_to_charhash( test, 'x', (void*)1 );
    printf( "After adding x -> 1, the charhash looks like: \n<br />" );
    printf( "%s<br />\n", charhashtreeprint(test) );

    add_to_charhash( test, 'y', (void*)1 );
    printf( "After adding y -> 1, the charhash looks like: \n<br />" );
    printf( "%s<br />\n", charhashtreeprint(test) );

    delete_from_charhash( test, 'y' );
    printf( "After deleting y -> 1, the charhash looks like: \n<br />" );
    printf( "%s<br />\n", charhashtreeprint(test) );

    add_to_charhash( test, 'a', (void*)12 );
    printf( "After adding a -> 12, the charhash looks like: \n<br />" );
    printf( "%s\n<br />", charhashtreeprint(test) );

    add_to_charhash( test, 'b', (void*)13 );
    printf( "After adding b -> 13, the charhash looks like: \n<br />" );
    printf( "%s\n<br />", charhashtreeprint(test) );

    printf( "The result of looking up 'a' in the charhash is: \n<br />" );
    struct charhash * looked_up;
    if ( looked_up = charhashlookup( test, 'a' ) ) {
	printf( "%d\n<br />", (int)(long int) looked_up->data );
    }
    else {
	printf( "0\n<br />" );
    }

    printf( "The result of looking up 'b' in the charhash is: \n<br />" );
    if ( looked_up = charhashlookup( test, 'b' ) ) {
	printf( "%d\n<br />", (int)(long int) looked_up->data );
    }
    else {
	printf( "0\n<br />" );
    }

    printf( "The result of looking up 'c' in the charhash is: \n<br />" );
    if ( looked_up = charhashlookup( test, 'c' ) ) {
	printf( "%d\n<br />", (int)(long int) looked_up->data );
    }
    else {
	printf( "0\n<br />" );
    }



    add_to_charhash( test, 'A', (void*)14 );

    printf( "After adding a -> 12, b -> 13, and A -> 14, the charhash looks like: \n<br />" );
    printf( "%s", charhashtreeprint(test) );

    add_to_charhash( test, 'Z', (void*)42 );

    printf( "After adding Z -> 42, the charhash looks like: \n<br />" );
    printf( "%s", charhashtreeprint(test) );
    printf( "And the result of chars_in is: \n<br />" );
    printf( "%s\n<br />", chars_in( test, 0, 0 ) );

    add_to_charhash( test, 'Z', (void*)42 );



    delete_from_charhash( test, 'Z' );
    printf( "After deleting Z -> 42, the charhash looks like: \n<br />" );
    printf( "%s", charhashtreeprint(test) );

    //
    // Test 1
    //

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
    
    test = new_charhash();
    add_to_charhash( test, (char)21, (void*)1 );
    add_to_charhash( test, (char)29, (void*)2 );
    printf( "After adding 21 -> 1, and 29 -> 2, the hash looks like: \n<br />" );
    printf( "%s", charhashtreeprint(test) );

    delete_from_charhash( test, (char) 8 );
    printf( "After deleting a key of 8, the hash looks likd: \n<br />" );
    printf( "%s", charhashtreeprint(test) );

    //
    // Test 5
    //

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

    test = new_charhash();
    printf( "Before adding anything, the charhash looks like: \n<br />" );
    printf( "%s\n<br />", charhashtreeprint(test) );
    
    add_to_charhash( test, 'a', (void*)1 );
    printf( "After adding a key of 'a', the charhash looks like: \n<br />" );
    printf( "%s\n<br />", charhashtreeprint(test) );

    delete_from_charhash( test, 'a' );
    printf( "After deleting the key of 'a', the charhash looks like: \n<br />" );
    printf( "%s\n<br />", charhashtreeprint(test) );


} // End function main.
