//
//  mylib.h
//  
//
//  Created by Simone Girardi on 13/05/16.
//
//
/******************************************
 
******************************************/

#ifndef MYLIB_H
#define MYLIB_H
 
#include <sys/ipc.h>
#include <sys/shm.h>

#include <stdio.h>  /* standard I/O functions */
#include <stdlib.h>
#include <unistd.h> /* standard unix functions like getpid(), write() etc */
#include <string.h>

#include "utils.h"

#define STDIN 0
#define STDOUT 1

struct list{
    char* value;
    struct list *next;
};

void syserr(char *prog, char *msg);
void syserr_ext(char *prog, char *msg, int line);
 
struct list *list_create(char* value);
struct list *list_add(char* value, struct list *next);
void fill_list_operations(struct list *list, struct operation *operations);
void list_print(struct list *this);
void list_free(struct list *first_element);

void print(const char* msg, const char*caller, int line);

#endif /* mylib_h */

