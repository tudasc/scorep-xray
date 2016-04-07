/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
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

/**
 * @file
 *
 * @brief      Implements conversion for types for the score tool.
 */

#include <config.h>
#include "SCOREP_Score_Types.hpp"

using namespace std;

static string scorep_score_type_names[] =
{
    #define SCOREP_SCORE_TYPE( type, flt ) #type,
    SCOREP_SCORE_TYPES
    #undef SCOREP_SCORE_TYPE
};

string
SCOREP_Score_getTypeName( uint64_t type )
{
    return scorep_score_type_names[ type ];
}

SCOREP_Score_FilterState
SCOREP_Score_getFilterState( uint64_t type )
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
SCOREP_Score_getFilterSymbol( SCOREP_Score_FilterState state )
{
    static char symbols[] =
    {
     #define SCOREP_SCORE_FILTER_STATE( state, symbol ) symbol,
        SCOREP_SCORE_FILTER_STATES
     #undef SCOREP_SCORE_FILTER_STATE
    };
    return symbols[ state ];
}
