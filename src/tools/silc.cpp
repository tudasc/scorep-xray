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
 * silc user interaction
 * provides wrapper for instrumentation and binary for steering measurement system
 */


#include <iostream>
#include <string>

#include <SILC_Utils.h>

#include "Silc_ApplicationType.hpp"
#include "Silc_Instrumenter.hpp"
#include "Silc_Measurement.hpp"


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
        if ( inst == "--instrument" || inst == "-inst" )
        {
            // select the application
            Silc_Application* appType = Silc_ApplicationType::getInstance().getSilcStage( "Instrumenter" );

            appType->silc_printParameter();

            appType->silc_parseCmdLine( argc, argv );

            if ( appType->silc_run() == SILC_SUCCESS )
            {
                SILC_DEBUG_PRINTF( SILC_DEBUG_USER, "Instrument the user code!" );
            }
            else
            {
                // catch - give an error here
            }
        }
        else if ( inst == "--measurement" || inst == "-measure" )
        {
            Silc_Application* appType = Silc_ApplicationType::getInstance().getSilcStage( "Measurement" );
            appType->silc_parseCmdLine( argc, argv );
            if ( appType->silc_run() == SILC_SUCCESS )
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
            std::cout << " This is the SILC user tool, for which you have the option to instrument your code \n"
                      << " (instrumenter) or analyse your instrumented application (measurement system).\n"
                      << "\n The following program options are supported:\n "
                      << " --instrument  <instOption>                  calls the instrumentation wrapper\n"
                      << "                                             which have to be steered by the <instOptions>\n"
                      << "                                             argument. Values are:\n\n"
                      << "      comp:type      (with type={gnu, ibm, pgi, ftrace,\n"
                      << "                      (pathscale), (openUH)} )\n"
                      << "      user:type      (with type={mpi, openmp, hybrid} )\n"
                      << "      bin:type       (with type={dyninst})\n\n"
                      << " --measurement  <measureOption>              Starts your previously instrumented application \n"
                      << "                                             with additioal options . Values are:\n\n"
                      << "      ...             (define use cases for that feature...)\n\n"
                      << " --help                                      Show help output. \n"
                      << std::endl;
        }
        else
        {
            SILC_DEBUG_PRINTF( SILC_DEBUG_USER, "Invalid instrumentation type!!! " );
        }
    }
    else
    {
        std::cerr << "no silc argument given, try '-h' or '--help' instead (to be implemented) \n" << std::endl;
    }

    return 0;
}
