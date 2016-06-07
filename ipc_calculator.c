//
//  ipc_calculator.c
//  IPC project
//
//  Created by Simone Girardi on 13/05/16.
//  Copyright © 2016 Simone Girardi. All rights reserved.
//

#include "mylib.h"
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <semaphore.h>
#include <signal.h>
 
#define STDIN 0
#define STDOUT 1
#define MYDEBUG printf ("This is line %d.\n", __LINE__);


struct operation{
	int id;
    int val1;
    int val2;
    char operator; 
};

struct operation* operations;
int *childs;
bool *free_childs;
int *results;
int id_number = 0;
int n_operations = -1;
int NPROC = 0;


void parent();
void child();

int main(int argc, char *argv[]){
    
    char *title = "~~~~~~~~~~~~~~~~~~~\n\e[7;36m  IPC CALCULATOR   \e[0m\n~~~~~~~~~~~~~~~~~~~\n";
    int count = (int) write(STDOUT, title, strlen(title));
    if (count == -1)
        syserr (argv[0], "write() failure");
    
    int fd;                      // a file descriptor
    int line_count = 0;
    
    // MARK: open file
    fd = open("configuration.txt",O_RDONLY|O_SYNC, S_IRUSR);
    if (fd < 0) {
        syserr (argv[0], "open() failure");
    }
    
    
    char line[50];
    int i = 0;
    struct list* first_element = NULL;
    struct list* last_element = NULL;
    
 
    while ((count = (int) read(fd, &line[i], 1)) > 0) {             // read byte to byte
        if(line[i] == '\n'){
            line[i] = '\0';
            
            line_count++;
            
            char * str_temp = (char*) malloc(sizeof(char)*i);
            strcpy(str_temp,  line);                                // save every line in to line
            
            if (first_element == NULL){
                first_element = list_create(str_temp);
                last_element = first_element;
            }
            else{
            	last_element =  list_add(str_temp, last_element);
            }
            
            i = 0;
        }
        else
            i++;
    }
    if (count == -1)
        syserr (argv[0], "write() failure");
    close(fd);
    
    if(first_element == NULL){
         syserr (argv[0], "file is empty!");
    }
    
    n_operations = line_count - 1;  // becouse the first line isn't a operation
    printf("n_operations %i\n", n_operations);
    
    NPROC = atoi(first_element->value);         // number of process to create
    
    operations = (struct operation*)malloc(sizeof(struct operation)*n_operations);
    
	// take first element of this list as the first operation
    struct list* list = first_element->next;
    i = 0; 
    
    while (list != NULL)
    {
    
        char* id = strtok(list->value, " "); 
        char* val1 = strtok(NULL, " "); 
        char* op = strtok(NULL, " "); 
        char* val2  = strtok(NULL, " ");
        
        if(val1 == NULL || op == NULL || val2 == NULL)
     		syserr (argv[0], "Wrong operation format");
     		
        //printf("process %s: \n	operation: %s %s %s\n",id, val1, op, val2);
        
        operations[i].id = atoi(id);
        operations[i].val1 = atoi(val1);
        operations[i].val2 = atoi(val2);
        operations[i].operator = op[0];
        i++;
         
        list = list->next;
    }  
    
    // free the lines memory
    list_free(first_element);
    first_element = NULL;
         
    childs = (int*) malloc(sizeof (int*) * NPROC);       // allocate memory for childs
    free_childs = (bool*) malloc(sizeof (bool*) * NPROC);       // allocate memory for childs
    
    results = (int*) malloc(sizeof (int*) * n_operations);  // array of results of operations
 
    pid_t pid; 
    
    printf("Generating childs...\nThe parent process id: %d\n", getpid());
    for (int i = 0; i < NPROC; i++) {
        pid = fork();
        if (pid < 0) {
            syserr("fork()", "cannot create a new process");
        } else if (pid == 0) {      // code execute from child
            id_number = i+1;        // assign id number to process (utile per identificare poi il processo a cui far eseguire l'operazione)
            child();
            break;
        } else {        
            childs[i] = pid;
            //printf("child %i:\n	pid: %d\n", id_number, pid);
        }
    }
    
    if(pid != 0) 
        parent(); 
    
    // i figli devono attendere ad un semaforo per eseguire un calcolo.
     
     
    free(childs);
    free(free_childs);
    free(results); 
    
    return 0;
}

void parent()
{
	printf("parent called\n");
}

void child()
{
	printf("child %i called\n", id_number);
}










