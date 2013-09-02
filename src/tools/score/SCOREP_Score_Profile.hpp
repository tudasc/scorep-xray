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
 * @file       SCOREP_Score_Profile.hpp
 *
 * @brief      Defines a class which represents a flat profile in the
 *             scorep-score tool.
 */

#ifndef SCOREP_SCORE_PROFILE_H
#define SCOREP_SCORE_PROFILE_H

#include <string>
#include <Cube.h>
#include "SCOREP_Score_Types.hpp"

class SCOREP_Score_Profile
{
public:
    SCOREP_Score_Profile( std::string cubeFile );
    virtual
    ~SCOREP_Score_Profile();

    double
    GetTotalTime( uint64_t region );

    uint64_t
    GetVisits( uint64_t region,
               uint64_t process );

    double
    GetTime( uint64_t region,
             uint64_t process );

    uint64_t
    GetTotalVisits( uint64_t region );

    uint64_t
    GetMaxVisits( uint64_t region );

    std::string
    GetRegionName( uint64_t region );

    std::string
    GetFileName( uint64_t region );

    uint64_t
    GetNumberOfRegions();

    uint64_t
    GetNumberOfProcesses();

    uint64_t
    GetNumberOfMetrics();

    void
    Print();

    SCOREP_Score_Type
    GetGroup( uint64_t region );

    bool
    HasEnterExit( uint64_t region );

    bool
    HasParameter( uint64_t region );

    bool
    HasSend( uint64_t region );

    bool
    HasIsend( uint64_t region );

    bool
    HasIsendComplete( uint64_t region );

    bool
    HasIrecvRequest( uint64_t region );

    bool
    HasRecv( uint64_t region );

    bool
    HasIrecv( uint64_t region );

    bool
    HasCollective( uint64_t region );

    bool
    HasFork( uint64_t region );

    bool
    HasJoin( uint64_t region );

    bool
    HasThreadTeam( uint64_t region );

    bool
    HasAcquireLock( uint64_t region );

    bool
    HasReleaseLock( uint64_t region );

    bool
    HasTaskCreateComplete( uint64_t region );

    bool
    HasTaskSwitch( uint64_t region );

    bool
    HasCudaMemcpy( uint64_t region );

    bool
    HasCudaStreamCreate( uint64_t region );

private:
    bool
    calculate_calltree_types( const std::vector<cube::Cnode*>* cnodes,
                              cube::Cnode*                     node );

    SCOREP_Score_Type
    get_definition_type( uint64_t region );

private:
    cube::Cube*                 m_cube;
    cube::Metric*               m_visits;
    cube::Metric*               m_time;
    std::vector<cube::Process*> m_processes;
    std::vector<cube::Region*>  m_regions;
    SCOREP_Score_Type*          m_region_types;
};


#endif // SCOREP_SCORE_PROFILE_H
