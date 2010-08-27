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
#include <stdio.h>
#include <stdlib.h>

#include <silc_utility/SILC_Utils.h>
#include <silc_utility/SILC_Error.h>

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

    silc_include_path = "";
    silc_library_path = "";
    external_libs     = "";
}

SILC_Instrumenter::~SILC_Instrumenter ()
{
}

int
SILC_Instrumenter::Run()
{
    if ( verbosity >= 2 )
    {
        PrintParameter();
    }

    if ( compiler_instrumentation == enabled )
    {
        prepare_compiler();
    }
    if ( opari_instrumentation == enabled )
    {
        prepare_opari();
    }
    if ( user_instrumentation == enabled )
    {
        prepare_user();
    }
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
            case silc_parse_mode_config:
                mode = parse_config( argv[ i ] );
                break;
        }
    }
    check_parameter();
    return ReadConfigFile( argv[ 0 ] );
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
        is_mpi_application  = enabled;
        return silc_parse_mode_param;
    }
    else if ( arg == "-nompi" )
    {
        is_mpi_application  = disabled;
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
    /* Configuration file */
    else if ( CheckForCommonArg( arg ) )
    {
        return silc_parse_mode_param;
    }
    else
    {
        std::cerr << "ERROR: Unknown parameter: " << arg << std::endl;
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
        std::cout << "ERROR: Could not identify compiler name." << std::endl;
        abort();
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

SILC_Instrumenter::silc_parse_mode_t
SILC_Instrumenter::parse_config( std::string arg )
{
    config_file = arg;
    return silc_parse_mode_param;
}

/* ****************************************************************************
   Config file parsing
******************************************************************************/

void
SILC_Instrumenter::SetCompilerFlags( std::string flags )
{
    compiler_instrumentation_flags = flags;
}

void
SILC_Instrumenter::AddIncDir( std::string dir )
{
    silc_include_path += " -I" + dir;
}

void
SILC_Instrumenter::AddLibDir( std::string dir )
{
    silc_library_path += " -L" + dir;
}

void
SILC_Instrumenter::AddLib( std::string lib )
{
    external_libs += " " + lib;
}

/* ****************************************************************************
   Preparation
******************************************************************************/
void
SILC_Instrumenter::prepare_compiler()
{
    compiler_flags += " " + compiler_instrumentation_flags;
}

void
SILC_Instrumenter::prepare_user()
{
    compiler_flags = " -DSILC_USER_ENABLE=1" + compiler_flags;
}

void
SILC_Instrumenter::prepare_opari()
{
    // Perform source code transformation on all source files

    // if linking: Generate POMP_Region_init() and add it
}

/* ****************************************************************************
   Command execution
******************************************************************************/
int
SILC_Instrumenter::execute_command()
{
    std::string silc_lib;
    if ( is_linking )
    {
        if ( is_mpi_application == enabled )
        {
            silc_lib = ( is_openmp_application == enabled ?
                         " -lsilc_mpi_omp" : " -lsilc_mpi" );
        }
        else
        {
            silc_lib = ( is_openmp_application == enabled ?
                         " -lsilc_omp" : " -lsilc_serial" );
        }
        silc_lib += " -lotf2 -lsilc_utilities";
    }
    std::string command = compiler_name
                          + silc_library_path
                          + silc_include_path
                          + input_files
                          + silc_lib
                          + compiler_flags
                          + external_libs
                          + " -o " + output_name;

    if ( verbosity >= 1 )
    {
        std::cout << command << std::endl;
    }
    return system( command.c_str() );
}
