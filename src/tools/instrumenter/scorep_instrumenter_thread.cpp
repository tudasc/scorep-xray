/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2014, 2016-2017, 2020, 2022-2023,
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
    SCOREP_Instrumenter_Selector* selector,
    const std::string&            variant,
    const std::string&            description )
    : SCOREP_Instrumenter_Paradigm( selector, "omp", variant, description )
{
    m_openmp_flags = { SCOREP_OPENMP_FLAGS_ALL };
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

/**
 * Checks whether the current argument is indicates whether OpenMP is used.
 * @param current     The current argument.
 * @returns true if @a current indicates OpenMP usage.
 */
bool
SCOREP_Instrumenter_Omp::checkForOpenmpOption( const std::string& current )
{
    for ( std::set<std::string>::const_iterator it = m_openmp_flags.begin(); it != m_openmp_flags.end(); ++it )
    {
        std::string openmp_flag = *it;

        if ( current == openmp_flag ||
             ( current.length() > openmp_flag.length() &&
               current.substr( 0, openmp_flag.length() + 1 ) == openmp_flag + "=" ) )
        {
            return true;
        }

        // handle OpenMP options that are combined with other compiler options
#if SCOREP_BACKEND_COMPILER_CC_IBM || SCOREP_BACKEND_COMPILER_CXX_IBM || SCOREP_BACKEND_COMPILER_FC_IBM
        if ( ( current.length() > openmp_flag.length() ) &&
             ( current.substr( 0, 6 ) == "-qsmp=" ) &&
             find_string_in_list( current.substr( 6 ), "omp", ':' )
             != std::string::npos )
        {
            return true;
        }
#endif  // SCOREP_BACKEND_COMPILER_CC_IBM || SCOREP_BACKEND_COMPILER_CXX_IBM || SCOREP_BACKEND_COMPILER_FC_IBM
#if SCOREP_BACKEND_COMPILER_CC_FUJITSU || SCOREP_BACKEND_COMPILER_CXX_FUJITSU || SCOREP_BACKEND_COMPILER_FC_FUJITSU
        if ( ( current.length() > openmp_flag.length() ) &&
             ( current.substr( 0, 2 ) == "-K" ) &&
             ( find_string_in_list( current.substr( 2 ), "openmp", ',' )
               != std::string::npos ) )
        {
            return true;
        }
#endif  // SCOREP_BACKEND_COMPILER_CC_FUJITSU || SCOREP_BACKEND_COMPILER_CXX_FUJITSU || SCOREP_BACKEND_COMPILER_FC_FUJITSU
    }
    return false;
}

/* *****************************************************************************
 * class SCOREP_Instrumenter_OmpOpari2
 * ****************************************************************************/
SCOREP_Instrumenter_OmpOpari2::SCOREP_Instrumenter_OmpOpari2
(
    SCOREP_Instrumenter_Selector* selector
) : SCOREP_Instrumenter_Omp( selector,
                             "opari2",
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

#if !SCOREP_BACKEND_HAVE_OMP_ANCESTRY && !SCOREP_BACKEND_HAVE_OMP_TPD
    unsupported();
#endif
}

void
SCOREP_Instrumenter_OmpOpari2::checkDependencies( void )
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
 * class SCOREP_Instrumenter_OmpOmpt
 * ****************************************************************************/
SCOREP_Instrumenter_OmpOmpt::SCOREP_Instrumenter_OmpOmpt
(
    SCOREP_Instrumenter_Selector* selector
) : SCOREP_Instrumenter_Omp( selector, "ompt", "OpenMP support using thread tracking via OMPT." ),
    m_warn_intel_openmp_flags( false )
{
    // For Fortran, pdt inserts a specification statement into the executable
    // section (omp parallel do), which is illegal. Subsequent opari2
    // processing fixes this issue. With ompt instead of opari2 the error
    // persists, thus, forbid the combination pdt + ompt (not only for Fortran,
    // but also for C and C++ as there enough alternatives to pdt
    // instrumentation)
    m_conflicts.push_back( SCOREP_INSTRUMENTER_ADAPTER_PDT );

    // We conflict with OPARI2 OpenMP instrumentation, but not with OPARI2
    // POMP user instrumentation. We can't express this via
    // SCOREP_INSTRUMENTER_ADAPTER_OPARI.

#if !HAVE( BACKEND_SCOREP_OMPT_SUPPORT )
    unsupported();
#endif // ! BACKEND_SCOREP_OMPT_SUPPORT
}

bool
SCOREP_Instrumenter_OmpOmpt::checkForOpenmpOption( const std::string& current )
{
    #if SCOREP_BACKEND_COMPILER_CC_INTEL_ONEAPI || SCOREP_BACKEND_COMPILER_CXX_INTEL_ONEAPI || SCOREP_BACKEND_COMPILER_FC_INTEL_ONEAPI
    // checkForOpenmpOption is called several times. Once we found a
    // problematic flag, check in the current and subsequent
    // invocations if we are using OMPT (either implicitly because
    // configured with --enable-default=ompt or explicitly via
    // --thread=omp:ompt). This is potentially reported by
    // m_selector->isParadigmSelected( "omp:ompt" ) in one invocation
    // and then in subsequent invocations as well.
    if ( current == "-fiopenmp" || current == "-qopenmp" )
    {
        m_warn_intel_openmp_flags = true;
    }
    if ( m_warn_intel_openmp_flags && m_selector->isParadigmSelected( "omp:ompt" ) )
    {
        UTILS_WARN_ONCE( "Detected flag -fiopenmp / -qopenmp. If you run into issues, please try to use -fopenmp instead or take a look at the open issues via 'scorep-info open-issues'!" );
    }
    #endif // SCOREP_BACKEND_COMPILER_CC_INTEL_ONEAPI || SCOREP_BACKEND_COMPILER_CXX_INTEL_ONEAPI || SCOREP_BACKEND_COMPILER_FC_INTEL_ONEAPI

    return SCOREP_Instrumenter_Omp::checkForOpenmpOption( current );
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

bool
SCOREP_Instrumenter_Pthread::is_pthread_library( const std::string& libraryName )
{
    return check_lib_name( libraryName, std::string( "pthread" ) ) ||
           check_lib_name( libraryName, std::string( "pthreads" ) ) ||
           check_lib_name( libraryName, std::string( "lthread" ) );
}

/* **************************************************************************************
 * class SCOREP_Instrumenter_Thread
 * *************************************************************************************/
SCOREP_Instrumenter_Thread::SCOREP_Instrumenter_Thread()
    : SCOREP_Instrumenter_Selector( "thread", false )
{
    m_paradigm_list.push_back( new SCOREP_Instrumenter_SingleThreaded( this ) );
#if HAVE( BACKEND_SCOREP_DEFAULT_OMPT )
    m_paradigm_list.push_back( new SCOREP_Instrumenter_OmpOmpt( this ) );
    m_paradigm_list.push_back( new SCOREP_Instrumenter_OmpOpari2( this ) );
#else
    m_paradigm_list.push_back( new SCOREP_Instrumenter_OmpOpari2( this ) );
    m_paradigm_list.push_back( new SCOREP_Instrumenter_OmpOmpt( this ) );
#endif
    m_paradigm_list.push_back( new SCOREP_Instrumenter_Pthread( this ) );
    m_current_selection.push_back( m_paradigm_list.front() );
}
