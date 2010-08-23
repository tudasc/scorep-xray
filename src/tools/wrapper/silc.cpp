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
#include "SILC_ApplicationType.hpp"
#include "SILC_Instrumenter.hpp"
#include "SILC_Measurement.hpp"


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

int
main
(
    int   argc,
    char* argv[]
)
{
    if ( argc > 1 )
    {
        std::string inst = argv[ 1 ];
        if ( inst == "--instrument" )
        {
            // select the application
            SILC_Application* appType = SILC_ApplicationType::getInstance().getSilcStage( "Instrumenter" );

            appType->ReadConfigFile( "silc_config.dat" );
            appType->ParseCmdLine( argc, argv );

            if ( appType->Run() == SILC_SUCCESS )
            {
                SILC_DEBUG_PRINTF( SILC_DEBUG_USER, "Instrument the user code!" );
            }
            else
            {
                // catch - give an error here
            }
        }
        else if ( inst == "--measure" )
        {
            SILC_Application* appType = SILC_ApplicationType::getInstance().getSilcStage( "Measurement" );
            appType->ParseCmdLine( argc, argv );
            if ( appType->Run() == SILC_SUCCESS )
            {
                SILC_DEBUG_PRINTF( SILC_DEBUG_USER, "Running the instrumented code using the silc measurement system!" );
            }
            else
            {
                // catch -give an error here
            }
        }
        else if ( inst == "--help" || inst == "-h" )
        {
            print_help( argv[ 0 ] );
        }
        else
        {
            std::cerr << "Invalid action specified" << std::endl;
            print_short_usage( argv[ 0 ] );
        }
    }
    else
    {
        std::cerr << "No action specified" << std::endl;
        print_short_usage( argv[ 0 ] );
    }

    return 0;
}
