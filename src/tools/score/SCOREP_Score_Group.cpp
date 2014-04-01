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
 * @brief      Implements a class which represents a group in the
 *             scorep-score tool.
 */

#include <config.h>
#include "SCOREP_Score_Group.hpp"
#include <stdlib.h>
#include <iostream>
#include <iomanip>

using namespace std;

SCOREP_Score_Group::SCOREP_Score_Group( uint64_t type,
                                        uint64_t processes,
                                        string   name )
{
    m_type       = type;
    m_processes  = processes;
    m_max_buf    = ( uint64_t* )calloc( processes, sizeof( uint64_t ) );
    m_total_buf  = 0;
    m_total_time = 0;
    m_name       = name;
    m_filter     = SCOREP_SCORE_FILTER_UNSPECIFIED;
    m_visits     = 0;
}


SCOREP_Score_Group::~SCOREP_Score_Group()
{
    free( m_max_buf );
}

void
SCOREP_Score_Group::addRegion( uint64_t numberOfVisits,
                               uint64_t bytesPerVisit,
                               double   time,
                               uint64_t process )
{
    m_visits             += numberOfVisits;
    m_total_buf          += numberOfVisits * bytesPerVisit;
    m_max_buf[ process ] += numberOfVisits * bytesPerVisit;
    m_total_time         += time;
}

void
SCOREP_Score_Group::print( double totalTime )
{
    cout.setf( ios::fixed, ios::floatfield );
    cout.setf( ios::showpoint );

    if ( m_total_buf > 0 )
    {
        cout << " " << SCOREP_Score_getFilterSymbol( m_filter ) << right
             << setw( 6 ) << SCOREP_Score_getTypeName( m_type )
             << setw( 16 ) << getMaxTraceBufferSize()
             << setw( 14 ) << m_visits
             << setw( 13 ) << setprecision( 2 ) << m_total_time
             << setw( 7 )  << setprecision( 1 ) << 100.0 / totalTime * m_total_time
             << left << "  " << m_name << endl;
    }
}

double
SCOREP_Score_Group::getTotalTime( void )
{
    return m_total_time;
}

uint64_t
SCOREP_Score_Group::getMaxTraceBufferSize( void )
{
    uint64_t max_buf = 0;
    for ( uint64_t i = 0; i < m_processes; i++ )
    {
        max_buf = m_max_buf[ i ] > max_buf ?  m_max_buf[ i ] : max_buf;
    }
    return max_buf;
}

uint64_t
SCOREP_Score_Group::getTotalTraceBufferSize( void )
{
    return m_total_buf;
}

void
SCOREP_Score_Group::doFilter( SCOREP_Score_FilterState state )
{
    m_filter = state;
}
