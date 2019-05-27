/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2015, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2017,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
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
 * @brief       Exports the subsystems array for the measurement system.
 */


#include <config.h>
#include "scorep_subsystem_management.h"

#include <stdlib.h>
#include <stdio.h>

#include <UTILS_Error.h>

#include <SCOREP_Location.h>
#include "scorep_environment.h"


/* scorep_subsystems[] and scorep_number_of_subsystems will be
 * provided by instrumenter or libscorep_confvars.la. */
/* List of linked in subsystems. */
extern const SCOREP_Subsystem* scorep_subsystems[];
extern const size_t            scorep_number_of_subsystems;


size_t
scorep_subsystems_get_number( void )
{
    return scorep_number_of_subsystems;
}


void
scorep_subsystems_register( void )
{
    /* call register functions for all subsystems */
    for ( size_t i = 0; i < scorep_number_of_subsystems; i++ )
    {
        if ( !scorep_subsystems[ i ]->subsystem_register )
        {
            continue;
        }

        SCOREP_ErrorCode error = scorep_subsystems[ i ]->subsystem_register( i );
        if ( SCOREP_SUCCESS != error )
        {
            UTILS_ERROR( error, "Cannot register %s subsystem",
                         scorep_subsystems[ i ]->subsystem_name );
            _Exit( EXIT_FAILURE );
        }
    }
}


void
scorep_subsystems_initialize( void )
{
    /* call initialization functions for all subsystems */
    for ( size_t i = 0; i < scorep_number_of_subsystems; i++ )
    {
        if ( !scorep_subsystems[ i ]->subsystem_init )
        {
            continue;
        }

        SCOREP_ErrorCode error = scorep_subsystems[ i ]->subsystem_init();
        if ( SCOREP_SUCCESS != error )
        {
            UTILS_ERROR( error, "Cannot initialize %s subsystem",
                         scorep_subsystems[ i ]->subsystem_name );
            _Exit( EXIT_FAILURE );
        }
        else if ( SCOREP_Env_RunVerbose() )
        {
            fprintf( stderr, "[Score-P] successfully initialized %s subsystem\n",
                     scorep_subsystems[ i ]->subsystem_name );
        }
    }
}


void
scorep_subsystems_begin( void )
{
    /* call begin functions for all subsystems */
    for ( size_t i = 0; i < scorep_number_of_subsystems; i++ )
    {
        if ( !scorep_subsystems[ i ]->subsystem_begin )
        {
            continue;
        }

        SCOREP_ErrorCode error = scorep_subsystems[ i ]->subsystem_begin();
        if ( SCOREP_SUCCESS != error )
        {
            UTILS_ERROR( error, "Subsystem %s cannot begin measurement",
                         scorep_subsystems[ i ]->subsystem_name );
            _Exit( EXIT_FAILURE );
        }
    }
}


void
scorep_subsystems_initialize_mpp( void )
{
    /* call MPP initialization functions for all subsystems */
    for ( size_t i = 0; i < scorep_number_of_subsystems; i++ )
    {
        if ( !scorep_subsystems[ i ]->subsystem_init_mpp )
        {
            continue;
        }

        SCOREP_ErrorCode error = scorep_subsystems[ i ]->subsystem_init_mpp();
        if ( SCOREP_SUCCESS != error )
        {
            UTILS_ERROR( error, "Cannot mpp-initialize %s subsystem",
                         scorep_subsystems[ i ]->subsystem_name );
            _Exit( EXIT_FAILURE );
        }
    }
}


void
scorep_subsystems_synchronize( SCOREP_SynchronizationMode syncMode )
{
    /* always called in forward order */
    for ( size_t i = 0; i < scorep_number_of_subsystems; i++ )
    {
        if ( !scorep_subsystems[ i ]->subsystem_synchronize )
        {
            continue;
        }

        scorep_subsystems[ i ]->subsystem_synchronize( syncMode );
    }
}


void
scorep_subsystems_end( void )
{
    /* call end functions for all subsystems */
    for ( size_t i = scorep_number_of_subsystems; i-- > 0; )
    {
        if ( !scorep_subsystems[ i ]->subsystem_end )
        {
            continue;
        }

        scorep_subsystems[ i ]->subsystem_end();
    }
}


/**
 * Initialize subsystems for existing locations.
 */
void
scorep_subsystems_initialize_location( SCOREP_Location* newLocation,
                                       SCOREP_Location* parentLocation )
{
    /* call initialization functions for all subsystems */
    for ( size_t i = 0; i < scorep_number_of_subsystems; i++ )
    {
        if ( !scorep_subsystems[ i ]->subsystem_init_location )
        {
            continue;
        }

        SCOREP_ErrorCode error = scorep_subsystems[ i ]->subsystem_init_location( newLocation,
                                                                                  parentLocation );
        if ( SCOREP_SUCCESS != error )
        {
            UTILS_ERROR( error, "Cannot initialize location for %s subsystem",
                         scorep_subsystems[ i ]->subsystem_name );
            _Exit( EXIT_FAILURE );
        }
        else if ( SCOREP_Env_RunVerbose() )
        {
            fprintf( stderr, "[Score-P] successfully initialized location for %s subsystem\n",
                     scorep_subsystems[ i ]->subsystem_name );
        }
    }
}


