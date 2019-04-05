/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2014-2017,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * Implements the base class for an instrumentation method.
 */

#include <config.h>
#include "scorep_instrumenter_adapter.hpp"
#include "scorep_instrumenter_cmd_line.hpp"
#include "scorep_instrumenter_install_data.hpp"
#include "scorep_instrumenter_utils.hpp"
#include "scorep_instrumenter.hpp"
#include <scorep_tools_utils.hpp>
#include <scorep_config_tool_backend.h>
#include <scorep_config_tool_mpi.h>

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <sstream>

#include <UTILS_Error.h>
#include <UTILS_IO.h>

/* **************************************************************************************
 * class SCOREP_Instrumenter_Adapter
 * *************************************************************************************/
SCOREP_Instrumenter_AdapterList SCOREP_Instrumenter_Adapter::m_adapter_list;


SCOREP_Instrumenter_Adapter::SCOREP_Instrumenter_Adapter( SCOREP_Instrumenter_AdapterId id,
                                                          const std::string&            name )
{
    m_id                 = id;
    m_name               = name;
    m_usage              = detect;
    m_adapter_list[ id ] = this;
    m_params             = "";
    m_unsupported        = false;
    m_use_params         = false;
}

SCOREP_Instrumenter_Adapter::~SCOREP_Instrumenter_Adapter()
{
}

SCOREP_Instrumenter_AdapterId
SCOREP_Instrumenter_Adapter::getId( void )
{
    return m_id;
}

void
SCOREP_Instrumenter_Adapter::checkDependencies( void )
{
    SCOREP_Instrumenter_DependencyList::iterator i;
    bool                                         prereqs = m_prerequisites.empty();

    for ( i = m_prerequisites.begin(); i != m_prerequisites.end(); i++ )
    {
        if ( getAdapter( *i )->isEnabled() )
        {
            prereqs = true;
            break;
        }
    }
    if ( !prereqs )
    {
        m_usage = disabled;
        std::cerr << "[Score-P] WARNING: Disabled '" << m_name << "' because none of its "
                  << "prerequisites are enabled" << std::endl;
    }

    for ( i = m_conflicts.begin(); i != m_conflicts.end(); i++ )
    {
        conflict( m_name, *i );
    }

    for ( i = m_requires.begin(); i != m_requires.end(); i++ )
    {
        require( m_name, *i );
    }
}


void
SCOREP_Instrumenter_Adapter::checkDefaults( void )
{
    SCOREP_Instrumenter_DependencyList::iterator i;

    for ( i = m_default_off.begin(); i != m_default_off.end(); i++ )
    {
        defaultOff( *i );
    }

    for ( i = m_default_on.begin(); i != m_default_on.end(); i++ )
    {
        defaultOn( *i );
    }
}

bool
SCOREP_Instrumenter_Adapter::isEnabled( void ) const
{
    return m_usage == enabled;
}

bool
SCOREP_Instrumenter_Adapter::supportInstrumentFilters( void ) const
{
    return false;
}

void
SCOREP_Instrumenter_Adapter::printHelp( void )
{
    static std::string space = "                  ";

    if ( !m_unsupported )
    {
        std::cout << "  --" << m_name;
        if ( m_use_params )
        {
            std::cout << "[=<parameter-list>]\n" << space;
        }
        else if ( m_name.length() > 14 )
        {
            std::cout << "\n" << space;
        }
        else
        {
            std::cout << space.substr( 0, 14 - m_name.length() );
        }
        std::cout << "Enables " << m_name << " instrumentation.\n";
        if ( m_use_params )
        {
            std::cout << space << "You may add additional parameters that are passed to "
                      << m_name << ".\n";
        }

        std::cout << wrap_lines( getDepList( m_requires,
                                             "It requires and, thus, automatically enables" ),
                                 18, 18 );
        std::cout << wrap_lines( getDepList( m_prerequisites,
                                             "It cannot be enabled, if not at least "
                                             "one of the following is enabled:" ),
                                 18, 18 );
        std::cout << wrap_lines( getDepList( m_conflicts,
                                             "It conflicts and, thus, automatically "
                                             "disables" ),
                                 18, 18 );
        std::cout << wrap_lines( getDepList( m_default_on,
                                             "By default, it enables also" ), 18, 18 );
        std::cout << wrap_lines( getDepList( m_default_off,
                                             "By default, it disables" ), 18, 18 );
    }
    std::cout << "  --no" << m_name;
    if ( m_name.length() > 12 )
    {
        std::cout << "\n" << space;
    }
    else
    {
        std::cout << space.substr( 0, 12 - m_name.length() );
    }
    std::cout << "Disables " << m_name << " instrumentation." << std::endl;
}

