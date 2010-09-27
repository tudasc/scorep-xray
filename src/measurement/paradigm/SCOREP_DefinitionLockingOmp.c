/*
 * This file is part of the SCOREP project (http://www.scorep.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


/**
 * @file       SCOREP_DefinitionLockingOmp.c
 *             Implementation of SCOREP_DefinitionLocking.h in the OpenMP
 *             case.
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 *
 */


#include <config.h>

#include <SCOREP_DefinitionLocking.h>
#include <scorep_definition_locking.h>

#include <omp.h>


static omp_lock_t scorep_source_file_definition_lock;
static omp_lock_t scorep_region_definition_lock;
static omp_lock_t scorep_mpi_communicator_definition_lock;
static omp_lock_t scorep_mpi_window_definition_lock;
static omp_lock_t scorep_mpi_cartesian_topology_definition_lock;
static omp_lock_t scorep_mpi_cartesian_coords_definition_lock;
static omp_lock_t scorep_counter_group_definition_lock;
static omp_lock_t scorep_counter_definition_lock;
static omp_lock_t scorep_io_file_group_definition_lock;
static omp_lock_t scorep_io_file_definition_lock;
static omp_lock_t scorep_marker_group_definition_lock;
static omp_lock_t scorep_marker_definition_lock;
static omp_lock_t scorep_parameter_definition_lock;
static omp_lock_t scorep_location_definition_lock;


void
SCOREP_DefinitionLocks_Initialize()
{
    omp_init_lock( &scorep_source_file_definition_lock );
    omp_init_lock( &scorep_region_definition_lock );
    omp_init_lock( &scorep_mpi_communicator_definition_lock );
    omp_init_lock( &scorep_mpi_window_definition_lock );
    omp_init_lock( &scorep_mpi_cartesian_topology_definition_lock );
    omp_init_lock( &scorep_mpi_cartesian_coords_definition_lock );
    omp_init_lock( &scorep_counter_group_definition_lock );
    omp_init_lock( &scorep_counter_definition_lock );
    omp_init_lock( &scorep_io_file_group_definition_lock );
    omp_init_lock( &scorep_io_file_definition_lock );
    omp_init_lock( &scorep_marker_group_definition_lock );
    omp_init_lock( &scorep_marker_definition_lock );
    omp_init_lock( &scorep_parameter_definition_lock );
    omp_init_lock( &scorep_location_definition_lock );
}


void
SCOREP_DefinitionLocks_Finalize()
{
#if defined ( __INTEL_COMPILER ) && ( __INTEL_COMPILER < 1120 )
    // Do nothing here. Intel OpenMP RTL shuts down at the end of main
    // function, so omp_destroy_lock, which is called after the end
    // of main from the atexit handler, causes segmentation fault. The
    // problem will be fixed in  Intel Compiler 11.1 update 6.
    // See http://software.intel.com/en-us/forums/showpost.php?p=110592
#else
    omp_destroy_lock( &scorep_source_file_definition_lock );
    omp_destroy_lock( &scorep_region_definition_lock );
    omp_destroy_lock( &scorep_mpi_communicator_definition_lock );
    omp_destroy_lock( &scorep_mpi_window_definition_lock );
    omp_destroy_lock( &scorep_mpi_cartesian_topology_definition_lock );
    omp_destroy_lock( &scorep_mpi_cartesian_coords_definition_lock );
    omp_destroy_lock( &scorep_counter_group_definition_lock );
    omp_destroy_lock( &scorep_counter_definition_lock );
    omp_destroy_lock( &scorep_io_file_group_definition_lock );
    omp_destroy_lock( &scorep_io_file_definition_lock );
    omp_destroy_lock( &scorep_marker_group_definition_lock );
    omp_destroy_lock( &scorep_marker_definition_lock );
    omp_destroy_lock( &scorep_parameter_definition_lock );
    omp_destroy_lock( &scorep_location_definition_lock );
#endif
}


void
SCOREP_LockSourceFileDefinition()
{
    omp_set_lock( &scorep_source_file_definition_lock );
}

void
SCOREP_UnlockSourceFileDefinition()
{
    omp_unset_lock( &scorep_source_file_definition_lock );
}


void
SCOREP_LockRegionDefinition()
{
    omp_set_lock( &scorep_region_definition_lock );
}

void
SCOREP_UnlockRegionDefinition()
{
    omp_unset_lock( &scorep_region_definition_lock );
}


void
SCOREP_LockMPICommunicatorDefinition()
{
    omp_set_lock( &scorep_mpi_communicator_definition_lock );
}

void
SCOREP_UnlockMPICommunicatorDefinition()
{
    omp_unset_lock( &scorep_mpi_communicator_definition_lock );
}


void
SCOREP_LockMPIWindowDefinition()
{
    omp_set_lock( &scorep_mpi_window_definition_lock );
}

void
SCOREP_UnlockMPIWindowDefinition()
{
    omp_unset_lock( &scorep_mpi_window_definition_lock );
}


void
SCOREP_LockMPICartesianTopologyDefinition()
{
    omp_set_lock( &scorep_mpi_cartesian_topology_definition_lock );
}

void
SCOREP_UnlockMPICartesianTopologyDefinition()
{
    omp_unset_lock( &scorep_mpi_cartesian_topology_definition_lock );
}


void
SCOREP_LockMPICartesianCoordsDefinition()
{
    omp_set_lock( &scorep_mpi_cartesian_coords_definition_lock );
}

void
SCOREP_UnlockMPICartesianCoordsDefinition()
{
    omp_unset_lock( &scorep_mpi_cartesian_coords_definition_lock );
}


void
SCOREP_LockCounterGroupDefinition()
{
    omp_set_lock( &scorep_counter_group_definition_lock );
}

void
SCOREP_UnlockCounterGroupDefinition()
{
    omp_unset_lock( &scorep_counter_group_definition_lock );
}


void
SCOREP_LockCounterDefinition()
{
    omp_set_lock( &scorep_counter_definition_lock );
}

void
SCOREP_UnlockCounterDefinition()
{
    omp_unset_lock( &scorep_counter_definition_lock );
}


void
SCOREP_LockIOFileGroupDefinition()
{
    omp_set_lock( &scorep_io_file_group_definition_lock );
}

void
SCOREP_UnlockIOFileGroupDefinition()
{
    omp_unset_lock( &scorep_io_file_group_definition_lock );
}


void
SCOREP_LockIOFileDefinition()
{
    omp_set_lock( &scorep_io_file_definition_lock );
}

void
SCOREP_UnlockIOFileDefinition()
{
    omp_unset_lock( &scorep_io_file_definition_lock );
}


void
SCOREP_LockMarkerGroupDefinition()
{
    omp_set_lock( &scorep_marker_group_definition_lock );
}

void
SCOREP_UnlockMarkerGroupDefinition()
{
    omp_unset_lock( &scorep_marker_group_definition_lock );
}


void
SCOREP_LockMarkerDefinition()
{
    omp_set_lock( &scorep_marker_definition_lock );
}

void
SCOREP_UnlockMarkerDefinition()
{
    omp_unset_lock( &scorep_marker_definition_lock );
}


void
SCOREP_LockParameterDefinition()
{
    omp_set_lock( &scorep_parameter_definition_lock );
}

void
SCOREP_UnlockParameterDefinition()
{
    omp_unset_lock( &scorep_parameter_definition_lock );
}


void
SCOREP_LockLocationDefinition()
{
    omp_set_lock( &scorep_location_definition_lock );
}

void
SCOREP_UnlockLocationDefinition()
{
    omp_unset_lock( &scorep_location_definition_lock );
}
