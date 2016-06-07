//
//  ipc_calculator.c
//  IPC project
//
//  Created by Simone Girardi on 13/05/16.
//  Copyright © 2016 Simone Girardi. All rights reserved.
//

#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// memoria condivisa
# include <sys/ipc.h>
# include <sys/shm.h>

 
// semafori 
#include <sys/types.h>
#include <sys/sem.h> 

#include "mylib.h"
#define STDIN 0
#define STDOUT 1
#define MYDEBUG printf ("This is line %d.\n", __LINE__);


#ifndef __APPLE__
union semun
{
	int val;                // value for SETVAL
	struct semid_ds* buf;   // buffer for IPC_STAT, IPC_SET
	unsigned short*  array; // array for GETALL, SETALL
	struct seminfo*  __buf; // buffer for IPC_INFO
}; 
#endif

struct operation{
	int id;
    int val1;
    int val2;
    char operator; 
};

struct operation* operations;
int *childs_pid; 
int id_number = 0;
int n_operations = -1;
int NPROC = 0;

void parent();
void child();


const int SMD_OP = 101;
const int SMD_RES = 102;
const int SMD_STATUS = 103;
struct operation* current_operation;
int * current_result; 
int * childs_pid; 
bool * childs_status;

struct sembuf sops;

void SemV(int semid, int num)
{
	sops.sem_op = 1;
	sops.sem_flg = SEM_UNDO;
	sops.sem_num = num;
	semop (semid, &sops , 1);
}

void SemP(int semid, int num)
{ 
	sops.sem_op = -1;
	sops.sem_flg = SEM_UNDO;
	sops.sem_num = num;
	semop (semid, &sops , 1);
}