bool
SCOREP_Instrumenter_Adapter::checkOption( const std::string& arg )
{
    if ( arg == "--" + m_name )
    {
        if ( m_unsupported )
        {
            error_unsupported();
        }
        m_usage = enabled;
        return true;
    }
    if ( arg == "--no" + m_name )
    {
        m_usage = disabled;
        return true;
    }
    if ( ( arg.length() > m_name.length() + 3 ) &&
         ( arg.substr( 0, 3 + m_name.length() ) == "--" + m_name + "=" ) )
    {
        if ( m_unsupported )
        {
            error_unsupported();
        }
        m_usage   = enabled;
        m_params += " " + arg.substr( 3 + m_name.length(), std::string::npos );
        return true;
    }
    return false;
}

std::string
SCOREP_Instrumenter_Adapter::preprocess( SCOREP_Instrumenter&         instrumenter,
                                         SCOREP_Instrumenter_CmdLine& cmdLine,
                                         const std::string&           input_file )
{
    return input_file;
}

std::string
SCOREP_Instrumenter_Adapter::precompile( SCOREP_Instrumenter&         instrumenter,
                                         SCOREP_Instrumenter_CmdLine& cmdLine,
                                         const std::string&           input_file )
{
    return input_file;
}

void
SCOREP_Instrumenter_Adapter::prelink( SCOREP_Instrumenter&         instrumenter,
                                      SCOREP_Instrumenter_CmdLine& cmdLine )
{
}

void
SCOREP_Instrumenter_Adapter::postlink( SCOREP_Instrumenter&         instrumenter,
                                       SCOREP_Instrumenter_CmdLine& cmdLine )
{
}

std::string
SCOREP_Instrumenter_Adapter::getConfigToolFlag( SCOREP_Instrumenter_CmdLine& /* cmdLine */ )
{
    if ( isEnabled() )
    {
        return " --" + m_name;
    }
    return "";
}

void
SCOREP_Instrumenter_Adapter::setBuildCheck( SCOREP_Instrumenter_CmdLine& cmdLine )
{
}

void
SCOREP_Instrumenter_Adapter::setConfigValue( const std::string& key,
                                             const std::string& value )
{
}

bool
SCOREP_Instrumenter_Adapter::checkCommand( const std::string& current,
                                           const std::string& next )
{
    return false;
}

void
SCOREP_Instrumenter_Adapter::checkCompilerName( const std::string& compiler )
{
}

void
SCOREP_Instrumenter_Adapter::checkObjects( SCOREP_Instrumenter& instrumenter )
{
}

bool
SCOREP_Instrumenter_Adapter::isInterpositionLibrary( const std::string& libraryName )
{
    return false;
}

/* ------------------------------------------------------------------------- protected */

void
SCOREP_Instrumenter_Adapter::unsupported( void )
{
    m_unsupported = true;
    m_usage       = disabled;
}

void
SCOREP_Instrumenter_Adapter::error_unsupported( void )
{
    std::cerr << "[Score-P] ERROR: This installation does not support '" << m_name << "' "
              << "instrumentation" << std::endl;
    exit( EXIT_FAILURE );
}

/* ---------------------------------------------------------------------------- static */
void
SCOREP_Instrumenter_Adapter::destroyAll( void )
{
    SCOREP_Instrumenter_AdapterList::iterator adapter;
    for ( adapter = m_adapter_list.begin(); adapter != m_adapter_list.end(); adapter++ )
    {
        delete ( adapter->second );
    }
    m_adapter_list.clear();
}

void
SCOREP_Instrumenter_Adapter::printAll( void )
{
    SCOREP_Instrumenter_AdapterList::iterator adapter;
    for ( adapter = m_adapter_list.begin(); adapter != m_adapter_list.end(); adapter++ )
    {
        adapter->second->printHelp();
    }
}

