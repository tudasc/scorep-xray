/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012, 2017,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014, 2019,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_SCORE_TYPES_HPP
#define SCOREP_SCORE_TYPES_HPP

/**
 * @file
 *
 * @brief      Defines types for the score tool.
 */

#include <string>
#include <stdint.h>

/* All types after COM, have call-paths to them marked as in COM */
#define SCOREP_SCORE_TYPES \
    SCOREP_SCORE_TYPE( ALL, POSSIBLE )  \
    SCOREP_SCORE_TYPE( FLT, YES )       \
    SCOREP_SCORE_TYPE( USR, POSSIBLE )  \
    SCOREP_SCORE_TYPE( LIB, POSSIBLE )  \
    SCOREP_SCORE_TYPE( COM, POSSIBLE )  \
    SCOREP_SCORE_TYPE( MPI, NO )        \
    SCOREP_SCORE_TYPE( OMP, NO )        \
    SCOREP_SCORE_TYPE( SHMEM, NO )      \
    SCOREP_SCORE_TYPE( PTHREAD, NO )    \
    SCOREP_SCORE_TYPE( CUDA, POSSIBLE ) \
    SCOREP_SCORE_TYPE( OPENCL, NO )     \
    SCOREP_SCORE_TYPE( OPENACC, NO )    \
    SCOREP_SCORE_TYPE( MEMORY, NO )     \
    SCOREP_SCORE_TYPE( SCOREP, NO )     \
    SCOREP_SCORE_TYPE( IO, NO )         \
    SCOREP_SCORE_TYPE( UNKNOWN, NO )


#define SCOREP_SCORE_FILTER_STATES \
    SCOREP_SCORE_FILTER_STATE( UNSPECIFIED, ' ' ) \
    SCOREP_SCORE_FILTER_STATE( YES,         '+' ) \
    SCOREP_SCORE_FILTER_STATE( NO,          '-' ) \
    SCOREP_SCORE_FILTER_STATE( POSSIBLE,    '*' ) \

/**
 * Defines an enumeration of the group types.
 */
typedef enum
{
    #define SCOREP_SCORE_TYPE( type, flt ) SCOREP_SCORE_TYPE_ ## type,
    SCOREP_SCORE_TYPES
    #undef SCOREP_SCORE_TYPE

    SCOREP_SCORE_TYPE_NUM
} SCOREP_Score_Type;

/**
 * Defines an enumaration of the available filter states.
 */
typedef enum
{
    #define SCOREP_SCORE_FILTER_STATE( state, symbol ) SCOREP_SCORE_FILTER_ ## state,
    SCOREP_SCORE_FILTER_STATES
    #undef SCOREP_SCORE_FILTER_STATE
} SCOREP_Score_FilterState;

/**
 * Returns the name associated with a group type.
 * @param type  The group type.
 */
std::string
SCOREP_Score_getTypeName( uint64_t type );

/**
 * Returns the filter possibilities for a group type.
 * @param type  The group type for which the filter state is requested.
 */
SCOREP_Score_FilterState
SCOREP_Score_getFilterState( uint64_t type );

/**
 * Returns the symbol (*,-,+) associated with a particular filter state.
 * @param state  The filter state.
 */
char
SCOREP_Score_getFilterSymbol( SCOREP_Score_FilterState state );

#endif // SCOREP_SCORE_TYPES_HPP
