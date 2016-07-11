
/**
 * @file child.h
 * @author Simone Girardi
 * @date 27 jun 2016.
 * @version 1.0
 */

#ifndef child_h
#define child_h

#include "mylib.h"
#include "utils.h"

/**
 * @brief Every child call this function and it exec the operation gived from parent
 *
 * @param id_number the id of child process
 * @param NPROC the number of processes
 * @param my_semaphores the array of semaphores
 * @param childs_ready a strcut pointer for manage started processes
 * @param current_operation a struct pointer for get the current operation
 * @param current_result a struct pointer for save the result of current operation
 * @param child_isFree array for check which child process is free
 * @return void
 */
void child(int id_number, int NPROC, int msqid);

/** 
 * @brief This function is used to print the status of the current child process in execution
 * @param info the information to print on STDOUT
 * @param id_number the id of child process
 * @see print_operation_info
 * @see print
 * @return void
 */
void print_child_info(const char *info, int id_number);


/** 
 * @brief This function is used to print the status of the current operation managed from a child process
 * @param info the information to print on STDOUT
 * @param id_number the id of child process
 * @param val1 the first operand of operation
 * @param val2 the first operand of operation
 * @param op the operator of operation
 * @see print_child_info
 * @see print
 * @return void
 */
void print_operation_info(const char *info, int id_number, int val1, char op, int val2);

void usr1_handler(int sig);

#endif /* child_h */


