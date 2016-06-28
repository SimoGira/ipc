//
//  utils.h
//  
//
//  Created by Simone Girardi on 21/06/16.
//
//

#ifndef utils_h
#define utils_h

#include "mylib.h"

typedef struct XMem {
    key_t key ;
    int shmid ;
    char buf [1];
} XMem ;

struct operation{
    int id;
    int val1;
    int val2;
    char operator;
};

struct result {
    int id;
    float val;
};

struct sembuf sops;

int do_semget(key_t key, int NPROC);
void initialize_sem(int semid, union semun *semarg, unsigned short values[]);
void sem_v(int semid, int num);
void sem_p(int semid, int num);
void delete_sem(int semid);

void * xmalloc ( key_t key , const size_t size );
void xfree(void * address);
void * xattach ( key_t key , const size_t size );

float process_operation(int val1, int val2, char op);

#endif /* utils_h */
