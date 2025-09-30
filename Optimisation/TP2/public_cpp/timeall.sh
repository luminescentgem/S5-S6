#!/bin/bash

TIMEFORMAT=%Rs
for opt in -Ofast # -O3 -O2 -O1 -O0
do
  echo Optimization: $opt
  g++ main.cpp -std=c++20 -Wfatal-errors -o main $opt
  for f in `ls -Sr ../input/*.json`
  do
    echo -n $f" "
    time ./main $f ${f%.instance.json}.solution.svg # > /dev/null
  done
done

