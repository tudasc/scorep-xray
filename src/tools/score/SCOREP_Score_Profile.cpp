/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
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
 * @brief      Implements a class which represents a flat profile in the
 *             scorep-score tool.
 */

#include <config.h>
#include "SCOREP_Score_Profile.hpp"
#include <Cube.h>
#include <CubeTypes.h>
#include <assert.h>
#include <sys/stat.h>

using namespace std;
using namespace cube;

SCOREP_Score_Profile::SCOREP_Score_Profile( string cubeFile )
{
    struct stat file_stats;
    stat( cubeFile.c_str(), &file_stats );
    m_file_size = file_stats.st_size;

    m_cube = new Cube();
    m_cube->openCubeReport( cubeFile );

    m_time   = m_cube->get_met( "time" );
    m_visits = m_cube->get_met( "visits" );
    // if visits metric is not present, the cube used tau atomics
    if ( m_visits == NULL )
    {
        m_visits = m_time;
    }

    m_processes = m_cube->get_procv();
    m_regions   = m_cube->get_regv();

    // Make sure the id of the region definitions match their position in the vector
    for ( uint32_t i = 0; i < getNumberOfRegions(); i++ )
    {
        m_regions[ i ]->set_id( i );
    }

    // Analyze region types
    m_region_types = ( SCOREP_Score_Type* )
                     malloc( sizeof( SCOREP_Score_Type ) * getNumberOfRegions() );
    for ( uint32_t i = 0; i < getNumberOfRegions(); i++ )
    {
        m_region_types[ i ] = get_definition_type( i );
    }
    vector<Cnode*> roots =  m_cube->get_root_cnodev();
    for ( uint64_t i = 0; i < roots.size(); i++ )
    {
        calculate_calltree_types( &m_cube->get_cnodev(), roots[ i ] );
    }
}

SCOREP_Score_Profile::~SCOREP_Score_Profile()
{
    delete ( m_cube );
}

double
SCOREP_Score_Profile::getTime( uint64_t region, uint64_t process )
{
    Value* value = m_cube->get_sev_adv( m_time, CUBE_CALCULATE_EXCLUSIVE,
                                        m_regions[ region ], CUBE_CALCULATE_EXCLUSIVE,
                                        m_processes[ process ], CUBE_CALCULATE_INCLUSIVE  );

    if ( !value )
    {
        return 0.0;
    }
    if ( value->myDataType() == CUBE_DATA_TYPE_TAU_ATOMIC )
    {
        TauAtomicValue* tau_value = ( TauAtomicValue* )value;
        return tau_value->getSum().getDouble();
    }
    else
    {
        return value->getDouble();
    }
}

double
SCOREP_Score_Profile::getTotalTime( uint64_t region )
{
    Value* value = m_cube->get_sev_adv( m_time, CUBE_CALCULATE_EXCLUSIVE,
                                        m_regions[ region ], CUBE_CALCULATE_EXCLUSIVE );

    if ( !value )
    {
        return 0.0;
    }
    if ( value->myDataType() == CUBE_DATA_TYPE_TAU_ATOMIC )
    {
        TauAtomicValue* tau_value = ( TauAtomicValue* )value;
        return tau_value->getSum().getDouble();
    }
    else
    {
        return value->getDouble();
    }
}

uint64_t
SCOREP_Score_Profile::getVisits( uint64_t region, uint64_t process )
{
    Value* value = m_cube->get_sev_adv( m_visits, CUBE_CALCULATE_EXCLUSIVE,
                                        m_regions[ region ], CUBE_CALCULATE_EXCLUSIVE,
                                        m_processes[ process ], CUBE_CALCULATE_INCLUSIVE );

    if ( !value )
    {
        return 0;
    }
    if ( value->myDataType() == CUBE_DATA_TYPE_TAU_ATOMIC )
    {
        TauAtomicValue* tau_value = ( TauAtomicValue* )value;
        return tau_value->getN().getUnsignedLong();
    }
    else
    {
        return value->getUnsignedLong();
    }
}

