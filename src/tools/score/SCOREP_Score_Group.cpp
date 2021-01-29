/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012, 2015,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014, 2019-2021,
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
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

static string
get_number_with_comma( uint64_t number )
{
    uint32_t blocks[ 20 ];
    int32_t  exp;

    if ( number == 0 )
    {
        return "0";
    }

    for ( exp = 0; number > 0; exp++ )
    {
        blocks[ exp ] = number % 1000;
        number       /= 1000;
    }

    stringstream result;
    exp--;
    result << blocks[ exp ];
    exp--;
    for (; exp >= 0; exp-- )
    {
        result << ",";
        if ( blocks[ exp ] < 10 )
        {
            result << "00";
        }
        else if ( blocks[ exp ] < 100 )
        {
            result << "0";
        }
        result << blocks[ exp ];
    }

    return result.str();
}

SCOREP_Score_Group::SCOREP_Score_Group( uint64_t      type,
                                        uint64_t      processes,
                                        const string& name )
{
    m_type         = type;
    m_processes    = processes;
    m_max_buf      = ( uint64_t* )calloc( processes, sizeof( uint64_t ) );
    m_total_buf    = 0;
    m_total_time   = 0;
    m_name         = name;
    m_mangled_name = name;
    m_file_name    = "";
    m_use_mangled  = false;
    m_filter       = SCOREP_SCORE_FILTER_UNSPECIFIED;
    m_visits       = 0;
    m_hits         = 0;
}

SCOREP_Score_Group::SCOREP_Score_Group( uint64_t           type,
                                        uint64_t           processes,
                                        const std::string& name,
                                        const std::string& mangledName,
                                        const std::string& fileName,
                                        bool               useMangled )
{
    m_type         = type;
    m_processes    = processes;
    m_max_buf      = ( uint64_t* )calloc( processes, sizeof( uint64_t ) );
    m_total_buf    = 0;
    m_total_time   = 0;
    m_name         = name;
    m_mangled_name = mangledName;
    m_file_name    = fileName;
    m_use_mangled  = useMangled;
    m_filter       = SCOREP_SCORE_FILTER_UNSPECIFIED;
    m_visits       = 0;
    m_hits         = 0;
}

SCOREP_Score_Group::~SCOREP_Score_Group()
{
    free( m_max_buf );
}

void
SCOREP_Score_Group::updateProcess( uint64_t process,
                                   uint64_t bytes,
                                   uint64_t numberOfVisits,
                                   uint64_t numberOfHits,
                                   double   time )
{
    m_total_buf          += bytes;
    m_max_buf[ process ] += bytes;
    m_visits             += numberOfVisits;
    m_hits               += numberOfHits;
    m_total_time         += time;
}

void
SCOREP_Score_Group::updateWidths( SCOREP_Score_FieldWidths& widths )
{
    ostringstream str;

    str.setf( ios::fixed, ios::floatfield );
    str.setf( ios::showpoint );

    if ( m_total_buf > 0 )
    {
        widths.m_type   = std::max<int>( widths.m_type, SCOREP_Score_getTypeName( m_type ).size() );
        widths.m_bytes  = std::max<int>( widths.m_bytes, get_number_with_comma( getMaxTraceBufferSize() ).size() );
        widths.m_visits = std::max<int>( widths.m_visits, get_number_with_comma( m_visits ).size() );
        if ( m_hits > 0 )
        {
            widths.m_hits = std::max<int>( widths.m_hits, get_number_with_comma( m_hits ).size() );
        }

        str << setprecision( 2 ) << m_total_time;
        widths.m_time = std::max<int>( widths.m_time, str.str().size() );
        str.clear();

        str << setprecision( 2 ) << m_total_time / m_visits * 1000000;
        widths.m_time_per_visit = std::max<int>( widths.m_time_per_visit, str.str().size() );
        str.clear();
    }
}

