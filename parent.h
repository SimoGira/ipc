//
//  parent.h
//  
//
//  Created by Simone Girardi on 27/06/16.
//
//

#ifndef parent_h
#define parent_h

#include "mylib.h"

float *my_parent(int my_semaphores[], int n_operations, int NPROC, int *childs_started, struct operation *operations, struct operation *current_operation, struct result *current_result, bool child_isFree[]);

int get_first_free_child(int NPROC, bool child_isFree[]);

void print_parent_info(const char *info, int child_id);

#endif /* parent_h */
