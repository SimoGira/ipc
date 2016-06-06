
# Sources:
SRCS:=base.c utils.c main.c
OBJS:=$(SRCS:.c=.o)


# Config:
CC:=gcc
CFLAGS:= -c
LD:=gcc

# Targets:

all: example

clean:
	@echo Cleaning.
	@rm -f *.o
	@rm -f example

example: $(OBJS)
	@echo $@
	@$(LD) -o $@ $^


base.o: base.c base.h
	@echo $@
	@$(CC) $(CFLAGS) -o $@ $<

utils.o: utils.c utils.h
	@echo $@
	@$(CC) $(CFLAGS) -o $@ $<

main.o: main.c mylib.h
	@echo $@
	@$(CC) $(CFLAGS) -o $@ $<

mylib.h: base.h utils.h
	@echo $@
	@touch $@


.PHONY: all clean
