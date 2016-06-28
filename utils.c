//
//  utils.c
//
//
//  Created by Simone Girardi on 21/06/16.
//
//

#include "utils.h"
#include "mylib.h"
#include "parent.h"
#include "child.h"

// ============================================================================================================
//                                               FUNCTIONS FOR - SEMAPHORE
// ============================================================================================================
int do_semget(key_t key, int NPROC){
    int semid;
    if (( semid = semget ( key, NPROC, IPC_CREAT | 0777)) == -1)
        syserr_ext ("do_semget()", " semget " , __LINE__);
    return semid;
}

// ------------------------------------------------------------------------------------------------------------
void initialize_sem(int semid, union semun *semarg, unsigned short values[]){
    
    semarg->array = values;
    
    if ( semctl(semid, 0, SETALL, *semarg) == -1){
        syserr_ext ("initialize_sem()" , "semctl",  __LINE__);
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
        syserr("delete_sem()", "Error deleting a semaphore");
}

// ============================================================================================================
//                                               FUNCTIONS FOR - SHARED MEMORY
// ============================================================================================================








