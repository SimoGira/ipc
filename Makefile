# Sources:
SRCS:=ipc_calculator.c mylib.c utils.c parent.c child.c

# Objects:
OBJS:=$(SRCS:.c=.o)

# Config:
CC:=gcc
CFLAGS:=-c -Wall
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

utils.o: utils.c utils.h
	@echo $@
	@$(CC) $(CFLAGS) -o $@ $<
	
parent.o: parent.c parent.h
	@echo $@
	@$(CC) $(CFLAGS) -o $@ $<
	
child.o: child.c child.h
	@echo $@
	@$(CC) $(CFLAGS) -o $@ $<

ipc_calculator.o: ipc_calculator.c mylib.h utils.h parent.h child.h
	@echo $@
	@$(CC) $(CFLAGS) -o $@ $<

mylib.h:
	@echo $@
	@touch $@

# Collects as dependencies targets which do not match
# any generated file name (e.g. all, clean).
.PHONY: all clean



