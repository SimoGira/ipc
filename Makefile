# Sources:
SRCS:=ipc_calculator.c mylib.c

# Objects:
OBJS:=$(SRCS:.c=.o)

# Config:
CC:=gcc
CFLAGS:= -c -g -Wall
LD:=gcc

# Default target:
all: ipc_calculator.x

clean:
	@echo Cleaning..
	@rm -f *.o
	@rm -f ipc_calculator.x

ipc_calculator.x: $(OBJS)
	@echo Linking $@
	@$(LD) -o $@ $^

# MAGARI CI SERVIRA'
#utils.o: utils.c utils.h
#	@echo $@
#	@$(CC) $(CFLAGS) -o $@ $<

ipc_calculator.o: ipc_calculator.c mylib.h
	@echo $@
	@$(CC) $(CFLAGS) -o $@ $<

mylib.h:
	@echo $@
	@touch $@

# Collects as dependencies targets which do not match
# any generated file name (e.g. all, clean).
.PHONY: all clean
