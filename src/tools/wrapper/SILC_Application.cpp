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
 * @status     alpha
 * @file       SILC_Application.cpp
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 */

#include <config.h>
#include <fstream>
#include <silc_utility/SILC_Utils.h>
#include "SILC_Application.hpp"

#define SILC_CONFIG_FILE_NAME "silc_config.dat"

SILC_Application::SILC_Application()
{
    config_file = "";
    verbosity   = 0;
}

SILC_Application::~
SILC_Application()
{
}

SILC_Error_Code
SILC_Application::ReadConfigFile( std::string arg0 )
{
    std::ifstream inFile;


    if ( open_config_file( &inFile, arg0 ) == SILC_SUCCESS )
    {
        while ( inFile.good() )
        {
            char line[ 512 ] = { "" };
            inFile.getline( line, 512 );
            read_parameter( line );
        }
        return SILC_SUCCESS;
    }
    else
    {
        return SILC_ERROR_FILE_CAN_NOT_OPEN;
    }
}

/* ****************************************************************************
   Config file parsing
******************************************************************************/
bool
SILC_Application::CheckForCommonArg( std::string arg )
{
    /* Configuration file */
    if ( arg.substr( 0, 7 ) == "-config" )
    {
        if ( arg.length() > 8 )
        {
            config_file = arg.substr( 8, arg.length() - 8 );
        }
        else
        {
            std::cerr << "ERROR: No config file name specified." << std::endl;
            abort();
        }
        return true;
    }
    /* Verbosity */
    else if ( arg.substr( 0, 10 ) == "-verbosity" )
    {
        if ( arg.length() > 11 )
        {
            verbosity = atol( arg.substr( 11, arg.length() - 11 ).c_str() );
        }
        else
        {
            std::cerr << "ERROR: No verbosity value specified." << std::endl;
            abort();
        }
        return true;
    }
    return false;
}

/* ****************************************************************************
   Config file parsing
******************************************************************************/

SILC_Error_Code
SILC_Application::open_config_file( std::ifstream* inFile,
                                    std::string    arg0 )
{
    // If configure file was specified via -config. Use this file.
    if ( config_file != "" )
    {
        inFile->open( config_file.c_str(), std::ios_base::in );
        if ( !( inFile->good() ) )
        {
            SILC_ERROR( SILC_ERROR_FILE_CAN_NOT_OPEN, "%s", config_file.c_str() );
            return SILC_ERROR_FILE_CAN_NOT_OPEN;
        }
        return SILC_SUCCESS;
    }

    // Else try standard locations
    // 1. Current path.
    inFile->open( SILC_CONFIG_FILE_NAME, std::ios_base::in );
    if ( inFile->good() )
    {
        return SILC_SUCCESS;
    }

    // 2. Try the executables path
    char*       path     = SILC_GetExecutablePath( arg0.c_str() );
    std::string filename = "/" SILC_CONFIG_FILE_NAME;
    if ( path != NULL )
    {
        filename = path + filename;
        inFile->open( filename.c_str(), std::ios_base::in );
        if ( inFile->good() )
        {
            return SILC_SUCCESS;
        }
    }
    free( path );

    // Unable to open config file
    SILC_ERROR( SILC_ERROR_FILE_CAN_NOT_OPEN,
                "No config file found at " SILC_CONFIG_FILE_NAME " or %s"
                ".\nPlease specify the location of your config file with the "
                "-config <filename> option.", filename.c_str() );
    return SILC_ERROR_FILE_CAN_NOT_OPEN;
}

SILC_Error_Code
SILC_Application::read_parameter( std::string line )
{
    /* check for comments */
    int pos = line.find( "#" );
    if ( pos == 0 )
    {
        return SILC_SUCCESS;                      // Whole line cemmented out
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
        return SILC_ERROR_PARSE_NO_SEPARATOR;
    }
    std::string key   = line.substr( 0, pos );
    std::string value = line.substr( pos + 2, line.length() - pos - 3 );

    /* indentify key */
    if ( key == "COMPILER_INSTRUMENTATION_CPPFLAGS" )
    {
        SetCompilerFlags( value );
    }
    else if ( key == "PREFIX" && value != "" )
    {
        AddIncDir( value + "/include/silc" );
        AddLibDir( value + "/lib" );
    }
    else if ( key == "LIBDIR" )
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
    else if ( key == "CC" && value != "" )
    {
        SetCompiler( value );
    }
    else if ( key == "NM" && value != "" )
    {
        SetNm( value );
    }
    else if ( key == "AWK" && value != "" )
    {
        SetAwk( value );
    }
    else if ( key == "OPARI" && value != "" )
    {
        SetOpari( value );
    }

    return SILC_SUCCESS;
}

void
SILC_Application::SetCompiler( std::string value )
{
}

void
SILC_Application::SetNm( std::string value )
{
}

void
SILC_Application::SetAwk( std::string value )
{
}

void
SILC_Application::SetOpari( std::string value )
{
}
