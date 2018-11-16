/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2015
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2015, 2017,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */

/**
 *  @file
 *
 *  This file contains the implementation of the initialization functions of the user
 *  adapter.
 */


#include <config.h>


#include "SCOREP_User_Init.h"
#include "SCOREP_Types.h"
#include <SCOREP_Location.h>


#include "scorep_user.h"
#include "scorep_selective_region.h"

#include "scorep_user_topology_confvars.inc.c"

/* size of hash map for the regions tagged by name */
#define SCOREP_USER_REGION_BY_NAME_SIZE 128

static size_t user_subsystem_id;


/** Registers the required configuration variables of the user adapter
    to the measurement system. Currently, it registers no variables.
 */
static SCOREP_ErrorCode
user_subsystem_register( size_t subsystemId )
{
    user_subsystem_id = subsystemId;

    scorep_selective_register();
    SCOREP_ConfigRegister( "topology", scorep_user_topology_confvars );

    return SCOREP_SUCCESS;
}

/** Initializes the user adapter.
 */
static SCOREP_ErrorCode
user_subsystem_init( void )
{
    scorep_selective_init();
    scorep_user_init_regions();
    scorep_user_init_metrics();

    return SCOREP_SUCCESS;
}

/** Initializes the location specific data of the user adapter */
static SCOREP_ErrorCode
user_subsystem_init_location( SCOREP_Location* locationData,
                              SCOREP_Location* parent )
{
    return SCOREP_SUCCESS;
}

/** Finalizes the user adapter.
 */
static void
user_subsystem_finalize( void )
{
    scorep_user_finalize_metrics();
    scorep_user_finalize_regions();
    scorep_selective_finalize();
}


const SCOREP_Subsystem SCOREP_Subsystem_UserAdapter =
{
    .subsystem_name          = "USER",
    .subsystem_register      = &user_subsystem_register,
    .subsystem_init          = &user_subsystem_init,
    .subsystem_init_location = &user_subsystem_init_location,
    .subsystem_finalize      = &user_subsystem_finalize,
    .subsystem_dump_manifest = &scorep_user_subsystem_dump_manifest,
};


/**
   Mutex to avoid parallel assignments to the same user metric.
 */
SCOREP_Mutex scorep_user_metric_mutex = SCOREP_INVALID_MUTEX;


/** Mutex to avoid parallel allocation of local topology data
 */
SCOREP_Mutex scorep_user_topo_mutex = SCOREP_INVALID_MUTEX;


void
scorep_user_init_metrics( void )
{
    SCOREP_MutexCreate( &scorep_user_metric_mutex );
    SCOREP_MutexCreate( &scorep_user_topo_mutex );
}

void
scorep_user_finalize_metrics( void )
{
    SCOREP_MutexDestroy( &scorep_user_metric_mutex );
    SCOREP_MutexDestroy( &scorep_user_topo_mutex );
}


/**
   Mutex for @ref scorep_user_file_table.
 */
SCOREP_Mutex scorep_user_file_table_mutex = SCOREP_INVALID_MUTEX;

/**
   Mutex to avoid parallel assignement of region handles to the same region.
 */
SCOREP_Mutex scorep_user_region_mutex         = SCOREP_INVALID_MUTEX;
SCOREP_Mutex scorep_user_region_by_name_mutex = SCOREP_INVALID_MUTEX;

/**
    @internal
    Hash table for mapping regions names to the User adapter region structs.
    Needed for the fortran regions which cannot be initialized in declaration. We can
    not determine by the handle value whether we initialized the region already. Thus, we need
    to lookup the name in an extra data structure.
 */
SCOREP_Hashtab* scorep_user_region_table = NULL;

/*
 * Hastable for the regions by name
 */
SCOREP_Hashtab* scorep_user_region_by_name_hash_table = NULL;

void
scorep_user_init_regions( void )
{
    SCOREP_MutexCreate( &scorep_user_region_mutex );
    SCOREP_MutexCreate( &scorep_user_region_by_name_mutex );
    SCOREP_MutexCreate( &scorep_user_file_table_mutex );
    scorep_user_region_table = SCOREP_Hashtab_CreateSize( 10, &SCOREP_Hashtab_HashString,
                                                          &SCOREP_Hashtab_CompareStrings );
    /* create empty hashtab for regions by name */
    scorep_user_region_by_name_hash_table = SCOREP_Hashtab_CreateSize( SCOREP_USER_REGION_BY_NAME_SIZE,
                                                                       &SCOREP_Hashtab_HashString,
                                                                       &SCOREP_Hashtab_CompareStrings );
}

void
scorep_user_finalize_regions( void )
{
    /* the value entry is stored in a structure that is allocated with the scorep
       memory management system. Thus, it must not free the value. */
    SCOREP_Hashtab_FreeAll( scorep_user_region_table,
                            &SCOREP_Hashtab_DeleteFree,
                            &SCOREP_Hashtab_DeleteNone );

    SCOREP_Hashtab_FreeAll( scorep_user_region_by_name_hash_table,
                            &SCOREP_Hashtab_DeleteNone,
                            &SCOREP_Hashtab_DeleteNone );

    scorep_user_region_table = NULL;
    SCOREP_MutexDestroy( &scorep_user_file_table_mutex );
    SCOREP_MutexDestroy( &scorep_user_region_mutex );
    SCOREP_MutexDestroy( &scorep_user_region_by_name_mutex );
}
