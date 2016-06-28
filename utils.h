
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
    int val;                 /* used for SETVAL only */
    struct semid_ds *buf;    /* used for IPC_STAT and IPC_SET */
    unsigned short *array;   /* used for GETALL and SETALL */
    struct seminfo*  __buf;  /* buffer for IPC_INFO (only linux) */
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
    key_t key ;
    int shmid ;
    char buf[1];
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
 * For each operation to execute is necessary create a sembuf struct. This struct contains
 * three fields: the number of semaphore to use, an integer N who describe the operation to
 * execute, an eventually flag for non block semaphore etc.
 *
 * @see sem_v
 * @see sem_p
 */
struct sembuf sops;

/**
 * @brief This function provide to get a descriptor for a semaphore
 *
 * This function creates some semaphore with a specific key 
 *
 * @param key a key to identify uniquely the semaphore
 * @param NPROC the number of processes
 * @return semid the id of new semaphore
 */
int do_semget(key_t key, int NPROC);

/**
 * @brief
 *
 *
 *
 * @param
 * @param
 * @return
 */
void initialize_sem(int semid, union semun *semarg, unsigned short values[]);

/**
 * @brief
 *
 *
 *
 * @param
 * @param
 * @return
 */
void sem_v(int semid, int num);

/**
 * @brief
 *
 *
 *
 * @param
 * @param
 * @return
 */
void sem_p(int semid, int num);

/**
 * @brief
 *
 *
 *
 * @param
 * @param
 * @return
 */
void delete_sem(int semid);

/**
 * @brief This function is similar to malloc() function, but it works for shared memory
 *
 * @code
 * int *xi = (int*) xmalloc(ftok(argv[0], ’a ’), sizeof(int) * 8);
 * @endcode
 * 
 * @param
 * @param
 * @see XMem
 * @return ret-> buf a pointer to char buf[1] in struct XMEM
 */
void *xmalloc( key_t key , const size_t size );

/**
 * @brief
 *
 * @code
 * xfree( my_pointer );
 * @endcode
 *
 * @param address
 * @see XMem
 * @return void
 */
void xfree(void * address);

/**
 * @brief
 *
 *
 *
 * @param
 * @param
 * @return
 */
void * xattach ( key_t key , const size_t size );

/**
 * @brief
 *
 *
 *
 * @param
 * @param
 * @return
 */
float process_operation(int val1, int val2, char op);

#endif /* utils_h */






