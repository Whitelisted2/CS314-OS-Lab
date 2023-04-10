#!/bin/bash

make buddy;
if [[ $# -eq 2 ]]; then
    ./buddy.o $1 > $2;
elif [[ $# -eq 1 ]]; then
    ./buddy.o $1 ;
fi
make clean;