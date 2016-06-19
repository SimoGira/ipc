
# Sources:
SRCS:=ipc_calculator.c mylib.c
OBJS:=$(SRCS:.c=.o)


# Config:
CC:=gcc
CFLAGS:= -c
LD:=gcc

# Targets:
all: ipc_calculator

clean:
	@echo Cleaning.
	@rm -f *.o
	@rm -f ipc_calculator

ipc_calculator: $(OBJS)
	@echo $@
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
