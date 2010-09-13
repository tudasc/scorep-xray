/*
 * This file is part of the SILC project (http://www.silc.de)
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
 * @file       SILC_DefinitionLockingOmp.c
 *             Implementation of SILC_DefinitionLocking.h in the OpenMP
 *             case.
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 *
 */


#include <config.h>

#include <SILC_DefinitionLocking.h>
#include <silc_definition_locking.h>

#include <omp.h>


static omp_lock_t silc_source_file_definition_lock;
static omp_lock_t silc_region_definition_lock;
static omp_lock_t silc_mpi_communicator_definition_lock;
static omp_lock_t silc_mpi_window_definition_lock;
static omp_lock_t silc_mpi_cartesian_topology_definition_lock;
static omp_lock_t silc_mpi_cartesian_coords_definition_lock;
static omp_lock_t silc_counter_group_definition_lock;
static omp_lock_t silc_counter_definition_lock;
static omp_lock_t silc_io_file_group_definition_lock;
static omp_lock_t silc_io_file_definition_lock;
static omp_lock_t silc_marker_group_definition_lock;
static omp_lock_t silc_marker_definition_lock;
static omp_lock_t silc_parameter_definition_lock;
static omp_lock_t silc_location_definition_lock;


void
SILC_DefinitionLocks_Initialize()
{
    omp_init_lock( &silc_source_file_definition_lock );
    omp_init_lock( &silc_region_definition_lock );
    omp_init_lock( &silc_mpi_communicator_definition_lock );
    omp_init_lock( &silc_mpi_window_definition_lock );
    omp_init_lock( &silc_mpi_cartesian_topology_definition_lock );
    omp_init_lock( &silc_mpi_cartesian_coords_definition_lock );
    omp_init_lock( &silc_counter_group_definition_lock );
    omp_init_lock( &silc_counter_definition_lock );
    omp_init_lock( &silc_io_file_group_definition_lock );
    omp_init_lock( &silc_io_file_definition_lock );
    omp_init_lock( &silc_marker_group_definition_lock );
    omp_init_lock( &silc_marker_definition_lock );
    omp_init_lock( &silc_parameter_definition_lock );
    omp_init_lock( &silc_location_definition_lock );
}


void
SILC_DefinitionLocks_Finalize()
{
#if defined ( __INTEL_COMPILER ) && ( __INTEL_COMPILER < 1120 )
    // Do nothing here. Intel OpenMP RTL shuts down at the end of main
    // function, so omp_destroy_lock, which is called after the end
    // of main from the atexit handler, causes segmentation fault. The
    // problem will be fixed in  Intel Compiler 11.1 update 6.
    // See http://software.intel.com/en-us/forums/showpost.php?p=110592
#else
    omp_destroy_lock( &silc_source_file_definition_lock );
    omp_destroy_lock( &silc_region_definition_lock );
    omp_destroy_lock( &silc_mpi_communicator_definition_lock );
    omp_destroy_lock( &silc_mpi_window_definition_lock );
    omp_destroy_lock( &silc_mpi_cartesian_topology_definition_lock );
    omp_destroy_lock( &silc_mpi_cartesian_coords_definition_lock );
    omp_destroy_lock( &silc_counter_group_definition_lock );
    omp_destroy_lock( &silc_counter_definition_lock );
    omp_destroy_lock( &silc_io_file_group_definition_lock );
    omp_destroy_lock( &silc_io_file_definition_lock );
    omp_destroy_lock( &silc_marker_group_definition_lock );
    omp_destroy_lock( &silc_marker_definition_lock );
    omp_destroy_lock( &silc_parameter_definition_lock );
    omp_destroy_lock( &silc_location_definition_lock );
#endif
}


void
SILC_LockSourceFileDefinition()
{
    omp_set_lock( &silc_source_file_definition_lock );
}

void
SILC_UnlockSourceFileDefinition()
{
    omp_unset_lock( &silc_source_file_definition_lock );
}


void
SILC_LockRegionDefinition()
{
    omp_set_lock( &silc_region_definition_lock );
}

void
SILC_UnlockRegionDefinition()
{
    omp_unset_lock( &silc_region_definition_lock );
}


void
SILC_LockMPICommunicatorDefinition()
{
    omp_set_lock( &silc_mpi_communicator_definition_lock );
}

void
SILC_UnlockMPICommunicatorDefinition()
{
    omp_unset_lock( &silc_mpi_communicator_definition_lock );
}


void
SILC_LockMPIWindowDefinition()
{
    omp_set_lock( &silc_mpi_window_definition_lock );
}

void
SILC_UnlockMPIWindowDefinition()
{
    omp_unset_lock( &silc_mpi_window_definition_lock );
}


void
SILC_LockMPICartesianTopologyDefinition()
{
    omp_set_lock( &silc_mpi_cartesian_topology_definition_lock );
}

void
SILC_UnlockMPICartesianTopologyDefinition()
{
    omp_unset_lock( &silc_mpi_cartesian_topology_definition_lock );
}


void
SILC_LockMPICartesianCoordsDefinition()
{
    omp_set_lock( &silc_mpi_cartesian_coords_definition_lock );
}

void
SILC_UnlockMPICartesianCoordsDefinition()
{
    omp_unset_lock( &silc_mpi_cartesian_coords_definition_lock );
}


void
SILC_LockCounterGroupDefinition()
{
    omp_set_lock( &silc_counter_group_definition_lock );
}

void
SILC_UnlockCounterGroupDefinition()
{
    omp_unset_lock( &silc_counter_group_definition_lock );
}


void
SILC_LockCounterDefinition()
{
    omp_set_lock( &silc_counter_definition_lock );
}

void
SILC_UnlockCounterDefinition()
{
    omp_unset_lock( &silc_counter_definition_lock );
}


void
SILC_LockIOFileGroupDefinition()
{
    omp_set_lock( &silc_io_file_group_definition_lock );
}

void
SILC_UnlockIOFileGroupDefinition()
{
    omp_unset_lock( &silc_io_file_group_definition_lock );
}


void
SILC_LockIOFileDefinition()
{
    omp_set_lock( &silc_io_file_definition_lock );
}

void
SILC_UnlockIOFileDefinition()
{
    omp_unset_lock( &silc_io_file_definition_lock );
}


void
SILC_LockMarkerGroupDefinition()
{
    omp_set_lock( &silc_marker_group_definition_lock );
}

void
SILC_UnlockMarkerGroupDefinition()
{
    omp_unset_lock( &silc_marker_group_definition_lock );
}


void
SILC_LockMarkerDefinition()
{
    omp_set_lock( &silc_marker_definition_lock );
}

void
SILC_UnlockMarkerDefinition()
{
    omp_unset_lock( &silc_marker_definition_lock );
}


void
SILC_LockParameterDefinition()
{
    omp_set_lock( &silc_parameter_definition_lock );
}

void
SILC_UnlockParameterDefinition()
{
    omp_unset_lock( &silc_parameter_definition_lock );
}


void
SILC_LockLocationDefinition()
{
    omp_set_lock( &silc_location_definition_lock );
}

void
SILC_UnlockLocationDefinition()
{
    omp_unset_lock( &silc_location_definition_lock );
}
