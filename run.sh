#!/bin/bash

if [[ $1 == "compile" || $1 == "c" ]]; then
	rm -rf a.out
	gcc ipc_calculator.c mylib.c  -Wall

elif [[ $1 == "compilerun" || $1 == "cr" ]]; then
	rm -rf a.out
	gcc ipc_calculator.c mylib.c  -Wall
	./a.out
	
elif [[ $1 == "push" ]]; then
	git status
	git add --all
	git commit -m "update"
	git push
else
	./a.out
fi


