/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014-2015, 2017,
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
 * Implements the class for OpenCL instrumentation.
 */

#include <config.h>
#include "scorep_instrumenter_opencl.hpp"
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
 * class SCOREP_Instrumenter_OpenCLAdapter
 * *************************************************************************************/
SCOREP_Instrumenter_OpenCLAdapter::SCOREP_Instrumenter_OpenCLAdapter( void )
    : SCOREP_Instrumenter_Adapter( SCOREP_INSTRUMENTER_ADAPTER_OPENCL, "opencl" )
{
#if !HAVE_BACKEND( OPENCL_SUPPORT )
    unsupported();
#endif
}

std::string
SCOREP_Instrumenter_OpenCLAdapter::getConfigToolFlag( SCOREP_Instrumenter_CmdLine& /* cmdLine */ )
{
    std::string flags;

    if ( isEnabled() )
    {
        flags += " --" + m_name;
        if ( m_wrapmode.size() )
        {
            flags += ":" + m_wrapmode;
        }
    }
    else
    {
        flags += " --no" + m_name;
    }

    return flags;
}

void
SCOREP_Instrumenter_OpenCLAdapter::printHelp( void )
{
    std::cout << "  --opencl[:<wrap-mode>]\n"
              << "                  Enables OpenCL instrumentation.\n"
              << "  --noopencl      Disables OpenCL instrumentation."
              << std::endl;
}

bool
SCOREP_Instrumenter_OpenCLAdapter::checkOption( const std::string& arg )
{
    if ( arg == "--opencl" )
    {
        m_usage    = enabled;
        m_wrapmode = SCOREP_LIBWRAP_DEFAULT_MODE;
#if HAVE_BACKEND( LIBWRAP_LINKTIME_SUPPORT )
        m_requires.push_back( SCOREP_INSTRUMENTER_ADAPTER_LINKTIME_WRAPPING );
#endif
        return true;
    }
    else if ( arg.substr( 0, 9 ) == "--opencl:" )
    {
        m_usage    = enabled;
        m_wrapmode = arg.substr( 9 );

#if HAVE_BACKEND( LIBWRAP_LINKTIME_SUPPORT )
        if ( m_wrapmode == "linktime" )
        {
            m_requires.push_back( SCOREP_INSTRUMENTER_ADAPTER_LINKTIME_WRAPPING );
            return true;
        }
#elif HAVE_BACKEND( LIBWRAP_RUNTIME_SUPPORT )
        if ( m_wrapmode == "runtime" )
        {
            return true;
        }
#endif

        std::cerr << "[Score-P] ERROR: Invalid or unsupported wrapping mode for OpenCL: " << m_wrapmode << std::endl;
        exit( EXIT_FAILURE );
    }
    if ( arg == "--noopencl" )
    {
        m_usage = disabled;
        return true;
    }
    return false;
}

bool
SCOREP_Instrumenter_OpenCLAdapter::checkCommand( const std::string& current,
                                                 const std::string& next )
{
    if ( ( m_usage == detect ) &&
         ( current == "-l" ) &&
         ( is_opencl_library( next ) ) )
    {
        m_usage = enabled;
        return true;
    }
    else if ( ( m_usage == detect ) &&
              ( current.substr( 0, 2 ) == "-l" ) &&
              ( is_opencl_library( current.substr( 2 ) ) ) )
    {
        m_usage = enabled;
        return false;
    }
    return false;
}

bool
SCOREP_Instrumenter_OpenCLAdapter::isInterpositionLibrary( const std::string& libraryName )
{
    return is_opencl_library( libraryName );
}

bool
SCOREP_Instrumenter_OpenCLAdapter::is_opencl_library( const std::string& libraryName )
{
    return check_lib_name( libraryName, "OpenCL" );
}
