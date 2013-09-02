/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
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
 *  @file       src/adapters/user/SCOREP_User_Init.c
 *
 *  This file contains the implmentation of the initialization functions of the user
 *  adapter.
 */


#include <config.h>


#include "SCOREP_User_Init.h"
#include "SCOREP_Types.h"
#include <SCOREP_Location.h>


#include "scorep_user.h"


int8_t scorep_user_is_initialized = 0;


static size_t scorep_user_subsystem_id;


/** Registers the required configuration variables of the user adapter
    to the measurement system. Currently, it registers no variables.
 */
static SCOREP_ErrorCode
scorep_user_register( size_t subsystem_id )
{
    scorep_user_subsystem_id = subsystem_id;

    return scorep_selective_register();
}

/** Initializes the user adapter.
 */
static SCOREP_ErrorCode
scorep_user_init( void )
{
    if ( scorep_user_is_initialized == 0 )
    {
        /* Set the intialization flag to indicate that the adapter is initialized */
        scorep_user_is_initialized = 1;

        scorep_selective_init();
        scorep_user_init_regions();
        scorep_user_init_metrics();
    }
    return SCOREP_SUCCESS;
}

/** Initializes the location specific data of the user adapter */
static SCOREP_ErrorCode
scorep_user_init_location( SCOREP_Location* locationData )
{
    return SCOREP_SUCCESS;
}

/** Finalizes the location specific data of the user adapter.
 */
static void
scorep_user_finalize_location( SCOREP_Location* locationData )
{
}

/** Finalizes the user adapter.
 */
static void
scorep_user_finalize( void )
{
    if ( scorep_user_is_initialized == 1 )
    {
        /*  Set the intialization flag to indicate that the adapter is finalized */
        scorep_user_is_initialized = 2;
        scorep_user_finalize_metrics();
        scorep_user_finalize_regions();
    }
    scorep_selective_finalize();
}

/** Deregisters the user adapter.
 */
static void
scorep_user_deregister( void )
{
}

const SCOREP_Subsystem SCOREP_Subsystem_UserAdapter =
{
    .subsystem_name              = "USER",
    .subsystem_register          = &scorep_user_register,
    .subsystem_init              = &scorep_user_init,
    .subsystem_init_location     = &scorep_user_init_location,
    .subsystem_finalize_location = &scorep_user_finalize_location,
    .subsystem_pre_unify         = NULL,
    .subsystem_post_unify        = NULL,
    .subsystem_finalize          = &scorep_user_finalize,
    .subsystem_deregister        = &scorep_user_deregister,
    .subsystem_control           = NULL
};


/**
   Mutex to avoid parallel assignments to the same user metric.
 */
SCOREP_Mutex scorep_user_metric_mutex;


void
scorep_user_init_metrics( void )
{
    SCOREP_MutexCreate( &scorep_user_metric_mutex );
}

void
scorep_user_finalize_metrics( void )
{
    SCOREP_MutexDestroy( &scorep_user_metric_mutex );
}


/**
   Mutex for @ref scorep_user_file_table.
 */
SCOREP_Mutex scorep_user_file_table_mutex;

/**
   Mutex to avoid parallel assignement of region handles to the same region.
 */
SCOREP_Mutex scorep_user_region_mutex;

/**
    @internal
    Hash table for mapping regions names to the User adapter region structs.
    Needed for the fortran regions which can not be initialized in declaration. We can
    not determine by the handle value whether we initialized the region already. Thus, we need
    to lookup the name in an extra data structure.
 */
SCOREP_Hashtab* scorep_user_region_table = NULL;


void
scorep_user_init_regions( void )
{
    SCOREP_MutexCreate( &scorep_user_region_mutex );
    SCOREP_MutexCreate( &scorep_user_file_table_mutex );
    scorep_user_region_table = SCOREP_Hashtab_CreateSize( 10, &SCOREP_Hashtab_HashString,
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

    scorep_user_region_table = NULL;
    SCOREP_MutexDestroy( &scorep_user_file_table_mutex );
    SCOREP_MutexDestroy( &scorep_user_region_mutex );
}
