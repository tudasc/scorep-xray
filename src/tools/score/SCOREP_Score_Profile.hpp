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
 * @file       SCOREP_Score_Profile.hpp
 * @maintainer Daniel Lorenz  <d.lorenz@fz-juelich.de>
 *
 * @brief      Defines a class which represents a flat profile in the
 *             scorep-score tool.
 */

#ifndef SCOREP_SCORE_PROFILE_H
#define SCOREP_SCORE_PROFILE_H

#include <string>
#include <Cube.h>

class SCOREP_Score_Profile
{
public:
    SCOREP_Score_Profile( std::string cube_file );
    virtual
    ~SCOREP_Score_Profile();

    double
    GetTotalTime( uint64_t region );

    uint64_t
    GetVisits( uint64_t region,
               uint64_t thread );

    uint64_t
    GetTotalVisits( uint64_t region );

    uint64_t
    GetMaxVisits( uint64_t region );

    std::string
    GetRegionName( uint64_t region );

    uint64_t
    GetNumberOfRegions();

    uint64_t
    GetNumberOfThreads();

    uint64_t
    GetNumberOfMetrics();

    void
    Print();

private:
    cube::Cube*                m_cube;
    cube::Metric*              m_visits;
    cube::Metric*              m_time;
    std::vector<cube::Thread*> m_threads;
    std::vector<cube::Region*> m_regions;
};


#endif // SCOREP_SCORE_PROFILE_H
