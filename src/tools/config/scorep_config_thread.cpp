/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2014, 2016,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2014, 2017,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
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

std::deque<SCOREP_Config_ThreadSystem*> SCOREP_Config_ThreadSystem::m_all;

SCOREP_Config_ThreadSystem* SCOREP_Config_ThreadSystem::current = NULL;

void
SCOREP_Config_ThreadSystem::init( void )
{
    m_all.push_back( new SCOREP_Config_MockupThreadSystem() );
#if SCOREP_BACKEND_HAVE_OMP_TPD || SCOREP_BACKEND_HAVE_OMP_ANCESTRY
    m_all.push_back( new SCOREP_Config_OmpThreadSystem() );
#endif
#if SCOREP_BACKEND_HAVE_PTHREAD_SUPPORT
    m_all.push_back( new SCOREP_Config_PthreadThreadSystem() );
#endif
    current = m_all.front();
}

void
SCOREP_Config_ThreadSystem::fini( void )
{
    current = NULL;
    std::deque<SCOREP_Config_ThreadSystem*>::iterator i;
    for ( i = m_all.begin(); i != m_all.end(); i++ )
    {
        delete ( *i );
    }
}

void
SCOREP_Config_ThreadSystem::printAll( void )
{
    std::cout << "   --thread=<threading system>\n"
              << "              Available threading systems are:\n";
    std::deque<SCOREP_Config_ThreadSystem*>::iterator i;
    for ( i = m_all.begin(); i != m_all.end(); i++ )
    {
        ( *i )->printHelp();
    }
}

