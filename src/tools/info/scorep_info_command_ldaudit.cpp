/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2022,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 */

#include <config.h>

#include "scorep_info_command_ldaudit.hpp"

#include <iostream>
#include <cstdlib>

#include <scorep_config_tool_backend.h>
#include "scorep_info.hpp"

SCOREP_Info_Command_Ldaudit::SCOREP_Info_Command_Ldaudit()
    : SCOREP_Info_Command( "ldaudit",
                           "Shows the LD_AUDIT value needed to extend address lookup to dynamically\n"
                           "    loaded shared objects." )
{
}


int
SCOREP_Info_Command_Ldaudit::run( const std::vector<std::string>& args )
{
    if ( args.size() != 0 )
    {
        std::cerr << "ERROR: Invalid number of options for info command "
                  << "'" << m_command << "'" << std::endl;
        return EXIT_FAILURE;
    }

    std::string command( "printf \"The value for LD_AUDIT is:\\n\\n"
                         "    LD_AUDIT=%s\\n\\n"
                         "See the explanation below on how to use this variable:\\n\\n%s\\n\" "
                         "\"$(" SCOREP_BINDIR "/scorep-config --ldaudit )\" "
                         "\"$(cat \"" SCOREP_DATADIR "/scorep_info_ldaudit_help.md\")\"" );
    if ( system( command.c_str() ) != 0 )
    {
        std::cerr << "ERROR: Execution failed: " << command << std::endl;
        return EXIT_FAILURE + 1;
    }
    return EXIT_SUCCESS;
}
