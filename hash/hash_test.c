#include <stdio.h>

#include "hash.h"
#include "list.h"

int main() {

    struct hash * testhash = new_hash();
    printf( "the new hash looks like:<br />\n");
    printf( "%s<br />\n", hashtreeprint(testhash,"") );
    add_to_hash( testhash, "f", (void *) 1 );
    printf( "After adding f, the new hash looks like:<br />\n" );
    printf( "%s<br />\n", hashtreeprint(testhash,"") );
    add_to_hash( testhash, "x", (void *) 1 );
    printf( "After adding x, the new hash looks like: <br />\n" );
    printf( "%s<br />\n", hashtreeprint(testhash,"") );
    delete_from_hash( testhash, "x" );
    printf( "After deleting x, the new hash looks like: <br />\n" );
    printf( "%s<br />\n", hashtreeprint(testhash,"") );



    add_to_hash( testhash, "newline_char", (void *) "string" );
    printf( "After adding newline_char as a key to the hash, it looks like: \n" );
    printf( "%s<br />\n", hashtreeprint( testhash, "" ) );
    add_to_hash( testhash, "tab_char", (void *) "poop" );
    printf( "After adding tab_char as a key to the hash, it looks like: \n" );
    printf( "%s<br />\n", hashtreeprint( testhash, "" ) );
    add_to_hash( testhash, "write_tab", (void *) "ppoopy" );
    printf( "After adding write_tab as a key to the hash, it looks like: \n" );
    printf( "%s<br />\n", hashtreeprint( testhash, "" ) );
    add_to_hash( testhash, "write_newline", (void *) &main );
    printf( "After adding write_newline as a key to the hash, it looks like: \n" );
    printf( "%s<br />\n", hashtreeprint( testhash, "" ) );
    add_to_hash( testhash, "write_default", (void *) "face" );
    printf( "After adding write_default as a key to the hash, it looks like: \n" );
    printf( "%s<br />\n", hashtreeprint( testhash, "" ) );

    printf( "The result of looking up write_default in the hash is: <br />\n" );
    printf( "%s<br />\n", (char *) hashlookup( testhash, "write_default" )->data );




    testhash = new_hash();
    printf( "Before adding anything to the hash, it has keys: \n<br />" );
    struct list * keys = new_list();
    char prefix[1] = { '\0' };
    list_keys_in_hash( testhash, keys, prefix );
    int i = keys->next_index;
    while ( i-- ) {
	char * key_string = listlookup( keys, i );
	printf( "%s\n<br />", key_string );
	free(key_string);
    }
    destroy_hash(keys->main_hash->main_hash);
    free(keys->main_hash);
    free(keys);

    printf( "Initially, the hash looks like: \n<br />" );
    printf( "%s\n<br />", hashtreeprint( testhash, "" ) );

    add_to_hash( testhash, "abba", (void*) 1 );
    printf( "After adding abba -> 1, the hash looks like: \n<br />" );
    printf( "%s\n<br />", hashtreeprint( testhash, "" ) );

    add_to_hash( testhash, "abb", (void*) 2 );
    printf( "After adding abb -> 2, the hash looks like: \n<br />" );
    printf( "%s\n<br />", hashtreeprint( testhash, "" ) );

    add_to_hash( testhash, "abby", (void*) 3 );
    printf( "After adding abby -> 3, the hash looks like: \n<br />" );
    printf( "%s\n<br />", hashtreeprint( testhash, "" ) );

    add_to_hash( testhash, "cat", (void*) 4 );
    printf( "After adding cat -> 4, the hash looks like: \n<br />" );
    printf( "%s\n<br />", hashtreeprint( testhash, "" ) );

    add_to_hash( testhash, "catalog", (void*) 5 );
    printf( "After adding catalog -> 5, the hash looks like: \n<br />" );
    printf( "%s\n<br />", hashtreeprint( testhash, "" ) );

    keys = new_list();
    list_keys_in_hash( testhash, keys, prefix );
    printf( "The keys in the hash are: \n<br />" );
    i = keys->next_index;
    while ( i-- ) {
	char * key_string = listlookup( keys, i );
	printf( "%s\n<br />", key_string );
	free(key_string);
    }
    destroy_hash(keys->main_hash->main_hash);
    free(keys->main_hash);
    free(keys);

    delete_from_hash( testhash, "abb" );
    printf("After deleting the key 'abb', the keys in the hash are now: \n<br />");
    keys = new_list();
    list_keys_in_hash( testhash, keys, prefix );
    i = keys->next_index;
    while ( i-- ) {
	char * key_string = listlookup( keys, i );
	printf( "%s\n<br />", key_string );
	free(key_string);
    }
    destroy_hash(keys->main_hash->main_hash);
    free(keys->main_hash);
    free(keys);


    struct hash * testhash3 = new_hash();

    add_pairs_to_hash( testhash3, 5, 
		       "abba",    (void*) 1,
		       "abb",     (void*) 2,
		       "abby",    (void*) 3,
		       "cat",     (void*) 4,
		       "catalog", (void*) 5
		       );

    printf( "After adding the same keys using the add_pairs_to_hash method, the hash looks like: \n<br />" );
    printf( "%s\n<br />", hashtreeprint( testhash3, "" ) );
		       


    struct hash * looked_up;
    
    printf( "The result of looking up abba in the hash is: \n<br />" );
    printf( "%ld\n<br />", ( looked_up = hashlookup( testhash, "abba" ) ) ? (long int) looked_up->data : 0 );

    printf( "The result of looking up abb in the hash is: \n<br />" );
    printf( "%ld\n<br />", ( looked_up = hashlookup( testhash, "abb" ) ) ? (long int) looked_up->data : 0 );

    printf( "The result of looking up abby in the hash is: \n<br />" );
    printf( "%ld\n<br />", ( looked_up = hashlookup( testhash, "abby" ) ) ? (long int) looked_up->data : 0 );

    printf( "The result of looking up cat in the hash is: \n<br />" );
    printf( "%ld\n<br />", ( looked_up = hashlookup( testhash, "cat" ) ) ? (long int) looked_up->data : 0 );

    printf( "The result of looking up catalog in the hash is: \n<br />" );
    printf( "%ld\n<br />", ( looked_up = hashlookup( testhash, "catalog" ) ) ? (long int) looked_up->data : 0 );


    delete_from_hash( testhash, "abb" );
    printf( "After deleting abb from the hash, it looks like: \n<br />" );
    printf( "%s\n<br />", hashtreeprint(testhash,"") );




    //
    // Test 1
    //
    printf("STARTING TEST1<br />\n");
    testhash = new_hash();
    add_to_hash( testhash, "abba", (void*)1 );
    add_to_hash( testhash, "abby", (void*)2 );
    printf( "After adding abba -> 1, and abby -> 2, the hash looks like: \n<br />" );
    printf( "%s\n<br />", hashtreeprint( testhash,"" ) );

    delete_from_hash( testhash, "abbs" );
    printf( "After deleting a key of abbs, the hash looks like: \n<br />" );
    printf( "%s\n<br />", hashtreeprint( testhash, "" ) );




    //
    // TEST 2
    //

    printf( "STARTING TEST2<br />\n" );
    testhash = new_hash();
    add_to_hash( testhash, "abba", (void*)1 );
    add_to_hash( testhash, "abby", (void*)2 );
    printf( "After adding abba -> 1, and abby -> 2, the hash looks like: \n<br />" );
    printf( "%s\n<br />", hashtreeprint( testhash, "" ) );

    delete_from_hash( testhash, "abb" );
    printf( "After deleting a key of abb, the hash looks like: \n<br />" );
    printf( "%s\n<br />", hashtreeprint( testhash, "" ) );




    //
    // Test 3
    //
    printf("STARTING TEST 3<br />\n");
    testhash = new_hash();
    add_to_hash( testhash, "abba", (void*)1 );
    add_to_hash( testhash, "abby", (void*)2 );
    add_to_hash( testhash, "catalog", (void*)3 );
    printf( "After adding abba -> 1, abby -> 2, and catalog -> 3, the hash looks like: \n<br />" );
    printf( "%s\n<br />", hashtreeprint( testhash, "" ) );

    delete_from_hash( testhash, "cat" );
    printf( "After deleting a key of cat, the hash looks like: \n<br />" );
    printf( "%s\n<br />", hashtreeprint( testhash, "" ) );




    //
    // Test 4
    //
    printf("STARTING TEST 4<br />\n");
    testhash = new_hash();
    add_to_hash( testhash, "abb", (void*)1 );
    add_to_hash( testhash, "abby", (void*)2 );
    printf( "After adding abb -> 1, abby -> 2, the hash looks like: \n<br />" );
    printf( "%s\n<br />", hashtreeprint( testhash, "" ) );

    delete_from_hash( testhash, "cat" );
    printf( "After deleting a key of cat, the hash looks like: \n<br />" );
    printf( "%s\n<br />", hashtreeprint( testhash, "" ) );





    //
    // Test 5
    //

    printf("STARTING TEST 5<br />\n");
    testhash = new_hash();
    add_to_hash( testhash, "abby", (void*)1 );
    add_to_hash( testhash, "abba", (void*)2 );
    add_to_hash( testhash, "cata", (void*)3 );
    printf( "After adding abby -> 1, abba -> 2, and cata -> 3, the hash looks like: \n<br />" );
    printf( "%s\n<br />", hashtreeprint( testhash, "" ) );

    delete_from_hash( testhash, "catalog" );
    printf( "After deleting a key of catalog, the hash looks like:\n<br />" );
    printf( "%s\n<br />", hashtreeprint( testhash, "" ) );




    //
    // Test 6
    //
    printf("STARTING TEST 6<br />\n");
    testhash = new_hash();
    add_to_hash( testhash, "abbs", (void*)1 );
    add_to_hash( testhash, "abba", (void*)2 );
    add_to_hash( testhash, "abby", (void*)3 );
    printf( "After adding abbs -> 1, abba -> 2, and abby -> 3, the hash looks like:\n<br />" );
    printf( "%s\n<br />", hashtreeprint( testhash, "" ) );

    delete_from_hash( testhash, "abba" );
    printf( "After deleteing the key abba, the hash looks like: \n<br />" );
    printf( "%s\n<br />", hashtreeprint( testhash, "" ) );



    //
    // Test 7
    //

    printf("STARTING TEST 7<br />\n");
    testhash = new_hash();
    add_to_hash( testhash, "abb", (void*)1 );
    add_to_hash( testhash, "abby", (void*)2 );
    add_to_hash( testhash, "abba", (void*)3 );
    printf( "After adding abb -> 1, abby -> 2, and abba -> 3, the hash looks like:\n<br />" );
    printf( "%s\n<br />", hashtreeprint( testhash, "" ) );

    delete_from_hash( testhash, "abb" );
    printf( "After deleting the key abb, the hash looks like: \n<br />" );
    printf( "%s\n<br />", hashtreeprint( testhash, "" ) );




    //
    // Test 8
    //

    printf("STARTING TEST 8<br />\n");
    testhash = new_hash();
    add_to_hash( testhash, "abby", (void*)1 );
    add_to_hash( testhash, "cat", (void*)2 );
    add_to_hash( testhash, "catalog", (void*)3 );
    printf( "After adding abby -> 1, cat -> 2, and catalog -> 3, the hash looks like: \n<br />" );
    printf( "%s\n<br />", hashtreeprint( testhash, "" ) );

    delete_from_hash( testhash, "cat" );
    printf( "After deleting the key cat, the hash looks like:\n<br />" );
    printf( "%s\n<br />", hashtreeprint( testhash, "" ) );



    //
    // Test 9
    //

    printf("STARTING TEST 9<br />\n");
    testhash = new_hash();
    add_to_hash( testhash, "abby", (void*)1 );
    add_to_hash( testhash, "cat", (void*)2 );
    add_to_hash( testhash, "catalog", (void*)3 );
    printf( "After adding abby -> 1, cat -> 2, and catalog -> 3, the hash looks like: \n<br />" );
    printf( "%s\n<br />", hashtreeprint( testhash, "" ) );

    delete_from_hash( testhash, "catalog" );
    printf( "After deleting the key catalog, the hash looks like: \n<br />" );
    printf( "%s\n<br />", hashtreeprint( testhash, "" ) );


} // End function main.
