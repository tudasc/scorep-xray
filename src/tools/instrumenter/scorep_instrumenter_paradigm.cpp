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
#include "scorep_instrumenter_paradigm.hpp"
#include "scorep_instrumenter_cmd_line.hpp"
#include "scorep_instrumenter_install_data.hpp"
#include "scorep_instrumenter_selector.hpp"
#include "scorep_instrumenter_utils.hpp"
#include "scorep_instrumenter.hpp"
#include <scorep_tools_utils.hpp>
#include <scorep_config_tool_backend.h>
#include <scorep_config_tool_mpi.h>
#include <scorep_config_tool_shmem.h>

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <sstream>

#include <UTILS_Error.h>
#include <UTILS_IO.h>

/* **************************************************************************************
 * class SCOREP_Instrumenter_Paradigm
 * *************************************************************************************/
SCOREP_Instrumenter_Paradigm::SCOREP_Instrumenter_Paradigm( SCOREP_Instrumenter_Selector* selector,
                                                            const std::string&            name,
                                                            const std::string&            variant,
                                                            const std::string&            description )
{
    m_selector    = selector;
    m_name        = name;
    m_variant     = variant;
    m_description = description;
    m_unsupported = false;
}

SCOREP_Instrumenter_Paradigm::~SCOREP_Instrumenter_Paradigm()
{
}

void
SCOREP_Instrumenter_Paradigm::checkDependencies( void )
{
    SCOREP_Instrumenter_DependencyList::iterator i;

    for ( i = m_conflicts.begin(); i != m_conflicts.end(); i++ )
    {
        SCOREP_Instrumenter_Adapter::conflict( m_name, *i );
    }

    for ( i = m_requires.begin(); i != m_requires.end(); i++ )
    {
        SCOREP_Instrumenter_Adapter::require( m_name, *i );
    }
}

void
SCOREP_Instrumenter_Paradigm::checkDefaults( void )
{
    SCOREP_Instrumenter_DependencyList::iterator i;

    for ( i = m_default_off.begin(); i != m_default_off.end(); i++ )
    {
        SCOREP_Instrumenter_Adapter::defaultOff( *i );
    }

    for ( i = m_default_on.begin(); i != m_default_on.end(); i++ )
    {
        SCOREP_Instrumenter_Adapter::defaultOn( *i );
    }
}


bool
SCOREP_Instrumenter_Paradigm::supportInstrumentFilters( void ) const
{
    return false;
}

void
SCOREP_Instrumenter_Paradigm::printHelp( void )
{
    static std::string space = "                  ";

    if ( !m_unsupported )
    {
        std::cout << "              " << getName() << "\n";
        std::cout << wrap_lines( m_description, 18, 18 ) << "\n";

        std::cout << wrap_lines( SCOREP_Instrumenter_Adapter::getDepList(
                                     m_requires, "It requires and, thus, "
                                     "automatically enables" ),
                                 18, 18 );
        std::cout << wrap_lines( SCOREP_Instrumenter_Adapter::getDepList(
                                     m_conflicts, "It conflicts and, thus, "
                                     "automatically disables" ),
                                 18, 18 );
        std::cout << wrap_lines( SCOREP_Instrumenter_Adapter::getDepList(
                                     m_default_on, "By default, it enables also" ),
                                 18, 18 );
        std::cout << wrap_lines( SCOREP_Instrumenter_Adapter::getDepList(
                                     m_default_off, "By default, it disables" ),
                                 18, 18 );
    }
}

bool
SCOREP_Instrumenter_Paradigm::checkOption( const std::string& arg )
{
    if ( isSupported() )
    {
        if ( ( arg == getName() ) || ( arg == m_name ) )
        {
            return true;
        }
        return false;
    }
    else if ( arg == getName() )
    {
        // If the user specified this particular variant we can say it is not supported.
        // Otherwise another variant exists that support the paradigm.
        std::cerr << "[Score-P] ERROR: '" << getName() << "' is not supported by "
                  << "this Score-P installation" << std::endl;
        exit( EXIT_FAILURE );
    }
    return false;
}

std::string
SCOREP_Instrumenter_Paradigm::getConfigName( void )
{
    return getName();
}

std::string
SCOREP_Instrumenter_Paradigm::getName( void )
{
    return m_variant == "" ? m_name : m_name + ":" + m_variant;
}

bool
SCOREP_Instrumenter_Paradigm::checkCommand( const std::string& current,
                                            const std::string& next )
{
    return false;
}

bool
SCOREP_Instrumenter_Paradigm::checkWrapperOption( const std::string& current,
                                                  const std::string& next )
{
    return false;
}

void
SCOREP_Instrumenter_Paradigm::setConfigValue( const std::string& key,
                                              const std::string& value )
{
}

bool
SCOREP_Instrumenter_Paradigm::isSupported( void )
{
    return !m_unsupported;
}

void
SCOREP_Instrumenter_Paradigm::checkCompilerName( const std::string& compiler )
{
}

void
SCOREP_Instrumenter_Paradigm::unsupported( void )
{
    m_unsupported = true;
}

void
SCOREP_Instrumenter_Paradigm::checkObjects( SCOREP_Instrumenter& instrumenter )
{
}

bool
SCOREP_Instrumenter_Paradigm::isInterpositionLibrary( const std::string& libraryName )
{
    return false;
}
