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
 * @file       src/tools/info/scorep_info.cpp
 *
 * @maintainer Bert Wesarg <bert.wesarg@tu-dresden.de>
 * @status     alpha
 *
 * The info tool of Score-P: scorep-info.
 *
 */

#include <config.h>

#include <iostream>
#include <string>
#include <cstdlib>

#include <SCOREP_Error.h>
#include <SCOREP_Debug.h>

#include <SCOREP_Platform.h>

/**
   Contains the name of the tool for help output
 */
const std::string toolname = "scorep-info";

/**
    Prints a short usage message.
 */
void
print_short_usage()
{
    std::cout << "This is the " << PACKAGE_NAME << " info tool." << std::endl;
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
/*
    std::cout << std::endl;
    std::cout << "    Info command options:" << std::endl;
    std::cout << "      --full        Also prints the instances for the system tree leves for this job." << std::endl;
 */
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
            SCOREP_Error_Code                      err = SCOREP_Platform_GetPathInSystemTree( &path );
            if ( err != SCOREP_SUCCESS )
            {
                SCOREP_ERROR( err, "Can't get system tree information." );
                return EXIT_SUCCESS;
            }
            SCOREP_Platform_SystemTreePathElement* node;
            SCOREP_PLATFORM_SYSTEM_TREE_FORALL( path, node )
            {
                std::cout << node->node_class << std::endl;
            }
            SCOREP_Platform_FreePath( path );
            return EXIT_SUCCESS;
        }
        std::cout << "Invalid info command:" << argv[ 1 ] << std::endl;
        print_short_usage();
        return EXIT_FAILURE;
    }
    else
    {
        print_short_usage();
    }
    return EXIT_SUCCESS;
}
