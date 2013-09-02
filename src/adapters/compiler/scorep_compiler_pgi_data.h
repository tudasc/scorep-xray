/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
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
 * @file       scorep_compiler_pgi_data.h
 *
 *
 * Contains helper data structures which are used by the PGI compiler adapter.
 */


#ifndef SCOREP_COMPILER_PGI_DATA_
#define SCOREP_COMPILER_PGI_DATA_

#include <stdint.h>

#include <SCOREP_Definitions.h>


/**
    Defines the maximum size of a callstack.
 */
#define CALLSTACK_MAX 128

/**
   Contains the callstack data for each location.
 */
typedef struct
{
    /**
        Pointer to the callstack starting position.
     */
    SCOREP_RegionHandle callstack_base[ CALLSTACK_MAX ];

    /**
        Counts the current level of nesting.
     */
    uint32_t callstack_count;

    /**
        Current callstack position.
     */
    uint32_t callstack_top;
} pgi_location_data;


#endif /* SCOREP_COMPILER_PGI_DATA_ */
