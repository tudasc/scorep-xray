/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
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
 * Implements the base class for selecting paradigm out of a group of mutual exclusive
 * paradigms.
 */

#include <config.h>
#include "scorep_instrumenter_selector.hpp"
#include "scorep_instrumenter_cmd_line.hpp"
#include "scorep_instrumenter_install_data.hpp"
#include "scorep_instrumenter_utils.hpp"
#include "scorep_instrumenter.hpp"
#include <scorep_config_tool_backend.h>
#include <scorep_config_tool_mpi.h>
#include <scorep_config_tool_shmem.h>

#include <scorep_tools_utils.hpp>

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <sstream>

#include <UTILS_Error.h>
#include <UTILS_IO.h>

/* **************************************************************************************
 * class SCOREP_Instrumenter_Selector
 * *************************************************************************************/
SCOREP_Instrumenter_SelectorList SCOREP_Instrumenter_Selector::m_selector_list;

SCOREP_Instrumenter_Selector::SCOREP_Instrumenter_Selector( const std::string& name,
                                                            bool               enableMultiSelection )
{
    m_name             = name;
    m_current_priority = default_selection;
    m_current_selection.clear();
    m_selector_list.push_back( this );
    enableMultiSelection ? m_mode = MULTI_SELECTION : m_mode = SINGLE_SELECTION;
}

SCOREP_Instrumenter_Selector::~SCOREP_Instrumenter_Selector()
{
    SCOREP_Instrumenter_ParadigmList::iterator paradigm;
    for ( paradigm = m_paradigm_list.begin();
          paradigm != m_paradigm_list.end();
          paradigm++ )
    {
        delete ( *paradigm );
    }
    m_paradigm_list.clear();
}

void
SCOREP_Instrumenter_Selector::printHelp( void )
{
    std::cout << "  --" << m_name << "=<paradigm>[:<variant>]";
    if ( m_mode == MULTI_SELECTION )
    {
        std::cout << "(,<paradigm>[:<variant>])*";
    }
    std::cout << "\n                  Possible paradigms and variants are:\n";

    SCOREP_Instrumenter_ParadigmList::iterator paradigm;
    for ( paradigm = m_paradigm_list.begin();
          paradigm != m_paradigm_list.end();
          paradigm++ )
    {
        ( *paradigm )->printHelp();
    }
}

bool
SCOREP_Instrumenter_Selector::checkCommand(  const std::string& current,
                                             const std::string& next )
{
    bool                                       skip_next = false;
    SCOREP_Instrumenter_ParadigmList::iterator paradigm;
    for ( paradigm = m_paradigm_list.begin();
          paradigm != m_paradigm_list.end();
          paradigm++ )
    {
        if ( ( *paradigm )->isSupported() )
        {
            skip_next |= ( *paradigm )->checkCommand( current, next );
        }
    }
    return skip_next;
}

bool
SCOREP_Instrumenter_Selector::checkWrapperOption(  const std::string& current,
                                                   const std::string& next )
{
    bool                                       return_value = false;
    SCOREP_Instrumenter_ParadigmList::iterator paradigm;
    for ( paradigm = m_current_selection.begin();
          paradigm != m_current_selection.end();
          paradigm++ )
    {
        return_value |= ( *paradigm )->checkWrapperOption( current, next );
    }
    return return_value;
}

bool
SCOREP_Instrumenter_Selector::checkOption( const std::string& arg )
{
    if ( arg == "help" )
    {
        printHelp();
        std::cout << std::endl;
        std::cout << "Type 'scorep --help' to get the full list of possible options" << std::endl;
        exit( EXIT_SUCCESS );
    }

    bool is_found = false;

    std::deque<std::string>           categories = string_to_deque( arg, "," );
    std::deque<std::string>::iterator category;
    for ( category = categories.begin();
          category != categories.end() && ( ( m_mode == MULTI_SELECTION ) || !is_found );
          category++ )
    {
        SCOREP_Instrumenter_ParadigmList::iterator paradigm;
        for ( paradigm = m_paradigm_list.begin();
              paradigm != m_paradigm_list.end() && ( ( m_mode == MULTI_SELECTION ) || !is_found );
              paradigm++ )
        {
            if ( ( *paradigm )->checkOption( *category ) )
            {
                select( *paradigm, true );
                is_found = true;
            }
        }
    }

    if ( is_found )
    {
        return true;
    }
    std::cerr << "ERROR: Unknown paradigm '" << arg << "' specified for '--" << m_name << "'\n"
              << "       Type 'scorep --" << m_name << "=help' to get a list of supported paradigms" << std::endl;
    std::cerr << "[Score-P] ERROR: Unknown paradigm '" << arg << "' specified for '--" << m_name << "'\n"
              << "                 Type 'scorep --" << m_name << "=help' to get a list of supported paradigms" << std::endl;
    exit( EXIT_FAILURE );

    /* To please IBM compiler */
    return false;
}

