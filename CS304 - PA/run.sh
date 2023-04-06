#!/bin/bash

make buddy;
if [[ $# -eq 2 ]]; then
    
./buddy.o $1 > output.txt;
make clean;