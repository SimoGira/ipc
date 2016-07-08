# Sources:
SRCS:=ipc_calculator.c mylib.c utils.c parent.c child.c

# Objects:
OBJS:=$(SRCS:.c=.o)

# Config:
HDIR:=../headers/
CC:=gcc
CFLAGS:=-Wall -c -g
LD:=gcc 
LDFLAGS:=-g

# Default target:
all: ipc_calculator.x

clean:
	@echo Cleaning..
	@rm -f *.o
	@rm -f ipc_calculator.x
	@echo done.

ipc_calculator.x: $(OBJS)
	@echo Linking $@
	@$(LD) $(LDFLAGS) -o $@ $^

utils.o: utils.c  $(HDIR)utils.h
	@echo $@
	@$(CC) $(CFLAGS) -o $@ $<
	
parent.o: parent.c  $(HDIR)parent.h
	@echo $@
	@$(CC) $(CFLAGS) -o $@ $<
	
child.o: child.c  $(HDIR)child.h
	@echo $@
	@$(CC) $(CFLAGS) -o $@ $<

ipc_calculator.o: ipc_calculator.c  $(HDIR)mylib.h  $(HDIR)utils.h  $(HDIR)parent.h  $(HDIR)child.h
	@echo $@
	@$(CC) $(CFLAGS) -o $@ $<
	

# Collects as dependencies targets which do not match
# any generated file name (e.g. all, clean).
.PHONY: all clean
