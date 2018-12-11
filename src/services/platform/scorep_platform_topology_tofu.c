/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2016-2017,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * topology information for the Fujitsu Tofu Topology with dimensions
 * for the  K computer.
 */


#include <config.h>

#include <SCOREP_Platform.h>
#include <SCOREP_Location.h>
#include <scorep_status.h>
#include <SCOREP_Memory.h>
#include <SCOREP_Subsystem.h>

#define SCOREP_DEBUG_MODULE_NAME TOPOLOGIES
#include <UTILS_Debug.h>

#include <unistd.h>
#include <string.h>
#include <stdio.h>

static uint32_t
get_subsystem_hardware_thread_id( SCOREP_Location* location );

typedef struct hardware_location_subsystem_data hardware_location_subsystem_data;
struct hardware_location_subsystem_data
{
    uint32_t hardware_thread_id;
};
/* sched_getcpu is available but, because of the -noansi compiler
 * option, a 'declared implicitly' warning is issued. Could be fixed
 * by adding an additional -ansi option. Forward declare it
 * instead. */
int
sched_getcpu( void );

/* Forward declare function from mpi-ext.h instead of including
 * mpi-ext.h. Don't want to compile libscorep_platform.la
 * using MPI compiler. FJMPI_Topology_sys_rank2xyzabc symbol
 * provided when linking with the MPI compiler. Symbol not used
 * in the non-MPP case. In a non-MPP case no platform topology
 * will be created as both platform calls have MPP checks. Since,
 * it is a single node case, the "Process x Threads" topology will
 * provide the same amount of information.
 */
extern int
FJMPI_Topology_sys_rank2xyzabc( int  rank,
                                int* x,
                                int* y,
                                int* z,
                                int* a,
                                int* b,
                                int* c );


static const uint32_t number_of_dimensions = 7;
static const char*    topology_name        = "Fujitsu Tofu Topology";
static size_t         subsystem_id;

static long
platform_tofu_num_cpus()
{
    long nprocs = sysconf( _SC_NPROCESSORS_ONLN );
    UTILS_BUG_ON( nprocs == -1, "Failed to query number of processes during platform topology creation." );
    return nprocs;
}

static uint32_t
get_hardware_thread_id( void )
{
    int cpuid = sched_getcpu();
    UTILS_BUG_ON( cpuid < 0, "Error calling sched_getcpu call" );
    return ( uint32_t )cpuid;
}

uint32_t
SCOREP_Platform_GetHardwareTopologyNumberOfDimensions( void )
{
    return number_of_dimensions;
}

bool
SCOREP_Platform_GenerateTopology( void )
{
    /* As the coordinate call is a MPI call, don't generate a platform topology without MPI*/
    return SCOREP_Status_IsMpp();
}

SCOREP_ErrorCode
SCOREP_Platform_GetHardwareTopologyInformation( char const** name,
                                                int          nDims,
                                                int*         procsPerDim,
                                                int*         periodicityPerDim,
                                                char*        dimNames[] )
{
    UTILS_DEBUG_ENTRY();

    if ( SCOREP_Status_IsMpp() && !SCOREP_Status_IsMppInitialized() )
    {
        return SCOREP_ERROR_INVALID;
    }

    UTILS_BUG_ON( nDims != number_of_dimensions, "Length of arrays differs from number of dimensions!" );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_TOPOLOGIES, "[tofu] Creating hardware topology." );

    *name         = topology_name;
    dimNames[ 0 ] = "x";
    dimNames[ 1 ] = "y";
    dimNames[ 2 ] = "z";
    dimNames[ 3 ] = "a";
    dimNames[ 4 ] = "b";
    dimNames[ 5 ] = "c";
    dimNames[ 6 ] = "core";

    /* hard-coded default values for K computer */
    /* currently no open acessible interface on other tofu systems  */
    /* 24 18 17 2 3 2 8 = 705,024 cores (top500) */
    procsPerDim[ 0 ] = 24;
    procsPerDim[ 1 ] = 18;
    procsPerDim[ 2 ] = 17;
    procsPerDim[ 3 ] =  2;
    procsPerDim[ 4 ] =  3;
    procsPerDim[ 5 ] =  2;
    procsPerDim[ 6 ] =  platform_tofu_num_cpus();

    for ( int i = 0; i < number_of_dimensions - 1; i++ )
    {
        periodicityPerDim[ i ] = 1;
    }
    periodicityPerDim[ 6 ] = 0;

    return SCOREP_SUCCESS;
}


