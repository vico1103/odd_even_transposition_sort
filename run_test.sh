#!/bin/bash

for run_test in {1..250..1}
  do
    for index in {1..24..1}
      do
        >&2 printf "%s, %s, " $run_test $index
        ./test.sh $index
      done
  done

