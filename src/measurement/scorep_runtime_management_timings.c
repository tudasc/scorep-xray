/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014,
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
 *
 */


/**
 * @file
 *
 *
 */

#include <config.h>
#include "scorep_runtime_management_timings.h"

#if HAVE( SCOREP_RUNTIME_MANAGEMENT_TIMINGS )

const char* scorep_timing_function_names[ scorep_timing_num_entries ] =
{
    #define SCOREP_TIMING_TRANSFORM_OP( function ) #function,
    SCOREP_TIMING_FUNCTIONS
    #undef SCOREP_TIMING_TRANSFORM_OP
};

uint64_t timing_start_MeasurementDuration;
double   scorep_timing_sendbuf_durations[ scorep_timing_num_entries ];
double   scorep_timing_recvbuf_durations_mean[ scorep_timing_num_entries ];
double   scorep_timing_recvbuf_durations_max[ scorep_timing_num_entries ];
double   scorep_timing_recvbuf_durations_min[ scorep_timing_num_entries ];

#endif
