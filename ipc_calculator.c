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

#include <semaphore.h>
#include <signal.h>


#define BUFLEN 1024
#define STDIN 0
#define STDOUT 1
#define MYDEBUG printf ("This is line %d.\n", __LINE__);


struct operation{
	int id;
    int val1;
    int val2;
    char operator; 
};



int main(int argc, char *argv[]){
    
    char *title = "~~~~~~~~~~~~~~~~~~~\n\e[7;36m  IPC CALCULATOR   \e[0m\n~~~~~~~~~~~~~~~~~~~\n";
    int count = (int) write(STDOUT, title, strlen(title));
    if (count == -1)
        syserr (argv[0], "write() failure");
    
    int fd;                      // a file descriptor
    int n_operations = -1;        // number of operations
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
    
    int NPROC = atoi(first_element->value);         // number of process to create
    
    struct operation* operations = (struct operation*)malloc(sizeof(struct operation)*n_operations);
    
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
     		
        printf("process %s: \n	operation: %s %s %s\n",id, val1, op, val2);
        
        operations[i].id = atoi(id);
        operations[i].val1 = atoi(val1);
        operations[i].val2 = atoi(val2);
        operations[i].operator = op[0];
        i++;
         
        list = list->next;
    } 
    
    
    
    
    exit(0);
    
    
    /*
     
     file_size = lseek(fd, 0L, SEEK_END);     // read until EOF
     buffer_file = (char*)malloc(file_size);
     lseek(fd, 0L, SEEK_SET);            // rewind location of cursor
     
     write(STDOUT, "The content of file is:\n", strlen("The content of file is:\n"));
     char c;
     int i = 0;
     while ((x = (int) read(fd, &c, 1)) > 0) {        // read byte to byte
     
     x = (int) write (STDOUT , &c , x);        // print content of file
     if (x == -1)
     syserr (argv[0], "write() failure");
     if (isOperator(c)) {
     n_operations++;
     }
     buffer_file[i++] = c;
     }
     */
    
    //-----------------------------------------------------------------------
    // MARK: setup configuration
    //int *childs = (int*) malloc(sizeof (int*) * NPROC);       // allocate memory for childs
    //int results[n_operations];                        // array of results of operations
    //int *id = (int*) malloc(sizeof (int) * n_operations);
    /*
    int id_number = 0 ;
    pid_t pid;
    
    
    printf("\nThe parent process id: %d\n", getpid());
    for (int i = 0; i < NPROC; i++) {
        pid = fork();
        if (pid < 0) {
            syserr("fork()", "cannot create a new process");
        } else if (pid == 0) {      // code execute from child
            id_number = i+1;        // assign id number to process (utile per identificare poi il processo a cui far eseguire l'operazione)
            child();
            break;
        } else {                    // code execute from parent
            parent();
            childs[i] = pid;
            printf("The child process ID is: %d\n", pid);
        }
    }
    
    // i figli devono attendere ad un semaforo per eseguire un calcolo.
    
    
    
    for (int i = 0; i < n_operations; i++) {
        if (id_number == id[i]) {
            // calculate operation
        }
    }
    
    
    
    close(fd);  // close file descriptor
    
    
    //for(int i = 0; i < file_size; i++){
    //printf("buffer_file[%i]: %c\n", i, buffer_file[i]);
    //}
    
    
    
    
    free(buffer_file);
    free(childs);
     */
    
    return 0;
}

