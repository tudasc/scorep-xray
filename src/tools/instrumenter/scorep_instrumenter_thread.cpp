/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2014, 2016-2017,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2015, 2017,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2017,
 * Technische Universitaet Darmstadt, Germany
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
#include "scorep_instrumenter_opari.hpp"
#include "scorep_instrumenter_cmd_line.hpp"
#include "scorep_instrumenter_install_data.hpp"
#include "scorep_instrumenter_utils.hpp"
#include "scorep_instrumenter.hpp"
#include <scorep_config_tool_backend.h>
#include <scorep_config_tool_mpi.h>

#include <scorep_tools_utils.hpp>

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

/* *****************************************************************************
 * class SCOREP_Instrumenter_Omp
 * ****************************************************************************/
SCOREP_Instrumenter_Omp::SCOREP_Instrumenter_Omp(
    SCOREP_Instrumenter_Selector* selector )
    : SCOREP_Instrumenter_Paradigm( selector, "omp", "",
#if SCOREP_BACKEND_HAVE_OMP_ANCESTRY
                                    "OpenMP support using thread tracking via "
                                    "ancestry functions\n"
                                    "                  in OpenMP 3.0 and later."
#elif SCOREP_BACKEND_HAVE_OMP_TPD
                                    "OpenMP support using thread tracking via "
                                    "OPARI2-instrumented\n"
                                    "                  threadprivate variable."
#endif
                                    )
{
    m_requires.push_back( SCOREP_INSTRUMENTER_ADAPTER_OPARI );
    m_openmp_cflag = SCOREP_OPENMP_CFLAGS;

#if !SCOREP_BACKEND_HAVE_OMP_ANCESTRY && !SCOREP_BACKEND_HAVE_OMP_TPD
    unsupported();
#endif
}

bool
SCOREP_Instrumenter_Omp::checkCommand( const std::string& current,
                                       const std::string& next )
{
    if ( checkForOpenmpOption( current ) )
    {
        m_selector->select( this, false );
    }
    return false;
}

void
SCOREP_Instrumenter_Omp::setConfigValue( const std::string& key,
                                         const std::string& value )
{
    if ( key == "OPENMP_CFLAGS" && value != "" )
    {
        m_openmp_cflag = value;
    }
}

/**
 * Checks whether the current argument is indicates whether OpenMP is used.
 * @param current     The current argument.
 * @returns true if @a current indicates OpenMP usage.
 */
bool
SCOREP_Instrumenter_Omp::checkForOpenmpOption( const std::string& current )
{
    if ( current == m_openmp_cflag ||
         ( current.length() > m_openmp_cflag.length() &&
           current.substr( 0, m_openmp_cflag.length() + 1 ) == m_openmp_cflag + "=" ) )
    {
        return true;
    }
#if SCOREP_BACKEND_COMPILER_INTEL
    if ( current == "-openmp" || current == "-qopenmp" )
    {
        return true;
    }
#endif
#if SCOREP_BACKEND_COMPILER_IBM
    std::string omp_string = "omp";
    if ( ( current.length() > m_openmp_cflag.length() ) &&
         ( current.substr( 0, 6 ) == "-qsmp=" ) )
    {
        std::string sub_string = current.substr( 6 );
        if ( find_string_in_list( sub_string, omp_string, ':' ) != std::string::npos )
        {
            return true;
        }
    }
#endif
#if SCOREP_BACKEND_COMPILER_FUJITSU
    if ( ( current.length() > m_openmp_cflag.length() ) &&
         ( current.substr( 0, 2 ) == "-K" ) &&
         ( find_string_in_list( current.substr( 2 ), "openmp", ',' )
           != std::string::npos ) )
    {
        return true;
    }
#endif
    return false;
}

void
SCOREP_Instrumenter_Omp::checkDependencies( void )
{
    SCOREP_Instrumenter_Paradigm::checkDependencies();

    SCOREP_Instrumenter_Adapter* adapter = SCOREP_Instrumenter_Adapter::getAdapter( SCOREP_INSTRUMENTER_ADAPTER_OPARI );
    if ( ( adapter != NULL ) )
    {
        ( ( SCOREP_Instrumenter_OpariAdapter* )adapter )->enableOpenmpDefault();
    }

#if SCOREP_BACKEND_HAVE_OMP_TPD && !SCOREP_BACKEND_HAVE_OMP_ANCESTRY
    if ( ( adapter != NULL ) )
    {
        ( ( SCOREP_Instrumenter_OpariAdapter* )adapter )->setTpdMode( true );
    }
#endif /* SCOREP_BACKEND_HAVE_OMP_TPD && !SCOREP_BACKEND_HAVE_OMP_ANCESTRY */
}

