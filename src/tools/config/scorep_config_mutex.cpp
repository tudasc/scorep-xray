/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2014,
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
 * Collects information about available mutex systems
 */

#include <config.h>
#include <scorep_config_tool_backend.h>
#include <scorep_config_tool_mpi.h>
#include "scorep_config_mutex.hpp"
#include "scorep_config_adapter.hpp"
#include <iostream>

/* **************************************************************************************
 * class SCOREP_Config_Mutex
 * *************************************************************************************/

std::deque<SCOREP_Config_Mutex*> SCOREP_Config_Mutex::all;

SCOREP_Config_Mutex* SCOREP_Config_Mutex::current = NULL;

void
SCOREP_Config_Mutex::init( void )
{
    all.push_back( new SCOREP_Config_MutexMockup() );
    all.push_back( new SCOREP_Config_MutexOmp() );
    #if SCOREP_BACKEND_HAVE_PTHREAD_MUTEX
    all.push_back( new SCOREP_Config_MutexPthread() );
    all.push_back( new SCOREP_Config_MutexPthreadWrap() );
    #endif
    #if SCOREP_BACKEND_HAVE_PTHREAD_SPINLOCK
    all.push_back( new SCOREP_Config_MutexPthreadSpinlock() );
    #endif
    current = all.front();
}

void
SCOREP_Config_Mutex::fini( void )
{
    current = 0;
    std::deque<SCOREP_Config_Mutex*>::iterator i;
    for ( i = all.begin(); i != all.end(); i++ )
    {
        delete ( *i );
    }
}

void
SCOREP_Config_Mutex::printAll( void )
{
    std::cout << "   --mutex=<locking system>[:<variant>]\n"
              << "              Available locking systems are:\n";
    std::deque<SCOREP_Config_Mutex*>::iterator i;
    for ( i = all.begin(); i != all.end(); i++ )
    {
        ( *i )->printHelp();
    }
    std::cout << "              If no variant is specified the default for the respective\n"
              << "              threading system is used.\n";
}

bool
SCOREP_Config_Mutex::checkAll( const std::string& arg )
{
    current = 0;

    std::deque<SCOREP_Config_Mutex*>::iterator i;
    for ( i = all.begin(); i != all.end(); i++ )
    {
        if ( ( *i )->checkArgument( arg ) )
        {
            current = *i;
            return true;
        }
    }
    return current != 0;
}



void
SCOREP_Config_Mutex::select( SCOREP_Config_MutexId mutexId )
{
    std::deque<SCOREP_Config_Mutex*>::iterator i;
    for ( i = all.begin(); i != all.end(); i++ )
    {
        if ( ( *i )->m_id == mutexId )
        {
            current = *i;
            return;
        }
    }
}

SCOREP_Config_Mutex::SCOREP_Config_Mutex( const std::string&    name,
                                          const std::string&    variant,
                                          const std::string&    library,
                                          SCOREP_Config_MutexId id )
    : m_name( name ),
    m_variant( variant ),
    m_library( library ),
    m_id( id )
{
}

SCOREP_Config_Mutex::~SCOREP_Config_Mutex()
{
}

void
SCOREP_Config_Mutex::printHelp( void )
{
    std::cout << "               " << m_name;
    if ( m_variant.length() > 0 )
    {
        std::cout << ":" << m_variant;
    }
    std::cout << std::endl;
}

bool
SCOREP_Config_Mutex::checkArgument( const std::string& system )
{
    if (  ( system == m_name + ":" + m_variant )
          || ( system == m_name ) )
    {
        current = this;
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
SCOREP_Config_Mutex::addCFlags( std::string&           cflags,
                                bool                   build_check,
                                SCOREP_Config_Language language,
                                bool                   nvcc )
{
}

void
SCOREP_Config_Mutex::addIncFlags( std::string&           incflags,
                                  bool                   build_check,
                                  SCOREP_Config_Language language,
                                  bool                   nvcc )
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

/* **************************************************************************************
 * class SCOREP_Config_MutexOmp
 * *************************************************************************************/

SCOREP_Config_MutexOmp::SCOREP_Config_MutexOmp()
    : SCOREP_Config_Mutex( "omp", "", "scorep_mutex_omp", SCOREP_CONFIG_MUTEX_ID_OMP )
{
}

/* **************************************************************************************
 * class SCOREP_Config_MutexPthread
 * *************************************************************************************/

SCOREP_Config_MutexPthread::SCOREP_Config_MutexPthread()
    : SCOREP_Config_Mutex( "pthread", "default", "scorep_mutex_pthread", SCOREP_CONFIG_MUTEX_ID_PTHREAD )
{
}

/* **************************************************************************************
 * class SCOREP_Config_MutexPthreadWrap
 * *************************************************************************************/

SCOREP_Config_MutexPthreadWrap::SCOREP_Config_MutexPthreadWrap()
    : SCOREP_Config_Mutex( "pthread", "wrap", "scorep_mutex_pthread_wrap", SCOREP_CONFIG_MUTEX_ID_PTHREAD_WRAP )
{
}

/* **************************************************************************************
 * class SCOREP_Config_MutexPthreadSpinlock
 * *************************************************************************************/

SCOREP_Config_MutexPthreadSpinlock::SCOREP_Config_MutexPthreadSpinlock()
    : SCOREP_Config_Mutex( "pthread", "spinlock", "scorep_mutex_pthread_spinlock", SCOREP_CONFIG_MUTEX_ID_PTHREAD_SPINLOCK )
{
}
