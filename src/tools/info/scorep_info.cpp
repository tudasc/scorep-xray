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

#include <UTILS_Error.h>

#include <SCOREP_Platform.h>
#include <SCOREP_Config.h>
#include <scorep_environment.h>
#include <scorep_config_tool_backend.h>

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
    std::cout << "  config-vars:" << std::endl;
    std::cout << "    Shows the list of all measurement config variables with a short description." << std::endl;
    std::cout << std::endl;
    std::cout << "    Info command options:" << std::endl;
    std::cout << "      --full        Displays a detailed description for each config variable." << std::endl;
    std::cout << "      --values      Displays the current values for each config variable." << std::endl;
    std::cout << "                    Warning: These values may be wrong, please consult the\n";
    std::cout << "                             manual of the batch system how to pass the values\n";
    std::cout << "                             to the measurement job." << std::endl;
    std::cout << std::endl;
    std::cout << "  config-summary:" << std::endl;
    std::cout << "    Shows the configure summary of the Score-P package." << std::endl;
    std::cout << std::endl;
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


        if ( info_command == "config-vars" )
        {
            SCOREP_ConfigInit();

            std::string mode( argc > 2 ? argv[ 2 ] : "" );
            if ( mode == "--values" )
            {
                // @todo print warning again
                SCOREP_RegisterAllConfigVariables();
                SCOREP_ConfigApplyEnv();
                SCOREP_ConfigDump( stdout );
            }
            else
            {
                bool full = false;
                bool html = false;
                if ( mode == "--full" )
                {
                    full = true;
                }
                else if ( mode == "--full=html" )
                {
                    full = true;
                    html = true;
                }
                else if ( mode == "--doxygen" )
                {
                    full = true;
                    html = true;
                    SCOREP_ConfigForceConditionalRegister();
                }
                else
                {
                    std::cout << "Invalid option for info command "
                              << info_command << ": " << mode << std::endl;
                    print_short_usage();
                    SCOREP_ConfigFini();
                    return EXIT_FAILURE;
                }

                SCOREP_RegisterAllConfigVariables();
                SCOREP_ConfigHelp( full, html );
            }

            SCOREP_ConfigFini();
            return EXIT_SUCCESS;
        }


        if ( info_command == "config-summary" )
        {
            std::string summary_command( "cat " CONFIG_SUMMARY_FILE );
            int         return_value = system( summary_command.c_str() );
            if ( return_value != 0 )
            {
                std::cerr << "Error executing: " << summary_command << std::endl;
                return EXIT_FAILURE;
            }
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
