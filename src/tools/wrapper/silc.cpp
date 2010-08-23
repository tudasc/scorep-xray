/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


/**
 * @maintainer Daniel Lorenz  <d.lorenz@fz-juelich.de>
 * @status alpha
 * silc user interaction
 * provides wrapper for instrumentation and binary for steering measurement system
 */
#include <config.h>
#include <iostream>
#include <string>

#include <SILC_Utils.h>
#include "SILC_Instrumenter.hpp"
#include "SILC_Measurement.hpp"

/**
    Prints a short usage message.
    @param toolname Name of the tool on this platform (argv[0]).
 */
void
print_short_usage( std::string toolname )
{
    std::cout << "\nThis is the SILC user tool. The usage is:\n"
              << toolname << " <action> <options> <orginal command>\n\n"
              << "The action can be one of the following:\n"
              << "  --instrument    Instruments the application.\n"
              << "  --measure       Performs a measurement run of the instrumented application.\n"
              << "  --help, -h      Show help output.\n\n"
              << std::endl;
}

/**
   Prints the long help text.
   @param toolname Name of the tool on this platform (argv[0]).
 */
void
print_help( std::string toolname )
{
    std::cout << "\nThis is the SILC user tool. The usage is:\n"
              << toolname << " <action> <options> <orginal command>\n\n"
              << "The action can be one of the following:\n"
              << "  --instrument    Instruments the application.\n"
              << "  --measure       Performs a measurement run of the instrumented application.\n"
              << "  --help, -h      Show help output.\n\n"
              << "For instrumentation the following options are supported:\n"
              << "  -config <file>  Specifies file for the instrumentation configuration.\n"
              << "  -verbosity=<value> Specifies the verbosity level. The following\n"
              << "                  levels are available:\n"
              << "                  0 = No output (default)\n"
              << "                  1 = Executed commands are displayed\n"
              << "                  2 = Detailed information is displayed\n"
              << "  -compiler       Enables compiler instrumentation Is enabled by default.\n"
              << "  -nocompiler     Disables compiler istrumentation.\n"
              << "  -mpi            Enables mpi wrapper. Is enabled by default if it is a\n"
              << "                  mpi program.\n"
              << "  -nompi          Disables mpi wrappers. They are disabled by default if\n"
              << "                  it is no mpi program.\n"
              << "  -opari          Enables Opari instrumentation. Is enabled by default\n"
              << "                  if it is an OpenMP program.\n"
              << "  -noopari        Disables Opari instrumentation. Is disabled by default\n"
              << "                  if it is no OpenMP program.\n"
              << "  -user           Enables manual user instrumentation.\n"
              << "  -nouser         Disables manual user instrumentation. Is disabled by default.\n"
              << "  -openmp_support Enables OpenMP support. Needed if the instrumentation\n"
              << "                  does not coorectly identify your application as OpenMP\n"
              << "                  program.\n"
              << "  -noopenmp_support Disables OpenMP support.\n"
              << std::endl;
}

/**
   Main routine of the silc wrapper tool. It decides which action to take, and
   invokes appropriate subclasses.
   @param argc Number of arguments.
   @param argv List of arguments
   @returns If an error occurs, -1 is returned, if help was called, 0 is
            returned. Else it returns the return value from the user command.
 */
int
main( int   argc,
      char* argv[] )
{
    SILC_Application* app = NULL;

    if ( argc > 1 )
    {
        // Select action
        std::string action = argv[ 1 ];
        if ( action == "--instrument" )
        {
            app = new SILC_Instrumenter();
        }
        else if ( action == "--measure" )
        {
            app = new SILC_Measurement();
        }
        else if ( action == "--help" || action == "-h" )
        {
            print_help( argv[ 0 ] );
            return 0;
        }
        else
        {
            std::cerr << "Invalid action specified" << std::endl;
            print_short_usage( argv[ 0 ] );
            return -1;
        }

        // Analyze options
        app->ParseCmdLine( argc, argv );

        // Perform action
        int retval = app->Run();
        delete ( app );
        return retval;
    }
    else
    {
        std::cerr << "No action specified" << std::endl;
        print_short_usage( argv[ 0 ] );
        return -1;
    }
}
