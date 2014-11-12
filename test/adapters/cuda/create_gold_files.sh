#!/bin/bash

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2014,
## Technische Universitaet Dresden, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##
##

## file       create_gold_files.sh

## Run this script once to create .gold files for CUDA testing in the test/adapters/src/data directory.
## Created files must be committed for automated testing.
##
## Gold files are created with:
## otf2-print <trace> | grep -e '^ENTER\|LEAVE' | grep -Fv 'tmpxft' | sed -e 's/[^ ]*[^ ]/0/3' | egrep | sed -e 's/<[0-9][0-9]*>$/<id>/g' > <gold>
##
## grep for lines starting with 'ENTER' or 'LEAVE' and which do not contain dynamic function names ('tmpxft')
## replace every 3rd word (timestamp) with '0'
## filter for host and device locations using egrep [-v]
## replace region id of ENTER/LEAVE records by general pattern

RESULT_DIR=tmp_cuda_trace
TARGET_DIR=$1
EXECUTABLE=$2
OTF2_PRINT=$3
num_streams=3

USAGE="usage: <script> <target-dir> <executable> <otf2-print>"

echo "===== Creating test gold files in '$TARGET_DIR' ====="

if [ ! -d "$TARGET_DIR" ]; then
  echo "Error: Target directory '$TARGET_DIR' does not exist"
  echo $USAGE
  exit 1
fi

if [ ! -e "$OTF2_PRINT" ]; then
  echo "Error: Did not find otf2-print here: '$OTF2_PRINT'"
  echo $USAGE
  exit 1
fi

if [ ! -e "$EXECUTABLE" ]; then
  echo "Error: Did not find executable here: '$EXECUTABLE'"
  echo $USAGE
  exit 1
fi

mkdir $RESULT_DIR 2> /dev/null

# advanced cuda tests have non-deterministic event orders, thus we test only basic version
# list all combinations that should be tested here
# note that combinations must be created in the same order as they are tested in 'run'cuda_test.sh'
for features in "driver" "runtime" "kernel" "memcpy" "driver,kernel" "driver,kernel,references" "kernel,idle" "kernel,pure_idle" "driver,kernel,sync" "driver,kernel,idle" "kernel,gpumemusage"
do
  echo "Creating combination '${features}'"

  SCOREP_CUDA_ENABLE=${features} \
  SCOREP_OVERWRITE_EXPERIMENT_DIRECTORY=true \
  SCOREP_ENABLE_TRACING=yes \
  SCOREP_EXPERIMENT_DIRECTORY=$RESULT_DIR \
  ${EXECUTABLE} -b -s ${num_streams}  > /dev/null
  res=$?

  if [ $res -ne 0 ]; then
    echo "Error executing CUDA program"
    rm -r $RESULT_DIR
    exit $res
  fi

  features_pure=${features//,/.}
  goldfile="b.$features_pure.gold"
  goldfile=$TARGET_DIR/${goldfile//".."/.}

  # print, replace timestamps with 0 and write to gold file
  $OTF2_PRINT $RESULT_DIR/traces.otf2 |
    grep '^ENTER\|LEAVE' |
    grep -Fv 'tmpxft' |
    sed -e 's/[^ ]*[^ ]/0/3' |
    egrep '^(ENTER|LEAVE)[[:space:]]+0' |
    sed -e 's/<[0-9][0-9]*>$/<id>/g' > $goldfile.host

  $OTF2_PRINT $RESULT_DIR/traces.otf2 |
    grep '^ENTER\|LEAVE' |
    grep -Fv 'tmpxft' |
    sed -e 's/[^ ]*[^ ]/0/3' |
    egrep -v '^(ENTER|LEAVE)[[:space:]]+0' |
    sed -e 's/<[0-9][0-9]*>$/<id>/g' > $goldfile.device
done
