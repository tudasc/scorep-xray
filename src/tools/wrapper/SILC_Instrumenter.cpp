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
 * @file       SILC_Instrumenter.cpp
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 */

#include <config.h>
#include <iostream>
#include <string>
#include <fstream>
#include <istream>
#include <unistd.h>

#include <SILC_Utils.h>
#include <SILC_Error.h>

#include "SILC_Instrumenter.hpp"

/* ****************************************************************************
   Main interface
******************************************************************************/
SILC_Instrumenter::SILC_Instrumenter()
{
    /* Initialize values */
    compiler_instrumentation = enabled;
    opari_instrumentation    = detect;
    user_instrumentation     = disabled;
    mpi_instrumentation      = detect;

    is_mpi_application    = detect;
    is_openmp_application = detect;

    is_compiling = true; // Opposite recognized if no source files in input
    is_linking   = true; // Opposite recognized on existence of -c flag
}

SILC_Instrumenter::~SILC_Instrumenter ()
{
}

int
SILC_Instrumenter::Run()
{
    /*
       if (compiler_instrumentation == enabled) prepare_compiler();
       if (opari_instrumentation == enabled) prepare_opari();
       if (user_instrumentation == enabled) prepare_user();
       if (mpi_instrumentation == enabled) prepare_mpi();
     */
    return execute_command();
}

SILC_Error_Code
SILC_Instrumenter::ParseCmdLine( int    argc,
                                 char** argv )
{
    silc_parse_mode_t mode = silc_parse_mode_param;

    for ( int i = 2; i < argc; i++ )
    {
        switch ( mode )
        {
            case silc_parse_mode_param:
                mode = parse_parameter( argv[ i ] );
                break;
            case silc_parse_mode_command:
                mode = parse_command( argv[ i ] );
                break;
            case silc_parse_mode_output:
                mode = parse_output( argv[ i ] );
                break;
        }
    }
    check_parameter();
    PrintParameter();
}

void
SILC_Instrumenter::PrintParameter()
{
    std::cout << "\nEnabled instrumentation:";
    if ( compiler_instrumentation == enabled )
    {
        std::cout << " compiler";
    }
    if ( opari_instrumentation == enabled )
    {
        std::cout << " opari";
    }
    if ( user_instrumentation == enabled )
    {
        std::cout << " user";
    }
    if ( mpi_instrumentation == enabled )
    {
        std::cout << " mpi";
    }

    std::cout << std::endl << "Linked SILC library type: ";
    if ( is_openmp_application == enabled )
    {
        if ( is_mpi_application == enabled )
        {
            std::cout << "hybrid" << std::endl;
        }
        else
        {
            std::cout << "OpenMP" << std::endl;
        }
    }
    else
    {
        if ( is_mpi_application == enabled )
        {
            std::cout << "MPI" << std::endl;
        }
        else
        {
            std::cout << "serial" << std::endl;
        }
    }

    std::cout << "\nCompiler name: " << compiler_name << std::endl;
    std::cout << "Compiler flags: " << compiler_flags << std::endl;
    std::cout << "Output file: " << output_name << std::endl;
    std::cout << "Input file(s): " << input_files << std::endl;

    std::cout << "\nCompiler instrumentation flags: "
              << compiler_instrumentation_flags << std::endl;
    std::cout << "SILC include path: " << silc_include_path << std::endl;
    std::cout << "SILC library path: " << silc_library_path << std::endl;
}

SILC_Error_Code
SILC_Instrumenter::ReadConfigFile( std::string fileName )
{
    std::ifstream inFile;
    inFile.open( fileName.c_str(), std::ios_base::in );

    if ( inFile.is_open() )
    {
        if ( !( inFile.good() ) )
        {
            return SILC_ERROR_ENOENT;
        }

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
        return SILC_ERROR_ENOENT;
    }
}

/* ****************************************************************************
   Command line parsing
******************************************************************************/
SILC_Instrumenter::silc_parse_mode_t
SILC_Instrumenter::parse_parameter( std::string arg )
{
    if ( arg[ 0 ] != '-' )
    {
        /* Assume its the compiler/linker command. Maybe we want to add a
           validity check later */
        compiler_name = arg;
        return silc_parse_mode_command;
    }

    /* Check for instrumenatation settings */
    else if ( arg == "-compiler" )
    {
        compiler_instrumentation = enabled;
        return silc_parse_mode_param;
    }
    else if ( arg == "-nocompiler" )
    {
        compiler_instrumentation = disabled;
        return silc_parse_mode_param;
    }
    else if ( arg == "-opari" )
    {
        opari_instrumentation = enabled;
        return silc_parse_mode_param;
    }
    else if ( arg == "-nouser" )
    {
        opari_instrumentation = disabled;
        return silc_parse_mode_param;
    }
    else if ( arg == "-user" )
    {
        user_instrumentation = enabled;
        return silc_parse_mode_param;
    }
    else if ( arg == "-nouser" )
    {
        user_instrumentation = disabled;
        return silc_parse_mode_param;
    }
    else if ( arg == "-mpi" )
    {
        mpi_instrumentation = enabled;
        return silc_parse_mode_param;
    }
    else if ( arg == "-nompi" )
    {
        mpi_instrumentation = disabled;
        return silc_parse_mode_param;
    }

    /* Check for application type settings */
    else if ( arg == "-openmp_support" )
    {
        is_openmp_application = enabled;
        return silc_parse_mode_param;
    }
    else if ( arg == "-noopenmp_support" )
    {
        is_openmp_application = disabled;
        return silc_parse_mode_param;
    }
    else if ( arg == "-mpi_support" )
    {
        is_mpi_application = enabled;
        return silc_parse_mode_param;
    }
    else if ( arg == "-nompi_support" )
    {
        is_mpi_application = disabled;
        return silc_parse_mode_param;
    }
    else
    {
        std::cout << "ERROR: Unknown parameter: " << arg << std::endl;
        abort();
    }
}

