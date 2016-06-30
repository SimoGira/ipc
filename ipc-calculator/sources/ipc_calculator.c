
/**
 * @file ipc_calculator.c
 * @author Simone Girardi
 * @date 27 jun 2016.
 * @version 1.0
 */

#include "../headers/mylib.h"
#include "../headers/utils.h"
#include "../headers/parent.h"
#include "../headers/child.h"

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#define GREEN   "\033[32m"
#define RESET   "\e[0m"
#define ORANGE  "\033[38;5;208m"
#define PARENT "["ORANGE"Parent"RESET"]"
#define CALLER "ipc_calculator.c"

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

/**
 * @brief the Main of the program
 *
 * @param argc the number of arguments
 * @param argv the vector of parameter
 * @return 0
 */
int main(int argc, char *argv[]){
    
    struct operation *operations;
    struct operation *current_operation;
    struct result* current_result;
    
    bool *child_isFree;
    int id_number = -1;
    int n_operations = -1;
    int NPROC = 0;
    int *childs_ready;
    
    const int SHM_COP = 101;
    const int SHM_RES = 102;
    const int SHM_STATUS = 103;
    const int SHM_STARTED = 104;
    
    print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
          "\e[7;36m            IPC CALCULATOR             \e[0m\n"
          "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n", CALLER, __LINE__);
    
    // ==========================================================================================================
    //                                                SETUP - FROM FILE
    // ==========================================================================================================
    int fd;
    int i = 0;
    char line[50];
    int line_count;
    struct list* first_element = NULL;
    struct list* last_element = NULL;
    
    if((fd = open("config.txt",O_RDONLY|O_SYNC, S_IRUSR)) == -1){
        syserr (argv[0], "open() failure");
    }
    
    /* read the content of configuration file */
    line_count = read_from_file(fd, line, &first_element, &last_element);
    
    close(fd);
    
    if(line_count == 0 || first_element == NULL){
        syserr (argv[0], "file is empty or wrong formatted!");
    }
    
    // check for the number of processes to create
    NPROC = atoi(first_element->value);
    if(NPROC > 500 || NPROC < 0){
        syserr (argv[0], "NPROC is wrong!");
    }
    
    print("The the content of file is the follow:\n", CALLER, __LINE__);
    /* print the content of file */
    list_print(first_element);
    
    // the first line isn't an operation
    n_operations = line_count - 1;
    
    // allocate memory for operations
    operations = (struct operation*)malloc(sizeof(struct operation)*n_operations);
    
    // take first element of this list as the first operation
    struct list* list = first_element->next;
    
    // fill the list with operations (atoi() for numbers)
    fill_list_operations(list, operations);
    
    // free the lines memory
    list_free(first_element);
    first_element = NULL;
    
    // ==========================================================================================================
    //                                                SETUP - SEMAPHORE
    // ==========================================================================================================
    union semun arg; // union to pass to semctl()
    
    unsigned short *sem_init = (unsigned short*) malloc(sizeof(unsigned short) * NPROC);
    for (i = 0; i < NPROC; i++) {
        sem_init[i] = 0;
    }
    
    /* CREATE semaphores for childs */
    sem_computing = do_semget(ftok(argv[0], 'a'), NPROC);
    sem_wait_data = do_semget(ftok(argv[0], 'b'), NPROC);
    sem_request_result = do_semget(ftok(argv[0], 'c'), NPROC);
    
    /* SETALL semphores to zero */
    initialize_sem(sem_computing, &arg, sem_init);
    initialize_sem(sem_wait_data, &arg, sem_init);
    initialize_sem(sem_request_result, &arg, sem_init);
    
    
    /* CREATE semaphore for parent */
    sem_parent = do_semget(ftok(argv[0], 'd') , 3);
    
    /* SETALL semaphore to follow values:
     * 0: mutex
     * 1: result_ready
     * 2: data_read */
    sem_init = (unsigned short*) realloc(sem_init, sizeof(unsigned short) * 3);
    *(sem_init) = 1;
    *(sem_init + 1) = 0;
    *(sem_init + 2) = 0;
    
    initialize_sem(sem_parent, &arg, sem_init);
    
    free(sem_init);
    
    int my_semaphores[] = { sem_computing, sem_wait_data, sem_request_result, sem_parent };
    
    // ==========================================================================================================
    //                                                SETUP - SHARED MEMORY
    // ==========================================================================================================
    current_operation = (struct operation*) xmalloc(SHM_COP, sizeof(struct operation));
    current_result = (struct result*) xmalloc(SHM_RES, sizeof(struct result));
    child_isFree = (bool*) xmalloc(SHM_STATUS, sizeof (bool*) * NPROC);
    childs_ready = (int*) xmalloc(SHM_STARTED, sizeof(int));
    
    /* initialize all the children as free */
    for(i = 0; i < NPROC; i++)
        child_isFree[i] = true;
    
    /* initialize the number of childs_ready */
    *childs_ready = 0;
    
    
    // ==========================================================================================================
    //                                                 START - FORK()
    // ==========================================================================================================
    print("---------------------------------------\n"
          "           START CALCULATIONS          \n"
          "---------------------------------------\n", CALLER, __LINE__);
    pid_t pid;
    char str_info[100];
    
    for (i = 0; i < NPROC; i++)
    {
        pid = fork();
        if (pid < 0) {
            syserr("fork()", "cannot create a new process");
        } else if (pid == 0) {      // code execute from child
            id_number = i;        // assign id number to child process
            child(id_number, NPROC, my_semaphores, childs_ready, current_operation, current_result, child_isFree);
            break;
        } else {
            if(snprintf(str_info, 100, ""PARENT" create child %d with pid = %d\n" , i+1, pid) == -1){
                syserr(argv[0], "snprintf() error oversized string");
            }
            print(str_info, CALLER, __LINE__);
        }
    }
    
    /* execute from parent */
    if(pid != 0)
    {
        float *results = my_parent(my_semaphores, n_operations, NPROC, operations, current_operation, current_result, child_isFree);
        
        /* free shared memory */
        xfree(childs_ready);
        xfree(current_operation);
        xfree(current_result);
        xfree(child_isFree);
        
        /* delete all semaphores */
        delete_sem(sem_computing);
        delete_sem(sem_wait_data);
        delete_sem(sem_request_result);
        delete_sem(sem_parent);
        
        // open/create the file for results
        fd = open("results.txt", O_CREAT|O_RDWR|O_TRUNC, S_IRUSR|S_IWUSR );
        if (fd < 0) {
            syserr ("results.txt", "open() failure");
        }
        
        
        // write the results of operations on a file
        char res[20];
        int ret_val;
        for(i = 0; i < n_operations; i++)
        {
            sprintf(res, "%.2f\n", results[i]);
            
            if ((ret_val = (int) write(fd, res, strlen(res))) == -1)
                syserr ("results", "write() on file failure");
        }
        
        close(fd);
        
        print("The file \"results.txt\" is written in the sources directory\n", CALLER, __LINE__);
    }
    print("Program "GREEN"complete"RESET".\n", CALLER, __LINE__);
    return 0;
}