bool
SCOREP_Config_ThreadSystem::checkAll( const std::string& arg )
{
    /* The format is --thread=<system>[:<variant>]
     * Note that the variant is optional. Thus, the user
     * may provide either both, or only a system. */

    std::string                                       system( arg );
    std::string                                       variant;
    std::string::size_type                            pos = system.find( ':' );
    std::deque<SCOREP_Config_ThreadSystem*>::iterator i;

    /* If system and variant are provided, both must match */
    if ( pos != std::string::npos )
    {
        system  = arg.substr( 0, pos );
        variant = arg.substr( pos + 1 );

        for ( i = m_all.begin(); i != m_all.end(); i++ )
        {
            if ( ( system == ( *i )->m_name ) &&
                 ( variant == ( *i )->m_variant ) )
            {
                current = *i;
                return true;
            }
        }
        return false;
    }

    /* If only the system is provided, choose the first matching system */
    for ( i = m_all.begin(); i != m_all.end(); i++ )
    {
        if ( system == ( *i )->m_name )
        {
            current = *i;
            return true;
        }
    }

    return false;
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
    std::cout << "               " << m_name;
    if ( m_variant.length() > 0 )
    {
        std::cout << ":" << m_variant;
    }
    if ( this == current )
    {
        std::cout << " (default)";
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
SCOREP_Config_ThreadSystem::addCFlags( std::string&           cflags,
                                       bool                   build_check,
                                       SCOREP_Config_Language language,
                                       bool                   nvcc )
{
}

void
SCOREP_Config_ThreadSystem::addLdFlags( std::string& ldflags,
                                        bool         nvcc )
{
}

SCOREP_Config_MutexId
SCOREP_Config_ThreadSystem::validateDependencies()
{
    if ( SCOREP_Config_Mutex::current->getId() == SCOREP_CONFIG_MUTEX_ID_NONE )
    {
        return m_mutexId;
    }
    return SCOREP_Config_Mutex::current->getId();
}

void
SCOREP_Config_ThreadSystem::addIncFlags( std::string&           incflags,
                                         bool                   build_check,
                                         SCOREP_Config_Language language,
                                         bool                   nvcc )
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
    : SCOREP_Config_ThreadSystem( "none", "", "scorep_thread_mockup",
                                  SCOREP_CONFIG_MUTEX_ID_NONE, SCOREP_CONFIG_THREAD_SYSTEM_ID_NONE )
{
}

void
SCOREP_Config_MockupThreadSystem::getInitStructName( std::deque<std::string>& init_structs )
{
}

/* **************************************************************************************
 * class SCOREP_Config_OmpThreadSystem
 * *************************************************************************************/

SCOREP_Config_OmpThreadSystem::SCOREP_Config_OmpThreadSystem()
    : SCOREP_Config_ThreadSystem( "omp", "", "scorep_thread_fork_join_omp",
                                  SCOREP_CONFIG_MUTEX_ID_OMP, SCOREP_CONFIG_THREAD_SYSTEM_ID_OMP )
{
}

void
SCOREP_Config_OmpThreadSystem::addLibs( std::deque<std::string>&           libs,
                                        SCOREP_Config_LibraryDependencies& deps )
{
    libs.push_back( "libscorep_adapter_opari2_openmp_event" );
    deps.addDependency( "libscorep_measurement", "libscorep_adapter_opari2_openmp_mgmt" );
    deps.addDependency( "libscorep_measurement", "libscorep_thread_fork_join_omp" );
}

void
SCOREP_Config_OmpThreadSystem::addCFlags( std::string&           cflags,
                                          bool                   build_check,
                                          SCOREP_Config_Language language,
                                          bool                   nvcc )
{
    SCOREP_Config_Opari2Adapter::printOpariCFlags( build_check,
                                                   true,
                                                   language,
                                                   nvcc );

#if SCOREP_BACKEND_COMPILER_IBM
    if ( language == SCOREP_CONFIG_LANGUAGE_FORTRAN )
    {
        cflags += "-d -WF,-qlanglvl=classic ";
    }
#endif
}

void
SCOREP_Config_OmpThreadSystem::addIncFlags( std::string&           incflags,
                                            bool                   build_check,
                                            SCOREP_Config_Language language,
                                            bool                   nvcc )
{
    SCOREP_Config_Opari2Adapter::printOpariCFlags( build_check,
                                                   false,
                                                   language,
                                                   nvcc );
}

void
SCOREP_Config_OmpThreadSystem::getInitStructName( std::deque<std::string>& init_structs )
{
    init_structs.push_back( "SCOREP_Subsystem_Opari2OpenmpAdapter" );
    init_structs.push_back( "SCOREP_Subsystem_ThreadForkJoin" );
}

/* ****************************************************************************
* class SCOREP_Config_PthreadThreadSystem
* ****************************************************************************/

SCOREP_Config_PthreadThreadSystem::SCOREP_Config_PthreadThreadSystem()
    : SCOREP_Config_ThreadSystem( "pthread",
                                  "",
                                  "scorep_thread_create_wait_pthread",
                                  SCOREP_CONFIG_MUTEX_ID_PTHREAD_WRAP,
                                  SCOREP_CONFIG_THREAD_SYSTEM_ID_PTHREAD )
{
}

void
SCOREP_Config_PthreadThreadSystem::addLibs( std::deque<std::string>&           libs,
                                            SCOREP_Config_LibraryDependencies& deps )
{
    libs.push_back( "libscorep_adapter_pthread_event" );
    deps.addDependency( "libscorep_measurement", "libscorep_adapter_pthread_mgmt" );
    deps.addDependency( "libscorep_thread_create_wait_pthread", "libscorep_adapter_pthread_mgmt" );
    deps.addDependency( "libscorep_measurement", "libscorep_thread_create_wait_pthread" );
}

void
SCOREP_Config_PthreadThreadSystem::addLdFlags( std::string& ldflags,
                                               bool         nvcc )
{
    ldflags += " -Wl,"
               "--undefined,__wrap_pthread_create,"
               "-wrap,pthread_create,"
               "-wrap,pthread_join,"
               "-wrap,pthread_exit,"
               "-wrap,pthread_cancel,"
               "-wrap,pthread_detach,"
               "-wrap,pthread_mutex_init,"
               "-wrap,pthread_mutex_destroy,"
               "-wrap,pthread_mutex_lock,"
               "-wrap,pthread_mutex_unlock,"
               "-wrap,pthread_mutex_trylock,"
               "-wrap,pthread_cond_init,"
               "-wrap,pthread_cond_signal,"
               "-wrap,pthread_cond_broadcast,"
               "-wrap,pthread_cond_wait,"
               "-wrap,pthread_cond_timedwait,"
               "-wrap,pthread_cond_destroy";
}

void
SCOREP_Config_PthreadThreadSystem::getInitStructName( std::deque<std::string>& init_structs )
{
    init_structs.push_back( "SCOREP_Subsystem_PthreadAdapter" );
    init_structs.push_back( "SCOREP_Subsystem_ThreadCreateWait" );
}
