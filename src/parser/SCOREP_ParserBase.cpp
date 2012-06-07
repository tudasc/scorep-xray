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

/**
 * @status     alpha
 * @file       MANGLE_NAME( ParserBase ).cpp
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 */

#include <config.h>
#include <fstream>
#include <stdlib.h>
#include <SCOREP_IO.h>
#include <SCOREP_ParserBase.hpp>
#include <SCOREP_Error.h>

MANGLE_NAME( ParserBase ) ::MANGLE_NAME( ParserBase ) ()
{
    config_file  = "";
    config_fname = "scorep_config.dat";
    verbosity    = 0;
}

MANGLE_NAME( ParserBase ) ::~
MANGLE_NAME( ParserBase ) ()
{
}

MANGLE_NAME( Error_Code )
MANGLE_NAME( ParserBase ) ::ReadConfigFile( std::string arg0 )
{
    std::ifstream inFile;

    if ( open_config_file( &inFile, arg0 ) == MANGLE_NAME( SUCCESS ) )
    {
        while ( inFile.good() )
        {
            char line[ 512 ] = { "" };
            inFile.getline( line, 512 );
            read_parameter( line );
        }
        return MANGLE_NAME( SUCCESS );
    }
    else
    {
        return MANGLE_NAME( ERROR_FILE_CAN_NOT_OPEN );
    }
}

/* ****************************************************************************
   Config file parsing
******************************************************************************/
bool
MANGLE_NAME( ParserBase ) ::CheckForCommonArg( std::string arg )
{
    /* Configuration file */
    if ( arg.substr( 0, 8 ) == "--config" )
    {
        if ( arg.length() > 9 )
        {
            config_file = arg.substr( 9, arg.length() - 9 );
        }
        else
        {
            std::cerr << "ERROR: No config file name specified." << std::endl;
            abort();
        }
        return true;
    }
    /* Verbosity */
    else if ( arg.substr( 0, 9 ) == "--verbose" )
    {
        if ( arg.length() > 10 )
        {
            verbosity = atol( arg.substr( 10, arg.length() - 10 ).c_str() );
        }
        else
        {
            verbosity = 1;
        }
        return true;
    }
    /* Verbosity */
    else if ( arg.substr( 0, 2 ) == "-v" )
    {
        verbosity = 1;
        return true;
    }
    return false;
}

/* ****************************************************************************
   Config file parsing
******************************************************************************/

MANGLE_NAME( Error_Code )
MANGLE_NAME( ParserBase ) ::open_config_file( std::ifstream * inFile,
                                              std::string arg0 )
{
    // If configure file was specified via -config. Use this file.
    if ( config_file != "" )
    {
        inFile->open( config_file.c_str(), std::ios_base::in );
        if ( !( inFile->good() ) )
        {
            MANGLE_NAME( ERROR ) ( MANGLE_NAME( ERROR_FILE_CAN_NOT_OPEN ), "%s", config_file.c_str() );
            return MANGLE_NAME( ERROR_FILE_CAN_NOT_OPEN );
        }
        return MANGLE_NAME( SUCCESS );
    }

    // Else try standard locations
    // 1. Current path.
    inFile->open( config_fname.c_str(), std::ios_base::in );
    if ( inFile->good() )
    {
        return MANGLE_NAME( SUCCESS );
    }

    // 2. Try the executables path
    char*       path     = MANGLE_NAME( GetExecutablePath ) ( arg0.c_str() );
    std::string filename = "/" + config_fname;
    if ( path != NULL )
    {
        filename = path + filename;
        inFile->open( filename.c_str(), std::ios_base::in );
        if ( inFile->good() )
        {
            return MANGLE_NAME( SUCCESS );
        }
    }
    free( path );

    // Unable to open config file
    MANGLE_NAME( ERROR ) ( MANGLE_NAME( ERROR_FILE_CAN_NOT_OPEN ),
                           "No config file found at %s or %s"
                           ".\nPlease specify the location of your config file with the "
                           "--config=<filename> option.", config_fname.c_str(), filename.c_str() );
    return MANGLE_NAME( ERROR_FILE_CAN_NOT_OPEN );
}

MANGLE_NAME( Error_Code )
MANGLE_NAME( ParserBase ) ::read_parameter( std::string line )
{
    /* check for comments */
    int pos = line.find( "#" );
    if ( pos == 0 )
    {
        return MANGLE_NAME( SUCCESS );                      // Whole line cemmented out
    }
    if ( pos != std::string::npos )
    {
        // Truncate line at comment
        line = line.substr( pos, line.length() - pos - 1 );
    }

    /* separate value and key */
    pos = line.find( "=" );
    if ( pos == std::string::npos )
    {
        return MANGLE_NAME( ERROR_PARSE_NO_SEPARATOR );
    }
    std::string key   = line.substr( 0, pos );
    std::string value = line.substr( pos + 2, line.length() - pos - 3 );

    /* indentify key */
    if ( key == "LIBDIR" )
    {
        while ( pos = line.find( ":" ) != std::string::npos )
        {
            if ( pos != 0 )
            {
                AddLibDir( value.substr( 0, pos ) );
            }
            value = value.substr( pos + 1, line.length() - pos - 1 );
        }
        if ( value != "" )
        {
            AddLibDir( value );
        }
    }
    else if ( key == "INCDIR" && value != "" )
    {
        AddIncDir( value );
    }
    else if ( key == "LIBS" && value != "" )
    {
        AddLib( value );
    }
    else
    {
        SetValue( key, value );
    }

    return MANGLE_NAME( SUCCESS );
}

void
MANGLE_NAME( ParserBase ) ::AddIncDir( std::string dir )
{
}

void
MANGLE_NAME( ParserBase ) ::AddLibDir( std::string dir )
{
}

void
MANGLE_NAME( ParserBase ) ::AddLib( std::string lib )
{
}

bool
MANGLE_NAME( ParserBase ) ::IsConfigFileSet()
{
    return config_file != "";
}
