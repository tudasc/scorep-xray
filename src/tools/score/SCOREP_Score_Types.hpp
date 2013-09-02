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
 * @file       SCOREP_Score_Types.hpp
 *
 * @brief      Defines types for the score tool.
 */

#ifndef SCOREP_SCORE_TYPES_H
#define SCOREP_SCORE_TYPES_H

#include <string>
#include <stdint.h>

#define SCOREP_SCORE_TYPES \
    SCOREP_SCORE_TYPE( ALL, POSSIBLE ) \
    SCOREP_SCORE_TYPE( FLT, YES )      \
    SCOREP_SCORE_TYPE( USR, POSSIBLE ) \
    SCOREP_SCORE_TYPE( COM, POSSIBLE ) \
    SCOREP_SCORE_TYPE( MPI, NO )       \
    SCOREP_SCORE_TYPE( OMP, NO )

#define SCOREP_SCORE_FILTER_STATES \
    SCOREP_SCORE_FILTER_STATE( UNSPECIFIED, ' ' ) \
    SCOREP_SCORE_FILTER_STATE( YES,         '+' ) \
    SCOREP_SCORE_FILTER_STATE( NO,          '-' ) \
    SCOREP_SCORE_FILTER_STATE( POSSIBLE,    '*' ) \

typedef enum
{
    #define SCOREP_SCORE_TYPE( type, flt ) SCOREP_SCORE_TYPE_ ## type,
    SCOREP_SCORE_TYPES
    #undef SCOREP_SCORE_TYPE
} SCOREP_Score_Type;

extern const uint64_t SCOREP_SCORE_TYPE_NUM;

typedef enum
{
    #define SCOREP_SCORE_FILTER_STATE( state, symbol ) SCOREP_SCORE_FILTER_ ## state,
    SCOREP_SCORE_FILTER_STATES
    #undef SCOREP_SCORE_FILTER_STATE
} SCOREP_Score_FilterState;


std::string
SCOREP_Score_GetTypeName( uint64_t type );

SCOREP_Score_FilterState
SCOREP_Score_GetFilterState( uint64_t type );

char
SCOREP_Score_GetFilterSymbol( SCOREP_Score_FilterState state );

#endif // SCOREP_SCORE_TYPES_H
