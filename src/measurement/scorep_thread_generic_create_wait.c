/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 *    RWTH Aachen, Germany
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
 * @file       src/measurement/scorep_thread_generic_create_wait.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include <config.h>
#include "scorep_thread.h"

#include <UTILS_Error.h>


/* *INDENT-OFF* */
/* *INDENT-ON*  */


uint32_t
SCOREP_ThreadCreate( SCOREP_ThreadModel model )
{
    UTILS_NOT_YET_IMPLEMENTED();
}


void
SCOREP_ThreadWait( uint32_t           forkSequenceCount,
                   SCOREP_ThreadModel model )
{
    UTILS_NOT_YET_IMPLEMENTED();
//    if ( forkSequenceCount == SCOREP_THREAD_INVALID_FORK_SEQUENCE_COUNT )
//    {
//        SCOREP_InvalidateProperty( SCOREP_PROPERTY_THREAD_UNIQUE_FORK_SEQUENCE_COUNTS );
//    }
}
