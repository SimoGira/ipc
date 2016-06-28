//
//  parent.c
//
//
//  Created by Simone Girardi on 27/06/16.
//
//

#include "parent.h"
#include "mylib.h"
#include "utils.h"
#include "child.h"

char str_info[50];

float *my_parent(int my_semaphores[], int n_operations, int NPROC, int *childs_started, struct operation *operations, struct operation *current_operation, struct result *current_result, bool child_isFree[]){
    
    float *results = (float*) malloc(sizeof(float) * n_operations);
    
    int sem_computing = my_semaphores[0];
    int sem_wait_data = my_semaphores[1];
    int sem_request_result = my_semaphores[2];
    int sem_parent = my_semaphores[3];
    
    print("[Parent] wainting that all childs are ready...\n", __LINE__);
    sem_p(sem_parent, 1);
    
    int op_id;
    for(op_id = 0; op_id < n_operations; op_id++)
    {
        
        // preleva l'id del figlio da liberare

        int i = operations[op_id].id;
        print_parent_info("[Parent] next operation is for child %d\n", i);
        
        if(i == -1)
            i = get_first_free_child(NPROC, child_isFree);
        
        
        //se il figlio è occupato
        if(!child_isFree[i])
        {
            // attende che abbia finito il calcolo
            print_parent_info("[Parent] waiting that child %d finish to process the operation\n", i);
            
            sem_p(sem_computing, i);
            
            // richiede eventuali dati precedenti
            print("[Parent] request the result...\n", __LINE__);
            sleep(1);
            
            sem_v(sem_request_result, i);
            
            //aspetta che i dati siano pronti da leggere
            print("[Parent] waiting for result...\n", __LINE__);
            
            sem_p(sem_parent, 1);
            
            
            results[current_result->id] = current_result->val;
        }
        
        // inserisce i dati della prossima operazione
        print("[Parent] insert data for the next operation\n", __LINE__);
        
        current_operation->id = op_id;
        current_operation->val1 = operations[op_id].val1;
        current_operation->val2 = operations[op_id].val2;
        current_operation->operator = operations[op_id].operator;
        
        // libera il figlio bloccato
        print_parent_info("[Parent] unlock child %d\n", i);
        
        sem_v(sem_wait_data, i);
        
        // aspetta che il figlio li abbia letti
        print_parent_info("[Parent] waiting that child %d finish to read data..\n", i);
        
        sem_p(sem_parent, 2);
    }
    
    print("[Parent] finish to send operations\n", __LINE__);
    
    int i;
    for(i = 0; i < NPROC; i++)
    {
        
        
        if(!child_isFree[i])
        {
            // attende che abbia finito il calcolo
            
            sem_p(sem_computing, i);
            
            // richiede eventuali dati precedenti
            
            sem_v(sem_request_result, i);
            
            //aspetta che i dati siano pronti da leggere
            
            sem_p(sem_parent, 1);
            
            
            results[current_result->id] = current_result->val;
        }
        
        //termina processo
        
        current_operation->operator = 'k';
        
        // libera il figlio bloccato
        
        sem_v(sem_wait_data, i);
        
        // aspetta che il figlio abbia letto
        
        sem_p(sem_parent, 2);
    }
    
    
    char res[20];
    for(i = 0; i < n_operations; i++)
    {
        sprintf(res, "%.2f\n", results[i]);
        print(res, __LINE__);
        
    }
    
    return results;
    
}

int get_first_free_child(int NPROC, bool child_isFree[])
{
    print("[Parent] searching for the first free child..\n", __LINE__);
    sleep(1);
    int i;
    for(i = 0; i < NPROC; i++)
    {
        if(child_isFree[i])
            print_parent_info("[Parent] the first free child have id %d\n", i);
            return i;
    }
    return 0;
}

void print_parent_info(const char *info, int child_id){
    if(snprintf(str_info, 50, info, child_id) == -1){
        syserr("parent()", "snprintf() error oversized string");
    }
    print(str_info, __LINE__);
}













