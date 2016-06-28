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

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h> /* standard unix functions like getpid() */
#include <ctype.h>
#include <stdio.h>  /* standard I/O functions */
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <signal.h>

#define STDIN 0
#define STDOUT 1

/* explicit declaration required for others OS */
#ifndef __APPLE__
union semun
{
    int val;                // value for SETVAL
    struct semid_ds* buf;   // buffer for IPC_STAT, IPC_SET
    unsigned short*  array; // array for GETALL, SETALL
    struct seminfo*  __buf; // buffer for IPC_INFO
};
#endif

struct list{
    char* value;
    struct list *next;
};

void syserr(char *prog, char *msg);
void syserr_ext(char *prog, char *msg, int line);
 
struct list *list_create(char* value);
struct list *list_add(char* value, struct list *next);
void list_free(struct list *first_element);
void list_print(struct list *this);

void print(const char* msg, const char*caller, int line);

#endif /* mylib_h */

