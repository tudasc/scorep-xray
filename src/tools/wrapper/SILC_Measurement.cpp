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

#include <config.h>
#include "SILC_Measurement.hpp"

/**
 * @status alpha
 * @maintainer Daniel Lorenz  <d.lorenz@fz-juelich.de>
 * @file SILC_Measurement.cpp
 *
 * class to steer the run of the instrumented application
 */


SILC_Measurement::SILC_Measurement()
{
    silc_library_path = "";
    user_command      = "";
}

SILC_Measurement::~SILC_Measurement ()
{
}

SILC_Error_Code
SILC_Measurement::ParseCmdLine
(
    int    argc,
    char** argv
)
{
    int parameter_mode = true;
    for ( int i = 2; i < argc; i++ )
    {
        if ( parameter_mode )
        {
            if ( *argv[ i ] != '-' )
            {
                parameter_mode = false;
            }
            else if ( !CheckForCommonArg( argv[ i ] ) )
            {
                std::cerr << "ERROR: Unknown parameter: " << argv[ i ]
                          << std::endl;
                abort();
            }
        }
        if ( !parameter_mode )
        {
            user_command += " ";
            user_command +=  argv[ i ];
        }
    }
    return ReadConfigFile( argv[ 0 ] );
}


int
SILC_Measurement::Run()
{
    if ( silc_library_path != "" )
    {
        user_command = "LD_LIBRARY_PATH="
                       + silc_library_path
                       + ":$LD_LIBRARY_PATH "
                       + user_command;
    }
    if ( verbosity >= 1 )
    {
        std::cout << user_command << std::endl;
    }
    return system( user_command.c_str() );
}

void
SILC_Measurement::PrintParameter()
{
}

/* ****************************************************************************
   Config file parsing
******************************************************************************/

void
SILC_Measurement::SetCompilerFlags( std::string flags )
{
}

void
SILC_Measurement::AddIncDir( std::string dir )
{
}

void
SILC_Measurement::AddLibDir( std::string dir )
{
    if ( silc_library_path == "" )
    {
        silc_library_path = dir;
    }
    else
    {
        silc_library_path += ":" + dir;
    }
}

void
SILC_Measurement::AddLib( std::string lib )
{
}
