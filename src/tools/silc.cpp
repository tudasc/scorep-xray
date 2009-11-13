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

/* user includes */

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

            std::cout << "\nparse the command line \n :" << std::endl;
            appType->silc_parseCmdLine( argc, argv );

            if ( appType->silc_run() == SILC_SUCCESS )
            {
                std::cout << " instrument user code: " << std::endl;
            }
            else
            {
                // catch
            }
        }
        else if ( inst == "--measurement" || inst == "-measure" )
        {
            Silc_Application* appType = Silc_ApplicationType::getInstance().getSilcStage( "Measurement" );
            if ( appType->silc_run() == SILC_SUCCESS )
            {
                std::cout << " running the instrumented code using the silc measurement system: " << std::endl;
            }
            else
            {
                // catch
            }
        }
    }
    else
    {
        std::cerr << "no silc argument given, try '-h' or '--help' instead (to be implemented) \n" << std::endl;
    }

    return 0;
}
