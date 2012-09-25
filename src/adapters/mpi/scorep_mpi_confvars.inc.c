/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 *    RWTH Aachen University, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


/**
 * @file       src/adapters/mpi/scorep_mpi_confvars.inc.c
 * @maintainer Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @status alpha
 *
 */



/**
 * @internal
 * Mapping of string keys to enabling group IDs
 * @note The values are sorted in decreasing order, to beautify the debug
 * output. Ie.: if all groups are enabled we get "ALL", because it matches first.
 */
static const SCOREP_ConfigType_SetEntry scorep_mpi_enable_groups[] = {
    { "all",       SCOREP_MPI_ENABLED_ALL       },
    { "cg",        SCOREP_MPI_ENABLED_CG        },
    { "coll",      SCOREP_MPI_ENABLED_COLL      },
    { "default",   SCOREP_MPI_ENABLED_DEFAULT   },
    { "env",       SCOREP_MPI_ENABLED_ENV       },
    { "err",       SCOREP_MPI_ENABLED_ERR       },
    { "ext",       SCOREP_MPI_ENABLED_EXT       },
    { "io",        SCOREP_MPI_ENABLED_IO        },
    { "p2p",       SCOREP_MPI_ENABLED_P2P       },
    { "misc",      SCOREP_MPI_ENABLED_MISC      },
    { "perf",      SCOREP_MPI_ENABLED_PERF      },
    { "rma",       SCOREP_MPI_ENABLED_RMA       },
    { "spawn",     SCOREP_MPI_ENABLED_SPAWN     },
    { "topo",      SCOREP_MPI_ENABLED_TOPO      },
    { "type",      SCOREP_MPI_ENABLED_TYPE      },
    { "xnonblock", SCOREP_MPI_ENABLED_XNONBLOCK },
    { "xreqtest",  SCOREP_MPI_ENABLED_XREQTEST  },
    { NULL,        0                            }
};

/**
   @internal
   Configuration variable that determined whether hierarchical unify is used.
   It can be set via environment variable SCOREP_MPI_HIERARCHICAL_UNIFY.
 */
bool scorep_mpi_hierarchical_unify;

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
   SCOREP_MPI_MAX_ACCESS_EPOCHS.
 */

uint64_t scorep_mpi_max_access_epochs;


/** Bit vector for runtime measurement wrapper enabling/disabling */
uint64_t scorep_mpi_enabled = 0;


bool scorep_mpi_hooks_on = false;


/**
   @internal
   Array of configuration variables.
   They are registered to the measurement system and are filled during until the
   initialization function is called.
 */
SCOREP_ConfigVariable scorep_mpi_configs[] = {
    {
        "hierarchical_unify",
        SCOREP_CONFIG_TYPE_BOOL,
        &scorep_mpi_hierarchical_unify,
        NULL,
        "true",
        "Use the hierarchical MPI unify algorithm",
        ""
    },
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
        "max_access_epochs",
        SCOREP_CONFIG_TYPE_NUMBER,
        &scorep_mpi_max_access_epochs,
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
        ( void* )scorep_mpi_enable_groups,
        "default",
        "The names of the function groups which are measured",
        "Other functions are not measured.\n"
        "Possible groups are:\n"
        "  all:       All MPI functions\n"
        "  cg:        Communicator and group management\n"
        "  coll:      Collective functions\n"
        "  default:   Default configuration\n"
        "  env:       Environmental management\n"
        "  err:       MPI Error handling\n"
        "  ext:       External interface functions\n"
        "  io:        MPI file I/O\n"
        "  misc:      Miscellaneous\n"
        "  perf:      PControl\n"
        "  p2p:       Peer-to-peer communication\n"
        "  rma:       One sided communication\n"
        "  spawn:     Process management\n"
        "  topo:      Topology\n"
        "  type:      MPI datatype functions\n"
        "  xnonblock: Extended non-blocking events\n"
        "  xreqtest:  Test events for uncompleted requests",
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
