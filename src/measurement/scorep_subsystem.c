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
 * @file        src/measurement/scorep_subsystem.c
 * @maintainer  Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @status      alpha
 *
 * @brief       Exports the subsystems array for the measurement system.
 */


#include <config.h>


#include <stdlib.h>
#include <stdio.h>


#include <UTILS_Error.h>


#include <SCOREP_Location.h>


#include "scorep_subsystem.h"
#include "scorep_environment.h"


/** @brief a NULL terminated list of linked in subsystems. */
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
    SCOREP_ErrorCode error;
    /* call register functions for all subsystems */
    for ( size_t i = 0; i < scorep_number_of_subsystems; i++ )
    {
        if ( scorep_subsystems[ i ]->subsystem_register )
        {
            error = scorep_subsystems[ i ]->subsystem_register( i );
        }

        if ( SCOREP_SUCCESS != error )
        {
            UTILS_ERROR( error, "Can't register %s subsystem",
                         scorep_subsystems[ i ]->subsystem_name );
            _Exit( EXIT_FAILURE );
        }
    }
}


void
scorep_subsystems_initialize( void )
{
    SCOREP_ErrorCode error;
    /* call initialization functions for all subsystems */
    for ( size_t i = 0; i < scorep_number_of_subsystems; i++ )
    {
        if ( scorep_subsystems[ i ]->subsystem_init )
        {
            error = scorep_subsystems[ i ]->subsystem_init();
        }

        if ( SCOREP_SUCCESS != error )
        {
            UTILS_ERROR( error, "Can't initialize %s subsystem",
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


/**
 * Initialize subsystems for existing locations.
 */
void
scorep_subsystems_initialize_location( SCOREP_Location* locationData )
{
    SCOREP_ErrorCode error;
    /* create location */

    /* call initialization functions for all subsystems */
    for ( size_t i = 0; i < scorep_number_of_subsystems; i++ )
    {
        if ( scorep_subsystems[ i ]->subsystem_init_location )
        {
            error = scorep_subsystems[ i ]->subsystem_init_location( locationData );
        }

        if ( SCOREP_SUCCESS != error )
        {
            UTILS_ERROR( error, "Can't initialize location for %s subsystem",
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
scorep_subsystems_finalize_location( SCOREP_Location* locationData )
{
    for ( size_t i = scorep_number_of_subsystems; i-- > 0; )
    {
        if ( scorep_subsystems[ i ]->subsystem_finalize_location )
        {
            scorep_subsystems[ i ]->subsystem_finalize_location( locationData );
        }

        if ( SCOREP_Env_RunVerbose() )
        {
            fprintf( stderr, "[Score-P] finalized %s subsystem location\n",
                     scorep_subsystems[ i ]->subsystem_name );
        }
    }
}


void
scorep_subsystems_finalize( void )
{
    /* call finalization functions for all subsystems */
    for ( size_t i = scorep_number_of_subsystems; i-- > 0; )
    {
        if ( scorep_subsystems[ i ]->subsystem_finalize )
        {
            scorep_subsystems[ i ]->subsystem_finalize();
        }

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
        if ( scorep_subsystems[ i ]->subsystem_deregister )
        {
            scorep_subsystems[ i ]->subsystem_deregister();
        }

        if ( SCOREP_Env_RunVerbose() )
        {
            fprintf( stderr, "[Score-P] de-registered %s subsystem\n",
                     scorep_subsystems[ i ]->subsystem_name );
        }
    }
}
