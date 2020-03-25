#!/bin/bash

# control for right count of argments
if [ $# -eq 1 ];then
    numbers=$1;
else
    >&2 echo "Illegal number of argument. Please try again ./test.sh 6"
    exit 251
fi;

# control if argument is number
check=$(echo "$@" | grep -E '^\-?[0-9]+$')

	if [[ "$check" == '' ]]; then
	  >&2 echo "Argumnt is not a number. Please try again ./test.sh 6"
    exit 251
fi;

# build, create numbers and run
mpic++ --prefix /usr/local/share/OpenMPI -o ots ots.cpp
dd if=/dev/random bs=1 count=$numbers of=numbers > /dev/null 2>&1
mpirun -np $numbers ots numbers

# remove files after run
rm -f numbers ots 