uint64_t
SCOREP_Score_Profile::getTotalVisits( uint64_t region )
{
    Value* value = m_cube->get_sev_adv( m_visits, CUBE_CALCULATE_EXCLUSIVE,
                                        m_regions[ region ], CUBE_CALCULATE_EXCLUSIVE );

    if ( !value )
    {
        return 0;
    }
    if ( value && value->myDataType() == CUBE_DATA_TYPE_TAU_ATOMIC )
    {
        TauAtomicValue* tau_value = ( TauAtomicValue* )value;
        return tau_value->getN().getUnsignedLong();
    }
    else
    {
        return value->getUnsignedLong();
    }
}

uint64_t
SCOREP_Score_Profile::getMaxVisits( uint64_t region )
{
    uint64_t max = 0;
    uint64_t process;
    uint64_t value;

    for ( process = 0; process < getNumberOfProcesses(); process++ )
    {
        value = getVisits( region, process );
        max   = value > max ? value : max;
    }
    return max;
}

string
SCOREP_Score_Profile::getRegionName( uint64_t region )
{
    return m_regions[ region ]->get_name();
}

string
SCOREP_Score_Profile::getFileName( uint64_t region )
{
    return m_regions[ region ]->get_mod();
}

uint64_t
SCOREP_Score_Profile::getNumberOfRegions()
{
    return m_regions.size();
}

uint64_t
SCOREP_Score_Profile::getNumberOfProcesses()
{
    return m_processes.size();
}

uint64_t
SCOREP_Score_Profile::getNumberOfMetrics()
{
    return m_cube->get_metv().size();
}

uint64_t
SCOREP_Score_Profile::getMaxNumberOfLocationsPerProcess()
{
    vector<LocationGroup*> loc_group =    m_cube->get_procv();
    uint64_t               max       = 0;
    uint64_t               val;
    for ( uint64_t i = 0; i < loc_group.size(); i++ )
    {
        val = loc_group[ i ]->num_children();
        max = val > max ? val : max;
    }
    return max;
}

void
SCOREP_Score_Profile::print()
{
    uint64_t region, process;

    cout << "group \t max visits \t total visits \t total time \t region" << endl;
    for ( region = 0; region < getNumberOfRegions(); region++ )
    {
        cout << getGroup( region );
        cout << "\t" << getMaxVisits( region );
        cout << "\t" << getTotalVisits( region );
        cout << "\t" << getTotalTime( region );
        cout << "\t" << getRegionName( region );
        cout << endl;
    }
}

SCOREP_Score_Type
SCOREP_Score_Profile::getGroup( uint64_t region )
{
    assert( region < getNumberOfRegions() );
    return m_region_types[ region ];
}

uint64_t
SCOREP_Score_Profile::getFileSize( void )
{
    return m_file_size;
}

/* **************************************************** private members */
SCOREP_Score_Type
SCOREP_Score_Profile::get_definition_type( uint64_t region )
{
    string name = getRegionName( region );
    if ( name.substr( 0, 4 ) == "MPI_" )
    {
        return SCOREP_SCORE_TYPE_MPI;
    }
    if ( name.substr( 0, 6 ) == "shmem_" )
    {
        return SCOREP_SCORE_TYPE_SHMEM;
    }
    if ( name.substr( 0, 6 ) == "!$omp " )
    {
        return SCOREP_SCORE_TYPE_OMP;
    }
    if ( name.substr( 0, 8 ) == "pthread_" )
    {
        return SCOREP_SCORE_TYPE_PTHREAD;
    }
    else
    {
        return SCOREP_SCORE_TYPE_USR;
    }
}

bool
SCOREP_Score_Profile::calculate_calltree_types( const vector<Cnode*>* cnodes,
                                                Cnode*                node )
{
    bool is_on_path = false;
    for ( uint32_t i = 0; i < node->num_children(); i++ )
    {
        is_on_path = calculate_calltree_types( cnodes, node->get_child( i ) ) ?
                     true : is_on_path;
    }

    uint32_t          region = node->get_callee()->get_id();
    SCOREP_Score_Type type   = getGroup( region );
    if ( is_on_path && type == SCOREP_SCORE_TYPE_USR )
    {
        m_region_types[ region ] = SCOREP_SCORE_TYPE_COM;
    }

    if ( type == SCOREP_SCORE_TYPE_OMP ||
         type == SCOREP_SCORE_TYPE_MPI ||
         type == SCOREP_SCORE_TYPE_SHMEM )
    {
        is_on_path = true;
    }
    return is_on_path;
}
