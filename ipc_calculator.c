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


int sem_parent;
int sem_request_result;
int sem_computing;
int sem_wait_data;
    
struct operation{
	int id;
    int val1;
    int val2;
    char operator; 
};


struct result {
	int id;
	int val;
};

struct operation* operations;
int *childs_pid; 
bool * free_child;
int id_number = -1;
int n_operations = -1;
int NPROC = 0;
int* childs_started;

void parent();
void child();

int* results;
const int SMD_OP = 101;
const int SMD_RES = 102;
const int SMD_STATUS = 103;
const int SMD_STARTED = 104;

struct operation* current_operation;
struct result* current_result; 

struct sembuf sops;

void sem_v(int semid, int num)
{
	sops.sem_op = 1;
	//sops.sem_flg = SEM_UNDO;
	sops.sem_num = num;
	if( semop (semid, &sops , 1) == -1)
		syserr("semaforo", "V");		  
}

void sem_p(int semid, int num)
{
	sops.sem_op = -1;
	//sops.sem_flg = SEM_UNDO;
	sops.sem_num = num;
	if( semop (semid, &sops , 1) == -1)
		syserr("semaforo", "P");
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
    results = (int*) malloc(sizeof(int) * n_operations);
    
    
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
        
        operations[i].id = atoi(id) - 1;
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
  
  
 	// semafori
    union semun arg;
    arg.val = 0;
        
    
    // per i figli
    
    if (( sem_computing = semget (ftok(argv[0], 'a') , NPROC, IPC_CREAT | 0777)) == -1) 
		syserr_ext (argv[0], " semget " , __LINE__); 
	for(int i = 0; i < NPROC; i++)
    if (semctl(sem_computing, i, SETVAL, arg) == -1) 
		syserr_ext (argv[0], " semctl " , __LINE__); 
	
    if (( sem_wait_data = semget (ftok(argv[0], 'b') , NPROC, IPC_CREAT | 0777)) == -1) 
		syserr_ext (argv[0], " semget " , __LINE__);
	for(int i = 0; i < NPROC; i++)
	if (semctl(sem_wait_data, i, SETVAL, arg) == -1) 
		syserr_ext (argv[0], " semctl " , __LINE__); 
	
    if (( sem_request_result = semget (ftok(argv[0], 'c') , NPROC, IPC_CREAT  | 0777)) == -1)  
		syserr_ext (argv[0], " semget " , __LINE__);
	for(int i = 0; i < NPROC; i++)
	if (semctl(sem_request_result, i, SETVAL, arg) == -1) 
		syserr_ext (argv[0], " semctl " , __LINE__); 
	
	// per il padre
	// 0: mutex   1: result_ready  2: data_read
	if (( sem_parent = semget (ftok(argv[0], 'd') , 3, IPC_CREAT | 0777)) == -1) {
		syserr_ext (argv[0], " semget " , __LINE__);
	} 
    unsigned short sem_init[3] = {1, 0, 0};
	arg.array = sem_init; 
	semctl(sem_parent, 3, SETALL, arg);
  
	// memoria condivisa
	current_operation = (struct operation*) xmalloc(SMD_OP, sizeof(struct operation));	
	current_result = (struct result*) xmalloc(SMD_RES, sizeof(struct result)); 
    free_child = (bool*) xmalloc(SMD_STATUS, sizeof (bool*) * NPROC);	 
	childs_started = (int*) xmalloc(SMD_STARTED, sizeof(int));
	*childs_started = 0;	
     
    pid_t pid; 
    for (int i = 0; i < NPROC; i++)
    {
        pid = fork();
        if (pid < 0) {
            syserr("fork()", "cannot create a new process");
        } else if (pid == 0) {      // code execute from child
            id_number = i;        // assign id number to process (utile per identificare poi il processo a cui far eseguire l'operazione)
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

int get_first_free_child()
{ 
	for(int j = 0; j < NPROC; j++)
    {
    	if(free_child[j])
    		return j;
    }
    return 0;
}

void parent()
{
	printf("PARENT: attendi\n"); 
 	sem_p(sem_parent, 1);
	printf("PARENT: sbloccato\n"); 
 	
    for(int op_id = 0; op_id < n_operations; op_id++)
    {
    
		printf("PARENT: operazione %i: %i %c %i \n",operations[op_id].id,operations[op_id].val1,operations[op_id].operator, operations[op_id].val2);
		
		// preleva l'id del figlio da liberare
    	int i = operations[op_id].id;
    	if(i == -1) 
    		i = get_first_free_child();
    	
		//se il figlio è occupato
    	if(!free_child[i])
    	{
			// attende che abbia finito il calcolo
			printf("PARENT: attende (%i) che abbia finito il calcolo \n", i);
    		sem_p(sem_computing, i);
    		
			// richiede eventuali dati precedenti
			printf("PARENT: richiede eventuali dati precedenti a (%i) \n", i);
    		sem_v(sem_request_result, i);
    		
			//aspetta che i dati siano pronti da leggere
			printf("PARENT: aspetta che i dati di (%i) siano pronti da leggere \n", i);
    		sem_p(sem_parent, 1);
    		
			printf("results[%i] = %i \n", current_result->id, current_result->val);
    		results[current_result->id] = current_result->val;
    	}

		// inserisce i dati della prossima operazione
		printf("PARENT: inserisce i dati della prossima operazione \n"); 
		current_operation->id = op_id;
		current_operation->val1 = operations[op_id].val1;
		current_operation->val2 = operations[op_id].val2;
		current_operation->operator = operations[op_id].operator;
				
		// libera il figlio bloccato
		printf("PARENT: libera il figlio bloccato %i\n", i);
		sem_v(sem_wait_data, i);
		
		// aspetta che il figlio li abbia letti
		printf("PARENT: spetta che il figlio ( %i ) li abbia letti\n", i);
		sem_p(sem_parent, 2);
    }
        
     printf("============ LEGGO I DATI COMPUTATI ===============\n");
    
	for(int i = 0; i < NPROC; i++)
    {
    	
    	printf("PARENT: child %i is free?:  %s \n", i, free_child[i]?"true":"false");
		if(!free_child[i])
		{
			// attende che abbia finito il calcolo
			printf("PARENT: attende che abbia finito il calcolo %i  \n",i );
    		sem_p(sem_computing, i);
    		
			// richiede eventuali dati precedenti
			printf("PARENT: richiede eventuali dati precedenti %i  \n", i);
    		sem_v(sem_request_result, i);
    		
			//aspetta che i dati siano pronti da leggere
			printf("PARENT: aspetta che i dati siano pronti da leggere  %i \n", i);
    		sem_p(sem_parent, 1);
    		
			printf("results[%i] = %i \n", current_result->id, current_result->val);
    		results[current_result->id] = current_result->val;
		}
	
		//termina processo 
		printf("PARENT: termina processo figlio %i \n", i); 
		current_operation->operator = 'k';
				
		// libera il figlio bloccato
		printf("PARENT: libera il figlio bloccato %i \n", i);
		sem_v(sem_wait_data, i);
		
		// aspetta che il figlio abbia letto
		printf("PARENT:aspetta che il figlio abbia letto %i \n", i);
		sem_p(sem_parent, 2);
	}

	printf("PARENT: printing results \n");
	for(int i = 0; i < n_operations; i++)
    {
    	printf("result: %i\n", results[i]);
    }
    
    xfree(childs_started);
    xfree(current_operation);
    xfree(current_result);
    xfree(free_child);
    free(childs_pid); 
	printf("padre terminato\n");
}

void child()
{
    int res;
    
  	sem_p(sem_parent, 0); 
    (*childs_started)++;
    printf("%i / %i\n", *childs_started, NPROC);
    
    if(*childs_started == NPROC)
    	sem_v(sem_parent, 1);
	
    sem_v(sem_parent, 0);
    
    while(true)
    {
        free_child[id_number] = true;
        
        // si mette in attesa di essere chiamato per il calcolo
        printf("%i) si mette in attesa di essere chiamato per il calcolo\n", id_number);
        sem_p(sem_wait_data, id_number);
        
        
        free_child[id_number] = false;
        
        
        // legge i dati
        printf("%i) legge i dati\n", id_number);
        int val1 = current_operation->val1;
        int val2 = current_operation->val2;
        char op = current_operation->operator; 
        int op_id = current_operation->id;
        
        // avvisa che ho finito di leggere
        printf("%i) avvisa che ho finito di leggere %i %c %i\n", id_number, val1, op, val2);
        sem_v(sem_parent, 2);
        
        
        // termina col comando k
        if(op == 'k'){
        	printf("%i) termina\n", id_number);
            exit(0);
        }
        
        
        // calcola
        res = process_operation(val1, val2, op);
        
        // avvisa di aver terminato il calcolo
        printf("%i) avvisa di aver terminato il calcolo\n", id_number);
        sem_v(sem_computing, id_number); // calcolo terminato
        
        // attende che il padre richieda i dati
        printf("%i) attende che il padre richieda i dati\n", id_number);
        sem_p(sem_request_result, id_number);
        
        // scrivi risultato calcolo
        printf("%i) scrivi risultato calcolo\n", id_number);
        current_result->val = res;
        current_result->id = op_id;
        
        // dice al padre che i dati sono pronti per essere letti
        printf("%i) dice al padre che i dati sono pronti per essere letti\n", id_number);
        sem_v(sem_parent, 1);
    }
	printf("figlio %i terminato\n", id_number);
}



