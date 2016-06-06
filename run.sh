#!/bin/bash

if [[ $1 == "compile" ]]; then
	gcc ipc_calculator.c mylib.c
	./a.out
else
	./a.out
fi

