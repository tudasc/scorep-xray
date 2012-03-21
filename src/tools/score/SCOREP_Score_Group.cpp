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
 * @file       SCOREP_Score_Group.cpp
 * @maintainer Daniel Lorenz  <d.lorenz@fz-juelich.de>
 *
 * @brief      Implements a class which represents a group in the
 *             scorep-score tool.
 */

#include <config.h>
#include <malloc.h>
#include <iostream>
#include <SCOREP_Score_Group.hpp>

using namespace std;

SCOREP_Score_Group::SCOREP_Score_Group( SCOREP_Score_GroupId group,
                                        uint64_t             threads )
{
    m_group_id   = group;
    m_threads    = threads;
    m_max_tbc    = ( uint64_t* )calloc( threads, sizeof( uint64_t ) );
    m_total_tbc  = 0;
    m_total_time = 0;
}


SCOREP_Score_Group::~SCOREP_Score_Group()
{
    free( m_max_tbc );
}

void
SCOREP_Score_Group::AddRegion( uint64_t tbc, double time, uint64_t thread )
{
    m_total_tbc        += tbc;
    m_max_tbc[ thread ] = tbc > m_max_tbc[ thread ] ?  tbc : m_max_tbc[ thread ];
    m_total_time       += time;
}

void
SCOREP_Score_Group::Print( double total_time )
{
    if ( m_total_tbc > 0 )
    {
        cout << " \t " << get_group_name() << "\t "
             << get_max_tbc() << "\t "
             << m_total_time << "\t "
             << 100 / total_time * m_total_time << "\t "
             << get_group_name() << endl;
    }
}

double
SCOREP_Score_Group::GetTotalTime()
{
    return m_total_time;
}

string
SCOREP_Score_Group::get_group_name()
{
    switch ( m_group_id )
    {
        case SCOREP_SCORE_GROUP_ALL:
            return "ALL";
        case SCOREP_SCORE_GROUP_USR:
            return "USR";
        case SCOREP_SCORE_GROUP_MPI:
            return "MPI";
        case SCOREP_SCORE_GROUP_OMP:
            return "OMP";
    }
    return "UNKNOWN";
}

uint64_t
SCOREP_Score_Group::get_max_tbc()
{
    uint64_t max_tbc = 0;
    for ( uint64_t i = 0; i < m_threads; i++ )
    {
        max_tbc = m_max_tbc[ i ] > max_tbc ?  m_max_tbc[ i ] : max_tbc;
    }
    return max_tbc;
}
