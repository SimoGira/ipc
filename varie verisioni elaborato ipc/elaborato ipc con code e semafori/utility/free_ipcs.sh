#!/bin/bash

if [[ $1 == "-m" ]]; then
	ipcs -m | cut -d' ' -f2 | grep '^[0-9]' | while read x; do ipcrm -m $x; done

elif [[ $1 == "-s" ]]; then
	ipcs -s | cut -d' ' -f2 | grep '^[0-9]' | while read x; do ipcrm -s $x; done
	
elif [[ $1 == "-q" ]]; then
    ipcs -q | cut -d' ' -f2 | grep '^[0-9]' | while read x; do ipcrm -q $x; done
else
	echo "Usage: ./free_ipcs.sh [-m] [-s] [-q]"
fi


