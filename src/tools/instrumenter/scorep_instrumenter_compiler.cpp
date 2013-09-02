/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file scorep_instrumenter_compiler.cpp
 *
 * Implements the class for compiler instrumentation.
 */

#include <config.h>
#include "scorep_instrumenter_compiler.hpp"
#include "scorep_instrumenter_cmd_line.hpp"
#include "scorep_instrumenter_install_data.hpp"
#include "scorep_instrumenter_utils.hpp"
#include "scorep_instrumenter.hpp"
#include <scorep_config_tool_backend.h>
#include <scorep_config_tool_mpi.h>

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <sstream>

#include <UTILS_Error.h>
#include <UTILS_IO.h>


static void
scorep_get_ibm_compiler_version( std::string compiler, int& major, int& minor )
{
    FILE*       console;
    char        version_string[ 64 ];
    std::string command = compiler + " -qversion | grep Version | awk '{print $2}'";
    console = popen( command.c_str(), "r" );
    if ( console == NULL )
    {
        std::cerr << "Error: Failed to query the compiler version number" << std::endl;
        exit( EXIT_FAILURE );
    }
    int bytes_read = fread( version_string, 1, 64, console );
    if ( bytes_read == 0 )
    {
        std::cerr << "Error: Failed to read the compiler version number" << std::endl;
        exit( EXIT_FAILURE );
    }
    pclose( console );

    char* current = version_string;
    while ( *current != '.' )
    {
        current++;
    }
    major = atoi( version_string );
    char* token = ++current;
    while ( *current != '.' )
    {
        current++;
    }
    minor = atoi( token );
}

/* **************************************************************************************
 * class SCOREP_Instrumenter_CompilerAdapter
 * *************************************************************************************/
SCOREP_Instrumenter_CompilerAdapter::SCOREP_Instrumenter_CompilerAdapter( void )
    : SCOREP_Instrumenter_Adapter( SCOREP_INSTRUMENTER_ADAPTER_COMPILER, "compiler" )
{
    m_default_off.push_back( SCOREP_INSTRUMENTER_ADAPTER_COBI );
    m_default_off.push_back( SCOREP_INSTRUMENTER_ADAPTER_PDT );
}

std::string
SCOREP_Instrumenter_CompilerAdapter::getConfigToolFlag( void )
{
    if ( !isEnabled() )
    {
        return " --no" + m_name;
    }
    return "";
}

std::string
SCOREP_Instrumenter_CompilerAdapter::precompile
(
    SCOREP_Instrumenter&         instrumenter,
    SCOREP_Instrumenter_CmdLine& cmdLine,
    const std::string&           input_file
)
{
    /* The sun compiler can only instrument Fortran files. Thus, any C/C++
       files are not instrumented. To avoid user confusion, the instrumenter
       aborts in case a C/C++ file should be compiler instrumented.
     */
#if SCOREP_BACKEND_COMPILER_STUDIO
    static bool checked = false;
    if ( !checked && !isEnabled() )
    {
        checked = true;
    }
    if ( !checked )
    {
        std::string current_file = "";
        std::string extension    = "";
        std::string input_files  = cmdLine.getInputFiles();
        size_t      old_pos      = 0;
        size_t      cur_pos      = 0;
        while ( cur_pos != std::string::npos )
        {
            cur_pos = input_files.find( " ", old_pos );
            if ( old_pos < cur_pos ) // Discard a blank
            {
                current_file = input_files.substr( old_pos, cur_pos - old_pos );
                if ( !is_fortran_file( current_file ) )
                {
                    std::cerr << "Compiler instrumentation with the Sun compiler is "
                              << "only possible for Fortran files. If you want to "
                              << "switch off compiler instrumentation, please use the "
                              << "--nocompiler option."
                              << std::endl;
                    exit( EXIT_FAILURE );
                }
            }
            // Setup for next file
            old_pos = cur_pos + 1;
        }
        checked = true;
    }
#endif // SCOREP_BACKEND_COMPILER_STUDIO

    /* For the XL compiler we have two interfaces. Check whether the version
       of the compilers match the used interface. */
#if SCOREP_BACKEND_COMPILER_IBM
    int major, minor, scorep_version, app_version;
    scorep_get_ibm_compiler_version( cmdLine.getInstallData()->getCC(), major, minor );
    scorep_version = major * 100 + minor;
    scorep_get_ibm_compiler_version( cmdLine.getCompilerName(), major, minor );
    app_version =  major * 100 + minor;

    if ( scorep_version > 1100 )
    {
        if ( app_version <= 1100 ||
             ( is_fortran_file( input_file ) && app_version <= 1300 ) )
        {
            std::cerr << "Error: This compiler version is too old to be used with this "
                      << "Score-P installation.\n"
                      << "       You need to use a Score-P installation that was compiled"
                      << " with XLC 11.0 or earlier." << std::endl;
            exit( EXIT_FAILURE );
        }
    }
    else
    {
        if ( app_version > 1300 ||
             ( !is_fortran_file( input_file ) && app_version > 1100 ) )
        {
            std::cerr << "Error: This compiler version is too new to be used with this "
                      << "Score-P installation.\n"
                      << "       You need to use a Score-P installation that was compiled"
                      << " with XLC 11.1 or higher." << std::endl;
            exit( EXIT_FAILURE );
        }
    }

#endif // SCOREP_BACKEND_COMPILER_IBM

    return input_file;
}
