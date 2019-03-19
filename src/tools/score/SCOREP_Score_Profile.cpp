/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2015-2017, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014, 2019,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013, 2015,
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
#include <sstream>
#include <cctype>

using namespace std;
using namespace cube;


SCOREP_Score_Profile::SCOREP_Score_Profile( cube::Cube* cube   ) : m_cube( cube )
{
    m_time   = m_cube->get_met( "time" );
    m_visits = m_cube->get_met( "visits" );
    // if visits metric is not present, the cube used tau atomics
    if ( m_visits == NULL )
    {
        m_visits = m_time;
    }
    m_hits = m_cube->get_met( "hits" );

    /* Collect all attributes from the definition counters and arguments */
    const string               attr_counter_prefix( "Score-P::DefinitionCounters::" );
    const string               attr_argument_prefix( "Score-P::DefinitionArguments::" );
    const map<string, string>& attributes = m_cube->get_attrs();
    for ( map<string, string>::const_iterator it = attributes.begin();
          it != attributes.end(); ++it )
    {
        const string&          key = it->first;
        map<string, uint64_t>* map_to_insert;
        string                 key_name;
        if ( key.size() > attr_counter_prefix.size()
             && 0 == key.compare( 0, attr_counter_prefix.size(), attr_counter_prefix ) )
        {
            map_to_insert = &m_definition_counters;
            key_name      = key.substr( attr_counter_prefix.size() );
        }
        else if ( key.size() > attr_argument_prefix.size()
                  && 0 == key.compare( 0, attr_argument_prefix.size(), attr_argument_prefix ) )
        {
            map_to_insert = &m_definition_arguments;
            key_name      = key.substr( attr_argument_prefix.size() );
        }
        else
        {
            continue;
        }

        istringstream value_as_string( it->second );
        uint64_t      value;
        try
        {
            value_as_string >> value;
        }
        catch ( ... )
        {
            cerr << "WARNING: Cannot parse '" << key << "' value as number: "
                 << "'" << it->second << "'" << endl;
            continue;
        }

        map_to_insert->insert( map<string, uint64_t>::value_type( key_name, value ) );
    }

    m_processes = m_cube->get_procv();
    m_regions   = m_cube->get_regv();

    // Make sure the id of the region definitions match their position in the vector
    // and add special regions to containers.
    for ( uint32_t i = 0; i < getNumberOfRegions(); i++ )
    {
        m_regions[ i ]->set_id( i );

        if ( getRegionParadigm( i ) == "measurement" )
        {
            m_regions_to_omit_in_trace_enter_leave_events.insert( i );
        }
        else if ( getRegionParadigm( i ) == "user"
                  && getRegionName( i ) == "MEASUREMENT OFF" )
        {
            m_regions_to_omit_in_trace_enter_leave_events.insert( i );
        }
        else if ( getRegionName( i ).substr( 0, 9 ) == "instance=" )
        {
            m_dynamic_regions.insert( i );
        }
        else if ( getRegionName( i ).find( '=', 0 ) != string::npos )
        {
            m_parameter_regions.insert( i );
        }
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
        m_root_region_names.insert( roots[ i ]->get_callee()->get_name() );
        calculate_calltree_types( &m_cube->get_cnodev(), roots[ i ] );
    }
}


SCOREP_Score_Profile::~SCOREP_Score_Profile()
{
};


bool
SCOREP_Score_Profile::hasHits( void ) const
{
    return m_hits != 0;
}

