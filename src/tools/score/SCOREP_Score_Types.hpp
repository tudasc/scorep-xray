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

typedef enum
{
    SCOREP_SCORE_GROUP_ALL = 0,
    SCOREP_SCORE_GROUP_USR = 1,
    SCOREP_SCORE_GROUP_MPI = 2,
    SCOREP_SCORE_GROUP_OMP = 3,

    SCOREP_SCORE_GROUP_NUM // Must stay the last entry
} SCOREP_Score_GroupId;

#endif // SCOREP_SCORE_TYPES_H
