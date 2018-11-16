/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2015
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014, 2017,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */

#ifndef SCOREP_USER_INIT_H
#define SCOREP_USER_INIT_H

/** @file

    This file contains the declaration of the initialization struct of the user
    adapter.
 */


#include "SCOREP_Subsystem.h"
#include <SCOREP_Mutex.h>
#include "SCOREP_Hashtab.h"
#include <stdbool.h>

/** @ingroup SCOREP_User_External
    @{
 */

/** The struct which contains the initialization functions for the user adapter.
 */
extern const SCOREP_Subsystem SCOREP_Subsystem_UserAdapter;

/**
   Mutex to avoid parallel assignments to the same user metric.
 */
extern SCOREP_Mutex scorep_user_metric_mutex;

/**
   Mutex for @ref scorep_user_file_table.
 */
extern SCOREP_Mutex scorep_user_file_table_mutex;

/**
   Mutex to avoid parallel assignement of region handles to the same region.
 */
extern SCOREP_Mutex scorep_user_region_mutex;
extern SCOREP_Mutex scorep_user_region_by_name_mutex;

/**
   Maximum size of by name region hash table
 */
extern SCOREP_Hashtab* scorep_user_region_by_name_hash_table;

/**
    @internal
    Hash table for mapping regions names to the User adapte region structs.
    Needed for the fortran regions which cannot be initialized in declaration. We can
    not determine by the handle value whether we initialized the region already. Thus, we need
    to lookup the name in a extra datastructure.
 */
extern SCOREP_Hashtab* scorep_user_region_table;

/**
   Variable storing the information on the recording of user topologies.
 */
extern bool scorep_user_enable_topologies;

/**
   Type for topology handles
 */
typedef struct SCOREP_User_Topology
{
    char*            name;
    SCOREP_AnyHandle handle;
    uint32_t         n_dims;
    int*             dim_sizes;
    int*             dim_periodicity;
    char**           dim_names;

    bool             initialized;
    uint32_t         dimension_counter;
} SCOREP_User_Topology;

/** Mutex to avoid parallel allocation of local topology data
 */
extern SCOREP_Mutex scorep_user_topo_mutex;

/**
   Dumps file information into the manifest file and copies files depending on the
   SCOREP_SELECTIVE_CONFIG_FILE
 */
void
scorep_user_subsystem_dump_manifest( FILE*       manifestFile,
                                     const char* relativeSourceDir,
                                     const char* targetDir );

/** @} */

#endif /* SCOREP_USER_INIT_H */
