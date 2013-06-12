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
 * @file scorep_config_adapter.cpp
 * @status alpha
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * Collects information about available adapters
 */

#include <config.h>
#include <scorep_config_tool_backend.h>
#include <scorep_config_tool_mpi.h>
#include "scorep_config_adapter.hpp"
#include "scorep_config_thread.hpp"
#include "scorep_config_utils.hpp"

#include <iostream>
#include <stdlib.h>

std::deque<SCOREP_Config_Adapter*> scorep_adapters;

void
scorep_config_init_adapters( void )
{
    scorep_adapters.push_back( new SCOREP_Config_CompilerAdapter() );
    scorep_adapters.push_back( new SCOREP_Config_UserAdapter() );
    scorep_adapters.push_back( new SCOREP_Config_PompAdapter() );
#if HAVE_BACKEND( CUDA )
    scorep_adapters.push_back( new SCOREP_Config_CudaAdapter() );
#endif
}

void
scorep_config_final_adapters( void )
{
    std::deque<SCOREP_Config_Adapter*>::iterator i;
    for ( i = scorep_adapters.begin(); i != scorep_adapters.end(); i++ )
    {
        delete ( *i );
    }
}

void
add_opari_cflags( bool build_check )
{
    static bool printed_once = false;
    if ( !printed_once )
    {
        printed_once = true;

#if !HAVE( EXTERNAL_OPARI2 )
        if ( build_check )
        {
            std::cout << "-I" PACKAGE_SRCDIR "/vendor/opari2/include ";
            std::cout.flush();
            return;
        }
#endif

        std::string opari_config = OPARI_CONFIG " --cflags";
        int         return_value = system( opari_config.c_str() );
        if ( return_value != 0 )
        {
            std::cerr << "Error executing: " << opari_config.c_str() << std::endl;
            exit( EXIT_FAILURE );
        }
        std::cout << " ";
        std::cout.flush();
    }
}

/* **************************************************************************************
 * class SCOREP_Config_Adapter
 * *************************************************************************************/

SCOREP_Config_Adapter::SCOREP_Config_Adapter( std::string name,
                                              std::string library,
                                              bool        is_default )
{
    m_name       = name;
    m_library    = library;
    m_is_enabled = is_default;
}


SCOREP_Config_Adapter::~SCOREP_Config_Adapter()
{
}

void
SCOREP_Config_Adapter::printHelp( void )
{
    std::cout << "   --" << m_name << "|--no" << m_name << "\n"
              << "            Specifies whether " << m_name << " instrumentation is used.\n"
              << "            On default " << m_name << " instrumentation is " \
              << ( m_is_enabled ? "enabled" : "disabled" ) << ".\n";
}

bool
SCOREP_Config_Adapter::checkArgument( std::string arg )
{
    if ( arg == "--" + m_name )
    {
        m_is_enabled = true;
        return true;
    }
    else if ( arg == "--no" + m_name )
    {
        m_is_enabled = false;
        return true;
    }
    return false;
}

void
SCOREP_Config_Adapter::addLibs( std::deque<std::string> &          libs,
                                SCOREP_Config_LibraryDependencies& deps )
{
    if ( m_is_enabled )
    {
        libs.push_back( "lib" + m_library );
    }
}

void
SCOREP_Config_Adapter::addCFlags( std::string &cflags, bool fortran )
{
}

void
SCOREP_Config_Adapter::addLdFlags( std::string &ldflags )
{
}

void
SCOREP_Config_Adapter::addIncFlags( std::string &incflags, bool build_check )
{
}

/* **************************************************************************************
 * Compiler adapter
 * *************************************************************************************/

SCOREP_Config_CompilerAdapter::SCOREP_Config_CompilerAdapter()
    : SCOREP_Config_Adapter( "compiler", "scorep_adapter_compiler", true )
{
}

void
SCOREP_Config_CompilerAdapter::addCFlags( std::string &cflags, bool fortran )
{
    if ( m_is_enabled )
    {
        cflags += "-g " SCOREP_CFLAGS " ";
    }
}

void
SCOREP_Config_CompilerAdapter::addLdFlags( std::string &ldflags )
{
    if ( m_is_enabled )
    {
        ldflags += " " SCOREP_LDFLAGS;
    }
}

/* **************************************************************************************
 * User adapter
 * *************************************************************************************/
SCOREP_Config_UserAdapter::SCOREP_Config_UserAdapter()
    : SCOREP_Config_Adapter( "user", "scorep_adapter_user", false )
{
}

void
SCOREP_Config_UserAdapter::addCFlags( std::string &cflags, bool fortran )
{
    if ( m_is_enabled )
    {
        if ( fortran )
        {
                #ifdef SCOREP_COMPILER_IBM
            cflags += "-WF,-DSCOREP_USER_ENABLE ";
                #else
            cflags += "-DSCOREP_USER_ENABLE ";
                #endif // SCOREP_COMPILER_IBM
        }
        else
        {
            cflags += "-DSCOREP_USER_ENABLE ";
        }
    }
}

/* **************************************************************************************
 * Cuda adapter
 * *************************************************************************************/
SCOREP_Config_CudaAdapter::SCOREP_Config_CudaAdapter()
    : SCOREP_Config_Adapter( "cuda", "scorep_adapter_cuda", false )
{
}

void
SCOREP_Config_CudaAdapter::addIncFlags( std::string &incflags, bool build_check )
{
    if ( m_is_enabled )
    {
        std::string pattern1 = " ";
        std::string replace1 = ",";

        incflags = remove_multiple_whitespaces( incflags );
        /* Replace all white-spaces by comma */
        incflags = replace_all( pattern1, replace1, incflags );

        incflags = " -Xcompiler " + incflags;
    }
}

void
SCOREP_Config_CudaAdapter::addLdFlags( std::string &ldflags )
{
    if ( m_is_enabled )
    {
        std::string pattern1 = " ";
        std::string replace1 = ",";
        std::string pattern2 = LIBDIR_FLAG_WL;
        std::string replace2 = "";

        ldflags = remove_multiple_whitespaces( ldflags );
        /* Replace all white-spaces by comma */
        ldflags = replace_all( pattern1, replace1, ldflags );
        /* Replace flag for passing arguments to linker through compiler
         * (flags not needed because we use '-Xlinker' to specify linker
         * flags when using CUDA compiler */
        if ( pattern2.length() != 0 )
        {
            ldflags = replace_all( pattern2, replace2, ldflags );
        }

        ldflags = " -Xlinker " + ldflags;
    }
}

/* **************************************************************************************
 * Pomp adapter
 * *************************************************************************************/
SCOREP_Config_PompAdapter::SCOREP_Config_PompAdapter()
    : SCOREP_Config_Adapter( "pomp", "scorep_adapter_pomp_user", false )
{
}

void
SCOREP_Config_PompAdapter::addIncFlags( std::string &incflags, bool build_check )
{
    if ( m_is_enabled )
    {
        add_opari_cflags( build_check );
    }
}
