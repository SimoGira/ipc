/**
 * @file child.c
 * @author Simone Girardi
 * @date 27 jun 2016.
 * @version 1.0
 */

#include "../headers/child.h"
#include "../headers/mylib.h"
#include "../headers/utils.h"
#include <sys/msg.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define CALLER "child.c"
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

/** A temporary string used from "print functions" to build a fromatted the string */
char str_info[100];

void child(int id_number, int NPROC, int msqid){
    
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
    // type 1 is simple message
    struct custom_msgbuf1 {
        long mtype;  /* must be positive */
        int id;
    };
    
    // type 2 is operations
    struct custom_msgbuf2 {
        long mtype;  /* must be positive */
        int val1;
        int val2;
        int op_id;
        char op;
    };
    
    // type 3 is results
    struct custom_msgbuf3 {
        long mtype;  /* must be positive */
        int id;
        float res;
    };
    
    struct custom_msgbuf1 cmb1;
    struct custom_msgbuf2 cmb2;
    struct custom_msgbuf3 cmb3;
    

    
    cmb1.mtype = 1;
    cmb1.id = id_number;
    
                                                       //#
    //####################################################
    //####################################################
                                                       //#
    
    msgsnd(msqid, &cmb1, sizeof(struct custom_msgbuf1)-sizeof(long), 0);
    
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
        
        msgrcv(msqid, &cmb2, sizeof(struct custom_msgbuf2)-sizeof(long), 2, 0);
        
        val1 = cmb2.val1;
        val2 = cmb2.val2;
        op = cmb2.op;
        op_id = cmb2.op_id;
        

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

        cmb3.mtype = 3;
        cmb3.id = op_id;
        cmb3.res = res;
        msgsnd(msqid, &cmb3, sizeof(struct custom_msgbuf3)-sizeof(long), 0);
        
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
