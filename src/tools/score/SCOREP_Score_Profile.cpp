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


SCOREP_Score_Profile::SCOREP_Score_Profile( cube::Cube* cube   ) : m_cube( cube ), m_num_arguments( -1 )
{
    m_time   = m_cube->get_met( "time" );
    m_visits = m_cube->get_met( "visits" );
    // if visits metric is not present, the cube used tau atomics
    if ( m_visits == NULL )
    {
        m_visits = m_time;
    }
    m_hits = m_cube->get_met( "hits" );

    /* Collect all attributes from the definition counters */
    const string               attr_prefix( "Score-P::DefinitionCounters::" );
    const map<string, string>& attributes = m_cube->get_attrs();
    for ( map<string, string>::const_iterator it = attributes.begin();
          it != attributes.end(); ++it )
    {
        const string& key = it->first;
        if ( key.size() <= attr_prefix.size() || 0 != key.compare( 0, attr_prefix.size(), attr_prefix ) )
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

        m_definition_counters.insert( map<string, uint64_t>::value_type(
                                          key.substr( attr_prefix.size() ),
                                          value ) );
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
        else if ( getRegionParadigm( i ) == "user"
                  && ( getRegionName( i ).substr( 0, 9 ) == "instance="
                       || getRegionName( i ).substr( 0, 10 ) == "iteration=" ) )
        {
            // These are the artificial child regions of a dynamic region.
            // Only the parent dynamic region is seen in the trace and thus
            // only this region contributes.
            // If there is more than one dynamic region, the artificial children
            // of the first are named iteration=<n> (as provided by cubelib), the
            // others are named instance=<n>.
            m_regions_to_omit_in_trace_enter_leave_events.insert( i );
            m_dynamic_regions.insert( i );
        }

        // process selected region attributes
        const map<string, string>&          attrs            = m_regions[ i ]->get_attrs();
        const string                        program_args_key = "Score-P::ProgramArguments::numberOfArguments";
        map<string, string>::const_iterator program_args_it  = attrs.find( program_args_key );
        if ( program_args_it != attrs.end() )
        {
            istringstream num_arguments_as_string( program_args_it->second );
            int64_t       num_arguments;
            try
            {
                num_arguments_as_string >> num_arguments;
                m_num_arguments = max( m_num_arguments, num_arguments );
            }
            catch ( ... )
            {
                cerr << "WARNING: Cannot parse " << program_args_key << " value as number: "
                     << "'" << program_args_it->second << "'" << endl;
            }
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
        m_root_regions.insert( roots[ i ]->get_callee()->get_id() );
        calculate_calltree_types( roots[ i ] );
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
    return m_root_regions.count( region );
}

bool
SCOREP_Score_Profile::omitInTraceEnterLeaveEvents( uint64_t region ) const
{
    return m_regions_to_omit_in_trace_enter_leave_events.count( region );
}

bool
SCOREP_Score_Profile::isDynamicRegion( uint64_t region ) const
{
    return m_dynamic_regions.count( region );
}

int64_t
SCOREP_Score_Profile::getNumberOfProgramArguments() const
{
    return m_num_arguments;
}

void
SCOREP_Score_Profile::iterate_calltree_rec( uint64_t                      process,
                                            SCOREP_Score_CalltreeVisitor& visitor,
                                            Cnode*                        node )
{
    uint64_t region        = node->get_callee()->get_id();
    uint64_t parent_region = -1;
    if ( node->get_parent() )
    {
        parent_region = node->get_parent()->get_callee()->get_id();
    }

    visitor( process,
             region,
             parent_region,
             get_visits( node, process ),
             get_time( node, process ),
             get_hits( node, process ),
             node->get_num_parameters().size(),
             node->get_str_parameters().size() );

    for ( uint32_t i = 0; i < node->num_children(); i++ )
    {
        iterate_calltree_rec( process, visitor, node->get_child( i ) );
    }
}

void
SCOREP_Score_Profile::iterateCalltree( uint64_t                      process,
                                       SCOREP_Score_CalltreeVisitor& visitor )
{
    vector<Cnode*> roots =  m_cube->get_root_cnodev();
    for ( uint64_t i = 0; i < roots.size(); i++ )
    {
        iterate_calltree_rec( process, visitor, roots[ i ] );
    }
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
    if ( paradigm == "io" )
    {
        return SCOREP_SCORE_TYPE_IO;
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
SCOREP_Score_Profile::calculate_calltree_types( Cnode* node )
{
    bool is_on_path = false;
    for ( uint32_t i = 0; i < node->num_children(); i++ )
    {
        is_on_path = calculate_calltree_types( node->get_child( i ) ) ?
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

uint64_t
SCOREP_Score_Profile::get_visits( Cnode*   node,
                                  uint64_t process )
{
    Value* value = m_cube->get_sev_adv( m_visits, CUBE_CALCULATE_EXCLUSIVE,
                                        node, CUBE_CALCULATE_EXCLUSIVE,
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

double
SCOREP_Score_Profile::get_time( Cnode*   node,
                                uint64_t process )
{
    Value* value = m_cube->get_sev_adv( m_time, CUBE_CALCULATE_INCLUSIVE,
                                        node, CUBE_CALCULATE_EXCLUSIVE,
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

uint64_t
SCOREP_Score_Profile::get_hits( Cnode*   node,
                                uint64_t process )
{
    if ( !m_hits )
    {
        return 0;
    }

    Value* value = m_cube->get_sev_adv( m_hits, CUBE_CALCULATE_EXCLUSIVE,
                                        node, CUBE_CALCULATE_EXCLUSIVE,
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
