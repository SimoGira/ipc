/**
 * @file parent.c
 * @author Simone Girardi
 * @date 27 jun 2016.
 * @version 1.0
 */

#include "../headers/parent.h"
#include "../headers/mylib.h"
#include "../headers/utils.h"

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#define RESET   "\e[0m"
#define ORANGE  "\033[38;5;208m"
#define PARENT "["ORANGE"Parent"RESET"]"
#define CALLER "parent.c"

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

/* sem_parent inizialized to {1, 0, 0}:
 * 0: mutex
 * 1: result_ready
 * 2: data_read */

/** A temporary string used from "print functions" to build a fromatted the string */
char str_info[100];

float *my_parent(int my_semaphores[], int n_operations, int NPROC, struct operation *operations, struct operation *current_operation, struct result *current_result, bool child_isFree[]){
    
    float *results = (float*) malloc(sizeof(float) * n_operations);
    
    sem_computing = my_semaphores[0];
    sem_wait_data = my_semaphores[1];
    sem_request_result = my_semaphores[2];
    sem_parent = my_semaphores[3];
    
    
    print(""PARENT" waiting that all childs are ready...\n", CALLER, __LINE__);
    sem_p(sem_parent, 1);
    
    int i;
    int op_id; // the operation number "op_id"
    for(op_id = 0; op_id < n_operations; op_id++)
    {
        
        // preleva l'id del figlio da liberare
        
        i = operations[op_id].id;
        print_parent_info(""PARENT" next operation is for child %d\n", i);
        
        /* -1 corresponds to 0 of the specifics of the project */
        if(i == -1)
        {
            print(""PARENT" searching for the first free child..\n",CALLER, __LINE__);
            i = get_first_free_child(NPROC, child_isFree);
            print_parent_info(""PARENT" the first free child have id %d\n", i);
        }
        
        /* only if child is busy */
        if(!child_isFree[i]) {
            
            /* call the busy child routine and save the returned result */
            results[current_result->id] = busy_child_routine(i, current_result);
        }
        
        // inserisce i dati della prossima operazione
        
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
    
    print(""PARENT" finish to send all operations\n",CALLER, __LINE__);
    
    for(i = 0; i < NPROC; i++)
    {
        /* only if child is busy */
        if(!child_isFree[i]) {
            
            /*  call the busy child routine and save the returned result */
            results[current_result->id] = busy_child_routine(i, current_result);
        }
        
        //termina processo
        print_parent_info(""PARENT" killing child %d...\n", i);
        
        current_operation->operator = 'k';
        
        // libera il figlio bloccato
        
        sem_v(sem_wait_data, i);
        
        // aspetta che il figlio abbia letto
        
        sem_p(sem_parent, 2);
        
        //wait(NULL);  < ================================================ DA PROVARE !!!!!!
    }
    
    print_results(results , n_operations);
    
    return results;
}

//----------------------------------------------------------------------------------------------------------------
int get_first_free_child(int NPROC, bool child_isFree[])
{
    int child_id;
    for(child_id = 0; child_id < NPROC; child_id++) {
        
        if(child_isFree[child_id]){
            return child_id;
        }
    }
    /* if all childs are busy we assume that the fisrt free child will have id "1" */
    return 0;
}


//----------------------------------------------------------------------------------------------------------------
float busy_child_routine(int child_id, struct result *current_result){
    
    // attende che abbia finito il calcolo
    print_parent_info(""PARENT" waiting that child %d finish to process the operation\n", child_id);
    
    sem_p(sem_computing, child_id);
    
    // richiede eventuali dati precedenti
    print_parent_info(""PARENT" request the result to child %d\n", child_id);
    
    sem_v(sem_request_result, child_id);
    
    //aspetta che i dati siano pronti da leggere
    print_parent_info(""PARENT" waiting for result from child %d ...\n", child_id);
    
    sem_p(sem_parent, 1);
    
    print_parent_info(""PARENT" saving the result of opration processed from child %d...\n", child_id);
    return current_result->val;
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
    print("-------------------------------------\n "PARENT" write results on STDOUT\n-------------------------------------\n",CALLER, __LINE__);
    char res[20];
    int i;
    
    for(i = 0; i < n_operations; i++)
    {
        sprintf(res, "%.2f\n", results[i]);
        print(res, CALLER, __LINE__);
        
    }
    print("-------------------------------------\n", CALLER, __LINE__);
}