std::string
SCOREP_Instrumenter_Selector::getConfigToolFlag( SCOREP_Instrumenter_CmdLine& /* cmdLine */ )
{
    std::string                                return_value;
    SCOREP_Instrumenter_ParadigmList::iterator paradigm;
    for ( paradigm = m_current_selection.begin();
          paradigm != m_current_selection.end();
          paradigm++ )
    {
        return_value += " --" + m_name + "=" + ( *paradigm )->getConfigName();
    }
    return return_value;
}

void
SCOREP_Instrumenter_Selector::setConfigValue( const std::string& key,
                                              const std::string& value )
{
    SCOREP_Instrumenter_ParadigmList::iterator paradigm;
    for ( paradigm = m_paradigm_list.begin();
          paradigm != m_paradigm_list.end();
          paradigm++ )
    {
        ( *paradigm )->setConfigValue( key, value );
    }
}

void
SCOREP_Instrumenter_Selector::checkCompilerName( const std::string& compiler )
{
    SCOREP_Instrumenter_ParadigmList::iterator paradigm;
    for ( paradigm = m_paradigm_list.begin();
          paradigm != m_paradigm_list.end();
          paradigm++ )
    {
        ( *paradigm )->checkCompilerName( compiler );
    }
}

void
SCOREP_Instrumenter_Selector::checkObjects( SCOREP_Instrumenter& instrumenter )
{
    SCOREP_Instrumenter_ParadigmList::iterator paradigm;
    for ( paradigm = m_paradigm_list.begin();
          paradigm != m_paradigm_list.end();
          paradigm++ )
    {
        ( *paradigm )->checkObjects( instrumenter );
    }
}

bool
SCOREP_Instrumenter_Selector::isInterpositionLibrary( const std::string& libraryName )
{
    SCOREP_Instrumenter_ParadigmList::iterator paradigm;
    for ( paradigm = m_paradigm_list.begin();
          paradigm != m_paradigm_list.end();
          paradigm++ )
    {
        if ( ( *paradigm )->isInterpositionLibrary( libraryName ) )
        {
            return true;
        }
    }
    return false;
}

bool
SCOREP_Instrumenter_Selector::isAlreadySelected( SCOREP_Instrumenter_Paradigm* paradigm )
{
    SCOREP_Instrumenter_ParadigmList::iterator paradigm_iter;
    for ( paradigm_iter = m_current_selection.begin();
          paradigm_iter != m_current_selection.end();
          paradigm_iter++ )
    {
        if ( ( *paradigm_iter ) == paradigm )
        {
            return true;
        }
    }

    return false;
}

/* -------------------------------------------------------------------- static methods */

void
SCOREP_Instrumenter_Selector::printAll( void )
{
    SCOREP_Instrumenter_SelectorList::iterator selector;
    for ( selector = m_selector_list.begin();
          selector != m_selector_list.end();
          selector++ )
    {
        ( *selector )->printHelp();
    }
}

bool
SCOREP_Instrumenter_Selector::checkAllOption( const std::string& origArg )
{
    std::string arg = origArg;

    /* Check for old arguments for backward compatibility */
    if ( arg == "--mpi" )
    {
        std::cerr << "[Score-P] WARNING: You are using the deprecated option '--mpi'\n"
                  << "                   Please use '--mpp=mpi' instead" << std::endl;
        arg = "--mpp=mpi";
    }
    else if ( arg == "--nompi" )
    {
        std::cerr << "[Score-P] WARNING: You are using the deprecated option '--nompi'\n"
                  << "                   Please use '--mpp=none' instead" << std::endl;
        arg = "--mpp=none";
    }
    else if ( arg == "--openmp" )
    {
        std::cerr << "[Score-P] WARNING: You are using the deprecated option '--openmp'\n"
                  << "                   Please use '--thread=omp' instead" << std::endl;
        arg = "--thread=omp";
    }
    else if ( arg == "--noopenmp" )
    {
        std::cerr << "[Score-P] WARNING: You are using the deprecated option '--noopenmp'\n"
                  << "                   Please use '--thread=none' instead" << std::endl;
        arg = "--thread=none";
    }

    /* Try selection */
    std::string::size_type pos = arg.find( '=' );
    if ( pos == std::string::npos )
    {
        return false;
    }

    std::string group_name = arg.substr( 0, pos );
    std::string paradigm   = arg.substr( pos + 1 );

    SCOREP_Instrumenter_SelectorList::iterator selector;
    for ( selector = m_selector_list.begin();
          selector != m_selector_list.end();
          selector++ )
    {
        if ( ( "--" + ( *selector )->m_name == group_name ) &&
             ( *selector )->checkOption( paradigm ) )
        {
            return true;
        }
    }
    return false;
}

