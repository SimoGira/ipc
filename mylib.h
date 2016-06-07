//
//  mylib.h
//  
//
//  Created by Simone Girardi on 13/05/16.
//
//
/******************************************
 MODULO: mylib.h
 SCOPO: definizioni per la libreria mylib
******************************************/

#ifndef MYLIB_H
#define MYLIB_H

#include <stdio.h>
#include <stdbool.h>
 
# include <sys/ipc.h>
# include <sys/shm.h>
#include <semaphore.h>

typedef struct XMem {
	key_t key ;
	int shmid ;
	char buf [1];
} XMem ;

void * xmalloc ( key_t key , const size_t size );
void xfree(void * address);
void * xattach ( key_t key , const size_t size );


void syserr(char *prog, char *msg);
void syserr_ext(char *prog, char *msg, int line);

struct list{
    char* value;
    struct list *next;
};
 
struct list *list_create(char* value);
struct list *list_add(char* value, struct list *next);
void list_free(struct list *first_element);
void list_print(struct list *this);



#endif /* mylib_h */

