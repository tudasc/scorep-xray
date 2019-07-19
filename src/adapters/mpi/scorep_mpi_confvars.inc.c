/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012, 2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012, 2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 *
 *
 */

#include "SCOREP_Mpi_Reg.h"
#include <SCOREP_Config.h>


/**
 * @internal
 * Mapping of string keys to enabling group IDs
 * @note The values are sorted in decreasing order, to beautify the debug
 * output. Ie.: if all groups are enabled we get "ALL", because it matches first.
 */
static const SCOREP_ConfigType_SetEntry mpi_enable_groups[] = {
    {
        "all",
        SCOREP_MPI_ENABLED_ALL,
        "All MPI functions"
    },
    {
        "cg",
        SCOREP_MPI_ENABLED_CG,
        "Communicator and group management"
    },
    {
        "coll",
        SCOREP_MPI_ENABLED_COLL,
        "Collective functions"
    },
    {
        "default",
        SCOREP_MPI_ENABLED_DEFAULT,
        "Default configuration.\n"
        "Includes:\n"
        "\240-\240cg\n"
        "\240-\240coll\n"
        "\240-\240env\n"
        "\240-\240io\n"
        "\240-\240p2p\n"
        "\240-\240rma\n"
        "\240-\240topo\n"
        "\240-\240xnonblock"
    },
    {
        "env",
        SCOREP_MPI_ENABLED_ENV,
        "Environmental management"
    },
    {
        "err",
        SCOREP_MPI_ENABLED_ERR,
        "MPI Error handling"
    },
    {
        "ext",
        SCOREP_MPI_ENABLED_EXT,
        "External interface functions"
    },
    {
        "io",
        SCOREP_MPI_ENABLED_IO,
        "MPI file I/O"
    },
    {
        "p2p",
        SCOREP_MPI_ENABLED_P2P,
        "Peer-to-peer communication"
    },
    {
        "misc",
        SCOREP_MPI_ENABLED_MISC,
        "Miscellaneous"
    },
    {
        "perf",
        SCOREP_MPI_ENABLED_PERF,
        "PControl"
    },
    {
        "rma",
        SCOREP_MPI_ENABLED_RMA,
        "One sided communication"
    },
    {
        "spawn",
        SCOREP_MPI_ENABLED_SPAWN,
        "Process management"
    },
    {
        "topo",
        SCOREP_MPI_ENABLED_TOPO,
        "Topology"
    },
    {
        "type",
        SCOREP_MPI_ENABLED_TYPE,
        "MPI datatype functions"
    },
    {
        "xnonblock",
        SCOREP_MPI_ENABLED_XNONBLOCK,
        "Extended non-blocking events"
    },
    {
        "xreqtest",
        SCOREP_MPI_ENABLED_XREQTEST,
        "Test events for uncompleted requests"
    },
    { NULL, 0, NULL }
};

/**
   @internal
   Configuration variable for the size of the communicator tracking array.
   Determines the number of cuncurrently trackable communicators per process.
   Can be defined via environment variable SCOREP_MPI_MAX_COMMUNICATORS.
 */
uint64_t scorep_mpi_max_communicators;

/**
   @internal
   Configuration variable for the size of the MPI groups tracking array.
   Determines the number of cuncurrently trackable MPI groups per process.
   Can be defined via environment variable SCOREP_MPI_MAX_GROUPS.
 */
uint64_t scorep_mpi_max_groups;

/**
   @internal
   Configuration variable for the size of the communicator tracking array.
   Determines the number of cuncurrently trackable communicators per process.
   Can be defined via environment variable SCOREP_MPI_MAX_COMMUNICATORS.
 */
uint64_t scorep_mpi_max_windows;

/**
   @internal
   Maximum amount of concurrently active access or exposure epochs per
   process. Can be configured via envrionment variable
   SCOREP_MPI_MAX_EPOCHS.
 */

uint64_t scorep_mpi_max_epochs;


/** Bit vector for runtime measurement wrapper enabling/disabling */
uint64_t scorep_mpi_enabled = 0;


bool scorep_mpi_memory_recording = false;


bool scorep_mpi_hooks_on = false;


/**
   Stores whether recording of MPI topologies is enabled
 */
bool scorep_mpi_enable_topologies;

/**
   @internal
   Array of configuration variables.
   They are registered to the measurement system and are filled during until the
   initialization function is called.
 */
static const SCOREP_ConfigVariable scorep_mpi_confvars[] = {
    {
        "max_communicators",
        SCOREP_CONFIG_TYPE_NUMBER,
        &scorep_mpi_max_communicators,
        NULL,
        "50",
        "Determines the number of concurrently used communicators per process",
        ""
    },
    {
        "max_windows",
        SCOREP_CONFIG_TYPE_NUMBER,
        &scorep_mpi_max_windows,
        NULL,
        "50",
        "Determines the number of concurrently used windows for MPI one-sided "
        "communication per process",
        ""
    },
    {
        "max_epochs",
        SCOREP_CONFIG_TYPE_NUMBER,
        &scorep_mpi_max_epochs,
        NULL,
        "50",
        "Maximum amount of concurrently active access or exposure epochs per "
        "process",
        ""
    },
    {
        "max_groups",
        SCOREP_CONFIG_TYPE_NUMBER,
        &scorep_mpi_max_groups,
        NULL,
        "50",
        "Maximum number of concurrently used MPI groups per process",
        ""
    },
    {
        "enable_groups",
        SCOREP_CONFIG_TYPE_BITSET,
        &scorep_mpi_enabled,
        ( void* )mpi_enable_groups,
        "default",
        "The names of the function groups which are measured",
        "Other functions are not measured.\n"
        "\n"
        "Possible groups are:"
    },
    {
        "memory_recording",
        SCOREP_CONFIG_TYPE_BOOL,
        &scorep_mpi_memory_recording,
        NULL,
        "false",
        "Enable tracking of memory allocations done by calls to MPI_ALLOC_MEM "
        "and MPI_FREE_MEM",
        "Requires that the MISC group is also recorded."
    },
    #if !defined( SCOREP_MPI_NO_HOOKS )
    {
        "online_analysis",
        SCOREP_CONFIG_TYPE_BOOL,
        &scorep_mpi_hooks_on,
        NULL,
        "false",
        "Enable online mpi wait states analysis",
        ""
    },
    #endif
    SCOREP_CONFIG_TERMINATOR
};

static const SCOREP_ConfigVariable scorep_mpi_topology_confvars[] = {
    {
        "mpi",
        SCOREP_CONFIG_TYPE_BOOL,
        &scorep_mpi_enable_topologies,
        NULL,
        "true",
        "Record MPI cartesian topologies.",
        ""
    },
    SCOREP_CONFIG_TERMINATOR
};
