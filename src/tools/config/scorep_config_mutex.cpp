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
 * @file scorep_config_mutex.cpp
 *
 * Collects information about available mutex systems
 */

#include <config.h>
#include <scorep_config_tool_backend.h>
#include <scorep_config_tool_mpi.h>
#include "scorep_config_mutex.hpp"
#include "scorep_config_adapter.hpp"
#include <iostream>

std::deque<SCOREP_Config_Mutex*> scorep_mutex_systems;

void
scorep_config_init_mutex_systems( void )
{
    scorep_mutex_systems.push_back( new SCOREP_Config_MutexMockup() );
    scorep_mutex_systems.push_back( new SCOREP_Config_MutexOmp() );
    #if SCOREP_BACKEND_HAVE_PTHREAD_MUTEX
    scorep_mutex_systems.push_back( new SCOREP_Config_MutexPthread() );
    #endif
    #if SCOREP_BACKEND_HAVE_PTHREAD_SPINLOCK
    scorep_mutex_systems.push_back( new SCOREP_Config_MutexPthreadSpinlock() );
    #endif
    SCOREP_Config_Mutex::current = scorep_mutex_systems.front();
}

void
scorep_config_final_mutex_systems( void )
{
    SCOREP_Config_Mutex::current = NULL;
    std::deque<SCOREP_Config_Mutex*>::iterator i;
    for ( i = scorep_mutex_systems.begin(); i != scorep_mutex_systems.end(); i++ )
    {
        delete ( *i );
    }
}

/* **************************************************************************************
 * class SCOREP_Config_Mutex
 * *************************************************************************************/

SCOREP_Config_Mutex* SCOREP_Config_Mutex::current = NULL;

SCOREP_Config_Mutex::SCOREP_Config_Mutex( std::string           name,
                                          std::string           variant,
                                          std::string           library,
                                          SCOREP_Config_MutexId id )
{
    m_name    = name;
    m_variant = variant;
    m_library = library;
    m_id      = id;
}

SCOREP_Config_Mutex::~SCOREP_Config_Mutex()
{
}

void
SCOREP_Config_Mutex::printHelp( void )
{
    std::cout << "         " << m_name;
    if ( m_variant.length() > 0 )
    {
        std::cout << ":" << m_variant;
    }
    std::cout << std::endl;
}

bool
SCOREP_Config_Mutex::checkArgument( std::string system )
{
    if (  ( system == m_name + ":" + m_variant )
          || ( system == m_name ) )
    {
        SCOREP_Config_Mutex::current = this;
        return true;
    }
    return false;
}

bool
SCOREP_Config_Mutex::checkId( SCOREP_Config_MutexId mutexId )
{
    if ( mutexId == m_id )
    {
        SCOREP_Config_Mutex::current = this;
        return true;
    }
    return false;
}

void
SCOREP_Config_Mutex::addLibs( std::deque<std::string>&           libs,
                              SCOREP_Config_LibraryDependencies& deps )
{
    deps.addDependency( "libscorep_measurement", "lib" + m_library );
}

void
SCOREP_Config_Mutex::addCFlags( std::string& cflags,
                                bool         build_check,
                                bool         fortran,
                                bool         nvcc )
{
}

void
SCOREP_Config_Mutex::addIncFlags( std::string& incflags, bool build_check, bool nvcc )
{
}

SCOREP_Config_MutexId
SCOREP_Config_Mutex::getId( void )
{
    return m_id;
}

/* **************************************************************************************
 * class SCOREP_Config_MutexMockup
 * *************************************************************************************/

SCOREP_Config_MutexMockup::SCOREP_Config_MutexMockup()
    : SCOREP_Config_Mutex( "none", "", "scorep_mutex_mockup", SCOREP_CONFIG_MUTEX_ID_NONE )
{
}

void
SCOREP_Config_MutexMockup::addLibs( std::deque<std::string>&           libs,
                                    SCOREP_Config_LibraryDependencies& deps )
{
    SCOREP_Config_Mutex::addLibs( libs, deps );
}

/* **************************************************************************************
 * class SCOREP_Config_MutexOmp
 * *************************************************************************************/

SCOREP_Config_MutexOmp::SCOREP_Config_MutexOmp()
    : SCOREP_Config_Mutex( "omp", "", "scorep_mutex_omp", SCOREP_CONFIG_MUTEX_ID_OMP )
{
}

void
SCOREP_Config_MutexOmp::addLibs( std::deque<std::string>&           libs,
                                 SCOREP_Config_LibraryDependencies& deps )
{
    SCOREP_Config_Mutex::addLibs( libs, deps );
}

/* **************************************************************************************
 * class SCOREP_Config_MutexPthread
 * *************************************************************************************/

SCOREP_Config_MutexPthread::SCOREP_Config_MutexPthread()
    : SCOREP_Config_Mutex( "pthread", "", "scorep_mutex_pthread", SCOREP_CONFIG_MUTEX_ID_PTHREAD )
{
}

void
SCOREP_Config_MutexPthread::addLibs( std::deque<std::string>&           libs,
                                     SCOREP_Config_LibraryDependencies& deps )
{
    SCOREP_Config_Mutex::addLibs( libs, deps );
}

/* **************************************************************************************
 * class SCOREP_Config_MutexPthreadSpinlock
 * *************************************************************************************/

SCOREP_Config_MutexPthreadSpinlock::SCOREP_Config_MutexPthreadSpinlock()
    : SCOREP_Config_Mutex( "pthread", "spinlock", "scorep_mutex_pthread_spinlock", SCOREP_CONFIG_MUTEX_ID_PTHREAD_SPINLOCK )
{
}

void
SCOREP_Config_MutexPthreadSpinlock::addLibs( std::deque<std::string>&           libs,
                                             SCOREP_Config_LibraryDependencies& deps )
{
    SCOREP_Config_Mutex::addLibs( libs, deps );
}
