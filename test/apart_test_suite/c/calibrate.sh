#!/bin/sh

CC=$1

eval $CC calibrate1.c -o calib1
./calib1 || exit 1

for i in 2 3 4 5 6 7 8 9
do
  eval $CC calibrate2.c work.c -o calib2
  ./calib2 $i || exit 0
done
exit 0
