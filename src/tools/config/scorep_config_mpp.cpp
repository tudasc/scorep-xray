/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 * @file scorep_config_mpp.cpp
 * @status alpha
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * Collects information about available multi process systems.
 */

#include <config.h>
#include <scorep_config_tool_backend.h>
#include <scorep_config_tool_mpi.h>
#include "scorep_config_mpp.hpp"
#include <iostream>

std::deque<SCOREP_Config_MppSystem*> scorep_mpp_systems;

void
scorep_config_init_mpp_systems( void )
{
    scorep_mpp_systems.push_back( new SCOREP_Config_MppMpi() );
    scorep_mpp_systems.push_back( new SCOREP_Config_MppNone() );
    SCOREP_Config_MppSystem::current = scorep_mpp_systems.front();
}

void
scorep_config_final_mpp_systems( void )
{
    SCOREP_Config_MppSystem::current = NULL;
    std::deque<SCOREP_Config_MppSystem*>::iterator i;
    for ( i = scorep_mpp_systems.begin(); i != scorep_mpp_systems.end(); i++ )
    {
        delete ( *i );
    }
}

/* **************************************************************************************
 * class SCOREP_Config_MppSystem
 * *************************************************************************************/

SCOREP_Config_MppSystem* SCOREP_Config_MppSystem::current = NULL;

SCOREP_Config_MppSystem::SCOREP_Config_MppSystem( std::string name )
{
    m_name = name;
}

SCOREP_Config_MppSystem::~SCOREP_Config_MppSystem()
{
}

void
SCOREP_Config_MppSystem::printHelp( void )
{
    std::cout << "         " << m_name;
    if ( this == SCOREP_Config_MppSystem::current )
    {
        std::cout << "\tThis is the default.";
    }
    std::cout << std::endl;
}

bool
SCOREP_Config_MppSystem::checkArgument( std::string system )
{
    if ( system == m_name )
    {
        SCOREP_Config_MppSystem::current = this;
        return true;
    }
    return false;
}

void
SCOREP_Config_MppSystem::addLibs( std::deque<std::string> &          libs,
                                  SCOREP_Config_LibraryDependencies &deps )
{
}

/* **************************************************************************************
 * class SCOREP_Config_MppNone
 * *************************************************************************************/

SCOREP_Config_MppNone::SCOREP_Config_MppNone() : SCOREP_Config_MppSystem( "none" )
{
}

void
SCOREP_Config_MppNone::addLibs( std::deque<std::string> &          libs,
                                SCOREP_Config_LibraryDependencies &deps )
{
    deps.addDependency( "libscorep_measurement", "libscorep_mpp_mockup" );
    deps.addDependency( "libscorep_measurement", "libscorep_sion_mockup" );
    deps.addDependency( "libscorep_measurement", "libscorep_online_access_spp" );
}

/* **************************************************************************************
 * class SCOREP_Config_MppMpi
 * *************************************************************************************/

SCOREP_Config_MppMpi::SCOREP_Config_MppMpi() : SCOREP_Config_MppSystem( "mpi" )
{
}

void
SCOREP_Config_MppMpi::addLibs( std::deque<std::string> &          libs,
                               SCOREP_Config_LibraryDependencies &deps )
{
    libs.push_back( "libscorep_adapter_mpi" );
    deps.addDependency( "libscorep_measurement", "libscorep_sion_mpp_mpi" );
    deps.addDependency( "libscorep_measurement", "libscorep_mpp_mpi" );
    deps.addDependency( "libscorep_measurement", "libscorep_online_access_mpp_mpi" );
}
