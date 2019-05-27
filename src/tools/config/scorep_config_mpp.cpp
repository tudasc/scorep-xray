/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013, 2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2013-2014, 2016-2017, 2019,
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
 * Collects information about available multi-process systems.
 */

#include <config.h>
#include <scorep_config_tool_backend.h>
#include <scorep_config_tool_mpi.h>
#include "scorep_config_mpp.hpp"
#include "scorep_config_utils.hpp"
#include <iostream>

/* **************************************************************************************
 * class SCOREP_Config_MppSystem
 * *************************************************************************************/

std::deque<SCOREP_Config_MppSystem*> SCOREP_Config_MppSystem::all;

SCOREP_Config_MppSystem* SCOREP_Config_MppSystem::current = 0;

void
SCOREP_Config_MppSystem::init( void )
{
    all.push_back( new SCOREP_Config_MockupMppSystem() );
#if HAVE_BACKEND( MPI_SUPPORT )
    all.push_back( new SCOREP_Config_MpiMppSystem() );
#endif
#if HAVE_BACKEND( SHMEM_SUPPORT )
    all.push_back( new SCOREP_Config_ShmemMppSystem() );
#endif
    current = all.front();
}

void
SCOREP_Config_MppSystem::fini( void )
{
    current = 0;
    std::deque<SCOREP_Config_MppSystem*>::iterator i;
    for ( i = all.begin(); i != all.end(); i++ )
    {
        delete ( *i );
    }
}

void
SCOREP_Config_MppSystem::printAll( void )
{
    std::cout << "   --mpp=<multi-process paradigm>\n"
              << "              Available multi-process paradigms are:\n";
    std::deque<SCOREP_Config_MppSystem*>::iterator i;
    for ( i = all.begin(); i != all.end(); i++ )
    {
        ( *i )->printHelp();
    }
}

bool
SCOREP_Config_MppSystem::checkAll( const std::string& arg )
{
    current = 0;

    std::deque<SCOREP_Config_MppSystem*>::iterator i;
    for ( i = all.begin(); i != all.end(); i++ )
    {
        if ( arg == ( *i )->m_name )
        {
            current = *i;
            return true;
        }
    }
    return false;
}

SCOREP_Config_MppSystem::SCOREP_Config_MppSystem( const std::string& name )
    : m_name( name )
{
}

SCOREP_Config_MppSystem::~SCOREP_Config_MppSystem()
{
}

void
SCOREP_Config_MppSystem::printHelp( void )
{
    std::cout << "               " << m_name;
    if ( this == current )
    {
        std::cout << " (default)";
    }
    std::cout << std::endl;
}

void
SCOREP_Config_MppSystem::addLibs( std::deque<std::string>&           libs,
                                  SCOREP_Config_LibraryDependencies& deps,
                                  bool                               withOnlineAccess )
{
}

void
SCOREP_Config_MppSystem::addLdFlags( std::string& /* ldflags */,
                                     bool /* buildCheck */,
                                     bool /* nvcc */ )
{
}

/* **************************************************************************************
 * class SCOREP_Config_MockupMppSystem
 * *************************************************************************************/

SCOREP_Config_MockupMppSystem::SCOREP_Config_MockupMppSystem()
    : SCOREP_Config_MppSystem( "none" )
{
}

void
SCOREP_Config_MockupMppSystem::addLibs( std::deque<std::string>&           libs,
                                        SCOREP_Config_LibraryDependencies& deps,
                                        bool                               withOnlineAccess )
{
    deps.addDependency( "libscorep_measurement", "libscorep_mpp_mockup" );
#if HAVE_BACKEND( ONLINE_ACCESS )
    if ( withOnlineAccess )
    {
        deps.addDependency( "libscorep_measurement", "libscorep_online_access_spp" );
    }
    else
#endif /* HAVE_BACKEND( ONLINE_ACCESS ) */
    {
        deps.addDependency( "libscorep_measurement", "libscorep_online_access_mockup" );
    }
}

void
SCOREP_Config_MockupMppSystem::getInitStructName( std::deque<std::string>& init_structs )
{
}

/* **************************************************************************************
 * class SCOREP_Config_MpiMppSystem
 * *************************************************************************************/

SCOREP_Config_MpiMppSystem::SCOREP_Config_MpiMppSystem()
    : SCOREP_Config_MppSystem( "mpi" )
{
}

void
SCOREP_Config_MpiMppSystem::addLibs( std::deque<std::string>&           libs,
                                     SCOREP_Config_LibraryDependencies& deps,
                                     bool                               withOnlineAccess )
{
    libs.push_back( "libscorep_adapter_mpi_event" );
    deps.addDependency( "libscorep_measurement", "libscorep_adapter_mpi_mgmt" );
    deps.addDependency( "libscorep_measurement", "libscorep_mpp_mpi" );
#if HAVE_BACKEND( ONLINE_ACCESS )
    if ( withOnlineAccess )
    {
        deps.addDependency( "libscorep_measurement", "libscorep_online_access_mpp_mpi" );
    }
    else
#endif /* HAVE_BACKEND( ONLINE_ACCESS ) */
    {
        deps.addDependency( "libscorep_measurement", "libscorep_online_access_mockup" );
    }

    deps.addDependency( "libscorep_adapter_mpi_mgmt", "libscorep_alloc_metric" );
    deps.addDependency( "libscorep_adapter_mpi_mgmt", "libscorep_adapter_utils" );
}

void
SCOREP_Config_MpiMppSystem::addLdFlags( std::string& ldflags,
                                        bool         buildCheck,
                                        bool /* nvcc */ )
{
#if SCOREP_BACKEND_COMPILER_PGI
    // see 'Linking issues with PGI compilers and Open MPI 1.8' #951
    ldflags += " -pgf90libs";
#endif // SCOREP_BACKEND_COMPILER_PGI
}

void
SCOREP_Config_MpiMppSystem::getInitStructName( std::deque<std::string>& init_structs )
{
    init_structs.push_back( "SCOREP_Subsystem_IoManagement" );
    init_structs.push_back( "SCOREP_Subsystem_MpiAdapter" );
}

/* **************************************************************************************
 * class SCOREP_Config_ShmemMppSystem
 * *************************************************************************************/

SCOREP_Config_ShmemMppSystem::SCOREP_Config_ShmemMppSystem()
    : SCOREP_Config_MppSystem( "shmem" )
{
}

void
SCOREP_Config_ShmemMppSystem::addLibs( std::deque<std::string>&           libs,
                                       SCOREP_Config_LibraryDependencies& deps,
                                       bool                               withOnlineAccess )
{
    libs.push_back( "libscorep_adapter_shmem_event" );
    deps.addDependency( "libscorep_measurement", "libscorep_adapter_shmem_mgmt" );
    deps.addDependency( "libscorep_measurement", "libscorep_mpp_shmem" );
    deps.addDependency( "libscorep_measurement", "libscorep_online_access_mockup" );
    deps.addDependency( "libscorep_adapter_shmem_mgmt", "libscorep_alloc_metric" );
}

void
SCOREP_Config_ShmemMppSystem::addLdFlags( std::string& ldflags,
                                          bool         buildCheck,
                                          bool         nvcc )
{
#if !HAVE_BACKEND( SHMEM_PROFILING_INTERFACE )
    ldflags += get_ld_wrap_flag( "shmem", buildCheck, nvcc );
#endif
}

void
SCOREP_Config_ShmemMppSystem::getInitStructName( std::deque<std::string>& init_structs )
{
    init_structs.push_back( "SCOREP_Subsystem_ShmemAdapter" );
}
