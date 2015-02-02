/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014-2015,
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

    // Per default, the scorep-config returns OpenCL libs.
    if ( !isEnabled() )
    {
        flags += " --no" + m_name;
    }

    return flags;
}

void
SCOREP_Instrumenter_OpenCLAdapter::printHelp( void )
{
    std::cout << "  --opencl        Enables OpenCL instrumentation.\n";
    std::cout << "  --noopencl      Disables OpenCL instrumentation.\n"
              << std::endl;
}

bool
SCOREP_Instrumenter_OpenCLAdapter::checkOption( std::string arg )
{
    if ( arg == "--opencl" )
    {
        m_usage = enabled;
        return true;
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