bool
SCOREP_Instrumenter_Selector::checkAllCommand( const std::string& current,
                                               const std::string& next )
{
    bool                                       skip_next = false;
    SCOREP_Instrumenter_SelectorList::iterator selector;
    for ( selector = m_selector_list.begin();
          selector != m_selector_list.end();
          selector++ )
    {
        skip_next |= ( *selector )->checkCommand( current, next );
    }
    return skip_next;
}

bool
SCOREP_Instrumenter_Selector::checkAllWrapperOption( const std::string& current,
                                                     const std::string& next )
{
    bool                                       is_wrapper_option = false;
    SCOREP_Instrumenter_SelectorList::iterator selector;
    for ( selector = m_selector_list.begin();
          selector != m_selector_list.end();
          selector++ )
    {
        is_wrapper_option |= ( *selector )->checkWrapperOption( current, next );
    }
    return is_wrapper_option;
}

void
SCOREP_Instrumenter_Selector::setAllConfigValue( const std::string& key,
                                                 const std::string& value )
{
    SCOREP_Instrumenter_SelectorList::iterator selector;
    for ( selector = m_selector_list.begin();
          selector != m_selector_list.end();
          selector++ )
    {
        ( *selector )->setConfigValue( key, value );
    }
}

void
SCOREP_Instrumenter_Selector::checkAllCompilerName( const std::string& compiler )
{
    SCOREP_Instrumenter_SelectorList::iterator selector;
    for ( selector = m_selector_list.begin();
          selector != m_selector_list.end();
          selector++ )
    {
        ( *selector )->checkCompilerName( compiler );
    }
}

void
SCOREP_Instrumenter_Selector::checkAllSupported( void )
{
    SCOREP_Instrumenter_SelectorList::iterator selector;
    for ( selector = m_selector_list.begin();
          selector != m_selector_list.end();
          selector++ )
    {
        SCOREP_Instrumenter_ParadigmList::iterator paradigm;
        for ( paradigm = ( *selector )->m_current_selection.begin();
              paradigm != ( *selector )->m_current_selection.end();
              paradigm++ )
        {
            if ( !( *paradigm )->isSupported() )
            {
                std::string name = ( *paradigm )->getName();
                std::cerr << "[Score-P] ERROR: This Score-P installation does not support '" << name << "'\n"
                          << "                 To analyze an application with '" << name << "',\n"
                          << "                 you need a Score-P installation with support for "
                          << "'" << name << "'\n"
                          << "                 However, you can override a wrong auto-detection with "
                          << "'--" << ( *selector )->m_name << "=<paradigm>'\n"
                          << "                 You can see a list of supported paradigms with "
                          << "'scorep --help'" << std::endl;
                exit( EXIT_FAILURE );
            }
        }
    }
}

void
SCOREP_Instrumenter_Selector::select( SCOREP_Instrumenter_Paradigm* selection,
                                      bool                          is_user_selection )
{
    if ( !selection->isSupported() )
    {
        std::cerr << "[Score-P] ERROR: Selection of an unsupported paradigm implementation: "
                  << "'" << selection->getName() << "'" << std::endl;
        exit( EXIT_FAILURE );
    }

    selection_priority_t priority = ( is_user_selection ? user_selection : auto_selection );

    if (  m_current_priority < priority )
    {
        if ( m_current_priority == default_selection )
        {
            /*
             * Clear the default selection if another
             * option is selected manually/automatically.
             */
            m_current_selection.clear();
        }
        m_current_selection.push_back( selection );
        m_current_priority = priority;
        return;
    }

    SCOREP_Instrumenter_ParadigmList::iterator paradigm;
    for ( paradigm = m_current_selection.begin();
          paradigm != m_current_selection.end();
          paradigm++ )
    {
        if ( ( *paradigm ) == selection )
        {
            /* Do not throw errors, if we confirm the current selection */
            return;
        }
    }

    if ( m_current_priority == priority )
    {
        if ( priority == user_selection && m_mode == SINGLE_SELECTION )
        {
            std::cerr << "[Score-P] ERROR: You can use '--" << m_name << "' only once\n"
                      << "                 It is not possible to select two paradigms from the same group" << std::endl;
            exit( EXIT_FAILURE );
        }
        else
        {
            m_current_selection.push_back( selection );
            return;
        }
        /* If we have multiple paradigms that can be applied, use the first one.
           Thus, do nothing now. */
    }

    // Auto-selections are ignored, when user selection present.
}

