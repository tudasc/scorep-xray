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
make clean-local-scorep-config-tool
make scorep-config-tool-local
. ./scorep_config.dat

run_test()
{

	#get hostname and pick random port for periscope emulator
	REG_HOST=`hostname`
	REG_PORT=$((40000+$RANDOM%10000))
	echo Starting OA_C_TEST with Periscope Emulator on $REG_HOST:$REG_PORT
	
	#start periscope emulator
	./online_access_registry $REG_PORT test=$SRC_ROOT/tools/oa_registry/scenario_serial  &
	REGSRV_PID=$!
	
	#start online access test
	SCOREP_ONLINEACCESS_BASE_PORT=50000 SCOREP_ONLINEACCESS_REG_PORT=$REG_PORT SCOREP_ONLINEACCESS_REG_HOST=$REG_HOST ./oa_c_test &
	TEST_PID=$!
	
	#wait for periscope emulator to finish
	wait $REGSRV_PID
	if [ $? -ne 0 ]; then
	    rm -rf scorep-measurement-tmp start_ls.log
	    kill -9 $TEST_PID
	    exit 1
	fi
	
	#wait for online access test to finish
	wait $TEST_PID
	if [ $? -ne 0 ]; then
	    rm -rf scorep-measurement-tmp start_ls.log
	    exit 1
	fi
	
	exit 0

}

#cleanup the background processes when timeout happens
cleanup()
{
    trap - ALRM               							#reset handler to default
    kill -ALRM $a 2>/dev/null 							#stop timer subshell if running
    kill $! 2>/dev/null &&    							#kill last job

    #find out pids of the periscope emulator and online access test and kill both
    pids_to_clean=`ps -f | grep "online_access_registry" | grep -v "grep" | awk '{print $2;}'`
    kill -9 $pids_to_clean 2>/dev/null
    pids_to_clean=`ps -f | grep "\/oa_c_test" | grep -v "grep" | awk '{print $2;}'`
    kill -9 $pids_to_clean 2>/dev/null
    
    if test -n "$pids_to_clean" ; then
    	echo Test failed due to timeout.
    	rm -rf scorep-measurement-tmp start_ls.log
    fi
    
    exit 124                #exit with 124 if timedout
}

#timeout function
watchit()
{
    trap "cleanup" ALRM
    sleep $1& wait
    kill -ALRM $$
}

watchit 100 & a=$!         #start the timeout of 100 sec.
trap "cleanup" ALRM INT    #cleanup after timeout
run_test &                 #start the test
test_pid=$!
wait $test_pid; RET=$?     #wait for test and save its return value
kill -ALRM $a              #send ALRM signal to watchit
wait $a                    #wait for watchit to finish cleanup
if [ $RET -eq 0 ]; then
echo OA_C_TEST successful.
fi
rm -f config.h scorep_config.dat
exit $RET                  #return the value

