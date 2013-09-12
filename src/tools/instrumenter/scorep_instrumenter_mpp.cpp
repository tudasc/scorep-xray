/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * See the COPYING file in the package base directory for details.
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
    if ( compiler.substr( 0, 2 ) == "mp" )
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
