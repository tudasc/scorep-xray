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
 * Implements threading paradigms.
 */

#include <config.h>
#include "scorep_instrumenter_thread.hpp"
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
 * class SCOREP_Instrumenter_SingleThreaded
 * *************************************************************************************/
SCOREP_Instrumenter_SingleThreaded::SCOREP_Instrumenter_SingleThreaded
(
    SCOREP_Instrumenter_Selector* selector
) : SCOREP_Instrumenter_Paradigm( selector, "none", "", "No thread support." )
{
}

/* **************************************************************************************
 * class SCOREP_Instrumenter_OmpTpd
 * *************************************************************************************/
SCOREP_Instrumenter_OmpTpd::SCOREP_Instrumenter_OmpTpd
(
    SCOREP_Instrumenter_Selector* selector
) : SCOREP_Instrumenter_Paradigm( selector, "omp", "pomp_tpd",
                                  "OpenMP support using OPARI2 thread tracking" )
{
    m_requires.push_back( SCOREP_INSTRUMENTER_ADAPTER_OPARI );
    m_openmp_cflag = SCOREP_OPENMP_CFLAGS;
}

bool
SCOREP_Instrumenter_OmpTpd::checkCommand( const std::string& current,
                                          const std::string& next )
{
    if ( current == m_openmp_cflag )
    {
        m_selector->select( this, false );
    }
#if SCOREP_BACKEND_COMPILER_INTEL
    if ( current == "-openmp" )
    {
        m_selector->select( this, false );
    }
#endif
#if SCOREP_BACKEND_COMPILER_IBM
    if ( ( current.length() > m_openmp_cflag.length() ) &&
         ( current.substr( 0, 6 ) == "-qsmp=" ) )
    {
        size_t end;
        for ( size_t start = 5; start != std::string::npos; start = end )
        {
            end = current.find( ':', start + 1 );
            if ( current.substr( start + 1, end - start - 1 ) == "omp" )
            {
                m_selector->select( this, false );
            }
        }
    }
#endif
    return false;
}

void
SCOREP_Instrumenter_OmpTpd::setConfigValue( const std::string& key,
                                            const std::string& value )
{
    if ( key == "OPENMP_CFLAGS" && value != "" )
    {
        m_openmp_cflag = value;
    }
}

/* **************************************************************************************
 * class SCOREP_Instrumenter_Thread
 * *************************************************************************************/
SCOREP_Instrumenter_Thread::SCOREP_Instrumenter_Thread()
    : SCOREP_Instrumenter_Selector( "thread" )
{
    m_paradigm_list.push_back( new SCOREP_Instrumenter_SingleThreaded( this ) );
    m_paradigm_list.push_back( new SCOREP_Instrumenter_OmpTpd( this ) );
    m_current_selection = m_paradigm_list.front();
}
