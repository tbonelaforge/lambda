#ifndef _LIST_H_
#define _LIST_H_

#include "hash_default.h"


/*============================================================

Definition: list

============================================================*/

struct list {
    struct hash_default * main_hash;
    int next_index;
};


struct list * new_list();
void * listlookup( struct list *, int );
struct list * append_to_list( struct list *, void * );
struct list * append_items_to_list( struct list *, int, ... );
void * pop_from_list( struct list * );
struct list * extend_list( struct list *, struct list * );
int get_line_as_list( struct list * );
void print_list_as_chars( struct list * );
void destroy_empty_list( struct list * );
void destroy_list( struct list * );


#endif // #ifndef _LIST_H
