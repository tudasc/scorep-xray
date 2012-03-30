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
 * @file       SCOREP_Score_Group.hpp
 * @maintainer Daniel Lorenz  <d.lorenz@fz-juelich.de>
 *
 * @brief      Defines a class which represents a group in the
 *             scorep-score tool.
 */

#ifndef SCOREP_SCORE_GROUP_H
#define SCOREP_SCORE_GROUP_H

#include <SCOREP_Score_Types.hpp>
#include <string>
#include <stdint.h>

class SCOREP_Score_Group
{
public:
    SCOREP_Score_Group( uint64_t    type,
                        uint64_t    processes,
                        std::string name );
    virtual
    ~SCOREP_Score_Group();

    void
    AddRegion( uint64_t bytesCount,
               double   time,
               uint64_t process );

    void
    Print( double totalTime );

    double
    GetTotalTime();

    uint64_t
    GetMaxTBC();

    uint64_t
    GetTotalTBC();

    void
    DoFilter( SCOREP_Score_FilterState state );

private:
    uint64_t                 m_type;
    uint64_t                 m_processes;
    uint64_t*                m_max_tbc;
    uint64_t                 m_total_tbc;
    double                   m_total_time;
    std::string              m_name;
    SCOREP_Score_FilterState m_filter;
};

#endif
