#!/bin/bash

if [[ $1 == "compile" ]]; then
	rm -rf a.out
	gcc ipc_calculator.c mylib.c 
fi
sudo ipcrm -s 32768
./a.out