void
SCOREP_Instrumenter_Selector::checkDependencies( void )
{
    SCOREP_Instrumenter_SelectorList::iterator selector;
    for ( selector = m_selector_list.begin();
          selector != m_selector_list.end();
          selector++ )
    {
        SCOREP_Instrumenter_ParadigmList::iterator paradigm;
        for ( paradigm = ( *selector )->m_current_selection.begin();
              paradigm != ( *selector )->m_current_selection.end();
              paradigm++ )
        {
            ( *paradigm )->checkDependencies();
        }
    }
}

void
SCOREP_Instrumenter_Selector::checkDefaults( void )
{
    SCOREP_Instrumenter_SelectorList::iterator selector;
    for ( selector = m_selector_list.begin();
          selector != m_selector_list.end();
          selector++ )
    {
        SCOREP_Instrumenter_ParadigmList::iterator paradigm;
        for ( paradigm = ( *selector )->m_current_selection.begin();
              paradigm != ( *selector )->m_current_selection.end();
              paradigm++ )
        {
            ( *paradigm )->checkDefaults();
        }
    }
}

bool
SCOREP_Instrumenter_Selector::supportInstrumentFilters( void )
{
    SCOREP_Instrumenter_SelectorList::const_iterator selector;
    for ( selector = m_selector_list.begin();
          selector != m_selector_list.end();
          selector++ )
    {
        SCOREP_Instrumenter_ParadigmList::iterator paradigm;
        for ( paradigm = ( *selector )->m_current_selection.begin();
              paradigm != ( *selector )->m_current_selection.end();
              paradigm++ )
        {
            if ( ( *paradigm )->supportInstrumentFilters() )
            {
                return true;
            }
        }
    }
    return false;
}

void
SCOREP_Instrumenter_Selector::printSelectedParadigms( void )
{
    SCOREP_Instrumenter_SelectorList::iterator selector;
    for ( selector = m_selector_list.begin();
          selector != m_selector_list.end();
          selector++ )
    {
        SCOREP_Instrumenter_ParadigmList::iterator paradigm;
        for ( paradigm = ( *selector )->m_current_selection.begin();
              paradigm != ( *selector )->m_current_selection.end();
              paradigm++ )
        {
            std::cerr << " " << ( *selector )->m_name << "="
                      << ( *paradigm )->getName();
        }
    }
}

std::string
SCOREP_Instrumenter_Selector::getAllConfigToolFlags( SCOREP_Instrumenter_CmdLine& cmdLine )
{
    std::string                                flags;
    SCOREP_Instrumenter_SelectorList::iterator selector;
    for ( selector = m_selector_list.begin();
          selector != m_selector_list.end();
          selector++ )
    {
        flags += ( *selector )->getConfigToolFlag( cmdLine );
    }
    return flags;
}

bool
SCOREP_Instrumenter_Selector::isParadigmSelected( const std::string& name )
{
    SCOREP_Instrumenter_SelectorList::iterator selector;
    for ( selector = m_selector_list.begin();
          selector != m_selector_list.end();
          selector++ )
    {
        SCOREP_Instrumenter_ParadigmList::iterator paradigm;
        for ( paradigm = ( *selector )->m_current_selection.begin();
              paradigm != ( *selector )->m_current_selection.end();
              paradigm++ )
        {
            if ( ( *paradigm )->checkOption( name ) )
            {
                return true;
            }
        }
    }
    return false;
}

void
SCOREP_Instrumenter_Selector::checkAllObjects( SCOREP_Instrumenter& instrumenter )
{
    SCOREP_Instrumenter_SelectorList::iterator selector;
    for ( selector = m_selector_list.begin();
          selector != m_selector_list.end();
          selector++ )
    {
        ( *selector )->checkObjects( instrumenter );
    }
}

bool
SCOREP_Instrumenter_Selector::isAllInterpositionLibrary( const std::string& libraryName )
{
    SCOREP_Instrumenter_SelectorList::iterator selector;
    for ( selector = m_selector_list.begin();
          selector != m_selector_list.end();
          selector++ )
    {
        if ( ( *selector )->isInterpositionLibrary( libraryName ) )
        {
            return true;
        }
    }
    return false;
}
