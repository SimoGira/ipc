/**
 * @file parent.h
 * @author Simone Girardi
 * @date 27 jun 2016.
 * @version 1.0
 */

#ifndef parent_h
#define parent_h

#include "mylib.h"
#include "utils.h"

/**
 * @brief This function is called from the parent process after it created the childs with fork()
 *
 * Initially the parent wait (on semaphore: sem_parent 1) that all his childs are ready. So when all chids are ready, for each operations,
 * it read the id number of the child to which to perform the operation. If the is id number is "-1" the parent search for the first child
 * that is ready to perform the next operation. (1) Now if the slected child is ready to read the operation, the parent write the next operation 
 * to do for the current selected child into the struct current_operation (see current_operation in utils), unclok the selected child which is
 * waiting on sem_wait_data and wait that the selected child has read the data to perform the operation. Finally the parent can pass to
 * the next operation to perform. If, at point (1), the selected child isn't ready to read the operation, then the parent calls the 
 * busy_child_routine, and it continue as point (1)
 * When the parent finishes to send all operation to his childs, it wait that all operations are performed and then kill the current selected 
 * child
 *
 * @param my_semaphores array of semaphores
 * @param n_operations number of operations to perform
 * @param NPROC the number of processes
 * @param my_semaphores the array of semaphores
 * @param operations a strcut that contains all operations to perform
 * @param current_operation a struct pointer for get the current operation
 * @param current_result a struct pointer for save the result of current operation
 * @param child_isFree array for check which child process is free
 * @see busy_child_routine
 * @see child
 * @return results the array of results of operations performed
 */
float *my_parent(int n_operations, int NPROC, struct operation *operations, int queues[]);


/** 
 * @brief Called if the id number read from parent is "-1".
 *
 * The parent doesn't return from this function until it has not found a free child
 *
 * @param NPROC the number of processes
 * @param child_isFree array for check which child process is free
 * @see my_parent
 * @return child_id id of the first free child
 */
int get_first_free_child(int NPROC, bool child_isFree[]);

/**
 * @brief This function is a routing that is called from the parent when it wants receive the result of an operation from a specific child, or it 
 * wants terminate a specific child.
 *
 * If the selected child isn't ready to read the data for the new operation, then the parent wait the
 * selected child complete to compute the operation and then it request for the result. When the result is ready, the parent read it and save the
 * result in current_result struct.
 *
 * @param child_id
 * @param current_result
 * @see my_parent
 * @return current_result->val
 */
float busy_child_routine(int child_id, struct result *current_result);

/**
 * @brief This function is a routing that is called from the parent when it wants receive the result of an operation from a specific child, or it 
 * wants terminate a specific child.
 *
 * If the selected child isn't ready to read the data for the new operation, then the parent wait the
 * selected child complete to compute the operation and then it request for the result. When the result is ready, the parent read it and save the
 * result in current_result struct.
 *
 * @param child_id
 * @see my_parent
 * @return void
 */
void busy_child_routine_2(int child_id);

/**
 * @ This function is used to print on STDOUT the current status of parent
 * 
 * This function allows us to see how the parent manage his childs
 *
 * @param info the information to print on STDOUT
 * @param child_id the id of current managed child process
 * @see print
 * @return void
 */
void print_parent_info(const char *info, int child_id);


/**
 * @brief This function is used to print the results of operations on STDOUT
 *
 * When all operations have been completed and all childs have been terminated, the parent will print the results on STDOUT
 *
 * @param resutls the array of results
 * @param n_operations the length of array results
 * @see print
 * @return void
 */
void print_results(float resutls[], int n_operations);

#endif /* parent_h */