void
scorep_subsystems_activate_cpu_location( SCOREP_Location*        locationData,
                                         SCOREP_Location*        parentLocation,
                                         uint32_t                forkSequenceCount,
                                         SCOREP_CPULocationPhase phase )
{
    /* call initialization functions for all subsystems */
    for ( size_t i = 0; i < scorep_number_of_subsystems; i++ )
    {
        if ( !scorep_subsystems[ i ]->subsystem_activate_cpu_location )
        {
            continue;
        }

        SCOREP_ErrorCode error =
            scorep_subsystems[ i ]->subsystem_activate_cpu_location(
                locationData,
                parentLocation,
                forkSequenceCount,
                phase );
        if ( SCOREP_SUCCESS != error )
        {
            UTILS_ERROR( error, "Cannot activate CPU location for %s subsystem",
                         scorep_subsystems[ i ]->subsystem_name );
            _Exit( EXIT_FAILURE );
        }
    }
}


void
scorep_subsystems_deactivate_cpu_location( SCOREP_Location*        locationData,
                                           SCOREP_Location*        parentLocation,
                                           SCOREP_CPULocationPhase phase )
{
    /* call initialization functions for all subsystems */
    for ( size_t i = scorep_number_of_subsystems; i-- > 0; )
    {
        if ( !scorep_subsystems[ i ]->subsystem_deactivate_cpu_location )
        {
            continue;
        }

        scorep_subsystems[ i ]->subsystem_deactivate_cpu_location(
            locationData,
            parentLocation,
            phase );
    }
}


void
scorep_subsystems_finalize_location( SCOREP_Location* locationData )
{
    for ( size_t i = scorep_number_of_subsystems; i-- > 0; )
    {
        if ( !scorep_subsystems[ i ]->subsystem_finalize_location )
        {
            continue;
        }

        scorep_subsystems[ i ]->subsystem_finalize_location( locationData );

        if ( SCOREP_Env_RunVerbose() )
        {
            fprintf( stderr, "[Score-P] finalized %s subsystem location\n",
                     scorep_subsystems[ i ]->subsystem_name );
        }
    }
}


/**
 * Called after the unification step.
 */
void
scorep_subsystems_pre_unify( void )
{
    for ( size_t i = 0; i < scorep_number_of_subsystems; i++ )
    {
        if ( !scorep_subsystems[ i ]->subsystem_pre_unify )
        {
            continue;
        }

        SCOREP_ErrorCode error = scorep_subsystems[ i ]->subsystem_pre_unify();
        if ( SCOREP_SUCCESS != error )
        {
            UTILS_ERROR( error, "pre-unify hook failed for %s subsystem",
                         scorep_subsystems[ i ]->subsystem_name );
            _Exit( EXIT_FAILURE );
        }
    }
}


/**
 * Called before the unification process starts.
 */
void
scorep_subsystems_post_unify( void )
{
    /* ?reverse order? */
    for ( size_t i = 0; i < scorep_number_of_subsystems; i++ )
    {
        if ( !scorep_subsystems[ i ]->subsystem_post_unify )
        {
            continue;
        }

        SCOREP_ErrorCode error = scorep_subsystems[ i ]->subsystem_post_unify();
        if ( SCOREP_SUCCESS != error )
        {
            UTILS_ERROR( error, "post-unify hook failed for %s subsystem",
                         scorep_subsystems[ i ]->subsystem_name );
            _Exit( EXIT_FAILURE );
        }
    }
}


void
scorep_subsystems_finalize( void )
{
    /* call finalization functions for all subsystems */
    for ( size_t i = scorep_number_of_subsystems; i-- > 0; )
    {
        if ( !scorep_subsystems[ i ]->subsystem_finalize )
        {
            continue;
        }

        scorep_subsystems[ i ]->subsystem_finalize();

        if ( SCOREP_Env_RunVerbose() )
        {
            fprintf( stderr, "[Score-P] finalized %s subsystem\n",
                     scorep_subsystems[ i ]->subsystem_name );
        }
    }
}


void
scorep_subsystems_deregister( void )
{
    /* call de-register functions for all subsystems */
    for ( size_t i = scorep_number_of_subsystems; i-- > 0; )
    {
        if ( !scorep_subsystems[ i ]->subsystem_deregister )
        {
            continue;
        }

        scorep_subsystems[ i ]->subsystem_deregister();

        if ( SCOREP_Env_RunVerbose() )
        {
            fprintf( stderr, "[Score-P] de-registered %s subsystem\n",
                     scorep_subsystems[ i ]->subsystem_name );
        }
    }
}

void
scorep_subsystems_dump_manifest( FILE*       manifestFile,
                                 const char* relativeSourceDir,
                                 const char* targetDir )
{
    /* call dump measurement functions for all subsystems */
    for ( size_t i = scorep_number_of_subsystems; i-- > 0; )
    {
        if ( !scorep_subsystems[ i ]->subsystem_dump_manifest )
        {
            continue;
        }

        scorep_subsystems[ i ]->subsystem_dump_manifest( manifestFile, relativeSourceDir, targetDir );

        if ( SCOREP_Env_RunVerbose() )
        {
            fprintf( stderr, "[Score-P] dumped measurement information and configs of %s subsystem\n",
                     scorep_subsystems[ i ]->subsystem_name );
        }
    }
}
