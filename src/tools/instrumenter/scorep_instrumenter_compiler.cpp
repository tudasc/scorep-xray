/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2013-2016, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
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

#if HAVE( POPEN )
static void
scorep_get_ibm_compiler_version( const std::string& compiler,
                                 int&               major,
                                 int&               minor )
{
    FILE*       console;
    char        version_string[ 64 ];
    std::string command = compiler + " -qversion | grep Version | awk '{print $2}'";
    console = popen( command.c_str(), "r" );
    if ( console == NULL )
    {
        std::cerr << "[Score-P] ERROR: Failed to query the compiler version number" << std::endl;
        exit( EXIT_FAILURE );
    }
    int bytes_read = fread( version_string, 1, 64, console );
    if ( bytes_read == 0 )
    {
        std::cerr << "[Score-P] ERROR: Failed to read the compiler version number" << std::endl;
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
#endif

/* **************************************************************************************
 * class SCOREP_Instrumenter_CompilerAdapter
 * *************************************************************************************/
SCOREP_Instrumenter_CompilerAdapter::SCOREP_Instrumenter_CompilerAdapter( void )
    : SCOREP_Instrumenter_Adapter( SCOREP_INSTRUMENTER_ADAPTER_COMPILER, "compiler" )
{
//    m_default_off.push_back( SCOREP_INSTRUMENTER_ADAPTER_COBI );
    m_default_off.push_back( SCOREP_INSTRUMENTER_ADAPTER_PDT );

#if !HAVE_BACKEND( COMPILER_INSTRUMENTATION )
    unsupported();
#endif /*!HAVE_BACKEND( COMPILER_INSTRUMENTATION )*/
}

bool
SCOREP_Instrumenter_CompilerAdapter::supportInstrumentFilters( void ) const
{
#if HAVE_BACKEND( GCC_PLUGIN_SUPPORT )
    return true;
#else
    return false;
#endif
}

std::string
SCOREP_Instrumenter_CompilerAdapter::getConfigToolFlag( SCOREP_Instrumenter_CmdLine& cmdLine )
{
    if ( !isEnabled() )
    {
        return " --no" + m_name;
    }

    std::string flags;

#if HAVE_BACKEND( GCC_PLUGIN_SUPPORT )
    if ( cmdLine.getVerbosity() >= 1 )
    {
        std::ostringstream verbosity_arg;
        verbosity_arg << " --compiler-plugin-arg=verbosity=";
        verbosity_arg << cmdLine.getVerbosity();
        flags += verbosity_arg.str();
    }
    const std::vector<std::string>& filter_files = cmdLine.getInstrumentFilterFiles();
    for ( std::vector<std::string>::const_iterator file_it = filter_files.begin();
          file_it != filter_files.end();
          ++file_it )
    {
        flags += " --compiler-plugin-arg=filter=" + *file_it;
    }
#endif // HAVE_BACKEND( GCC_PLUGIN_SUPPORT )

    return flags;
}

std::string
SCOREP_Instrumenter_CompilerAdapter::precompile( SCOREP_Instrumenter&         instrumenter,
                                                 SCOREP_Instrumenter_CmdLine& cmdLine,
                                                 const std::string&           input_file )
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
        std::string               current_file = "";
        std::string               extension    = "";
        std::vector<std::string>* input_files  = cmdLine.getInputFiles();
        for ( std::vector<std::string>::iterator current_file = input_files->begin();
              current_file != input_files->end();
              current_file++ )
        {
            if ( !is_fortran_file( *current_file ) )
            {
                std::cerr << "[Score-P] ERROR: Compiler instrumentation with the Sun compiler is "
                          << "only possible for Fortran files\n"
                          << "                 Use the '--nocompiler' option to disable compiler "
                          << "instrumentation" << std::endl;
                exit( EXIT_FAILURE );
            }
        }
        checked = true;
    }
#endif // SCOREP_BACKEND_COMPILER_STUDIO

    /* For the XL compiler we have two interfaces. Check whether the version
       of the compilers match the used interface. */
#if SCOREP_BACKEND_COMPILER_IBM && HAVE( POPEN )
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
            std::cerr << "[Score-P] ERROR: This compiler version is too old to be used with this "
                      << "Score-P installation\n"
                      << "                 You need to use a Score-P installation that was compiled"
                      << " with XLC 11.0 or earlier" << std::endl;
            exit( EXIT_FAILURE );
        }
    }
    else
    {
        if ( app_version > 1300 ||
             ( !is_fortran_file( input_file ) && app_version > 1100 ) )
        {
            std::cerr << "[Score-P] ERROR: This compiler version is too new to be used with this "
                      << "Score-P installation\n"
                      << "                 You need to use a Score-P installation that was compiled"
                      << " with XLC 11.1 or higher" << std::endl;
            exit( EXIT_FAILURE );
        }
    }

#endif // SCOREP_BACKEND_COMPILER_IBM

    return input_file;
}

void
SCOREP_Instrumenter_CompilerAdapter::prelink( SCOREP_Instrumenter&         instrumenter,
                                              SCOREP_Instrumenter_CmdLine& cmdLine )
{
#if HAVE_BACKEND( GCC_PLUGIN_SUPPORT )
    if ( !cmdLine.isTargetSharedLib() )
    {
        if ( cmdLine.isBuildCheck() )
        {
            instrumenter.prependInputFile(
                cmdLine.getPathToBinary() + "../build-backend/scorep_compiler_gcc_plugin_begin." OBJEXT );
            instrumenter.appendInputFile(
                cmdLine.getPathToBinary() + "../build-backend/scorep_compiler_gcc_plugin_end." OBJEXT );
        }
        else
        {
            instrumenter.prependInputFile(
                SCOREP_BACKEND_PKGLIBDIR "/scorep_compiler_gcc_plugin_begin." OBJEXT );
            instrumenter.appendInputFile(
                SCOREP_BACKEND_PKGLIBDIR "/scorep_compiler_gcc_plugin_end." OBJEXT );
        }
    }
#endif
}
