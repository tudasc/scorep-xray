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

#include "scorep_info_command_config_vars.hpp"

#include <iostream>
#include <cstdlib>

#include <SCOREP_Config.h>
#include <scorep_environment.h>
#include <scorep_config_tool_backend.h>

#include "scorep_info.hpp"

SCOREP_Info_Command_ConfigVars::SCOREP_Info_Command_ConfigVars()
    : SCOREP_Info_Command( "config-vars",
                           "Shows the list of all measurement config variables with a short description." )
{
}

SCOREP_Info_Command_ConfigVars::~SCOREP_Info_Command_ConfigVars()
{
}

void
SCOREP_Info_Command_ConfigVars::printHelp( bool full )
{
    SCOREP_Info_Command::printHelp( full );
    if ( full )
    {
        std::cout << "    Info command options:\n"
                  << "      --help        Displays a description of the Score-P measurement configuration system.\n"
                  << "      --full        Displays a detailed description for each config variable.\n"
                  << "      --values      Displays the current values for each config variable.\n"
                  << "                    Warning: These values may be wrong, please consult the\n"
                  << "                             manual of the batch system how to pass the values\n"
                  << "                             to the measurement job.\n"
                  << std::endl;
    }
}

int
SCOREP_Info_Command_ConfigVars::run( const std::vector<std::string>& args )
{
    if ( args.size() > 1 )
    {
        std::cerr << "ERROR: Invalid number of options for info command "
                  << "'config-vars'" << std::endl;
        return EXIT_FAILURE;
    }

    std::string mode( args.size() == 1 ? args[ 0 ] : "" );
    bool        values = false;
    bool        full   = false;
    bool        html   = false;

    if ( mode == "--help" )
    {
        std::string help_command( PAGER_COMMAND " <" SCOREP_DATADIR "/scorep_info_confvars_help.md" );
        int         return_value = system( help_command.c_str() );
        if ( return_value != 0 )
        {
            std::cerr << "ERROR: Execution failed: " << help_command << std::endl;
            return EXIT_FAILURE + 1;
        }
        return EXIT_SUCCESS;
    }

    if ( mode == "--values" )
    {
        // @todo print warning again
        values = true;
    }
    else if ( mode == "--full" )
    {
        full = true;
    }
    else if ( mode == "--doxygen" )
    {
        full = true;
        html = true;
    }
    else if ( mode != "" )
    {
        std::cerr << "ERROR: Invalid option for info command "
                  << "'" << m_command << "': '" << mode << "'" << std::endl;
        return EXIT_FAILURE;
    }

    FILE* out = stdout;
#if HAVE( POPEN )
    if ( !html )
    {
        out = popen( PAGER_COMMAND, "w" );
    }
#endif

    SCOREP_ConfigInit();
    if ( html )
    {
        SCOREP_ConfigForceConditionalRegister();
    }
    SCOREP_RegisterAllConfigVariables();

    if ( values )
    {
        SCOREP_ConfigApplyEnv();
        SCOREP_ConfigDump( out );
    }
    else
    {
        SCOREP_ConfigHelp( full, html, out );
    }

    SCOREP_ConfigFini();

#if HAVE( POPEN )
    if ( !html )
    {
        pclose( out );
    }
#endif

    return EXIT_SUCCESS;
}
