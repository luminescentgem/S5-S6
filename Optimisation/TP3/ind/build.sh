#!/bin/bash

g++ -Wall -std=c++20 -Ofast -o main main.cpp

for a in ../instances/*.edges
do
  time ./main $a
  python3 testind.py $a ${a%.edges}.ind
done

