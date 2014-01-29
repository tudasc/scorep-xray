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
) : SCOREP_Instrumenter_Paradigm( selector, "pthread", "",
                                  "Pthread mutex locks" )
{
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
 * class SCOREP_Instrumenter_MutexPthreadSpinlock
 * *************************************************************************************/
SCOREP_Instrumenter_MutexPthreadSpinlock::SCOREP_Instrumenter_MutexPthreadSpinlock
(
    SCOREP_Instrumenter_Selector* selector
) : SCOREP_Instrumenter_Paradigm( selector, "pthread", "spinlock",
                                  "Pthread spinlocks" )
{
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
    : SCOREP_Instrumenter_Selector( "mutex" )
{
    m_paradigm_list.push_back( new SCOREP_Instrumenter_MutexMockup( this ) );
#if SCOREP_BACKEND_HAVE_PTHREAD_MUTEX
    m_paradigm_list.push_back( new SCOREP_Instrumenter_MutexPthread( this ) );
#endif
#if SCOREP_BACKEND_HAVE_PTHREAD_SPINLOCK
    m_paradigm_list.push_back( new SCOREP_Instrumenter_MutexPthreadSpinlock( this ) );
#endif
    m_paradigm_list.push_back( new SCOREP_Instrumenter_MutexOmp( this ) );
    m_current_selection = m_paradigm_list.front();
}