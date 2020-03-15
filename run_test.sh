#!/bin/bash

for index in {1..12..1}
  do
    tFile="con_time$index"
    ./test.sh $index 2> $tFile
  done