double
SCOREP_Score_Profile::getTime( uint64_t region,
                               uint64_t process )
{
    Value* value = m_cube->get_sev_adv( m_time, CUBE_CALCULATE_EXCLUSIVE,
                                        m_regions[ region ], CUBE_CALCULATE_EXCLUSIVE,
                                        m_processes[ process ], CUBE_CALCULATE_INCLUSIVE );

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
SCOREP_Score_Profile::getInclusiveTime( uint64_t region,
                                        uint64_t process )
{
    Value* value = m_cube->get_sev_adv( m_time, CUBE_CALCULATE_EXCLUSIVE,
                                        m_regions[ region ], CUBE_CALCULATE_INCLUSIVE,
                                        m_processes[ process ], CUBE_CALCULATE_INCLUSIVE );

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
SCOREP_Score_Profile::getVisits( uint64_t region,
                                 uint64_t process )
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

uint64_t
SCOREP_Score_Profile::getHits( uint64_t region,
                               uint64_t process )
{
    if ( !m_hits )
    {
        return 0;
    }

    Value* value = m_cube->get_sev_adv( m_hits, CUBE_CALCULATE_EXCLUSIVE,
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

string
SCOREP_Score_Profile::getRegionName( uint64_t region ) const
{
    return m_regions[ region ]->get_name();
}

string
SCOREP_Score_Profile::getMangledName( uint64_t region )
{
    return m_regions[ region ]->get_mangled_name();
}

string
SCOREP_Score_Profile::getRegionParadigm( uint64_t region ) const
{
    string paradigm = m_regions[ region ]->get_paradigm();
    if ( paradigm == "unknown" )
    {
        paradigm = m_regions[ region ]->get_descr();
    }
    return paradigm;
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

const map<string, uint64_t>&
SCOREP_Score_Profile::getDefinitionCounters( void )
{
    return m_definition_counters;
}

const map<string, uint64_t>&
SCOREP_Score_Profile::getDefinitionArguments( void )
{
    return m_definition_arguments;
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
    if ( region >= getNumberOfRegions() )
    {
        cerr << "[Score Error]: Region Id " << region
             << " is bigger than number ("
             << getNumberOfRegions()
             << ") of known regions. Unknown type." << endl;
        return SCOREP_SCORE_TYPE_UNKNOWN;
    }
    return m_region_types[ region ];
}

bool
SCOREP_Score_Profile::isRootRegion( uint64_t region ) const
{
    return m_root_region_names.count( getRegionName( region ) );
}

bool
SCOREP_Score_Profile::omitInTraceEnterLeaveEvents( uint64_t region ) const
{
    return m_regions_to_omit_in_trace_enter_leave_events.count( region );
}

bool
SCOREP_Score_Profile::isParameterRegion( uint64_t region ) const
{
    return m_parameter_regions.count( region );
}

bool
SCOREP_Score_Profile::isDynamicRegion( uint64_t region ) const
{
    return m_dynamic_regions.count( region );
}



/* **************************************************** private members */
SCOREP_Score_Type
SCOREP_Score_Profile::get_definition_type( uint64_t region )
{
    string paradigm = getRegionParadigm( region );
    if ( paradigm == "mpi" )
    {
        return SCOREP_SCORE_TYPE_MPI;
    }
    if ( paradigm == "shmem" )
    {
        return SCOREP_SCORE_TYPE_SHMEM;
    }
    if ( paradigm == "openmp" )
    {
        return SCOREP_SCORE_TYPE_OMP;
    }
    if ( paradigm == "pthread" )
    {
        return SCOREP_SCORE_TYPE_PTHREAD;
    }
    if ( paradigm == "cuda" )
    {
        return SCOREP_SCORE_TYPE_CUDA;
    }
    if ( paradigm == "opencl" )
    {
        return SCOREP_SCORE_TYPE_OPENCL;
    }
    if ( paradigm == "openacc" )
    {
        return SCOREP_SCORE_TYPE_OPENACC;
    }
    if ( paradigm == "memory" )
    {
        return SCOREP_SCORE_TYPE_MEMORY;
    }
    if ( paradigm == "measurement" )
    {
        return SCOREP_SCORE_TYPE_SCOREP;
    }
    if ( 0 == paradigm.compare( 0, 8, "libwrap:" ) )
    {
        return SCOREP_SCORE_TYPE_LIB;
    }
    if ( paradigm == "unknown" )
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
        if ( name.substr( 0, 6 ) == "!$omp " || name.substr( 0, 4 ) == "omp_" )
        {
            return SCOREP_SCORE_TYPE_OMP;
        }
        if ( name.substr( 0, 8 ) == "pthread_" )
        {
            return SCOREP_SCORE_TYPE_PTHREAD;
        }
        if ( has_prefix_then_upper( name, "cu" ) || has_prefix_then_upper( name, "cuda" ) )
        {
            return SCOREP_SCORE_TYPE_CUDA;
        }
        if ( has_prefix_then_upper( name, "cl" ) )
        {
            return SCOREP_SCORE_TYPE_OPENCL;
        }
    }

    return SCOREP_SCORE_TYPE_USR;
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

    if ( type > SCOREP_SCORE_TYPE_COM )
    {
        is_on_path = true;
    }
    return is_on_path;
}

bool
SCOREP_Score_Profile::has_prefix_then_upper( const string& str,
                                             const string& prefix )
{
    if ( str.size() <= prefix.size() )
    {
        return false;
    }
    if ( 0 != str.compare( 0, prefix.size(), prefix ) )
    {
        return false;
    }
    return isupper( str[ prefix.size() ] );
}
