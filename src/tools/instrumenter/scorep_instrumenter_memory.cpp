/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2016-2017,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2017,
 * Technische Universitaet Darmstadt, Germany
 *
 * Copyright (c) 2017,
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
 * Implements the class for memory tracking.
 */

#include <config.h>
#include "scorep_instrumenter_memory.hpp"
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
#include <deque>

#include <UTILS_Error.h>
#include <UTILS_IO.h>

/* **************************************************************************************
 * class SCOREP_Instrumenter_MemoryAdapter
 * *************************************************************************************/
SCOREP_Instrumenter_MemoryAdapter::SCOREP_Instrumenter_MemoryAdapter( void )
    : SCOREP_Instrumenter_Adapter( SCOREP_INSTRUMENTER_ADAPTER_MEMORY, "memory" )
{
    m_requires.push_back( SCOREP_INSTRUMENTER_ADAPTER_LINKTIME_WRAPPING );
#if !HAVE_BACKEND( MEMORY_SUPPORT )
    unsupported();
#endif
}


void
SCOREP_Instrumenter_MemoryAdapter::printHelp( void )
{
    if ( !m_unsupported )
    {
        std::cout << "  --memory        Enables memory usage instrumentation. It is enabled by default.\n";
    }
    std::cout << "  --nomemory      Disables memory usage instrumentation.\n";
}

std::string
SCOREP_Instrumenter_MemoryAdapter::getConfigToolFlag( SCOREP_Instrumenter_CmdLine& /* cmdLine */ )
{
    /* Explicit user arguments */
    if ( isEnabled() && m_params != "" )
    {
        return " --" + m_name + "=" + m_params.substr( 1 );
    }
    /* Auto-detected arguments */
    if ( isEnabled() && m_categories.size() )
    {
        std::stringstream categories;
        std::string       sep = " --" + m_name + "=";
        for ( std::set<std::string>::const_iterator category = m_categories.begin();
              category != m_categories.end(); category++ )
        {
            categories << sep << *category;
            sep = ",";
        }
        return categories.str();
    }
    /* Disabled */
    else if ( !isEnabled() )
    {
        return " --no" + m_name;
    }
    return "";
}

