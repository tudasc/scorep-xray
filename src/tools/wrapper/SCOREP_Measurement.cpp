/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
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

#include <config.h>
#include "SCOREP_Measurement.hpp"

/**
 * @status alpha
 * @maintainer Daniel Lorenz  <d.lorenz@fz-juelich.de>
 * @file SCOREP_Measurement.cpp
 *
 * class to steer the run of the instrumented application
 */


SCOREP_Measurement::SCOREP_Measurement()
{
    scorep_library_path = "";
    user_command        = "";
}

SCOREP_Measurement::~SCOREP_Measurement ()
{
}

SCOREP_Error_Code
SCOREP_Measurement::ParseCmdLine
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
SCOREP_Measurement::Run()
{
    if ( scorep_library_path != "" )
    {
        user_command = "LD_LIBRARY_PATH="
                       + scorep_library_path
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
SCOREP_Measurement::PrintParameter()
{
}

/* ****************************************************************************
   Config file parsing
******************************************************************************/

void
SCOREP_Measurement::SetCompilerFlags( std::string flags )
{
}

void
SCOREP_Measurement::AddIncDir( std::string dir )
{
}

void
SCOREP_Measurement::AddLibDir( std::string dir )
{
    if ( scorep_library_path == "" )
    {
        scorep_library_path = dir;
    }
    else
    {
        scorep_library_path += ":" + dir;
    }
}

void
SCOREP_Measurement::AddLib( std::string lib )
{
}
