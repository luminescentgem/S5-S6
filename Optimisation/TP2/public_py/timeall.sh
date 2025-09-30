#!/bin/bash

TIMEFORMAT=%Rs
for opt in -Ofast # -O3 -O2 -O1 -O0
do
  for f in `ls -Sr ../input/*.json`
  do
    echo -n $f" "
    time python main.py $f ${f%.instance.json}.solution.svg # > /dev/null
  done
done