SCOREP_ErrorCode
SCOREP_Platform_GetCartCoords( int                     nCoords,
                               int*                    coords,
                               struct SCOREP_Location* location )
{
    UTILS_DEBUG_ENTRY();

    if ( ( SCOREP_Status_IsMpp() && !SCOREP_Status_IsMppInitialized() ) || ( SCOREP_Location_GetType( location ) !=  SCOREP_LOCATION_TYPE_CPU_THREAD ) )
    {
        return SCOREP_ERROR_INVALID;
    }

    UTILS_BUG_ON( nCoords != number_of_dimensions, "Length of coordinate array differs from number of dimensions!" );

    int      rank = SCOREP_Status_GetRank();
    uint32_t cpu  = get_subsystem_hardware_thread_id( location );


    int rc = FJMPI_Topology_sys_rank2xyzabc( rank, &coords[ 0 ], &coords[ 1 ], &coords[ 2 ], &coords[ 3 ], &coords[ 4 ], &coords[ 5 ] );
    UTILS_BUG_ON( rc != 0, "FJMPI_Topology_sys_rank2xyzabc for gathering coordinate data failed with %d.", rc );

    coords[ 6 ] = ( int )cpu;

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_TOPOLOGIES, "[tofu] coords: rank=%d, thread=%d (%d, %d, %d, %d, %d, %d, %d)\n",
                        rank, cpu, coords[ 0 ], coords[ 1 ], coords[ 2 ], coords[ 3 ], coords[ 4 ], coords[ 5 ], coords[ 6 ] );

    return SCOREP_SUCCESS;
}

static void
update_hardware_thread_id( SCOREP_Location* location )
{
    hardware_location_subsystem_data* subsystem_data = SCOREP_Location_GetSubsystemData( location, subsystem_id );
    if ( subsystem_data == NULL )
    {
        /* first initialization */
        subsystem_data = SCOREP_Location_AllocForMisc( location, sizeof( *subsystem_data ) );
    }
    subsystem_data->hardware_thread_id = get_hardware_thread_id();
    SCOREP_Location_SetSubsystemData( location, subsystem_id, subsystem_data );
}

static uint32_t
get_subsystem_hardware_thread_id( SCOREP_Location* location )
{
    hardware_location_subsystem_data* subsystem_data = SCOREP_Location_GetSubsystemData( location, subsystem_id );
    if ( subsystem_data == NULL )
    {
        /* first initialization */
        subsystem_data                     = SCOREP_Location_AllocForMisc( location, sizeof( *subsystem_data ) );
        subsystem_data->hardware_thread_id = get_hardware_thread_id();
        SCOREP_Location_SetSubsystemData( location, subsystem_id, subsystem_data );
    }
    return subsystem_data->hardware_thread_id;
}

static SCOREP_ErrorCode
platform_subsystem_register( size_t subsystemId )
{
    UTILS_DEBUG_ENTRY();

    subsystem_id = subsystemId;
    return SCOREP_SUCCESS;
}

static SCOREP_ErrorCode
platform_subsystem_activate_cpu_location( SCOREP_Location*        location,
                                          SCOREP_Location*        parentLocation,
                                          uint32_t                forksequenceCount,
                                          SCOREP_CPULocationPhase phase )
{
    UTILS_DEBUG_ENTRY();

    update_hardware_thread_id( location );
    return SCOREP_SUCCESS;
}

static void
platform_subsystem_end( void )
{
    UTILS_DEBUG_ENTRY();

    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    update_hardware_thread_id( location );
}

const SCOREP_Subsystem SCOREP_Subsystem_PlatformTopology =
{
    .subsystem_name                  = "Platform Topology (tofu)",
    .subsystem_register              = &platform_subsystem_register,
    .subsystem_activate_cpu_location = &platform_subsystem_activate_cpu_location,
    .subsystem_end                   = &platform_subsystem_end,
};
