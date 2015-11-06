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
 * Copyright (c) 2009-2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013, 2015,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */

/**
 * @file
 *
 *
 */

#ifndef SCOREP_EVENTS_COMMON_H_
#define SCOREP_EVENTS_COMMON_H_


#include <stdbool.h>
#include <SCOREP_Timing.h>
#include "scorep_status.h"
#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_Location.h>


static inline uint64_t
scorep_get_timestamp( SCOREP_Location* location )
{
    uint64_t timestamp = SCOREP_GetClockTicks();
    SCOREP_Location_SetLastTimestamp( location, timestamp );
    return timestamp;
}


#endif /* SCOREP_EVENTS_COMMON_H_ */
