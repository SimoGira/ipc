
/**
 * @file mylib.h
 * @author Simone Girardi
 * @date 27 jun 2016.
 * @version 1.0
 */

#ifndef MYLIB_H
#define MYLIB_H
 
#include <sys/ipc.h>
#include <sys/shm.h>

#include <stdio.h>  /* standard I/O functions */
#include <stdlib.h>
#include <unistd.h> /* standard unix functions like getpid(), write() etc */
#include <string.h>

#include "utils.h"

#define STDIN 0
#define STDOUT 1


/**
 * @brief A simple linked list
 *
 * This list is used to save the content of file configuration, every element of the list is a line of the file
 */
struct list{
    char* value;
    struct list *next;
};

/**
 * @brief This function print system call error message and terminate the program. It has been defined for
 * convenience, it is an alternitve of perror
 *
 * @param prog the name of program or function that calls this function
 * @param msg a message to display on STDERR
 * @see syserr_ext
 * @return void
 */
void syserr(char *prog, char *msg);

/**
 * @brief This function is an extension of syserr, it will print also the number of line where it was called
 *
 * @param prog the name of program or function that calls this function
 * @param msg a message to display on STDERR
 * @param line
 * @see syserr
 * @return void
 */
void syserr_ext(char *prog, char *msg, int line);

/**
 * @brief This function create a linked list.
 *
 * @param value the value of the first element of list
 * @see list
 * @return new a pointer of the new struct list
 */
struct list *list_create(char* value);

/**
 * @brief This function add a new element to a existing list.
 *
 * @param value the value of the new element to add to list
 * @param previous the pointer of the previus element of the list
 * @see list
 * @return new a pointer of the new updated struct list
 */
struct list *list_add(char* value, struct list *previous);

/**
 * @brief This function fill the struct operation with oprations contained in a list.
 *
 * This function scroll through the entire list and it tokenizes every line with a specifics format (e.g. val1 op
 * val2) to save in a operation struct.
 *
 * @param list a list that contains some operations
 * @param operations a struct operation to save the formatted operation
 * @see list
 * @see operation
 * @return void
 */
void fill_list_operations(struct list *list, struct operation *operations);

/**
 * @brief This function print the content of all values in entire linked list
 *
 * @param this a list with at least one element
 * @see list
 * @see print
 * @return void
 */
void list_print(struct list *this);

/**
 * @brief This function deallocate the memory previous allecated for a list
 *
 * @param first_element the first element of the list
 * @see list
 * @return void
 */
void list_free(struct list *first_element);


/**
 * @brief This function print a simple string to the STDOUT
 *
 * @param msg the string to print on STDOUT
 * @param caller the name of the function to call this function
 * @param line the line where this function is called 
 * @return void
 */
void print(const char* msg, const char*caller, int line);

#endif /* mylib_h */









