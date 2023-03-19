#!/bin/bash

g++ lru.cpp -o lru.out
./lru.out $1 $2 $3 $4
# 60 20 60 req1.dat