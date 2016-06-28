//
//  child.c
//  
//
//  Created by Simone Girardi on 27/06/16.
//
//

#include "child.h"
#include "mylib.h"
#include "utils.h"
#define CALLER "child.c"

char str_info[100];

void child(int id_number, int NPROC, int my_semaphores[], int *childs_started, struct operation *current_operation, struct result *current_result, bool child_isFree[]){
    
    int sem_computing = my_semaphores[0];
    int sem_wait_data = my_semaphores[1];
    int sem_request_result = my_semaphores[2];
    int sem_parent = my_semaphores[3];
    
    float res; // result of operation
    
    /* sem_parent inizialized to {1, 0, 0}:
     * 0: mutex
     * 1: result_ready
     * 2: data_read */
    
    sem_p(sem_parent, 0);
    (*childs_started)++;
    
    /* the last child will unlock the parent */
    if(*childs_started == NPROC)
        sem_v(sem_parent, 1);
    
    sem_v(sem_parent, 0);
    
    sleep(NPROC+1);
    print_child_info("[Child %d] ready!\n", id_number);
    
    while(true)
    {
        child_isFree[id_number] = true;
        
        // si mette in attesa di essere chiamato per il calcolo
        
        sem_p(sem_wait_data, id_number);
        
        
        child_isFree[id_number] = false;
        
        
        // legge i dati
        
        print_child_info("[Child %d] unlocked from parent, reading data...\n", id_number);
    
        int val1 = current_operation->val1;
        int val2 = current_operation->val2;
        char op = current_operation->operator;
        int op_id = current_operation->id;
        
        // avvisa che ho finito di leggere
        
        print_operation_info("[Child %d] read: %d %c %d\n", id_number,val1,op,val2);
        
        sem_v(sem_parent, 2);
        
        
        // termina col comando k
        if(op == 'k'){
            print_child_info("[Child %d] exiting...\n", id_number);
            exit(0);
        }
        
        
        // calcola
        sleep(1);
        print_child_info("[Child %d] processing operation...\n", id_number);
        
        res = process_operation(val1, val2, op);
        
        // avvisa di aver terminato il calcolo
        sem_v(sem_computing, id_number); // calcolo terminato
        
        sleep(1);
        print_child_info("[Child %d] wait parent for request of result...\n", id_number);
        
        // attende che il padre richieda i dati
        sem_p(sem_request_result, id_number);
        
        print_child_info("[Child %d] write the result...\n", id_number);
        
        // scrivi risultato calcolo
        current_result->val = res;
        current_result->id = op_id;
        
        sleep(1);
        print_child_info("[Child %d] result ready to be read!\n", id_number);
        
        // dice al padre che i dati sono pronti per essere letti
        sem_v(sem_parent, 1);
    }
}

void print_child_info(const char *info, int id_number){
    if(snprintf(str_info, 100, info , id_number+1) == -1){
        syserr("child()", "snprintf() error oversized string");
    }
    print(str_info, CALLER, __LINE__);
}


void print_operation_info(const char *info, int id_number, int val1, int val2, char op){
    if(snprintf(str_info, 100, info , id_number+1, val1, val2, op) == -1){
        syserr("child()", "snprintf() error oversized string");
    }
    print(str_info, CALLER, __LINE__);
}
