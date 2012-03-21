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
    SCOREP_Score_Group( SCOREP_Score_GroupId group,
                        uint64_t             threads );
    virtual
    ~SCOREP_Score_Group();

    void
    AddRegion( uint64_t tbc,
               double   time,
               uint64_t thread );

    void
    Print( double total_time );

    double
    GetTotalTime();

protected:
    std::string
    get_group_name();

    uint64_t
    get_max_tbc();

private:
    SCOREP_Score_GroupId m_group_id;
    uint64_t             m_threads;
    uint64_t*            m_max_tbc;
    uint64_t             m_total_tbc;
    double               m_total_time;
};

#endif
