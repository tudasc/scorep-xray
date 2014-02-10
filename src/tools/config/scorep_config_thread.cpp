/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
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
 * Collects information about available thread systems
 */

#include <config.h>
#include <scorep_config_tool_backend.h>
#include <scorep_config_tool_mpi.h>
#include "scorep_config_thread.hpp"
#include "scorep_config_adapter.hpp"
#include "scorep_config_mutex.hpp"
#include <iostream>

/* **************************************************************************************
 * class SCOREP_Config_ThreadSystem
 * *************************************************************************************/

std::deque<SCOREP_Config_ThreadSystem*> SCOREP_Config_ThreadSystem::all;

SCOREP_Config_ThreadSystem* SCOREP_Config_ThreadSystem::current = 0;

void
SCOREP_Config_ThreadSystem::init( void )
{
    all.push_back( new SCOREP_Config_MockupThreadSystem() );
    all.push_back( new SCOREP_Config_PompTpdThreadSystem() );
#if SCOREP_BACKEND_HAVE_OMP_ANCESTRY
    all.push_back( new SCOREP_Config_OmpAncestryThreadSystem() );
#endif
    current = all.front();
}

void
SCOREP_Config_ThreadSystem::fini( void )
{
    current = 0;
    std::deque<SCOREP_Config_ThreadSystem*>::iterator i;
    for ( i = all.begin(); i != all.end(); i++ )
    {
        delete ( *i );
    }
}

void
SCOREP_Config_ThreadSystem::printAll( void )
{
    std::cout << "   --thread=<threading system>[:<variant>]\n"
              << "            Available threading systems are:\n";
    std::deque<SCOREP_Config_ThreadSystem*>::iterator i;
    for ( i = all.begin(); i != all.end(); i++ )
    {
        ( *i )->printHelp();
    }
    std::cout << "            If no variant is specified the first matching\n"
              << "            threading system is used.\n";
}

bool
SCOREP_Config_ThreadSystem::checkAll( const std::string& arg )
{
    current = 0;

    std::string system( arg );
    std::string variant;
    size_t      pos = system.find( ':' );
    if ( pos != std::string::npos )
    {
        system  = arg.substr( 0, pos );
        variant = arg.substr( pos + 1 );
    }

    std::deque<SCOREP_Config_ThreadSystem*>::iterator i;
    for ( i = all.begin(); i != all.end(); i++ )
    {
        if ( ( current == 0 && system == ( *i )->m_name ) ||
             ( system == ( *i )->m_name && variant == ( *i )->m_variant ) )
        {
            current = *i;
            break;
        }
    }
    return current != 0;
}

SCOREP_Config_ThreadSystem::SCOREP_Config_ThreadSystem( const std::string&           name,
                                                        const std::string&           variant,
                                                        const std::string&           library,
                                                        SCOREP_Config_MutexId        mutexId,
                                                        SCOREP_Config_ThreadSystemId id )
    : m_name( name ),
      m_variant( variant ),
      m_library( library ),
      m_mutexId( mutexId ),
      m_id( id )
{
}

SCOREP_Config_ThreadSystem::~SCOREP_Config_ThreadSystem()
{
}

void
SCOREP_Config_ThreadSystem::printHelp( void )
{
    std::cout << "         " << m_name;
    if ( m_variant.length() > 0 )
    {
        std::cout << ":" << m_variant;
    }
    if ( this == current )
    {
        std::cout << "\tThis is the default.";
    }
    std::cout << std::endl;
}

void
SCOREP_Config_ThreadSystem::addLibs( std::deque<std::string>&           libs,
                                     SCOREP_Config_LibraryDependencies& deps )
{
    deps.addDependency( "libscorep_measurement", "lib" + m_library );
    //  deps.addDependency( "libscorep_measurement", "lib" + m_mutexlib );
}

void
SCOREP_Config_ThreadSystem::addCFlags( std::string& cflags,
                                       bool         build_check,
                                       bool         fortran,
                                       bool         nvcc )
{
}
SCOREP_Config_MutexId
SCOREP_Config_ThreadSystem::validateDependencies()
{
    if ( SCOREP_Config_Mutex::current->getId()  == SCOREP_CONFIG_MUTEX_ID_NONE )
    {
        return m_mutexId;
    }
    return SCOREP_Config_Mutex::current->getId();
}

void
SCOREP_Config_ThreadSystem::addIncFlags( std::string& incflags,
                                         bool         build_check,
                                         bool         nvcc )
{
}

SCOREP_Config_ThreadSystemId
SCOREP_Config_ThreadSystem::getId( void )
{
    return m_id;
}

