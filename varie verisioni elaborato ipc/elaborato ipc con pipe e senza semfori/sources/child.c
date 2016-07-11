/**
 * @file child.c
 * @author Simone Girardi
 * @date 27 jun 2016.
 * @version 1.0
 */

#include "../headers/child.h"
#include "../headers/mylib.h"
#include "../headers/utils.h"

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define CALLER "child.c"
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

/** A temporary string used from "print functions" to build a fromatted the string */
char str_info[100];

void child(int id_number, int NPROC, int p1[], int p2[]){
    
    //sem_computing = my_semaphores[0];
    //sem_wait_data = my_semaphores[1];
    //sem_request_result = my_semaphores[2];
    //sem_parent = my_semaphores[3];
    
    /* support variable to perform the operation */
    float res;
    int val1, val2, op_id;
    char op;
    
    /* sem_parent inizialized to {1, 0, 0}:
     * 0: mutex
     * 1: result_ready
     * 2: data_read */
    
    //sem_p(sem_parent, 0);
    
    //####################################################
                                                       //#
    
    write(p2[1], &id_number, sizeof(id_number));
    
                                                       //#
    //####################################################
    
    /* the last child will unlock the parent */
    //if(childs_ready == NPROC)
    //    sem_v(sem_parent, 1);
    
    //sem_v(sem_parent, 0);
    
    print_child_info("[Child %d] ready!\n", id_number);


    while(true)
    {
        //child_isFree[id_number] = true;
        
        // si mette in attesa di essere chiamato per il calcolo
        
        //sem_p(sem_wait_data, id_number);
        
        /* make this child busy */
        
        //child_isFree[id_number] = false;
        

        /* read data sent from parent */
        print_child_info("[Child %d] unlocked from parent, reading data...\n", id_number);
        
        read(p1[0], &op_id, sizeof(op_id));
        read(p1[0], &val1, sizeof(val1));
        read(p1[0], &val2, sizeof(val2));
        read(p1[0], &op, sizeof(op));

        //val1 = current_operation->val1;
        //val2 = current_operation->val2;          /***************************/
        //op = current_operation->operator;        /***** CRITICAL SECTION ****/
        //op_id = current_operation->id;           /***************************/
        
        // avvisa che ho finito di leggere
        
        //sem_v(sem_parent, 2);
        
        // termina col comando k
        if(op == 'k'){
            print_child_info("[Child %d] read: 'k' exiting...\n", id_number);
            return;
        }
        
        print_operation_info("[Child %d] read: %d %c %d\n", id_number,val1,op,val2);
        
        // calcola
        print_child_info("[Child %d] processing operation...\n", id_number);
        
        res = process_operation(val1, val2, op);
        
        // avvisa di aver terminato il calcolo
        //sem_v(sem_computing, id_number); // calcolo terminato
        
        print_child_info("[Child %d] wait parent for request of result...\n", id_number);
        
        // attende che il padre richieda i dati
        //sem_p(sem_request_result, id_number);
        
        print_child_info("[Child %d] write the result...\n", id_number);

        
        //scrivi risultato calcolo            /****************************/
        //current_result->val = res;          /***** CRITICAL SECTION *****/
        //current_result->id = op_id;         /****************************/
        
        write(p2[1], &op_id, sizeof(op_id));
        write(p2[1], &res, sizeof(res));
        
        print_child_info("[Child %d] result ready to be read!\n", id_number);
        
        // dice al padre che i dati sono pronti per essere letti
        //sem_v(sem_parent, 1);
    }
}

//------------------------------------------------------------------------------------------------
void print_child_info(const char *info, int id_number){
    if(snprintf(str_info, 100, info , id_number+1) == -1){
        syserr("child()", "snprintf() error oversized string");
    }
    print(str_info, CALLER, __LINE__);
}

//------------------------------------------------------------------------------------------------
void print_operation_info(const char *info, int id_number, int val1, char op, int val2){
    if(snprintf(str_info, 100, info , id_number+1, val1, op, val2) == -1){
        syserr("child()", "snprintf() error oversized string");
    }
    print(str_info, CALLER, __LINE__);
}
