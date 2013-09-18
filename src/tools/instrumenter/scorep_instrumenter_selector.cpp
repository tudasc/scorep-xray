/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * Forschungszentrum Juelich GmbH, Germany
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

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <sstream>

#include <UTILS_Error.h>
#include <UTILS_IO.h>

/* **************************************************************************************
 * class SCOREP_Instrumenter_Selector
 * *************************************************************************************/
SCOREP_Instrumenter_SelectorList SCOREP_Instrumenter_Selector::m_selector_list;

SCOREP_Instrumenter_Selector::SCOREP_Instrumenter_Selector( std::string name )
{
    m_name              = name;
    m_current_selection = NULL;
    m_current_priority  = default_selection;
    m_selector_list.push_back( this );
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
    SCOREP_Instrumenter_ParadigmList::iterator paradigm;
    std::cout << "  --" << m_name << "=<paradigm>[:<variant>]\n"
              << "                  Possible paradigms and variants are:\n";
    for ( paradigm = m_paradigm_list.begin();
          paradigm != m_paradigm_list.end();
          paradigm++ )
    {
        ( *paradigm )->printHelp();
    }
    std::cout << std::endl;
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
        skip_next |= ( *paradigm )->checkCommand( current, next );
    }
    return skip_next;
}

bool
SCOREP_Instrumenter_Selector::checkOption( std::string arg )
{
    SCOREP_Instrumenter_ParadigmList::iterator paradigm;
    for ( paradigm = m_paradigm_list.begin();
          paradigm != m_paradigm_list.end();
          paradigm++ )
    {
        if ( ( *paradigm )->checkOption( arg ) )
        {
            select( *paradigm, true );
            return true;
        }
    }
    std::cerr << "ERROR: Unknown paradigm '" << arg << "' specified for --" << m_name
              << ".\n"
              << "       Type 'scorep --help' to get a list of supported paradigms."
              << std::endl;
    exit( EXIT_FAILURE );

    /* To please IBM compiler */
    return false;
}

std::string
SCOREP_Instrumenter_Selector::getConfigToolFlag( void )
{
    if ( m_current_selection != NULL )
    {
        return " --" + m_name + "=" + m_current_selection->getConfigName();
    }
    return "";
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
SCOREP_Instrumenter_Selector::checkAllOption( std::string arg )
{
    /* Check for old arguments for backward compatibility */
    if ( arg == "--mpi" )
    {
        std::cerr << "[Score-P] Warning: You are using the deprecated option --mpi.\n"
                  << "          Please use --mpp=mpi instead." << std::endl;
        arg = "--mpp=mpi";
    }
    else if ( arg == "--nompi" )
    {
        std::cerr << "[Score-P] Warning: You are using the deprecated option --nompi.\n"
                  << "          Please use --mpp=none instead." << std::endl;
        arg = "--mpp=none";
    }
    else if ( arg == "--openmp" )
    {
        std::cerr << "[Score-P] Warning: You are using the deprecated option --openmp.\n"
                  << "          Please use --thread=omp instead." << std::endl;
        arg = "--thread=omp";
    }
    else if ( arg == "--noopenmp" )
    {
        std::cerr << "[Score-P] Warning: You are using the deprecated option --noopenmp.\n"
                  << "          Please use --thread=none instead." << std::endl;
        arg = "--thread=none";
    }

    /* Try selection */
    size_t pos = arg.find( '=' );
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
        if ( !( *selector )->m_current_selection->isSupported() )
        {
            std::string name = ( *selector )->m_current_selection->getConfigName();
            std::cerr << "ERROR: This Score-P installation does not support "
                      << name << ".\n"
                      << "       To analyze an application with " << name << ",\n"
                      << "       you need a Score-P installation with suport for "
                      << name << ".\n"
                      << "       However, you can override a wrong auto-detection with --"
                      << ( *selector )->m_name << "=<paradigm>.\n"
                      << "       You can see a list of supported paradigms with "
                      << "'scorep --help'." << std::endl;
            exit( EXIT_FAILURE );
        }
    }
}

void
SCOREP_Instrumenter_Selector::select( SCOREP_Instrumenter_Paradigm* selection,
                                      bool                          is_user_selection )
{
    selection_priority_t priority = ( is_user_selection ? user_selection : auto_selection );

    if (  m_current_priority < priority )
    {
        m_current_selection = selection;
        m_current_priority  = priority;
    }
    else if ( selection == m_current_selection )
    {
        /* Do not throw errors, if we confirm the current selection */
        return;
    }
    else if ( m_current_priority == priority )
    {
        if ( priority == user_selection )
        {
            std::cerr << "ERROR: You can use '--" << m_name << "' only once.\n       "
                      << "It is not possible to select two paradigms from the same group"
                      << std::endl;
        }
        else
        {
            std::cerr << "ERROR: Unable to detect the correct paradigm for '--"
                      << m_name << "' automatically.\n"
                      << "       Found conlicting options for "
                      << m_current_selection->getConfigName() << " and "
                      << selection->getConfigName() << "." << std::endl;
        }
        exit( EXIT_FAILURE );
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
        if ( ( *selector )->m_current_selection != NULL )
        {
            ( *selector )->m_current_selection->checkDependencies();
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
        if ( ( *selector )->m_current_selection != NULL )
        {
            ( *selector )->m_current_selection->checkDefaults();
        }
    }
}

void
SCOREP_Instrumenter_Selector::printSelectedParadigms( void )
{
    SCOREP_Instrumenter_SelectorList::iterator selector;
    for ( selector = m_selector_list.begin();
          selector != m_selector_list.end();
          selector++ )
    {
        if ( ( *selector )->m_current_selection != NULL )
        {
            std::cout << "    " << ( *selector )->m_name << "="
                      << ( *selector )->m_current_selection->getConfigName() << "\n";
        }
    }
}

std::string
SCOREP_Instrumenter_Selector::getAllConfigToolFlags( void )
{
    std::string                                flags;
    SCOREP_Instrumenter_SelectorList::iterator selector;
    for ( selector = m_selector_list.begin();
          selector != m_selector_list.end();
          selector++ )
    {
        flags += ( *selector )->getConfigToolFlag();
    }
    return flags;
}

bool
SCOREP_Instrumenter_Selector::isParadigmSelected( std::string name )
{
    SCOREP_Instrumenter_SelectorList::iterator selector;
    for ( selector = m_selector_list.begin();
          selector != m_selector_list.end();
          selector++ )
    {
        if ( ( *selector )->m_current_selection != NULL )
        {
            if ( ( *selector )->m_current_selection->checkOption( name ) )
            {
                return true;
            }
        }
    }
    return false;
}
