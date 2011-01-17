#!/bin/sh

CC=$1

eval $CC calibrate1.f90 -o calib1
./calib1 || exit 1

eval $CC calibrate.f90 -c
eval $CC work.f90 -c

for i in 2 3 4 5 6 7 8 9
do
  eval $CC calibrate2.f90 work.f90 -o calib2
  ./calib2 $i || exit 0
done
exit 0