void
SCOREP_Score_Group::print( double                   totalTime,
                           SCOREP_Score_FieldWidths widths,
                           bool                     withHits )
{
    cout.setf( ios::fixed, ios::floatfield );
    cout.setf( ios::showpoint );

    if ( m_total_buf > 0 )
    {
        cout << " " << SCOREP_Score_getFilterSymbol( m_filter ) << " "
             << right
             << " " << setw( widths.m_type ) << SCOREP_Score_getTypeName( m_type )
             << " " << setw( widths.m_bytes ) << get_number_with_comma( getMaxTraceBufferSize() )
             << " " << setw( widths.m_visits ) << get_number_with_comma( m_visits );
        if ( withHits )
        {
            cout << " " << setw( widths.m_hits ) << get_number_with_comma( m_hits );
        }
        cout << " " << setw( widths.m_time ) << setprecision( 2 ) << m_total_time
             << " " << setw( 7 )  << setprecision( 1 ) << 100.0 / totalTime * m_total_time
             << " " << setw( widths.m_time_per_visit ) << setprecision( 2 ) << m_total_time / m_visits * 1000000
             << left
             << "  " << getDisplayName() << endl;
    }
}

std::string
SCOREP_Score_Group::getDisplayName()
{
    if ( m_use_mangled )
    {
        return m_mangled_name;
    }
    return m_name;
}

std::string
SCOREP_Score_Group::cleanName()
{
    std::string temp = m_mangled_name;
    // make the name string fit our regex semantic, in particular for C++
    std::replace( temp.begin(), temp.end(), ' ', '?' );
    std::replace( temp.begin(), temp.end(), ']', '?' );
    std::replace( temp.begin(), temp.end(), '[', '?' );
    std::replace( temp.begin(), temp.end(), '*', '?' );
    std::replace( temp.begin(), temp.end(), '!', '?' );
    return temp;
}

std::string
SCOREP_Score_Group::getFilterCandidate( uint64_t                 maxBuffer,
                                        double                   totalTime,
                                        SCOREP_Score_FieldWidths widths,
                                        double                   percentageOfTotalBufferSize,
                                        double                   thresholdTimePerVisits,
                                        uint64_t                 minVisits,
                                        double                   minBufferAbsolute,
                                        bool                     filterUSR,
                                        bool                     filterCOM )
{
    double buffer_ratio = ( ( double )getMaxTraceBufferSize() / ( double )maxBuffer ) * 100;
    double buffer_in_M  = ( double )getMaxTraceBufferSize() / ( 1024 * 1024 );
    if ( ( m_filter != SCOREP_SCORE_FILTER_YES ) &&
         (   buffer_ratio >= percentageOfTotalBufferSize
             && ( m_total_time / m_visits * 1000000 )   <= thresholdTimePerVisits
             && m_visits >= minVisits
             && buffer_in_M >= minBufferAbsolute
             && ( ( filterUSR && m_type == SCOREP_SCORE_TYPE_USR ) ||
                  ( filterCOM && m_type == SCOREP_SCORE_TYPE_COM ) ) ) )
    {
        string clean_name = cleanName();

        std::ostringstream temp;
        temp.setf( ios::fixed, ios::floatfield );
        temp.setf( ios::showpoint );
        temp << "# type=" << setw( 3 ) << SCOREP_Score_getTypeName( m_type )
             << " max_buf=" << setw( widths.m_bytes ) << get_number_with_comma( getMaxTraceBufferSize() )
             << " visits=" << setw( widths.m_visits ) << get_number_with_comma( m_visits )
             << ", time="   << setw( widths.m_time ) << setprecision( 2 ) << m_total_time
             << "s ("  << setw( 5 )  << setprecision( 1 ) << ( m_total_time / totalTime ) * 100 << "%)"
             << ", time/visit= " << setw( 7 ) << setprecision( 2 ) << getTimePerVisit() << "us";

        return "    " + temp.str() + "\n"
               + "    # name='" + m_name + "'\n"
               + "    # file='" + m_file_name + "'\n"
               + "    MANGLED " + clean_name + "\n";
    }
    return "";
}

std::string
SCOREP_Score_Group::getPreviouslyFiltered()
{
    if ( m_filter == SCOREP_SCORE_FILTER_YES )
    {
        return "    # name='" + m_name + "'\n"
               + "    # file='" + m_file_name + "'\n"
               + "    MANGLED " + cleanName() + "\n\n";
    }
    return "";
}

double
SCOREP_Score_Group::getTotalTime( void )
{
    return m_total_time;
}

const std::string&
SCOREP_Score_Group::getName( void )
{
    return m_name;
}

double
SCOREP_Score_Group::getTimePerVisit( void )
{
    if ( m_visits == 0 )
    {
        return 0;
    }
    return m_total_time / m_visits * 1000000;
}

uint64_t
SCOREP_Score_Group::getVisits( void )
{
    return m_visits;
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
