/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2017,
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
 */

#include <config.h>

#include "scorep_info_command_config_summary.hpp"

#include <iostream>
#include <cstdlib>

#include <scorep_config_tool_backend.h>

#include "scorep_info.hpp"

SCOREP_Info_Command_ConfigSummary::SCOREP_Info_Command_ConfigSummary()
    : SCOREP_Info_Command( "config-summary",
                           "Shows the configure summary of the Score-P package." )
{
}

SCOREP_Info_Command_ConfigSummary::~SCOREP_Info_Command_ConfigSummary()
{
}

int
SCOREP_Info_Command_ConfigSummary::run( const std::vector<std::string>& args )
{
    if ( args.size() != 0 )
    {
        std::cerr << "ERROR: Invalid number of options for info command "
                  << "'" << m_command << "'" << std::endl;
        return EXIT_FAILURE;
    }

    std::string summary_command( PAGER_COMMAND " <" SCOREP_DATADIR "/scorep.summary" );
    int         return_value = system( summary_command.c_str() );
    if ( return_value != 0 )
    {
        std::cerr << "ERROR: Execution failed: " << summary_command << std::endl;
        return EXIT_FAILURE + 1;
    }
    return EXIT_SUCCESS;
}
