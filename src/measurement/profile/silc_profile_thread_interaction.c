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
 * @file       silc_profile_thread_interaction.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status ALPHA
 *
 *
 */


#include "silc_profile_thread_interaction.h"

#include <SILC_Memory.h>



/**
 * Allocate and initialize a valid SILC_Profile_LocationData object.
 *
 */
SILC_Profile_LocationData*
SILC_Profile_CreateLocationData()
{
    SILC_Profile_LocationData* new_data = SILC_Memory_AllocForMultithreadedMisc( sizeof( SILC_Profile_LocationData ) );
    // create plain object, initialize in SILC_Profile_OnLocationCreation
    return new_data;
}


void
SILC_Profile_DeleteLocationData( SILC_Profile_LocationData* profileLocationData )
{
    if ( profileLocationData )
    {
        // clean up
    }
}


void
SILC_Profile_OnThreadCreation( SILC_Thread_LocationData* locationData,
                               SILC_Thread_LocationData* parentLocationData )
{
}


void
SILC_Profile_OnThreadActivation( SILC_Thread_LocationData* locationData,
                                 SILC_Thread_LocationData* parentLocationData )
{
}


void
SILC_Profile_OnThreadDectivation( SILC_Thread_LocationData* locationData,
                                  SILC_Thread_LocationData* parentLocationData )
{
}


void
SILC_Profile_OnLocationCreation( SILC_Thread_LocationData* locationData,
                                 SILC_Thread_LocationData* parentLocationData )
{
}
