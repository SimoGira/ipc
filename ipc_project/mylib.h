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


void syserr(char *prog, char *msg);

struct list{
    char* value;
    struct list *next;
};
 
struct list *list_create(char* value);
struct list *list_add(char* value, struct list *next);
void list_print(struct list *this);



#endif /* mylib_h */
