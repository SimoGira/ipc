//
//  mylib.c
//
//
//  Created by Simone Girardi on 13/05/16.
//
//
/******************************************
 MODULO: mylib.c
 SCOPO: libreria di funzioni d’utilita'
******************************************/

#include "mylib.h"
#include "utils.h"
#include "parent.h"
#include "child.h"
#define CALLER "mylib.c"

struct list *list_create(char* value){
    struct list *new = (struct list*) malloc(sizeof(struct list));
    new->value = value;
    new->next = NULL;
    return new;
}

struct list *list_add(char* value, struct list *previous){
	if(previous == NULL) 
         syserr ("list_add", "previous list element is NULL!");
	
    struct list *new = (struct list*) malloc(sizeof(struct list));
    new->value = value;
    new->next = NULL;
    previous->next = new;
    return new;
}

void list_print(struct list *this){
    if (this != NULL) {
        print(this->value, CALLER, __LINE__);
        list_print(this->next);
    }
}
 
void list_free(struct list *this){
    if (this != NULL) { 
        list_free(this->next);
        free(this);
    }
}

void syserr(char *prog, char *msg){
    //fprintf(stderr, "%s - \e[91merror\e[0m: %s\n",prog, msg);  // non si può usare
    //perror("system error");
    char *str_temp = strcat(prog, "- \e[91msystem error\e[0m: ");
    str_temp = strcat(str_temp, msg);
    perror(str_temp);
    exit(1);
}


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

void print(const char* msg, const char*caller, int line){
    int count = (int) write(STDOUT, msg, strlen(msg));
    if (count == -1){
        char line_str[50];
        sprintf(line_str ,"--- %s line %d ---" , caller, line);
        syserr (line_str, "print() failure");
    }
    fflush(stdout);
}










