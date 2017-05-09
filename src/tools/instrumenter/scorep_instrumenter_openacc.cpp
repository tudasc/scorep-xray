/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2016,
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
 * Implements the class for OpenACC instrumentation.
 */

#include <config.h>
#include "scorep_instrumenter_openacc.hpp"
#include "scorep_instrumenter_cmd_line.hpp"
#include "scorep_instrumenter_install_data.hpp"
#include "scorep_instrumenter_utils.hpp"
#include "scorep_instrumenter.hpp"
#include <scorep_config_tool_backend.h>

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <sstream>

#include <UTILS_Error.h>
#include <UTILS_IO.h>

/* **************************************************************************************
 * class SCOREP_Instrumenter_OpenACCAdapter
 * *************************************************************************************/
SCOREP_Instrumenter_OpenACCAdapter::SCOREP_Instrumenter_OpenACCAdapter( void )
    : SCOREP_Instrumenter_Adapter( SCOREP_INSTRUMENTER_ADAPTER_OPENACC, "openacc" )
{
#if !HAVE_BACKEND( OPENACC_SUPPORT )
    unsupported();
#endif
}

std::string
SCOREP_Instrumenter_OpenACCAdapter::getConfigToolFlag( SCOREP_Instrumenter_CmdLine& /* cmdLine */ )
{
    std::string flags;

    if ( !isEnabled() )
    {
        flags += " --no" + m_name;
    }

    return flags;
}

void
SCOREP_Instrumenter_OpenACCAdapter::printHelp( void )
{
    std::cout << "  --openacc       Enables OpenACC instrumentation.\n"
              << "  --noopenacc     Disables OpenACC instrumentation."
              << std::endl;
}
