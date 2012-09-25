/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 *    RWTH Aachen University, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 * @file       src/tools/backend-info/scorep_backend_info.cpp
 *
 * @maintainer Bert Wesarg <bert.wesarg@tu-dresden.de>
 * @status     alpha
 *
 * The info tool of Score-P: scorep-backend-info.
 *
 */

#include <config.h>

#include <iostream>
#include <string>
#include <cstdlib>

#include <UTILS_Error.h>

#include <SCOREP_Platform.h>
#include <SCOREP_Config.h>
#include <scorep_environment.h>


/**
   Contains the name of the tool for help output
 */
const std::string toolname = "scorep-backend-info";

/**
    Prints a short usage message.
 */
void
print_short_usage()
{
    std::cout << "This is the " << PACKAGE_NAME << " backend info tool." << std::endl;
    std::cout << "Usage: " << toolname << " <info command> <command options>" << std::endl;
    std::cout << "       " << toolname << " --help" << std::endl;
}

/**
   Prints the long help text.
 */
void
print_help()
{
    print_short_usage();
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
    if ( argc > 1 )
    {
        std::string info_command( argv[ 1 ] );
        if ( info_command == "--help" )
        {
            print_help();
            return EXIT_SUCCESS;
        }

        if ( info_command == "system-tree" )
        {
            SCOREP_Platform_SystemTreePathElement* path;
            SCOREP_ErrorCode                       err = SCOREP_Platform_GetPathInSystemTree( &path );
            if ( err != SCOREP_SUCCESS )
            {
                std::cout << "Can't get system tree information." << std::endl;
                return EXIT_FAILURE;
            }
            SCOREP_Platform_SystemTreePathElement* node;
            SCOREP_PLATFORM_SYSTEM_TREE_FORALL( path, node )
            {
                std::cout << node->node_class << std::endl;
            }
            SCOREP_Platform_FreePath( path );
            return EXIT_SUCCESS;
        }


        if ( info_command == "config-vars" )
        {
            SCOREP_ConfigInit();
            SCOREP_RegisterAllConfigVariables();
            SCOREP_ConfigApplyEnv();
            SCOREP_ConfigDump( stdout );
            SCOREP_ConfigFini();
            return EXIT_SUCCESS;
        }


        std::cout << "Invalid info command: " << argv[ 1 ] << std::endl;
        print_short_usage();
        return EXIT_FAILURE;
    }
    else
    {
        print_short_usage();
    }
    return EXIT_SUCCESS;
}
