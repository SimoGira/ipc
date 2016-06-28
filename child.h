//
//  child.h
//  
//
//  Created by Simone Girardi on 27/06/16.
//
//

#ifndef child_h
#define child_h

#include "mylib.h"

//void mychild();

void child(int id_number, int NPROC, int my_semaphores[], int *childs_started, struct operation *current_operation, struct result *current_result, bool child_isFree[]);

void print_child_info(const char *info, int id_number);

void print_operation_info(const char *info, int val1, int val2, char op, int res);

#endif /* child_h */