bool
SCOREP_Instrumenter_Adapter::checkAllOption( const std::string& arg )
{
    SCOREP_Instrumenter_AdapterList::iterator adapter;
    for ( adapter = m_adapter_list.begin(); adapter != m_adapter_list.end(); adapter++ )
    {
        if ( adapter->second->checkOption( arg ) )
        {
            return true;
        }
    }
    return false;
}

void
SCOREP_Instrumenter_Adapter::checkAllDependencies( void )
{
    SCOREP_Instrumenter_AdapterList::iterator adapter;
    for ( adapter = m_adapter_list.begin(); adapter != m_adapter_list.end(); adapter++ )
    {
        if ( adapter->second->isEnabled() )
        {
            adapter->second->checkDependencies();
        }
    }
}

void
SCOREP_Instrumenter_Adapter::checkAllDefaults( void )
{
    SCOREP_Instrumenter_AdapterList::iterator adapter;
    for ( adapter = m_adapter_list.begin(); adapter != m_adapter_list.end(); adapter++ )
    {
        if ( adapter->second->isEnabled() )
        {
            adapter->second->checkDefaults();
        }
    }
}

bool
SCOREP_Instrumenter_Adapter::supportAnyInstrumentFilters( void )
{
    SCOREP_Instrumenter_AdapterList::const_iterator adapter_it;
    for ( adapter_it = m_adapter_list.begin();
          adapter_it != m_adapter_list.end();
          adapter_it++ )
    {
        const SCOREP_Instrumenter_Adapter& adapter = *adapter_it->second;
        if ( adapter.isEnabled() && adapter.supportInstrumentFilters() )
        {
            return true;
        }
    }
    return false;
}

void
SCOREP_Instrumenter_Adapter::printEnabledAdapterList( void )
{
    SCOREP_Instrumenter_AdapterList::iterator adapter;
    for ( adapter = m_adapter_list.begin(); adapter != m_adapter_list.end(); adapter++ )
    {
        if ( adapter->second->isEnabled() )
        {
            std::cerr << " " << adapter->second->m_name;
        }
    }
}

std::string
SCOREP_Instrumenter_Adapter::getAllConfigToolFlags( SCOREP_Instrumenter_CmdLine& cmdLine )
{
    std::string                               flags;
    SCOREP_Instrumenter_AdapterList::iterator adapter;
    for ( adapter = m_adapter_list.begin(); adapter != m_adapter_list.end(); adapter++ )
    {
        flags += adapter->second->getConfigToolFlag( cmdLine );
    }
    return flags;
}

SCOREP_Instrumenter_Adapter*
SCOREP_Instrumenter_Adapter::getAdapter( SCOREP_Instrumenter_AdapterId id )
{
    return m_adapter_list[ id ];
}

void
SCOREP_Instrumenter_Adapter::setAllBuildCheck( SCOREP_Instrumenter_CmdLine& cmdLine )
{
    SCOREP_Instrumenter_AdapterList::iterator adapter;
    for ( adapter = m_adapter_list.begin(); adapter != m_adapter_list.end(); adapter++ )
    {
        adapter->second->setBuildCheck( cmdLine );
    }
}

void
SCOREP_Instrumenter_Adapter::setAllConfigValue( const std::string& key,
                                                const std::string& value )
{
    SCOREP_Instrumenter_AdapterList::iterator adapter;
    for ( adapter = m_adapter_list.begin(); adapter != m_adapter_list.end(); adapter++ )
    {
        adapter->second->setConfigValue( key, value );
    }
}

bool
SCOREP_Instrumenter_Adapter::checkAllCommand(  const std::string& current,
                                               const std::string& next )
{
    bool                                      skip_next = false;
    SCOREP_Instrumenter_AdapterList::iterator adapter;
    for ( adapter = m_adapter_list.begin(); adapter != m_adapter_list.end(); adapter++ )
    {
        skip_next |= adapter->second->checkCommand( current, next );
    }
    return skip_next;
}

