/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
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
 * @status      alpha
 * @file        SCOREP_Profile_Tasking.c
 * @maintainer  Dirk Schmidl <schmidl@rz.rwth-aachen.de>
 *
 * @brief   Implementation of the Tasking functions of the profile interface
 *
 */

#include <config.h>
#include <SCOREP_Profile.h>
#include <SCOREP_Profile_Tasking.h>
#include <scorep_utility/SCOREP_Utils.h>

#include <scorep_profile_definition.h>

void
SCOREP_Profile_TaskCreateBegin( SCOREP_RegionHandle regionHandle,
                                uint64_t            taskId,
                                uint64_t            timestamp )
{
    if ( scorep_profile_is_initialized )
    {
        SCOREP_WARN_ONCE( "Tasking is not yet supported in profiling mode.\n"
                          "Disabled profiling because a tasking event occured." );
        SCOREP_PROFILE_STOP;
    }
}

void
SCOREP_Profile_TaskCreateEnd( SCOREP_RegionHandle regionHandle,
                              uint64_t            taskId,
                              uint64_t            timestamp )
{
    if ( scorep_profile_is_initialized )
    {
        SCOREP_WARN_ONCE( "Tasking is not yet supported in profiling mode.\n"
                          "Disabled profiling because a tasking event occured." );
        SCOREP_PROFILE_STOP;
    }
}

void
SCOREP_Profile_TaskBegin( SCOREP_RegionHandle regionHandle,
                          uint64_t            taskId,
                          uint64_t            timestamp )
{
    if ( scorep_profile_is_initialized )
    {
        SCOREP_WARN_ONCE( "Tasking is not yet supported in profiling mode.\n"
                          "Disabled profiling because a tasking event occured." );
        SCOREP_PROFILE_STOP;
    }
}

void
SCOREP_Profile_TaskResume( SCOREP_RegionHandle regionHandle,
                           uint64_t            taskId,
                           uint64_t            timestamp )
{
    if ( scorep_profile_is_initialized )
    {
        SCOREP_WARN_ONCE( "Tasking is not yet supported in profiling mode.\n"
                          "Disabled profiling because a tasking event occured." );
        SCOREP_PROFILE_STOP;
    }
}

void
SCOREP_Profile_TaskCompleted( SCOREP_RegionHandle regionHandle,
                              uint64_t            timestamp )
{
    if ( scorep_profile_is_initialized )
    {
        SCOREP_WARN_ONCE( "Tasking is not yet supported in profiling mode.\n"
                          "Disabled profiling because a tasking event occured." );
        SCOREP_PROFILE_STOP;
    }
}
