#!/bin/bash

if [ "$#" == 1 ]; then
    sed '/^</d' < $1 > cleanInput.txt
    gcc rr-hash.c -o a.out
    ./a.out cleanInput.txt
    rm cleanInput.txt
elif [ "$#" == 2 ]; then
    sed '/^</d' < $1 > cleanInput.txt
    gcc rr-hash.c -o a.out
    ./a.out cleanInput.txt > $2
    rm cleanInput.txt
else
    echo "Incorrect format! Please enter command as: bash run2.sh <path-to-input-file> [<path-to-output-file>]"
fi