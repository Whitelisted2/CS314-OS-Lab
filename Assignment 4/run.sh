#!/bin/bash

sed '/^</ d' < inputs/process1.dat > cleanInput.txt
gcc main.c -o main
./main cleanInput.txt