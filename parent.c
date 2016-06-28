/**
 * @file parent.h
 * @author Simone Girardi
 * @date 27 jun 2016.
 * @version 1.0
 */

#include "parent.h"
#include "mylib.h"
#include "utils.h"

#define CALLER "parent.c"
#define PARENT "[\033[38;5;208mParent\033[m]"

/* sem_parent inizialized to {1, 0, 0}:
 * 0: mutex
 * 1: result_ready
 * 2: data_read */

char str_info[100];

float *my_parent(int my_semaphores[], int n_operations, int NPROC, int *childs_started, struct operation *operations, struct operation *current_operation, struct result *current_result, bool child_isFree[]){
    
    float *results = (float*) malloc(sizeof(float) * n_operations);
    
    int sem_computing = my_semaphores[0];
    int sem_wait_data = my_semaphores[1];
    int sem_request_result = my_semaphores[2];
    int sem_parent = my_semaphores[3];
    
    print(""PARENT" waiting that all childs are ready...\n", CALLER, __LINE__);
    sem_p(sem_parent, 1);
    sleep(NPROC);
    
    int i;
    int op_id; // operation number "op_id"
    for(op_id = 0; op_id < n_operations; op_id++)
    {
        
        // preleva l'id del figlio da liberare

        i = operations[op_id].id;
        print_parent_info(""PARENT" next operation is for child %d\n", i);
        
        /* -1 corresponds to 0 from the specifics */
        if(i == -1){
            print(""PARENT" searching for the first free child..\n",CALLER, __LINE__);
            i = get_first_free_child(NPROC, child_isFree);
        }
        
        
        /* only if child is busy */
        if(!child_isFree[i])
        {
            // attende che abbia finito il calcolo
            print_parent_info(""PARENT" waiting that child %d finish to process the operation\n", i);
            
            sem_p(sem_computing, i);
            
            // richiede eventuali dati precedenti
            print(""PARENT" request the result...\n",CALLER, __LINE__);
            sleep(1);
            
            sem_v(sem_request_result, i);
            
            //aspetta che i dati siano pronti da leggere
            print_parent_info(""PARENT" waiting for result from child %d ...\n", i);
            
            sem_p(sem_parent, 1);
            
            
            results[current_result->id] = current_result->val;
        }
        
        // inserisce i dati della prossima operazione
        print(""PARENT" insert data for the next operation\n",CALLER, __LINE__);
        
        current_operation->id = op_id;
        current_operation->val1 = operations[op_id].val1;
        current_operation->val2 = operations[op_id].val2;
        current_operation->operator = operations[op_id].operator;
        
        // libera il figlio bloccato
        print_parent_info(""PARENT" unlock child %d\n", i);
        
        sem_v(sem_wait_data, i);
        
        // aspetta che il figlio li abbia letti
        print_parent_info(""PARENT" waiting that child %d finish to read data..\n", i);
        
        sem_p(sem_parent, 2);
    }
    
    print(""PARENT" finish to send operations\n",CALLER, __LINE__);
    
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
            
            /* save the result */
            results[current_result->id] = current_result->val;
        }
        
        //termina processo
        
        current_operation->operator = 'k';
        
        // libera il figlio bloccato
        
        sem_v(sem_wait_data, i);
        
        // aspetta che il figlio abbia letto
        
        sem_p(sem_parent, 2);
    }
    
    print_results(results , n_operations);
    
    return results;
    
}

//----------------------------------------------------------------------------------------------------------------
int get_first_free_child(int NPROC, bool child_isFree[])
{
    int i;
    while (1) {
        for(i = 0; i < NPROC; i++)
        {
            if(child_isFree[i]){
                print_parent_info(""PARENT" the first free child have id %d\n", i+1);
                return i;
            }
        }
        sleep(1);
        /*  try again */
    }
}

//----------------------------------------------------------------------------------------------------------------
void print_parent_info(const char *info, int child_id){
    if(snprintf(str_info, 100, info, child_id+1) == -1){
        syserr("parent()", "snprintf() error oversized string");
    }
    print(str_info, CALLER, __LINE__);
}

//----------------------------------------------------------------------------------------------------------------
void print_results(float results[] , int n_operations){
    print("-------------------------------------\n "PARENT" write results to STDOUT\n-------------------------------------\n",CALLER, __LINE__);
    char res[20];
    int i;
    
    for(i = 0; i < n_operations; i++)
    {
        sprintf(res, "%.2f\n", results[i]);
        print(res, CALLER, __LINE__);
        
    }
    print("-------------------------------------\n", CALLER, __LINE__);
}












