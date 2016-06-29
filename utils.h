
/**
 * @file utils.h
 * @author Simone Girardi
 * @date 27 jun 2016.
 * @version 1.0
 */

#ifndef utils_h
#define utils_h

#include <stdbool.h>
#include <sys/sem.h>
#include <fcntl.h>  /* for open() etc */
//#include <signal.h> /* for siganls - pipe - fifo */

/**
 * \defgroup Variables Global semaphores
 */
/**@{*/

/** semaphore for childs/parent inizialized to {1, 0, 0}:
 * 0: mutex
 * 1: result_ready
 * 2: data_read
 */
int sem_parent;

/** semaphore for parent/childs:
 * the childs attend that parent resquest result and then notify to the parent that the result is written and
 * ready to be read */
int sem_request_result;

/** semaphore for parent/childs used from parent to wait that child pressed the operation and from child to notify 
 * the parent when the data ha been processed 
 */
int sem_computing;

/** semaphore for parent/child used from from child to notify the parent that he is ready to process a new 
  * operation and from parent to unlock the child */
int sem_wait_data;

/**@}*/



#ifndef __APPLE__
/** 
 * @brief This union is necessary to call semctl()
 *
 * The various fields in the union semun are used depending on the value of the cmd parameter to setctl() 
 * (e.g. SETVAL, GETVAL, SETALL, GETALL, IPC_RMID, IPC_STAT...see your local man for more)
 * In OS-X isn't need to explicit declaration but for others OS it is required
 *
 * @see initialize_sem
 */
union semun {
    int val;                 /**< used for SETVAL only */
    struct semid_ds *buf;    /**< used for IPC_STAT and IPC_SET */
    unsigned short *array;   /**< used for GETALL and SETALL */
    struct seminfo*  __buf;  /**< buffer for IPC_INFO (only linux) */
};
#endif

/**
 * @brief This structure is necessary to not lose the reference to shared memory, so we can alsways
 * retrieve the id of the shared memory area and deallocate it.
 *
 * @see xmalloc
 * @see xfree
 */
typedef struct XMem {
    key_t key ;         /**< the key to idetify univocally a segment of shared memory #key */
    int shmid ;         /**< the id of shared memory segment #shmid */
    char buf[1];        /**< a buffer necessary to refer a specific address of shared memory #buf */
} XMem;

/**
 * @brief This structure is used to contain the data for an operartion
 *
 * The parent and the childs use this struct for read/write the data necessary to perform an operation
 *
 * @see fill_list_operation
 * @see parent
 * @see child
 */
struct operation{
    int id;         /**< the number of the operation#id */
    int val1;       /**< the first operand of the operation#val1 */
    int val2;       /**< the second operand of the operation#val2 */
    char operator;  /**< the operator of the operation#operator */
};

/**
 * @brief This structure is used to contain the current result of a performed operation
 *
 * The parent and the childs use this struct for read/write the result of an operation
 *
 * @see parent
 * @see child
 */
struct result {
    int id;         /**< the number of the operation#id */
    float val;      /**< the value of result of the operation#val */
};

/**
 * @brief This declaration of sembuf structure is necessary to use the semop() function
 *
 * For each operation to execute is necessary create a sembuf struct.
 * This struct contains three fields: the number of semaphore to use, an integer N who describe the operation 
 * to execute, an eventually flag for non block semaphore etc.
 *
 * @see sem_v
 * @see sem_p
 */
struct sembuf sops;

/**
 * @brief This function provide to get a descriptor for a semaphore using semget() function
 *
 * This function creates some semaphore with a specific key, necessary to create/retrieve the system 
 * object
 *
 * @param key a key to identify univocally the semaphore
 * @param nsems the number of semaphore that will be contained in semophores vector
 * @return semid the id of new semaphore
 */
int do_semget(key_t key, int nsems);

/**
 * @brief This function initilize all semaphores using an array of values using semctl() function
 *
 * @param semid the id of semaphore (returned from a semget())
 * @param semarg the union semun to pass to semctl()
 * @param values the array of values to set semaphore
 * @see semun
 * @return void
 */
void initialize_sem(int semid, union semun *semarg, unsigned short values[]);

/**
 * @brief This function increment the value of a semaphore using semop() function
 *
 * @param semid the id of semaphore to increment
 * @param num the number of semaphore to increment (of vector refered from semid)
 * @see sops
 * @return void
 */
void sem_v(int semid, int num);

/**
 * @brief This function decrement the value of a semaphore using semop() function
 *
 * @param semid the id of semaphore to decrement
 * @param num the number of semaphore to decrement (of vector refered from semid)
 * @see sops
 * @return void
 */
void sem_p(int semid, int num);

/**
 * @brief This function deletes a semaphore using semctl() function
 *
 * @param semid the id of semaphore to delete
 * @return void
 */
void delete_sem(int semid);

/**
 * @brief This function is similar to malloc() function, but it works for shared memory follow see an example of usage:
 *
 * @code
 * int *ptr = (int*) xmalloc(ftok(argv[0], ’a ’), sizeof(int) * 8);
 * @endcode
 * 
 * @param key a key to identify univocally the segment allocated
 * @param size the size of segment to allocate
 * @see XMem
 * @return ret-> buf a pointer to char buf[1] in struct XMEM
 */
void *xmalloc( key_t key , const size_t size );

/**
 * @brief This function is similar to free() function, but it works for shared memory, follow an example of usage:
 *
 * @code
 * xfree(ptr);
 * @endcode
 *
 * @param address pointer to shared memory segment previous allocated
 * @see XMem
 * @return void
 */
void xfree(void * address);

/** @brief This function gets the result of a binary operation.
 *
 * @param val1 the fist operand 1
 * @param op the operator
 * @param val2 the second operand 2
 * @see child
 * @return result of operation.*/
float process_operation(int val1, int val2, char op);

#endif /* utils_h */






