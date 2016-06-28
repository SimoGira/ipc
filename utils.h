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

struct sembuf sops;
int do_semget(key_t key, int NPROC);
void initialize_sem(int semid, union semun *semarg, unsigned short values[]);
void sem_v(int semid, int num);
void sem_p(int semid, int num);
void delete_sem(int semid);

#endif /* utils_h */
