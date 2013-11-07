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
 * Implements multi-process paradigms.
 */

#include <config.h>
#include "scorep_instrumenter_mpp.hpp"
#include "scorep_instrumenter_paradigm.hpp"
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
 * class SCOREP_Instrumenter_NoMpp
 * *************************************************************************************/
SCOREP_Instrumenter_NoMpp::SCOREP_Instrumenter_NoMpp
(
    SCOREP_Instrumenter_Selector* selector
) : SCOREP_Instrumenter_Paradigm( selector, "none", "", "No multi-process support." )
{
}

/* **************************************************************************************
 * class SCOREP_Instrumenter_Mpi
 * *************************************************************************************/
SCOREP_Instrumenter_Mpi::SCOREP_Instrumenter_Mpi
(
    SCOREP_Instrumenter_Selector* selector
) : SCOREP_Instrumenter_Paradigm( selector, "mpi", "",
                                  "MPI support using library wrapping" )
{
    if ( !SCOREP_HAVE_PDT_MPI_INSTRUMENTATION )
    {
        m_conflicts.push_back( SCOREP_INSTRUMENTER_ADAPTER_PDT );
    }
#if !( HAVE_BACKEND( MPI_SUPPORT ) )
    unsupported();
#endif
}

void
SCOREP_Instrumenter_Mpi::checkCompilerName( const std::string& compiler )
{
    if ( remove_path( compiler ).substr( 0, 2 ) == "mp" )
    {
        m_selector->select( this, false );
    }
}

bool
SCOREP_Instrumenter_Mpi::checkCommand( const std::string& current,
                                       const std::string& next )
{
    if ( ( current == "-l" ) &&
         ( is_mpi_library( next ) ) )
    {
        m_selector->select( this, false );
        return true;
    }
    else if ( ( current.substr( 0, 2 ) == "-l" ) &&
              ( is_mpi_library( current.substr( 2 ) ) ) )
    {
        m_selector->select( this, false );
        return false;
    }
    return false;
}

void
SCOREP_Instrumenter_Mpi::checkObjects( SCOREP_Instrumenter* instrumenter )
{
    if ( m_selector->getSelection() == this )
    {
        return;
    }

    std::vector<std::string>* object_list = instrumenter->getInputFiles();

    for ( std::vector<std::string>::iterator current_file = object_list->begin();
          current_file != object_list->end();
          current_file++ )
    {
        std::string command = SCOREP_NM " " + *current_file + " | "
                              SCOREP_EGREP " -l 'U (MPI|mpi)_' >/dev/null 2>&1";
        int return_value = system( command.c_str() );
        if ( return_value == 0 )
        {
            m_selector->select( this, false );
            break;
        }
    }
}

/* **************************************************************************************
 * class SCOREP_Instrumenter_Mpp
 * *************************************************************************************/
SCOREP_Instrumenter_Mpp::SCOREP_Instrumenter_Mpp()
    : SCOREP_Instrumenter_Selector( "mpp" )
{
    m_paradigm_list.push_back( new SCOREP_Instrumenter_NoMpp( this ) );
    m_paradigm_list.push_back( new SCOREP_Instrumenter_Mpi( this ) );
    m_current_selection = m_paradigm_list.front();
}
