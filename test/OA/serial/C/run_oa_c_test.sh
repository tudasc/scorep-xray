#!/bin/bash

## 
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2011, 
##    RWTH Aachen University, Germany
##    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##    Technische Universitaet Dresden, Germany
##    University of Oregon, Eugene, USA
##    Forschungszentrum Juelich GmbH, Germany
##    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##    Technische Universitaet Muenchen, Germany
##
## See the COPYING file in the package base directory for details.
##

## file       run_oa_c_test.sh
## maintainer Yury Oleynik <oleynik@in.tum.de>

# Get SRC_ROOT
make clean-local-scorep-config-tool >/dev/null
make scorep-config-tool-local >/dev/null
. ./scorep_config.dat >/dev/null

REGSRV_PID=
TEST_PID=
TEST_NAME=oa_c_test
TIMEOUT=60
RETURN_VALUE=0;
SCENARIO_FILE=scenario_serial

#cleanup the background processes when timeout happens
cleanup()
{
    trap "dummy" ALRM               					#reset handler to dummy
    echo cleaning $1 $2 $3
    kill -ALRM $3								#stop timer subshell if running
   # wait $3
    trap - ALRM               							#reset handler to default
    
	if [ $1 -ne 0 -o $2 -ne 0 ]; then
    	echo Test failed due to timeout \($TIMEOUT sec\). Killing periscope\(pid $1\) and $TEST_NAME\(pid $2\)
    	echo $TEST_NAME FAILED! $TEST_ID
    	
    	clean_scorep_crash
    	clean_scorep
    	
    	kill -9 $1 $2 2>/dev/null		# kill periscope emulator and oa test
    	wait $1 $2	2>/dev/null
    	
    	exit 124
    fi
}

#dummy function needed misguide the double issue of the signal when cleaning pids
dummy()
{
dummy=
}

#clean experimental folder
clean_scorep()
{
	rm -f config.h scorep_config.dat command_file	
}

#clean experiment folder in case of failure
clean_scorep_crash(){
	rm -rf scorep-measurement-tmp start_ls.log command_file
}

#kill sleep in the timer
cleanup_timer()
{
	echo killing timer_sleep
	kill -9 $1
}

#timeout function
watchit()
{
    sleep $1 & 
    SLEEP_PID=$!
    echo timer_sleep=$SLEEP_PID
    trap "cleanup_timer $SLEEP_PID" ALRM	#clean sleep in case of finish before the timeout
    wait $SLEEP_PID
    kill -ALRM $$

}

watchit $TIMEOUT &					 #start the timeout of 100 sec.
TIMER_PID=$!       

###########################################################Starting tests###################################################
#create GDB batch mode command file
echo -e "run\nbt\nquit" > command_file

#get hostname and pick random port for periscope emulator
REG_HOST=`hostname`
REG_PORT=$((40000+$RANDOM%10000))
PORT_BUSY=`netstat -anp 2>/dev/null | grep $REG_PORT `

#check whether the port is busy
while test -n "$PORT_BUSY"
do
	echo Randomly picked port $REG_PORT is busy :$PORT_BUSY:
	REG_PORT=$((40000+$RANDOM%10000))
	PORT_BUSY=`netstat -ap 2>/dev/null | grep $REG_PORT`
done

BASE_PORT=$(($REG_PORT+1))
echo Starting $TEST_NAME with Periscope Emulator on $REG_HOST:$REG_PORT	

#start periscope emulator 
./online-access-registry $REG_PORT test=$SRC_ROOT/tools/oa_registry/$SCENARIO_FILE  &
REGSRV_PID=$!

#start online access test
SCOREP_ONLINEACCESS_ENABLE=1 SCOREP_ONLINEACCESS_BASE_PORT=$BASE_PORT SCOREP_ONLINEACCESS_REG_PORT=$REG_PORT SCOREP_ONLINEACCESS_REG_HOST=$REG_HOST gdb --command command_file --batch ./$TEST_NAME & 
TEST_PID=$!

echo registry=$REGSRV_PID application=$TEST_PID timer=$TIMER_PID
############################################################################################################################

trap "cleanup $REGSRV_PID $TEST_PID $TIMER_PID" ALRM INT    #cleanup in case of timeout

###########################################################Waiting tests####################################################
#echo wait_test PIDS: $REGSRV_PID $TEST_PID

#wait for periscope emulator to finish
wait $REGSRV_PID
REG_RETURN=$?
if [ $REG_RETURN -ne 0 ]; then
    kill -9 $TEST_PID
    echo FAILED registry 
    RETURN_VALUE=$REG_RETURN
fi

#wait for online access test to finish
wait $TEST_PID
TEST_RETURN=$?
if [ $TEST_RETURN -ne 0 ]; then
    if [ $REG_RETURN -eq 0 ]; then
    	echo FAILED application
    fi 
    RETURN_VALUE=$TEST_RETURN
fi

############################################################################################################################

cleanup 0 0 $TIMER_PID

if [ $RETURN_VALUE -eq 0 ]; then
	
	echo $TEST_NAME successful.
else
	clean_scorep_crash
	echo $TEST_NAME FAILED! 
fi 


clean_scorep

exit $RETURN_VALUE                  #return the value

