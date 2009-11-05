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
    }
    else
    {
        std::cerr << "no silc argument given, try '-h' or '--help' instead (to be implemented) \n" << std::endl;
    }

    return 0;
}
