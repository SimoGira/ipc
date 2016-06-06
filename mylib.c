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
#include<stdlib.h>
#include <errno.h>

#include "mylib.h"

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
    previous->next = new;
    return new;
}

void list_print(struct list *this){
    if (this != NULL) {
        printf("%s ", this->value);
        list_print(this->next);
    }
}

void syserr(char *prog, char *msg){
    fprintf(stderr, "%s - \e[91merror\e[0m: %s\n",prog, msg);
    perror("system error");
    exit(1);
}