/* **************************************************************************************
 * class SCOREP_Config_MockupThreadSystem
 * *************************************************************************************/

SCOREP_Config_MockupThreadSystem::SCOREP_Config_MockupThreadSystem()
    : SCOREP_Config_ThreadSystem( "none", "", "scorep_thread_fork_join_mockup",
                                  SCOREP_CONFIG_MUTEX_ID_NONE, SCOREP_CONFIG_THREAD_SYSTEM_ID_NONE )
{
}

void
SCOREP_Config_MockupThreadSystem::addLibs( std::deque<std::string>&           libs,
                                           SCOREP_Config_LibraryDependencies& deps )
{
    SCOREP_Config_ThreadSystem::addLibs( libs, deps );
    deps.addDependency( "libscorep_measurement", "libscorep_adapter_pomp_omp_mgmt_mockup" );
}

/* **************************************************************************************
 * class SCOREP_Config_PompTpdThreadSystem
 * *************************************************************************************/

SCOREP_Config_PompTpdThreadSystem::SCOREP_Config_PompTpdThreadSystem()
    : SCOREP_Config_ThreadSystem( "omp", "pomp_tpd", "scorep_thread_fork_join_omp_tpd",
                                  SCOREP_CONFIG_MUTEX_ID_OMP, SCOREP_CONFIG_THREAD_SYSTEM_ID_POMP_TPD )
{
}

void
SCOREP_Config_PompTpdThreadSystem::addLibs( std::deque<std::string>&           libs,
                                            SCOREP_Config_LibraryDependencies& deps )
{
    libs.push_back( "libscorep_adapter_pomp_omp_event" );
    deps.addDependency( "libscorep_measurement", "libscorep_adapter_pomp_omp_mgmt" );
    deps.addDependency( "libscorep_measurement", "libscorep_thread_fork_join_omp_tpd" );
}

void
SCOREP_Config_PompTpdThreadSystem::addCFlags( std::string& cflags,
                                              bool         build_check,
                                              bool         fortran,
                                              bool         nvcc )
{
    SCOREP_Config_PompAdapter::printOpariCFlags( build_check,
                                                 true,
                                                 fortran,
                                                 nvcc );

#if SCOREP_BACKEND_COMPILER_IBM
    if ( fortran )
    {
        cflags += "-d -WF,-qlanglvl=classic ";
    }
#endif
}

void
SCOREP_Config_PompTpdThreadSystem::addIncFlags( std::string& incflags,
                                                bool         build_check,
                                                bool         nvcc )
{
    SCOREP_Config_PompAdapter::printOpariCFlags( build_check,
                                                 false,
                                                 false,
                                                 nvcc );
}

/* **************************************************************************************
 * class SCOREP_Config_OmpAncestryThreadSystem
 * *************************************************************************************/

SCOREP_Config_OmpAncestryThreadSystem::SCOREP_Config_OmpAncestryThreadSystem()
    : SCOREP_Config_ThreadSystem( "omp", "ancestry", "scorep_thread_fork_join_omp_ancestry",
                                  SCOREP_CONFIG_MUTEX_ID_OMP, SCOREP_CONFIG_THREAD_SYSTEM_ID_OMP_ANCESTRY )
{
}

void
SCOREP_Config_OmpAncestryThreadSystem::addLibs( std::deque<std::string>&           libs,
                                                SCOREP_Config_LibraryDependencies& deps )
{
    libs.push_back( "libscorep_adapter_pomp_omp_event" );
    deps.addDependency( "libscorep_measurement", "libscorep_adapter_pomp_omp_mgmt" );
    deps.addDependency( "libscorep_measurement", "libscorep_thread_fork_join_omp_ancestry" );
}

void
SCOREP_Config_OmpAncestryThreadSystem::addCFlags( std::string& cflags,
                                                  bool         build_check,
                                                  bool         fortran,
                                                  bool         nvcc )
{
    SCOREP_Config_PompAdapter::printOpariCFlags( build_check,
                                                 true,
                                                 fortran,
                                                 nvcc );

#if SCOREP_BACKEND_COMPILER_IBM
    if ( fortran )
    {
        cflags += "-d -WF,-qlanglvl=classic ";
    }
#endif
}

void
SCOREP_Config_OmpAncestryThreadSystem::addIncFlags( std::string& incflags,
                                                    bool         build_check,
                                                    bool         nvcc )
{
    SCOREP_Config_PompAdapter::printOpariCFlags( build_check,
                                                 false,
                                                 false,
                                                 nvcc );
}