void
SCOREP_Instrumenter_MemoryAdapter::checkObjects( SCOREP_Instrumenter& instrumenter )
{
    if ( m_usage != detect &&
         !( isEnabled() && m_params == "" ) )
    {
        return;
    }

    std::vector<std::string>* object_list = instrumenter.getInputFiles();

    std::stringstream all_objects_stream;
    for ( std::vector<std::string>::iterator current_file = object_list->begin();
          current_file != object_list->end();
          current_file++ )
    {
        /* we currently only wrap at link-time, thus exclude dynamic libraries here */
        if ( is_object_file( *current_file ) || is_library( *current_file, false ) )
        {
            all_objects_stream << " " << *current_file;
        }
    }

    /* Get all static libs, exclude shared, as we can't wrap them anymore */
    all_objects_stream << instrumenter.getCommandLine().getLibraryFiles( false );

    // check for malloc and friends, i.e., "libc"
    {
        std::string command = SCOREP_NM + all_objects_stream.str() + " 2>/dev/null | "
                              SCOREP_EGREP " -l ' U (malloc|free|calloc|realloc|memalign|posix_memalign|valloc)$' >/dev/null 2>&1";
        if ( instrumenter.getCommandLine().getVerbosity() >= 1 )
        {
            std::cerr << command << std::endl;
        }
        int return_value = system( command.c_str() );
        if ( return_value == 0 )
        {
            m_categories.insert( "libc" );
        }
    }

    // check for hbw_malloc and friends from the hbwmalloc.h API
    {
        std::string command = SCOREP_NM + all_objects_stream.str() + " 2>/dev/null | "
                              SCOREP_EGREP " -l ' U (hbw_malloc|hbw_free|hbw_calloc|hbw_realloc|hbw_posix_memalign|hbw_posix_memalign_psize)$' >/dev/null 2>&1";
        if ( instrumenter.getCommandLine().getVerbosity() >= 1 )
        {
            std::cerr << command << std::endl;
        }
        int return_value = system( command.c_str() );
        if ( return_value == 0 )
        {
            m_categories.insert( "hbwmalloc" );
        }
    }

    // check for aligned_alloc, i.e., "C11"
    {
        std::string command = SCOREP_NM + all_objects_stream.str() + " 2>/dev/null | "
                              SCOREP_EGREP " -l ' U aligned_alloc$' >/dev/null 2>&1";
        if ( instrumenter.getCommandLine().getVerbosity() >= 1 )
        {
            std::cerr << command << std::endl;
        }
        int return_value = system( command.c_str() );
        if ( return_value == 0 )
        {
            m_categories.insert( "libc11" );
        }
    }

    // check for new/delete and friends in L32, i.e., "c++L32"
    {
        std::string command = SCOREP_NM + all_objects_stream.str() + " 2>/dev/null | "
                              SCOREP_EGREP " -l ' U (_Znwj|_Znaj)$' >/dev/null 2>&1";
        if ( instrumenter.getCommandLine().getVerbosity() >= 1 )
        {
            std::cerr << command << std::endl;
        }
        int return_value = system( command.c_str() );
        if ( return_value == 0 )
        {
            m_categories.insert( "c++L32" );
        }
    }

    // check for new/delete and friends in L64, i.e., "c++L64"
    {
        std::string command = SCOREP_NM + all_objects_stream.str() + " 2>/dev/null | "
                              SCOREP_EGREP " -l ' U (_Znwm|_Znam)$' >/dev/null 2>&1";
        if ( instrumenter.getCommandLine().getVerbosity() >= 1 )
        {
            std::cerr << command << std::endl;
        }
        int return_value = system( command.c_str() );
        if ( return_value == 0 )
        {
            m_categories.insert( "c++L64" );
        }
    }

    // check for C++14 delete and friends in L32, i.e., "c++14L32"
    {
        std::string command = SCOREP_NM + all_objects_stream.str() + " 2>/dev/null | "
                              SCOREP_EGREP " -l ' U _Zd[la]Pvj$' >/dev/null 2>&1";
        if ( instrumenter.getCommandLine().getVerbosity() >= 1 )
        {
            std::cerr << command << std::endl;
        }
        int return_value = system( command.c_str() );
        if ( return_value == 0 )
        {
            m_categories.insert( "c++14L32" );
        }
    }

    // check for C++14 delete and friends in L64, i.e., "c++14L64"
    {
        std::string command = SCOREP_NM + all_objects_stream.str() + " 2>/dev/null | "
                              SCOREP_EGREP " -l ' U _Zd[la]Pvm$' >/dev/null 2>&1";
        if ( instrumenter.getCommandLine().getVerbosity() >= 1 )
        {
            std::cerr << command << std::endl;
        }
        int return_value = system( command.c_str() );
        if ( return_value == 0 )
        {
            m_categories.insert( "c++14L64" );
        }
    }

    // check for new/delete and friends in L32 (old PGI/EDG C++ ABI), i.e., "pgCCL32"
    {
        std::string command = SCOREP_NM + all_objects_stream.str() + " 2>/dev/null | "
                              SCOREP_EGREP " -l ' U (__nw__FUi|__nwa__FUi)$' >/dev/null 2>&1";
        if ( instrumenter.getCommandLine().getVerbosity() >= 1 )
        {
            std::cerr << command << std::endl;
        }
        int return_value = system( command.c_str() );
        if ( return_value == 0 )
        {
            m_categories.insert( "pgCCL32" );
        }
    }

    // check for new/delete and friends in L64 (old PGI/EDG C++ ABI), i.e., "pgCCL64"
    {
        std::string command = SCOREP_NM + all_objects_stream.str() + " 2>/dev/null | "
                              SCOREP_EGREP " -l ' U (__nw__FUl|__nwa__FUl)$' >/dev/null 2>&1";
        if ( instrumenter.getCommandLine().getVerbosity() >= 1 )
        {
            std::cerr << command << std::endl;
        }
        int return_value = system( command.c_str() );
        if ( return_value == 0 )
        {
            m_categories.insert( "pgCCL64" );
        }
    }

    if ( m_categories.size() )
    {
        m_usage = enabled;
    }
}

bool
SCOREP_Instrumenter_MemoryAdapter::isInterpositionLibrary( const std::string& libraryName )
{
    return check_lib_name( libraryName, "memkind" );
}
