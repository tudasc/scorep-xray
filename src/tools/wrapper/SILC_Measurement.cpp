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


#include "SILC_Measurement.hpp"

/**
 * @status alpha
 * @maintainer Daniel Lorenz  <d.lorenz@fz-juelich.de>
 * @file SILC_Measurement.cpp
 *
 * class to steer the run of the instrumented application
 */


SILC_Measurement::SILC_Measurement
    ()
    : _userCommand( "" )
{
    printf(
        "calling the measurement system by parsing the instrumented user application \n" );
}


SILC_Error_Code
SILC_Measurement::ReadConfigFile
(
    std::string fileName
)
{
    printf( "reads the global configuration file: %s \n ", fileName.c_str() );

    return SILC_SUCCESS;
}


SILC_Error_Code
SILC_Measurement::ParseCmdLine
(
    int    argc,
    char** argv
)
{
    SILC_Error_Code exitStatus =  SILC_ERROR_ENOTSUP;
    std::string
                    instStr( argv[ 1 ] );
    int             pos = instStr.find( "--measurement", 0 );
    if ( pos != std::string::npos
         && argc > 2
         )
    {
        for ( int loop = 2; loop < argc; loop++ )
        {
            std::string
            flag( argv[ loop ] );
            _userCommand += flag + " ";
            std::cout << " flag " << flag << "   " << argv[ loop ] << std::endl;
        }
        exitStatus = SILC_SUCCESS;
    }

    std::cout << " number of args: " << argc << "  user command: " << _userCommand << std::endl;

    return exitStatus;
}


int
SILC_Measurement::Run
(
)
{
    int32_t exitCode = -1;
    printf( "run the user code \n " );

    std::string compCommand = _userCommand + " ";


    std::cout << "  silc_run: " << _userCommand << std::endl;
    exitCode = system( compCommand.c_str() );

    return exitCode;

    return 0;
}


/*
 * @brief display the parameters needed for the measurement system
 */
void
SILC_Measurement::PrintParameter
(
)
{
    printf( "print the measurement system parameter: \n " );
}
