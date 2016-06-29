#!/bin/bash
if [[ $1 == "compile" || $1 == "c" ]]; then
	rm -rf a.out
	gcc ipc_calculator.c mylib.c  -Wall -g

elif [[ $1 == "compilerun" || $1 == "cr" ]]; then
	rm -rf a.out
	gcc ipc_calculator.c mylib.c  -Wall -g
	./a.out
	
elif [[ $1 == "push" ]]; then
    cd ..
	git status
	git add --all
	git commit -m "update"
	git push
else
	echo "Usage: ./run.sh [-c] [-cr] [push]"
fi


