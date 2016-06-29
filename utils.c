
/**
 * @file utils.c
 * @author Simone Girardi
 * @date 27 jun 2016.
 * @version 1.0
 */

#include "utils.h"
#include "mylib.h"
#include "parent.h"
#include "child.h"


/******************************************
 MODULO: utils.c
 SCOPO: libreria di funzioni d’utilita' estesa
 ******************************************/

// ============================================================================================================
//                                               FUNCTIONS FOR - SEMAPHORE
// ============================================================================================================
int do_semget(key_t key, int nsems){
    int semid;
    if (( semid = semget(key, nsems, IPC_CREAT | 0777)) == -1)
        syserr_ext ("utils.c", " semget " , __LINE__);
    return semid;
}

// ------------------------------------------------------------------------------------------------------------
void initialize_sem(int semid, union semun *semarg, unsigned short values[]){
    
    semarg->array = values;
    
    if ( semctl(semid, 0, SETALL, *semarg) == -1){
        syserr_ext ("utils.c" , "semctl",  __LINE__);
    }
}

// ------------------------------------------------------------------------------------------------------------
void sem_v(int semid, int num)
{
    sops.sem_op = 1;
    sops.sem_num = num;
    if( semop (semid, &sops , 1) == -1)
        syserr("semaphore", "V");
}

// ------------------------------------------------------------------------------------------------------------
void sem_p(int semid, int num)
{
    sops.sem_op = -1;
    sops.sem_num = num;
    if( semop (semid, &sops , 1) == -1){
        syserr("semaphore", "P");
    }
}

// ------------------------------------------------------------------------------------------------------------
void delete_sem(int semid){
    if (semctl(semid, 0, IPC_RMID, NULL) == -1)
        syserr_ext ("utils.c" , "semctl",  __LINE__);
}

// ============================================================================================================
//                                               FUNCTIONS FOR - SHARED MEMORY
// ============================================================================================================
// Usage: int * xi = ( int *) xmalloc ( ftok ( argv [0] , ’a ’) , sizeof ( int ) * 8);
void *xmalloc( key_t key , const size_t size ) {
    const int shmid = shmget ( key , size + sizeof ( XMem ) - sizeof ( char ) , 0666| IPC_CREAT );
    if ( shmid == -1)
        syserr ("xmalloc", "shmget");
    
    /* When a segment of shared memory has been create it must be linket to data area of the the processes that want to use it. */
    XMem *ret = ( XMem *) shmat( shmid , NULL , 0);
    if ( ret == ( void *) -1)
        syserr ("xmalloc", "shmat");
    
    ret->key = key ;
    ret->shmid = shmid ;
    return ret-> buf ;
}

// ------------------------------------------------------------------------------------------------------------
// Usage: xfree( my_pointer );
void xfree ( void * ptr ) {
    XMem tmp ;
    XMem * mem = ( XMem *)((( char *) ptr ) - ((( char *)&tmp.buf ) - (( char *)& tmp.key )));
    const int shmid = mem-> shmid ;
    shmdt ( mem );
    shmctl ( shmid , IPC_RMID , NULL );
}

// ============================================================================================================
//                                               FUNCTIONS FOR - OTHER
// ============================================================================================================
float process_operation(int val1, int val2, char op){
    switch (op) {
        case '+':
            return (float)val1 + val2;
            break;
        case '-':
            return (float)val1 - val2;
            break;
        case '*':
            return (float)val1 * val2;
            break;
        case '/':
            return (float)val1 / val2;
            break;
        default:
            syserr("process operation() ", "not valid operator");
            break;
    }
    return -1;
}





