/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2022,
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
 * Implements the class for HIP instrumentation.
 */

#include <config.h>
#include "scorep_instrumenter_hip.hpp"
#include "scorep_instrumenter_cmd_line.hpp"
#include "scorep_instrumenter_install_data.hpp"
#include "scorep_instrumenter_utils.hpp"
#include "scorep_instrumenter.hpp"
#include <scorep_config_tool_backend.h>

#include <scorep_tools_utils.hpp>

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <sstream>

#include <UTILS_Error.h>
#include <UTILS_IO.h>

/* **************************************************************************************
 * class SCOREP_Instrumenter_HipAdapter
 * *************************************************************************************/
SCOREP_Instrumenter_HipAdapter::SCOREP_Instrumenter_HipAdapter( void )
    : SCOREP_Instrumenter_Adapter( SCOREP_INSTRUMENTER_ADAPTER_HIP, "hip" )
{
#if !HAVE_BACKEND( HIP_SUPPORT )
    unsupported();
#endif
}

void
SCOREP_Instrumenter_HipAdapter::checkCompilerName( const std::string& compiler )
{
    if ( remove_path( compiler ).substr( 0, 3 ) == "hip" )
    {
        if ( m_usage == detect )
        {
            m_usage = enabled;
        }

        /* pro forma disable also preprocessing, didn't worked great with nvcc */
        m_conflicts.push_back( SCOREP_INSTRUMENTER_ADAPTER_PREPROCESS );
        m_hipcc_compiler = true;
    }
}

std::string
SCOREP_Instrumenter_HipAdapter::getConfigToolFlag( SCOREP_Instrumenter_CmdLine& /* cmdLine */,
                                                   const std::string& /* inputFile */ )
{
    std::string flags;

    if ( isEnabled() )
    {
        flags += " --" + m_name;
    }
    else
    {
        flags += " --no" + m_name;
    }

    return flags;
}

void
SCOREP_Instrumenter_HipAdapter::printHelp( void )
{
    std::cout << "\
  --hip           Enables HIP instrumentation. Enabled by default, if the\n\
                  hipcc compiler is in use. In this case it also conflicts and\n\
                  thus automatically disables preprocessing.\n\
  --nohip         Disables HIP instrumentation."
              << std::endl;
}
