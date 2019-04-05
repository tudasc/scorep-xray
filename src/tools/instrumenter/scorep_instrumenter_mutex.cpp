/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2016,
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
 * Implements mutex locks.
 */

#include <config.h>
#include "scorep_instrumenter_mutex.hpp"
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
 * class SCOREP_Instrumenter_MutexMockup
 * *************************************************************************************/
SCOREP_Instrumenter_MutexMockup::SCOREP_Instrumenter_MutexMockup
(
    SCOREP_Instrumenter_Selector* selector
) : SCOREP_Instrumenter_Paradigm( selector, "none", "", "serial case, no locking" )
{
}

/* **************************************************************************************
 * class SCOREP_Instrumenter_MutexOmp
 * *************************************************************************************/
SCOREP_Instrumenter_MutexOmp::SCOREP_Instrumenter_MutexOmp
(
    SCOREP_Instrumenter_Selector* selector
) : SCOREP_Instrumenter_Paradigm( selector, "omp", "",
                                  "OpenMP locks" )
{
}

bool
SCOREP_Instrumenter_MutexOmp::checkCommand( const std::string& current,
                                            const std::string& next )
{
    if ( current == m_variant )
    {
        m_selector->select( this, false );
    }
    return false;
}

/* **************************************************************************************
 * class SCOREP_Instrumenter_MutexPthread
 * *************************************************************************************/
SCOREP_Instrumenter_MutexPthread::SCOREP_Instrumenter_MutexPthread
(
    SCOREP_Instrumenter_Selector* selector
) : SCOREP_Instrumenter_Paradigm( selector, "pthread", "default",
                                  "Pthread mutex locks" )
{
#if !SCOREP_BACKEND_HAVE_PTHREAD_MUTEX
    unsupported();
#endif
    m_conflicts.push_back( SCOREP_INSTRUMENTER_ADAPTER_PTHREAD );
}

bool
SCOREP_Instrumenter_MutexPthread::checkCommand( const std::string& current,
                                                const std::string& next )
{
    if ( current == m_variant )
    {
        m_selector->select( this, false );
    }
    return false;
}

/* **************************************************************************************
 *  * class SCOREP_Instrumenter_MutexPthreadWrap
 *   * *************************************************************************************/
SCOREP_Instrumenter_MutexPthreadWrap::SCOREP_Instrumenter_MutexPthreadWrap
(
    SCOREP_Instrumenter_Selector* selector
) : SCOREP_Instrumenter_Paradigm( selector, "pthread", "wrap",
                                  "Wrapped Pthread mutex locks" )
{
#if !SCOREP_BACKEND_HAVE_PTHREAD_MUTEX
    unsupported();
#endif
}

/* **************************************************************************************
 * class SCOREP_Instrumenter_MutexPthreadSpinlock
 * *************************************************************************************/
SCOREP_Instrumenter_MutexPthreadSpinlock::SCOREP_Instrumenter_MutexPthreadSpinlock
(
    SCOREP_Instrumenter_Selector* selector
) : SCOREP_Instrumenter_Paradigm( selector, "pthread", "spinlock",
                                  "Pthread spinlocks" )
{
#if !SCOREP_BACKEND_HAVE_PTHREAD_SPINLOCK
    unsupported();
#endif
}

bool
SCOREP_Instrumenter_MutexPthreadSpinlock::checkCommand( const std::string& current,
                                                        const std::string& next )
{
    if ( current == m_variant )
    {
        m_selector->select( this, false );
    }
    return false;
}


/* **************************************************************************************
 * class SCOREP_Instrumenter_Mutex
 * *************************************************************************************/
SCOREP_Instrumenter_Mutex::SCOREP_Instrumenter_Mutex()
    : SCOREP_Instrumenter_Selector( "mutex", false )
{
    m_paradigm_list.push_back( new SCOREP_Instrumenter_MutexMockup( this ) );
    m_paradigm_list.push_back( new SCOREP_Instrumenter_MutexPthread( this ) );
    m_paradigm_list.push_back( new SCOREP_Instrumenter_MutexPthreadWrap( this ) );
    m_paradigm_list.push_back( new SCOREP_Instrumenter_MutexPthreadSpinlock( this ) );
    m_paradigm_list.push_back( new SCOREP_Instrumenter_MutexOmp( this ) );
    m_current_selection.push_back( m_paradigm_list.front() );
}
