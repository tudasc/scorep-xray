/**
 * silc user interaction
 * provides wrapper for instrumentation and binary for steering measurement system
 */


#include <iostream>
#include <string>

/* user includes */

#include "Silc_Instrumenter.hpp"
#include "Silc_Measurement.hpp"
#include "Silc_ApplicationType.hpp"


int
main
(
    int   argc,
    char* argv[]
)
{
    std::string silcConf =  "silc.conf";

    if ( argc > 1 )
    {
        std::cout << " argument 1: " << argv[ 1 ] << " and total number of arguments: " << argc << std::endl;

        std::string inst = argv[ 1 ];
        if ( inst == "--instrument" || inst == "-inst" )
        {
            //parse config file 'silcConf' to application
            // select the application
            // check wheter config stage went well

            Silc_Application* appType = Silc_ApplicationType::getInstance().getSilcStage( "Instrumenter" );

            appType->silc_printParameter();
        }
    }
    else
    {
        std::cerr << "no silc argument given, try '-h' or '--help' instead (to be implemented) \n" << std::endl;
    }

    return 0;
}
