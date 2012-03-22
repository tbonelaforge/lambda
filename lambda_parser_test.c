#include <stdio.h>

#include "lambda_parser.h"
#include "lambda_expr.h"
#include "charhash.h"

int main() {

/*
    struct trie_node * trie_node1 = new_trie_node();
    trie_node1->accepting_type = "type1";

    struct trie_node * trie_node2 = new_trie_node();
    trie_node2->accepting_type = "type2";

    struct trie_node * trie_node3 = new_trie_node();
    add_to_charhash( trie_node3->child, 'a', (void*) trie_node1 );
    add_to_charhash( trie_node3->child, 'y', (void*) trie_node2 );

    struct trie_node * trie_node4 = new_trie_node();
    add_to_charhash( trie_node4->child, 'b', (void*) trie_node3 );

    struct trie_node * trie_node5 = new_trie_node();
    add_to_charhash( trie_node5->child, 'b', (void*) trie_node4 );

    struct trie_node * trie_node6 = new_trie_node();
    add_to_charhash( trie_node6->child, 'a', (void*) trie_node5 );

    char * input_string = "abbaabby";

    int pos = 0;
    struct token my_token;
    my_token = get_next_token( input_string, pos, trie_node6 );
    printf( "The result of get_next_token is: \n" );
    printf( "token type : %s\n", my_token.type );
    printf( "token first : %d\n", my_token.first );
    printf( "token last : %d\n", my_token.last );
    pos = my_token.last + 1;

    my_token = get_next_token( input_string, pos, trie_node6 );
    printf( "The result of get_next_token is: \n" );
    printf( "token type : %s\n", my_token.type );
    printf( "token first : %d\n", my_token.first );
    printf( "token last : %d\n", my_token.last );
    pos = my_token.last + 1;

*/

    fill_TABLE();
    fill_GOTO();
    fill_TRIE();
    init_NODE_HASH();
//    char * test_string = "\\m\\n(m,n)";
//    char * test_string = "\\x.x";
//    char * test_string = "\\m(m,m)";
//    char * test_string = "\\m(m,lookmanohands)";
//    char * test_string = "(m,m)";
//    char * test_string = "(";
//    char * test_string = "(\\x.y,\\x.y)";
//    char * test_string = "((x,y),(x,y))";
//    char * test_string = "\\x\\y.y";
//    char * test_string = "\\m\\n\\f\\x((m,f),((n,f),x))";
//    char * test_string = "(\\m\n\\f\\x((m,f),((n,f),x)),\\f\\x(f,x))";
    char * test_string = "(\\x.y,z)";
    char * error;
printf( "Inside lambda_parser_test, about to call parse.<br />\n" );
    struct lambda_expr * parse_tree = parse( test_string, error );
    printf( "The result of the parse is: <br />\n" );
    printf( "%s<br />\n", print_lambda_expr( parse_tree ) );

}
