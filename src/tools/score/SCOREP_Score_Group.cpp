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
 * Copyright (c) 2009-2012,
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
 * @file       SCOREP_Score_Group.cpp
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
    m_max_tbc    = ( uint64_t* )calloc( processes, sizeof( uint64_t ) );
    m_total_tbc  = 0;
    m_total_time = 0;
    m_name       = name;
    m_filter     = SCOREP_SCORE_FILTER_UNSPECIFIED;
}


SCOREP_Score_Group::~SCOREP_Score_Group()
{
    free( m_max_tbc );
}

void
SCOREP_Score_Group::AddRegion( uint64_t byteCount, double time, uint64_t process )
{
    m_total_tbc          += byteCount;
    m_max_tbc[ process ] += byteCount;
    m_total_time         += time;
}

void
SCOREP_Score_Group::Print( double totalTime )
{
    cout.setf( ios::fixed, ios::floatfield );
    cout.setf( ios::showpoint );

    if ( m_total_tbc > 0 )
    {
        cout << " " << SCOREP_Score_GetFilterSymbol( m_filter ) << right
             << setw( 6 ) << SCOREP_Score_GetTypeName( m_type )
             << setw( 16 ) << GetMaxTBC()
             << setw( 13 ) << setprecision( 2 ) << m_total_time
             << setw( 7 )  << setprecision( 1 ) << 100.0 / totalTime * m_total_time
             << left << " " << m_name << endl;
    }
}

double
SCOREP_Score_Group::GetTotalTime()
{
    return m_total_time;
}

uint64_t
SCOREP_Score_Group::GetMaxTBC()
{
    uint64_t max_tbc = 0;
    for ( uint64_t i = 0; i < m_processes; i++ )
    {
        max_tbc = m_max_tbc[ i ] > max_tbc ?  m_max_tbc[ i ] : max_tbc;
    }
    return max_tbc;
}

uint64_t
SCOREP_Score_Group::GetTotalTBC()
{
    return m_total_tbc;
}

void
SCOREP_Score_Group::DoFilter( SCOREP_Score_FilterState state )
{
    m_filter = state;
}