/* *****************************************************************************
 * class SCOREP_Instrumenter_Pthread
 * ****************************************************************************/

SCOREP_Instrumenter_Pthread::SCOREP_Instrumenter_Pthread(
    SCOREP_Instrumenter_Selector* selector ) :
    SCOREP_Instrumenter_Paradigm( selector,
                                  "pthread",
                                  "",
                                  "Pthread support using thread tracking via "
                                  "library wrapping." ),
    m_pthread_cflag( SCOREP_BACKEND_PTHREAD_CFLAGS ),
    m_pthread_lib( SCOREP_BACKEND_PTHREAD_LIBS )
{
    m_requires.push_back( SCOREP_INSTRUMENTER_ADAPTER_PTHREAD );
    m_conflicts.push_back( SCOREP_INSTRUMENTER_ADAPTER_OPARI );
    m_requires.push_back( SCOREP_INSTRUMENTER_ADAPTER_LINKTIME_WRAPPING );
#if !SCOREP_BACKEND_HAVE_PTHREAD_SUPPORT
    unsupported();
#endif
}

bool
SCOREP_Instrumenter_Pthread::checkCommand( const std::string& current,
                                           const std::string& next )
{
    // See ax_pthread.m4:
    // possible cflags: -Kthread -kthread -pthread -pthreads -mthreads --thread-safe -mt
    // possible libs: -lpthreads -llthread -lpthread
    if ( current == m_pthread_cflag )
    {
        m_selector->select( this, false );
    }
    else if ( current == m_pthread_lib )
    {
        m_selector->select( this, false );
    }
    else if ( ( current.substr( 0, 2 ) == "-l" ) &&
              ( is_pthread_library( current.substr( 2 ) ) ) )
    {
        m_selector->select( this, false );
    }
    else if ( ( current == "-l" ) &&
              ( is_pthread_library( next ) ) )
    {
        m_selector->select( this, false );
        return true;
    }

    return false;
}

void
SCOREP_Instrumenter_Pthread::setConfigValue( const std::string& key,
                                             const std::string& value )
{
    if ( key == "PTHREAD_CFLAGS" && value != "" )
    {
        m_pthread_cflag = value;
    }
}

bool
SCOREP_Instrumenter_Pthread::is_pthread_library( const std::string& libraryName )
{
    return check_lib_name( libraryName, std::string( "pthread" ) ) ||
           check_lib_name( libraryName, std::string( "pthreads" ) ) ||
           check_lib_name( libraryName, std::string( "lthread" ) );
}

/* *****************************************************************************
 * class SCOREP_Instrumenter_PthreadAdapter
 * ****************************************************************************/
SCOREP_Instrumenter_PthreadAdapter::SCOREP_Instrumenter_PthreadAdapter( void )
    : SCOREP_Instrumenter_Adapter( SCOREP_INSTRUMENTER_ADAPTER_PTHREAD, "pthread" )
{
}

void
SCOREP_Instrumenter_PthreadAdapter::printHelp( void )
{
    return;
}

std::string
SCOREP_Instrumenter_PthreadAdapter::getConfigToolFlag( SCOREP_Instrumenter_CmdLine& /* cmdLine */ )
{
    return "";
}

/* **************************************************************************************
 * class SCOREP_Instrumenter_Thread
 * *************************************************************************************/
SCOREP_Instrumenter_Thread::SCOREP_Instrumenter_Thread()
    : SCOREP_Instrumenter_Selector( "thread", false )
{
    m_paradigm_list.push_back( new SCOREP_Instrumenter_SingleThreaded( this ) );
    m_paradigm_list.push_back( new SCOREP_Instrumenter_Omp( this ) );
    m_paradigm_list.push_back( new SCOREP_Instrumenter_Pthread( this ) );
    m_current_selection.push_back( m_paradigm_list.front() );
}
