
/**
 * @file mylib.c
 * @author Simone Girardi
 * @date 27 jun 2016.
 * @version 1.0
 */

#include "mylib.h"
#include "utils.h"

#define CALLER "mylib.c"

// ============================================================================================================
//                                                FUNCTIONS FOR - LISTS
// ============================================================================================================
struct list *list_create(char* value){
    struct list *new = (struct list*) malloc(sizeof(struct list));
    new->value = value;
    new->next = NULL;
    return new;
}

// ------------------------------------------------------------------------------------------------------------
struct list *list_add(char* value, struct list *previous){
	if(previous == NULL) 
         syserr ("list_add", "previous list element is NULL!");
	
    struct list *new = (struct list*) malloc(sizeof(struct list));
    new->value = value;
    new->next = NULL;
    previous->next = new;
    return new;
}

// ------------------------------------------------------------------------------------------------------------
void fill_list_operations(struct list *list, struct operation *operations){
    int i = 0;
    while (list != NULL){
        char* id = strtok(list->value, " ");
        char* val1 = strtok(NULL, " ");         /* NULL must be used to get tokens from the previous string now */
        char* op = strtok(NULL, " ");
        char* val2  = strtok(NULL, " ");
        
        if(val1 == NULL || op == NULL || val2 == NULL)
            syserr ("fill_list_operations()", "Wrong operation format");
        
        operations[i].id = atoi(id) - 1;
        operations[i].val1 = atoi(val1);
        operations[i].val2 = atoi(val2);
        operations[i].operator = op[0];
        i++;
        
        list = list->next;
    }
}

// ------------------------------------------------------------------------------------------------------------
void list_print(struct list *this){
    if (this != NULL) {
        print(this->value, CALLER, __LINE__);
        list_print(this->next);
    }
}

// ------------------------------------------------------------------------------------------------------------
void list_free(struct list *this){
    if (this != NULL) { 
        list_free(this->next);
        free(this);
    }
}

// ------------------------------------------------------------------------------------------------------------
void syserr(char *prog, char *msg){
    //fprintf(stderr, "%s - \e[91merror\e[0m: %s\n",prog, msg);  // non si può usare
    //perror("system error");
    char *str_temp = strcat(prog, "- \e[91msystem error\e[0m: ");
    str_temp = strcat(str_temp, msg);
    perror(str_temp);
    exit(1);
}

// ============================================================================================================
//                                                FUNCTIONS FOR - STDERR / STDOUT
// ============================================================================================================
void syserr_ext(char *prog, char *msg, int line){
    //fprintf(stderr, "%s | line: %d | \e[91merror\e[0m: %s\n",prog, line, msg);  // non si può usare
    //perror("system error");

    char line_str[20];
    sprintf(line_str ,"%d" , line);
    char *str_temp = strcat(prog, " | line: ");
    str_temp = strcat(str_temp, line_str);
    str_temp = strcat(str_temp, " | \e[91merror\e[0m ");
    str_temp = strcat(str_temp, msg);
    perror(str_temp);
     
    exit(1);
}

// ------------------------------------------------------------------------------------------------------------
void print(const char* msg, const char*caller, int line){
    int count = (int) write(STDOUT, msg, strlen(msg));
    if (count == -1){
        char line_str[50];
        sprintf(line_str ,"--- %s line %d ---" , caller, line);
        syserr (line_str, "print() failure");
    }
    fflush(stdout);
}


