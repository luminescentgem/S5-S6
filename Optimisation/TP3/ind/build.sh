#!/bin/bash

g++ -Wfatal-errors -std=c++20 -Ofast -o main main.cpp

for a in ../instances/*.edges
do
 time ./main $a
 python testind.py $a ${a%.edges}.ind
done

