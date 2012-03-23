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
 * @status     alpha
 * @file       SCOREP_Score_Types.hpp
 * @maintainer Daniel Lorenz  <d.lorenz@fz-juelich.de>
 *
 * @brief      Implements conversion for types for the score tool.
 */

#include <config.h>
#include <SCOREP_Score_Types.hpp>

using namespace std;

static string scorep_score_type_names[] =
{
    #define SCOREP_SCORE_TYPE( type, flt ) #type,
    SCOREP_SCORE_TYPES
    #undef SCOREP_SCORE_TYPE
};

const uint64_t SCOREP_SCORE_TYPE_NUM = sizeof( scorep_score_type_names ) / sizeof( string );

string
SCOREP_Score_GetTypeName( uint64_t type )
{
    return scorep_score_type_names[ type ];
}

SCOREP_Score_FilterState
SCOREP_Score_GetFilterState( uint64_t type )
{
    static SCOREP_Score_FilterState filter_type[] =
    {
        #define SCOREP_SCORE_TYPE( type, flt ) SCOREP_SCORE_FILTER_ ## flt,
        SCOREP_SCORE_TYPES
        #undef SCOREP_SCORE_TYPE
    };

    return filter_type[ type ];
}

char
SCOREP_Score_GetFilterSymbol( SCOREP_Score_FilterState state )
{
    static char symbols[] =
    {
     #define SCOREP_SCORE_FILTER_STATE( state, symbol ) symbol,
        SCOREP_SCORE_FILTER_STATES
     #undef SCOREP_SCORE_FILTER_STATE
    };
    return symbols[ state ];
}
