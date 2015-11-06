/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2015,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @ file      task_migration_test.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * Tests the task migration algorithm inside the profiling.
 * Because it is hard to guarantee that a task might migrate, we
 * just create the desired order of profiling events.
 *
 */

#include <config.h>

#include <SCOREP_Location.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Types.h>
#include <SCOREP_Task.h>
#include <SCOREP_Profile.h>
#include <SCOREP_Profile_Tasking.h>
#include <SCOREP_RuntimeManagement.h>
#include <scorep_task_internal.h>

#include <stdio.h>
#include <stdlib.h>

#define SEC( x ) ( ( uint64_t )( x ) * ( uint64_t )1000000 * ( uint64_t )1000 )
#define UNUSED_TASK_END_PARAMETERS SCOREP_INVALID_PARADIGM_TYPE, SCOREP_MOVABLE_NULL, 0, 0

int
main( int argc, char** argv )
{
    SCOREP_InitMeasurement();

    SCOREP_Location* location1 = SCOREP_Location_GetCurrentCPULocation();
    SCOREP_Location* location2 =
        SCOREP_Location_CreateNonCPULocation( location1,
                                              SCOREP_LOCATION_TYPE_GPU,
                                              "test_thread_2" );

    SCOREP_RegionHandle parallel = SCOREP_Definitions_NewRegion( "parallel",
                                                                 "parallel",
                                                                 SCOREP_INVALID_SOURCE_FILE,
                                                                 SCOREP_INVALID_LINE_NO,
                                                                 SCOREP_INVALID_LINE_NO,
                                                                 SCOREP_PARADIGM_OPENMP,
                                                                 SCOREP_REGION_PARALLEL );

    SCOREP_RegionHandle region1 = SCOREP_Definitions_NewRegion( "region1",
                                                                "region1",
                                                                SCOREP_INVALID_SOURCE_FILE,
                                                                SCOREP_INVALID_LINE_NO,
                                                                SCOREP_INVALID_LINE_NO,
                                                                SCOREP_PARADIGM_OPENMP,
                                                                SCOREP_REGION_TASK_UNTIED );
    SCOREP_RegionHandle region2 = SCOREP_Definitions_NewRegion( "region2",
                                                                "region2",
                                                                SCOREP_INVALID_SOURCE_FILE,
                                                                SCOREP_INVALID_LINE_NO,
                                                                SCOREP_INVALID_LINE_NO,
                                                                SCOREP_PARADIGM_OPENMP,
                                                                SCOREP_REGION_TASK_UNTIED );
    SCOREP_RegionHandle region3 = SCOREP_Definitions_NewRegion( "region3",
                                                                "region3",
                                                                SCOREP_INVALID_SOURCE_FILE,
                                                                SCOREP_INVALID_LINE_NO,
                                                                SCOREP_INVALID_LINE_NO,
                                                                SCOREP_PARADIGM_OPENMP,
                                                                SCOREP_REGION_TASK_UNTIED );
    SCOREP_RegionHandle foo_region = SCOREP_Definitions_NewRegion( "foo",
                                                                   "foo",
                                                                   SCOREP_INVALID_SOURCE_FILE,
                                                                   SCOREP_INVALID_LINE_NO,
                                                                   SCOREP_INVALID_LINE_NO,
                                                                   SCOREP_PARADIGM_USER,
                                                                   SCOREP_REGION_FUNCTION );

    SCOREP_RegionHandle barrier = SCOREP_Definitions_NewRegion( "barrier",
                                                                "barrier",
                                                                SCOREP_INVALID_SOURCE_FILE,
                                                                SCOREP_INVALID_LINE_NO,
                                                                SCOREP_INVALID_LINE_NO,
                                                                SCOREP_PARADIGM_USER,
                                                                SCOREP_REGION_BARRIER );

    SCOREP_Profile_Enter( location1, SEC( 1 ), parallel, NULL );
    SCOREP_Profile_Enter( location2, SEC( 1 ), parallel, NULL );

    SCOREP_Profile_Enter( location1, SEC( 1 ), foo_region, NULL );
    SCOREP_Profile_Enter( location2, SEC( 1 ), foo_region, NULL );

    SCOREP_TaskHandle implicit1 = SCOREP_Task_GetCurrentTask( location1 );
    SCOREP_TaskHandle implicit2 = SCOREP_Task_GetCurrentTask( location2 );

    SCOREP_TaskHandle task1 = scorep_task_create( location1, 1, 1 );
    SCOREP_TaskHandle task2 = scorep_task_create( location2, 2, 1 );
    SCOREP_TaskHandle task3 = scorep_task_create( location1, 1, 2 );

    SCOREP_Profile_TaskBegin( location1, SEC( 2 ), region1, NULL, SCOREP_INVALID_PARADIGM_TYPE, 0, 0, 0, task1 );
    SCOREP_Profile_TaskBegin( location2, SEC( 2 ), region2, NULL, SCOREP_INVALID_PARADIGM_TYPE, 0, 0, 0, task2 );
    SCOREP_Profile_TaskSwitch( location1, SEC( 3 ), NULL, SCOREP_INVALID_PARADIGM_TYPE, 0, 0, 0, implicit1 );
    SCOREP_Profile_TaskBegin( location1, SEC( 4 ), region3, NULL, SCOREP_INVALID_PARADIGM_TYPE, 0, 0, 0, task3 );

    SCOREP_Profile_Enter( location2, SEC( 5 ), foo_region, NULL );
    SCOREP_Profile_Exit( location2, SEC( 6 ), foo_region, NULL );

    SCOREP_Profile_TaskSwitch( location2, SEC( 7 ), NULL, SCOREP_INVALID_PARADIGM_TYPE, 0, 0, 0, task1 );
    SCOREP_Profile_TaskSwitch( location1, SEC( 8 ), NULL, SCOREP_INVALID_PARADIGM_TYPE, 0, 0, 0, task2 );
    SCOREP_Profile_Enter( location1, SEC( 9 ), foo_region, NULL );
    SCOREP_Profile_TaskSwitch( location1, SEC( 10 ), NULL, SCOREP_INVALID_PARADIGM_TYPE, 0, 0, 0, task3 );
    SCOREP_Profile_TaskSwitch( location2, SEC( 12 ), NULL, SCOREP_INVALID_PARADIGM_TYPE, 0, 0, 0, task2 );
    SCOREP_Profile_Exit( location2, SEC( 11 ), foo_region, NULL );
    SCOREP_Profile_TaskSwitch( location1, SEC( 13 ), NULL, SCOREP_INVALID_PARADIGM_TYPE, 0, 0, 0, task1 );
    SCOREP_Profile_TaskSwitch( location2, SEC( 14 ), NULL, SCOREP_INVALID_PARADIGM_TYPE, 0, 0, 0, task2 );
    SCOREP_Profile_TaskSwitch( location1, SEC( 15 ), NULL, SCOREP_INVALID_PARADIGM_TYPE, 0, 0, 0, task3 );
    SCOREP_Profile_TaskSwitch( location2, SEC( 16 ), NULL, SCOREP_INVALID_PARADIGM_TYPE, 0, 0, 0, task2 );
    SCOREP_Profile_TaskSwitch( location2, SEC( 17 ), NULL, SCOREP_INVALID_PARADIGM_TYPE, 0, 0, 0, task1 );
    SCOREP_Profile_TaskSwitch( location1, SEC( 18 ), NULL, SCOREP_INVALID_PARADIGM_TYPE, 0, 0, 0, task2 );
    SCOREP_Profile_TaskSwitch( location2, SEC( 19 ), NULL, SCOREP_INVALID_PARADIGM_TYPE, 0, 0, 0, task3 );
    SCOREP_Profile_TaskSwitch( location1, SEC( 20 ), NULL, SCOREP_INVALID_PARADIGM_TYPE, 0, 0, 0, task2 );
    SCOREP_Profile_TaskSwitch( location2, SEC( 21 ), NULL, SCOREP_INVALID_PARADIGM_TYPE, 0, 0, 0, task1 );
    SCOREP_Profile_TaskSwitch( location1, SEC( 22 ), NULL, SCOREP_INVALID_PARADIGM_TYPE, 0, 0, 0, task2 );
    SCOREP_Profile_TaskSwitch( location2, SEC( 23 ), NULL, SCOREP_INVALID_PARADIGM_TYPE, 0, 0, 0, task3 );
    SCOREP_Profile_TaskSwitch( location1, SEC( 24 ), NULL, SCOREP_INVALID_PARADIGM_TYPE, 0, 0, 0, task2 );
    SCOREP_Profile_TaskSwitch( location1, SEC( 25 ), NULL, SCOREP_INVALID_PARADIGM_TYPE, 0, 0, 0, task1 );

    SCOREP_Profile_TaskEnd( location1, SEC( 26 ), region1, NULL, UNUSED_TASK_END_PARAMETERS, task1 );
    SCOREP_Profile_TaskSwitch( location1, SEC( 26 ), NULL, SCOREP_INVALID_PARADIGM_TYPE, 0, 0, 0, task2 );
    SCOREP_Profile_TaskEnd( location1, SEC( 27 ), region2, NULL, UNUSED_TASK_END_PARAMETERS, task2 );
    SCOREP_Profile_TaskEnd( location2, SEC( 28 ), region3, NULL, UNUSED_TASK_END_PARAMETERS, task3 );

    scorep_task_complete( location1, task1 );
    scorep_task_complete( location2, task2 );
    scorep_task_complete( location1, task3 );

    /* Test memory backflow */
    printf( "===================================================================\n" );
    printf( "Memory backflow test.\n" );
    printf( "Warnings that we get too many stub objects or too many task objects\n" );
    printf( "are expected. They do not indicate a problem.\n" );
    printf( "===================================================================\n" );

    const uint64_t steps      = 10;
    const uint64_t iterations = 2000;
    for ( uint64_t i = 0; i < iterations; i++ )
    {
        task1 = scorep_task_create( location1, 1, i + 4 );
        SCOREP_Profile_TaskBegin( location1, SEC( i * steps + 30 ), region1, NULL, SCOREP_INVALID_PARADIGM_TYPE, 0, 0, 0, task1 );
        SCOREP_Profile_Enter( location1, SEC( i * steps + 31 ), foo_region, NULL );
        SCOREP_Profile_Enter( location1, SEC( i * steps + 32 ), foo_region, NULL );
        SCOREP_Profile_Exit(  location1, SEC( i * steps + 33 ), foo_region, NULL );
        SCOREP_Profile_TaskSwitch( location1, SEC( i * steps + 34 ), NULL, SCOREP_INVALID_PARADIGM_TYPE, 0, 0, 0, implicit1 );
        SCOREP_Profile_TaskSwitch( location2, SEC( i * steps + 35 ), NULL, SCOREP_INVALID_PARADIGM_TYPE, 0, 0, 0, task1 );
        SCOREP_Profile_Enter( location2, SEC( i * steps + 36 ), foo_region, NULL );
        SCOREP_Profile_Exit(  location2, SEC( i * steps + 37 ), foo_region, NULL );
        SCOREP_Profile_Exit(  location2, SEC( i * steps + 38 ), foo_region, NULL );

        SCOREP_Profile_TaskEnd( location2, SEC( i * steps + 39 ), region1, NULL, UNUSED_TASK_END_PARAMETERS, task1 );
        scorep_task_complete( location2, task1 );
    }

    printf( "===================================================================\n" );
    printf( "End of backflow test\n\n" );

    SCOREP_Profile_Exit( location1, SEC( iterations * steps + 50 ), foo_region, NULL );
    SCOREP_Profile_Exit( location2, SEC( iterations * steps + 51 ), foo_region, NULL );

    SCOREP_Profile_Enter( location1, SEC( iterations * steps + 52 ), barrier, NULL );
    SCOREP_Profile_Exit( location1, SEC( iterations * steps + 53 ), barrier, NULL );
    SCOREP_Profile_Enter( location2, SEC( iterations * steps + 52 ), barrier, NULL );
    SCOREP_Profile_Exit( location2, SEC( iterations * steps + 53 ), barrier, NULL );

    SCOREP_Profile_Exit( location1, SEC( iterations * steps + 53 ), parallel, NULL );
    SCOREP_Profile_Exit( location2, SEC( iterations * steps + 53 ), parallel, NULL );
    return 0;
}
