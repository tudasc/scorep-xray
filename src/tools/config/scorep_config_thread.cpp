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
 * @file scorep_config_thread.cpp
 *
 * Collects information about available thread systems
 */

#include <config.h>
#include <scorep_config_tool_backend.h>
#include <scorep_config_tool_mpi.h>
#include "scorep_config_thread.hpp"
#include "scorep_config_adapter.hpp"
#include <iostream>

std::deque<SCOREP_Config_ThreadSystem*> scorep_thread_systems;

void
scorep_config_init_thread_systems( void )
{
    scorep_thread_systems.push_back( new SCOREP_Config_MockupThreadSystem() );
    scorep_thread_systems.push_back( new SCOREP_Config_PompTpdThreadSystem() );
    SCOREP_Config_ThreadSystem::current = scorep_thread_systems.front();
}

void
scorep_config_final_thread_systems( void )
{
    SCOREP_Config_ThreadSystem::current = NULL;
    std::deque<SCOREP_Config_ThreadSystem*>::iterator i;
    for ( i = scorep_thread_systems.begin(); i != scorep_thread_systems.end(); i++ )
    {
        delete ( *i );
    }
}

/* **************************************************************************************
 * class SCOREP_Config_ThreadSystem
 * *************************************************************************************/

SCOREP_Config_ThreadSystem* SCOREP_Config_ThreadSystem::current = NULL;

SCOREP_Config_ThreadSystem::SCOREP_Config_ThreadSystem( std::string                  name,
                                                        std::string                  variant,
                                                        std::string                  library,
                                                        std::string                  mutexlib,
                                                        SCOREP_Config_ThreadSystemId id )
{
    m_name     = name;
    m_variant  = variant;
    m_library  = library;
    m_mutexlib = mutexlib;
    m_id       = id;
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
    if ( this == SCOREP_Config_ThreadSystem::current )
    {
        std::cout << "\tThis is the default.";
    }
    std::cout << std::endl;
}

bool
SCOREP_Config_ThreadSystem::checkArgument( std::string system )
{
    size_t pos = system.find( ':' );
    if ( ( ( pos == std::string::npos ) && ( system == m_name ) ) ||
         ( ( system.substr( 0, pos ) == m_name ) && ( system.substr( pos + 1 ) == m_variant ) ) )
    {
        SCOREP_Config_ThreadSystem::current = this;
        return true;
    }
    return false;
}

void
SCOREP_Config_ThreadSystem::addLibs( std::deque<std::string>&           libs,
                                     SCOREP_Config_LibraryDependencies& deps )
{
    deps.addDependency( "libscorep_measurement", "lib" + m_library );
    deps.addDependency( "libscorep_measurement", "lib" + m_mutexlib );
}

void
SCOREP_Config_ThreadSystem::addCFlags( std::string& cflags,
                                       bool         build_check,
                                       bool         fortran,
                                       bool         nvcc )
{
}

void
SCOREP_Config_ThreadSystem::addIncFlags( std::string& incflags, bool build_check, bool nvcc )
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
                                  "scorep_mutex_mockup", SCOREP_CONFIG_THREAD_SYSTEM_ID_NONE )
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
                                  "scorep_mutex_omp", SCOREP_CONFIG_THREAD_SYSTEM_ID_POMP_TPD )
{
}

void
SCOREP_Config_PompTpdThreadSystem::addLibs( std::deque<std::string>&           libs,
                                            SCOREP_Config_LibraryDependencies& deps )
{
    libs.push_back( "libscorep_adapter_pomp_omp_event" );
    deps.addDependency( "libscorep_measurement", "libscorep_adapter_pomp_omp_mgmt" );
    deps.addDependency( "libscorep_measurement", "libscorep_thread_fork_join_omp_tpd" );
    deps.addDependency( "libscorep_thread_fork_join_omp_tpd", "libscorep_mutex_omp" );
}

void
SCOREP_Config_PompTpdThreadSystem::addCFlags( std::string& cflags,
                                              bool         build_check,
                                              bool         fortran,
                                              bool         nvcc )
{
    add_opari_cflags( build_check, true, fortran, nvcc );

#if SCOREP_BACKEND_COMPILER_IBM
    if ( fortran )
    {
        cflags += "-d -WF,-qlanglvl=classic ";
    }
#endif
}

void
SCOREP_Config_PompTpdThreadSystem::addIncFlags( std::string& incflags, bool build_check, bool nvcc )
{
    add_opari_cflags( build_check, false, false, nvcc );
}