int main(int argc, char *argv[]){
    
    char *title = "~~~~~~~~~~~~~~~~~~~\n\e[7;36m  IPC CALCULATOR   \e[0m\n~~~~~~~~~~~~~~~~~~~\n";
    int count = (int) write(STDOUT, title, strlen(title));
    if (count == -1)
        syserr (argv[0], "write() failure"); 
        
    int line_count = 0;
    
    // open file
    int fd = open("configuration.txt",O_RDONLY|O_SYNC, S_IRUSR);
    if (fd < 0) {
        syserr (argv[0], "open() failure");
    }
    
    
    char line[50];
    int i = 0;
    struct list* first_element = NULL;
    struct list* last_element = NULL;
    
    while ((count = (int) read(fd, &line[i], 1)) > 0) {             // read byte to byte
        if(line[i] == '\n'){
            line[i] = '\0';
            
            line_count++;
            
            char * str_temp = (char*) malloc(sizeof(char)*i);
            strcpy(str_temp,  line);                                // save every line in to line
            
            if (first_element == NULL){
                first_element = list_create(str_temp);
                last_element = first_element;
            }
            else{
            	last_element =  list_add(str_temp, last_element);
            }
            
            i = 0;
        }
        else
            i++;
    }
    if (count == -1)
        syserr (argv[0], "write() failure");
    close(fd);
    
    if(first_element == NULL){
         syserr (argv[0], "file is empty!");
    }
    
    n_operations = line_count - 1;  // becouse the first line isn't a operation
    printf("n_operations %i\n", n_operations);
    
    NPROC = atoi(first_element->value);         // number of process to create
    
    operations = (struct operation*)malloc(sizeof(struct operation)*n_operations);
    
    
	// take first element of this list as the first operation
    struct list* list = first_element->next;
    i = 0; 
    while (list != NULL)
    {
    
        char* id = strtok(list->value, " "); 
        char* val1 = strtok(NULL, " "); 
        char* op = strtok(NULL, " "); 
        char* val2  = strtok(NULL, " ");
        
        if(val1 == NULL || op == NULL || val2 == NULL)
     		syserr (argv[0], "Wrong operation format");
     		
       	//printf("process %s: \n	operation: %s %s %s\n",id, val1, op, val2);
        
        operations[i].id = atoi(id);
        operations[i].val1 = atoi(val1);
        operations[i].val2 = atoi(val2);
        operations[i].operator = op[0];
        i++;
         
        list = list->next; 
    } 
     
    // free the lines memory
    list_free(first_element);
    first_element = NULL;
         
    childs_pid = (int*) malloc(sizeof (int*) * NPROC);       // allocate memory for childs 
  
    // semafori per i figli
    
    int sem_computing;
    if (( sem_computing = semget (ftok(argv[0], 'a') , NPROC, IPC_CREAT | IPC_EXCL | 0666)) == -1) {
		syserr_ext (argv[0], " semget " , __LINE__);
	}
	
    int sem_wait_data;
    if (( sem_wait_data = semget (ftok(argv[0], 'b') , NPROC, IPC_CREAT | IPC_EXCL | 0666)) == -1) {
		syserr_ext (argv[0], " semget " , __LINE__);
	}
	
    int sem_request_result;
    if (( sem_request_result = semget (ftok(argv[0], 'c') , NPROC, IPC_CREAT | IPC_EXCL | 0666)) == -1) {
		syserr_ext (argv[0], " semget " , __LINE__);
	}
	
	//semafori per il padre
	// 0: mutex   1: result_ready
	int sem_parent;
	if (( sem_parent = semget (ftok(argv[0], 'd') , 2, IPC_CREAT | IPC_EXCL | 0666)) == -1) {
		syserr_ext (argv[0], " semget " , __LINE__);
	}
	 
    unsigned short sem_init[2] = {1, 0};
    unsigned short sem_out_test[2];
    
    union semun arg; 
	arg.array = sem_init;
	semctl(sem_parent, 2, SETALL, arg);
 
 	// TEST
	arg.array = sem_out_test;
	semctl(sem_parent, 2, GETALL, arg); 
	printf("Semaphore init values: %d %d\n",sem_out_test[0], sem_out_test[1]);


	// memoria condivisa
	current_operation = (struct operation*) xmalloc(SMD_OP, sizeof(struct operation));	
	current_result = (int*) xmalloc(SMD_RES, sizeof(int));	 
    childs_status = (bool*) xmalloc(SMD_STATUS, sizeof (bool*) * NPROC);
    
    //liberi
    for(int i = 0; i < NPROC; i++)
    	childs_status[i] = true;
    
    pid_t pid; 
    for (int i = 0; i < NPROC; i++)
    {
        pid = fork();
        if (pid < 0) {
            syserr("fork()", "cannot create a new process");
        } else if (pid == 0) {      // code execute from child
            id_number = i+1;        // assign id number to process (utile per identificare poi il processo a cui far eseguire l'operazione)
            child();
            break;
        } else {        
            childs_pid[i] = pid;
            //printf("child %i:\n	pid: %d\n", id_number, pid);
        }
    }

    if(pid != 0)
	{
        parent(); 
        
        // cancella semafori
		if (semctl(sem_computing, 0, IPC_RMID, NULL) == -1) 
			syserr(argv[0], "Error deleting sem_computing!"); 
		if (semctl(sem_wait_data, 0, IPC_RMID, NULL) == -1) 
			syserr(argv[0], "Error deleting sem_wait_data!"); 
		if (semctl(sem_request_result, 0, IPC_RMID, NULL) == -1) 
			syserr(argv[0], "Error deleting sem_request_result!"); 
		if (semctl(sem_parent, 0, IPC_RMID, NULL) == -1) 
			syserr(argv[0], "Error deleting sem_parent!"); 
    }
     
    return 0;
}

void parent()
{
	printf("> padre called\n"); 
    
    sleep(1);
    
    xfree(current_operation);
    xfree(current_result);
    xfree(childs_status);
    free(childs_pid); 
	printf("padre terminato\n");
}

void child()
{  
	
	
	printf("figlio %i terminato\n", id_number);
}





