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
 * @maintainer Daniel Lorenz  <d.lorenz@fz-juelich.de>
 *
 * @brief      Defines types for the score tool.
 */

#ifndef SCOREP_SCORE_TYPES_H
#define SCOREP_SCORE_TYPES_H

#include <string>
#include <stdint.h>

#define SCOREP_SCORE_TYPES \
    SCOREP_SCORE_TYPE( ALL ) \
    SCOREP_SCORE_TYPE( USR ) \
    SCOREP_SCORE_TYPE( COM ) \
    SCOREP_SCORE_TYPE( MPI ) \
    SCOREP_SCORE_TYPE( OMP )

typedef enum
{
    #define SCOREP_SCORE_TYPE( type ) SCOREP_SCORE_TYPE_ ## type,
    SCOREP_SCORE_TYPES
    #undef SCOREP_SCORE_TYPE
} SCOREP_Score_Type;

extern const uint64_t SCOREP_SCORE_TYPE_NUM;

std::string
SCOREP_Score_GetTypeName( uint64_t type );


#endif // SCOREP_SCORE_TYPES_H
