/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file       src/tools/backend-info/scorep_backend_info.cpp
 *
 *
 * The info tool of Score-P: scorep-backend-info.
 *
 */

#include <config.h>

#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>

#include <UTILS_Error.h>

#include <SCOREP_Platform.h>
#include <SCOREP_Config.h>
#include <scorep_environment.h>


/**
   Contains the name of the tool for help output
 */
static const std::string toolname = "scorep-backend-info";

/**
    Prints a short usage message.
 */
static void
print_short_usage( std::ostream& out )
{
    out << "Usage: " << toolname << " <info command> <command options>" << std::endl;
    out << "       " << toolname << " --help" << std::endl;
    out << "This is the " << PACKAGE_NAME << " backend info tool." << std::endl;
}

/**
   Prints the long help text.
 */
static void
print_help( void )
{
    print_short_usage( std::cout );
    std::cout << std::endl;
    std::cout << "Available info commands:" << std::endl;
    std::cout << std::endl;
    std::cout << "  system-tree:" << std::endl;
    std::cout << "    Shows the available system tree levels, starting with the root." << std::endl;
    std::cout << std::endl;
    std::cout << "  config-vars:" << std::endl;
    std::cout << "    Shows the current values of all measurement config variables." << std::endl;
}

int
main( int   argc,
      char* argv[] )
{
    if ( argc == 1 )
    {
        print_short_usage( std::cout );
        return EXIT_SUCCESS;
    }

    std::string info_command( argv[ 1 ] );
    if ( info_command == "--help" || info_command == "-h" )
    {
        print_help();
        return EXIT_SUCCESS;
    }

    std::vector< char* > args( argv + 2,  argv + argc );


    if ( info_command == "system-tree" )
    {
        if ( args.size() != 0 )
        {
            std::cerr << "ERROR: Invalid number of options for info command "
                      << "'" << info_command << "'" << std::endl;
            print_short_usage( std::cerr );
            return EXIT_FAILURE;
        }

        SCOREP_ConfigInit();
        SCOREP_RegisterAllConfigVariables();
        SCOREP_ConfigApplyEnv();

        SCOREP_Platform_SystemTreePathElement* path = NULL;
        SCOREP_ErrorCode                       err  =
            SCOREP_Platform_GetPathInSystemTree( &path,
                                                 SCOREP_Env_GetMachineName(),
                                                 SCOREP_PLATFORM_NAME );
        if ( SCOREP_SUCCESS != err )
        {
            std::cerr << "ERROR: Cannot get system tree information" << std::endl;
            return EXIT_FAILURE;
        }
        SCOREP_Platform_SystemTreePathElement* node;
        SCOREP_PLATFORM_SYSTEM_TREE_FORALL( path, node )
        {
            std::cout << node->node_class << std::endl;
        }
        SCOREP_Platform_FreePath( path );
        SCOREP_ConfigFini();
        return EXIT_SUCCESS;
    }


    if ( info_command == "config-vars" )
    {
        if ( args.size() != 0 )
        {
            std::cerr << "ERROR: Invalid number of options for info command "
                      << "'" << info_command << "'" << std::endl;
            print_short_usage( std::cerr );
            return EXIT_FAILURE;
        }

        SCOREP_ConfigInit();
        SCOREP_RegisterAllConfigVariables();
        SCOREP_ConfigApplyEnv();
        SCOREP_ConfigDump( stdout );
        SCOREP_ConfigFini();
        return EXIT_SUCCESS;
    }


    std::cerr << "ERROR: Invalid info command: '" << info_command << "'" << std::endl;
    print_short_usage( std::cerr );
    return EXIT_FAILURE;
}
