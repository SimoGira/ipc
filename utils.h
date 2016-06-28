//
//  utils.h
//  
//
//  Created by Simone Girardi on 21/06/16.
//
//

#ifndef utils_h
#define utils_h

#include <stdbool.h>
#include <sys/sem.h>
#include <fcntl.h>  /* for open() etc */
//#include <signal.h> /* for siganls - pipe - fifo */

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

typedef struct XMem {
    key_t key ;
    int shmid ;
    char buf [1];
} XMem;

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