void
SCOREP_Instrumenter_Adapter::checkAllCompilerName( const std::string& compiler )
{
    SCOREP_Instrumenter_AdapterList::iterator adapter;
    for ( adapter = m_adapter_list.begin(); adapter != m_adapter_list.end(); adapter++ )
    {
        adapter->second->checkCompilerName( compiler );
    }
}

void
SCOREP_Instrumenter_Adapter::checkAllObjects( SCOREP_Instrumenter& instrumenter )
{
    SCOREP_Instrumenter_AdapterList::iterator adapter;
    for ( adapter = m_adapter_list.begin(); adapter != m_adapter_list.end(); adapter++ )
    {
        adapter->second->checkObjects( instrumenter );
    }
}

bool
SCOREP_Instrumenter_Adapter::isAllInterpositionLibrary( const std::string& libraryName )
{
    SCOREP_Instrumenter_AdapterList::iterator adapter;
    for ( adapter = m_adapter_list.begin(); adapter != m_adapter_list.end(); adapter++ )
    {
        if ( adapter->second->isInterpositionLibrary( libraryName ) )
        {
            return true;
        }
    }
    return false;
}

void
SCOREP_Instrumenter_Adapter::require( const std::string&            caller,
                                      SCOREP_Instrumenter_AdapterId id )
{
    SCOREP_Instrumenter_AdapterList::iterator adapter;

    adapter = m_adapter_list.find( id );
    if ( adapter == m_adapter_list.end() )
    {
        std::cerr << "[Score-P] ERROR: Required adapter for '" << caller << "' adapter "
                  << "is not available" << std::endl;
        exit( EXIT_FAILURE );
    }
    if ( adapter->second->m_usage == disabled )
    {
        std::cerr << "[Score-P] ERROR: Cannot disable '" << adapter->second->m_name << "' adapter\n"
                  << "                 Is required by " << caller << " adapter" << std::endl;
        exit( EXIT_FAILURE );
    }
    if ( adapter->second->m_usage == detect )
    {
        adapter->second->m_usage = enabled;
        adapter->second->checkDependencies();
    }
}

void
SCOREP_Instrumenter_Adapter::conflict( const std::string&            caller,
                                       SCOREP_Instrumenter_AdapterId id )
{
    SCOREP_Instrumenter_AdapterList::iterator adapter;

    adapter = m_adapter_list.find( id );
    if ( adapter == m_adapter_list.end() )
    {
        return;
    }
    if ( adapter->second->m_usage == enabled )
    {
        std::cerr << "[Score-P] ERROR: Cannot enable '" << adapter->second->m_name << "' adapter\n"
                  << "                 Conflicts with '" << caller << "' adapter" << std::endl;
        exit( EXIT_FAILURE );
    }
    adapter->second->m_usage = disabled;
}

void
SCOREP_Instrumenter_Adapter::defaultOn( SCOREP_Instrumenter_AdapterId id )
{
    SCOREP_Instrumenter_AdapterList::iterator adapter;

    adapter = m_adapter_list.find( id );
    if ( ( adapter != m_adapter_list.end() ) &&
         ( adapter->second->m_usage == detect ) )
    {
        adapter->second->m_usage = enabled;
        adapter->second->checkDependencies();
        adapter->second->checkDefaults();
    }
}

void
SCOREP_Instrumenter_Adapter::defaultOff( SCOREP_Instrumenter_AdapterId id )
{
    SCOREP_Instrumenter_AdapterList::iterator adapter;

    adapter = m_adapter_list.find( id );
    if ( ( adapter != m_adapter_list.end() ) &&
         ( adapter->second->m_usage == detect ) )
    {
        adapter->second->m_usage = disabled;
    }
}

std::string
SCOREP_Instrumenter_Adapter::getDepList( const SCOREP_Instrumenter_DependencyList& list,
                                         const std::string&                        entry )
{
    std::stringstream                                  output;
    SCOREP_Instrumenter_DependencyList::const_iterator i;
    if ( !list.empty() )
    {
        output << entry << " ";
        for ( i = list.begin(); i != list.end(); ++i )
        {
            if ( i != list.begin() )
            {
                if ( i + 1 == list.end() )
                {
                    output << " and ";
                }
                else
                {
                    output << ", ";
                }
            }
            output << getAdapter( *i )->m_name;
        }
        output << " instrumentation.\n";
    }

    return output.str();
}