SILC_Instrumenter::silc_parse_mode_t
SILC_Instrumenter::parse_command( std::string arg )
{
    if ( arg[ 0 ] != '-' )
    {
        /* Assume it is a input file */
        input_files += " " + arg;
        return silc_parse_mode_command;
    }
    else if ( arg == "-c" )
    {
        is_linking = false;
    }
    else if ( arg == "-o" )
    {
        return silc_parse_mode_output;
    }
    else if ( arg == "-openmp" || arg == "-fopenmp" )
    {
        if ( is_openmp_application == detect )
        {
            is_openmp_application = enabled;
        }
        if ( opari_instrumentation == detect )
        {
            opari_instrumentation = enabled;
        }
    }

    /* In any case that not yet returned, save the flag */
    compiler_flags += " " + arg;
    return silc_parse_mode_command;
}

SILC_Instrumenter::silc_parse_mode_t
SILC_Instrumenter::parse_output( std::string arg )
{
    output_name = arg;
    return silc_parse_mode_command;
}

void
SILC_Instrumenter::check_parameter()
{
    if ( compiler_name == "" )
    {
        std::cout << "WARNING: Could not identify compiler name." << std::endl;
    }

    if ( output_name == "" )
    {
        std::cout << "WARNING: Could not identify output file name." << std::endl;
    }

    if ( input_files == "" )
    {
        std::cout << "WARNING: Found no input files." << std::endl;
    }

    /* If is_mpi_application not manually specified, try a guess from the
       compiler name */
    if ( is_mpi_application == detect )
    {
        if ( compiler_name.substr( 0, 3 ) == "mpi" )
        {
            is_mpi_application = enabled;
        }
        else
        {
            is_mpi_application = disabled;
        }
    }

    /* If openmp is not manuelly specified and no openmp flags found, it is
       probably not an openmp application. */
    if ( is_openmp_application == detect )
    {
        is_openmp_application = disabled;
    }

    /* Set mpi and opari instrumenatation if not done manually by the user */
    if ( opari_instrumentation == detect )
    {
        opari_instrumentation = is_openmp_application;
    }

    if ( mpi_instrumentation == detect )
    {
        mpi_instrumentation = is_mpi_application;
    }
}

/* ****************************************************************************
   Config file parsing
******************************************************************************/

SILC_Error_Code
SILC_Instrumenter::read_parameter( std::string line )
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
    std::cout << "Key: " << key << std::endl;
    std::cout << "Value: " << value << std::endl;
    if ( key == "COMPILER_INSTRUMENTATION_CPPFLAGS" )
    {
        compiler_instrumentation_flags = value;
    }
    else if ( key == "PREFIX" )
    {
        silc_include_path += " -I" + value + "/include/silc";
        silc_library_path += " -L" + value + "/lib";
    }
    else if ( key == "LIBDIR" )
    {
        silc_library_path += " -L" + value;
    }
    else if ( key == "INCDIR" )
    {
        silc_library_path += " -I" + value;
    }
    return SILC_SUCCESS;
}

/* ****************************************************************************
   Preperation
******************************************************************************/

/* ****************************************************************************
   Command execution
******************************************************************************/
/**
 * @brief runs the user specified instrumentation command
 *

 * Depending on the instrumentation type the compiler command gets defined
 * and the user code gets compiled. The return value of the function delivers
 * the error code of the system call of the compiler command.
 */
int
SILC_Instrumenter::execute_command()
{
    int exitCode = system( "" );

    return exitCode;
}


/* ****************************************************************************
   old stuff
******************************************************************************/
void
ka()
{
    /* call the configuration reader
     * this is hardcoded, since the file location and name should be defined during configure stage
     */
    /*
       // get the application path
       char cCurrentPath[ 200 ] = { 0 };
       char pCurrentPath[ 200 ] = { 0 };

       sprintf( cCurrentPath, "/proc/%d/exe", getpid() );

       if ( readlink( cCurrentPath, pCurrentPath, sizeof( pCurrentPath ) ) == -1 )
       {
          SILC_ERROR( SILC_ERROR_ENOENT, "" );
       }

       std::cout << "The REAL path to the executable is " << pCurrentPath << std::endl;


       // cut of the last 'silc' exe name to demangle path to binary

       std::string cut   = "silc";
       std::string fname =  "/silc.conf";
       std::string
                  pathStr
       (
          pCurrentPath
       );

       // find last silc in path
       size_t found = pathStr.rfind( cut );
       if ( found != std::string::npos )
       {
          pathStr.replace( found, cut.length(), "" );
       }

       std::cout << pathStr << std::endl;
       pathStr = pathStr + "/../share" + fname;

       if (    silc_readConfigFile( pathStr ) == SILC_SUCCESS )
       {
          std::cout << "calling the instrumentation phase \n";
       }
       // temp: in the svn source tree relative to silc binary
       else if (    silc_readConfigFile( "../src/tools/silc.conf" ) == SILC_SUCCESS )
       {
          std::cout << "calling the instrumentation phase \n";
       }
       // local
       else if (    silc_readConfigFile( "silc.conf" ) == SILC_SUCCESS )
       {
          std::cout << "calling the instrumentation phase \n";
       }
       else
       {
          SILC_ERROR( SILC_ERROR_ENOENT, "" );
       }
     */
}
