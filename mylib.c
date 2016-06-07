//
//  mylib.c
//
//
//  Created by Simone Girardi on 13/05/16.
//
//
/******************************************
 MODULO: mylib.c
 SCOPO: libreria di funzioni d’utilita'
******************************************/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "mylib.h"

int process_operation(int val1, int val2, char op){
    switch (op) {
        case '+':
            return val1 + val2;
            break;
        case '-':
            return val1 - val2;
            break;
        case '*':
            return val1 * val2;
            break;
        case '/':
            return val1 / val2;
            break;
        default:
            syserr("process operation: ", "not valid operator");
            break;
    }
    return -1;
}

//shared memory


void * xmalloc ( key_t key , const size_t size ) {
	const int shmid = shmget ( key , size + sizeof ( XMem ) - sizeof ( char ) , 0666| IPC_CREAT );
	if ( shmid == -1)
         syserr ("xmalloc", "shmget");
         
	XMem * ret = ( XMem *) shmat ( shmid , NULL , 0);
	if ( ret == ( void *) -1) 
         syserr ("xmalloc", "shmat");
         
	ret->key = key ;
	ret->shmid = shmid ;
	return ret-> buf ;
}
// int * xi = ( int *) xmalloc ( ftok ( argv [0] , ’a ’) , sizeof ( int ) * 8);

void * xattach ( key_t key , const size_t size ) {
	const int shmid = shmget ( key , size , 0);
	if ( shmid == -1)
         syserr ("xattach", "shmget");
	void * ret = shmat ( shmid , NULL , 0);
	if ( ret == ( void *) -1)
         syserr ("xattach", "shmat");
	return ret;
}


void xfree ( void * ptr ) {
	XMem tmp ;
	XMem * mem = ( XMem *)((( char *) ptr ) - ((( char *)& tmp.buf ) - (( char *)& tmp.key )));
	const int shmid = mem-> shmid ;
	shmdt ( mem );
	shmctl ( shmid , IPC_RMID , NULL );
} 
// xfree ( my_pointer );

 
struct list *list_create(char* value){
    struct list *new = (struct list*) malloc(sizeof(struct list));
    new->value = value;
    new->next = NULL;
    return new;
}

struct list *list_add(char* value, struct list *previous){
	if(previous == NULL) 
         syserr ("list_add", "previous list element is NULL!");
	
    struct list *new = (struct list*) malloc(sizeof(struct list));
    new->value = value;
    new->next = NULL;
    previous->next = new;
    return new;
}

void list_print(struct list *this){
    if (this != NULL) {
        printf("%s ", this->value);
        list_print(this->next);
    }
}
 
void list_free(struct list *this){
    if (this != NULL) { 
        list_free(this->next);
        free(this);
    }
}

void syserr(char *prog, char *msg){
    fprintf(stderr, "%s - \e[91merror\e[0m: %s\n",prog, msg);
    perror("system error");
    exit(1);
}


void syserr_ext(char *prog, char *msg, int line){
    fprintf(stderr, "%s | line: %d | \e[91merror\e[0m: %s\n",prog, line, msg);
    perror("system error");
    exit(1);
}


