/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012, 2014, 2016-2017,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 *
 * The info tool of Score-P: scorep-info.
 *
 */

#include <config.h>

#include "scorep_info.hpp"

#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>

#include "scorep_info_command.hpp"
#include "scorep_info_command_config_summary.hpp"
#include "scorep_info_command_config_vars.hpp"
#include "scorep_info_command_libwrap_summary.hpp"
#include "scorep_info_command_open_issues.hpp"

/**
   Contains the name of the tool for help output
 */
const std::string toolname = "scorep-info";

/**
    Prints a short usage message.
 */
static void
print_short_usage( std::ostream& out )
{
    out << "Usage: " << toolname << " <info command> <command options>" << std::endl;
    out << "       " << toolname << " --help [<info command>]" << std::endl;
    out << "This is the " << PACKAGE_NAME << " info tool." << std::endl;
}

/**
   Prints the long help text.
 */
static int
print_help( bool               withOptions,
            const std::string& command = std::string() )
{
    print_short_usage( std::cout );
    std::cout << std::endl;

    int ret;
    if ( command.size() )
    {
        ret = SCOREP_Info_Command::onePrintHelp( command, withOptions );
    }
    else
    {
        ret = SCOREP_Info_Command::allPrintHelp( withOptions );
    }

    std::cout << "Report bugs to <" << PACKAGE_BUGREPORT << ">" << std::endl;

    return ret;
}

int
main( int   argc,
      char* argv[] )
{
    new SCOREP_Info_Command_ConfigSummary();
    new SCOREP_Info_Command_ConfigVars();
    new SCOREP_Info_Command_LibwrapSummary();
    new SCOREP_Info_Command_OpenIssues();

    if ( argc == 1 )
    {
        std::cerr << "ERROR: Missing info command" << std::endl;
        print_help( false );
        return EXIT_FAILURE;
    }

    std::string command( argv[ 1 ] );
    if ( command == "--help" || command == "-h" )
    {
        if ( argc > 3 )
        {
            std::cerr << "ERROR: Too many arguments" << std::endl;
            print_short_usage( std::cerr );
            return EXIT_FAILURE;
        }

        return print_help( true, argc == 3 ? std::string( argv[ 2 ] ) : "" );
    }

    std::vector< std::string > args;
    for ( int i = 2; i < argc; i++ )
    {
        args.push_back( std::string( argv[ i ] ) );
    }

    int ret = SCOREP_Info_Command::run( command, args );
    if ( ret == EXIT_FAILURE )
    {
        print_short_usage( std::cerr );
    }
    return ret;
}
