/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013, 2019-2020, 2022,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2013-2016, 2019, 2021,
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
#include <scorep_tools_utils.hpp>
#include <scorep_config_tool_backend.h>
#include <scorep_config_tool_mpi.h>
#include <SCOREP_Filter.h>

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <sstream>

#include <UTILS_Error.h>
#include <UTILS_IO.h>

#if HAVE( POSIX_PIPES )
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
    m_default_off.push_back( SCOREP_INSTRUMENTER_ADAPTER_PDT );

#if !HAVE_BACKEND( SCOREP_COMPILER_INSTRUMENTATION )
    unsupported();
#endif /*!HAVE_BACKEND( SCOREP_COMPILER_INSTRUMENTATION )*/
}

bool
SCOREP_Instrumenter_CompilerAdapter::supportInstrumentFilters( void ) const
{
    // So far, this method is used to print a warning if filtering was requested
    // but is not supported by the instrumentation method. With language specific
    // instrumentation methods and the language not available here, be pragmatic
    // and return true if any of the configured instrumentation methods supports
    // filtering.
#if HAVE_BACKEND( SCOREP_COMPILER_INSTRUMENTATION_GCC_PLUGIN ) || \
    HAVE_BACKEND( SCOREP_COMPILER_INSTRUMENTATION_VT_INTEL )
    return true;
#else
    return false;
#endif
}

#if HAVE_BACKEND( SCOREP_COMPILER_INSTRUMENTATION_VT_INTEL )
// Converts a Score-P filter regex expression by the Intel tcollect format
static std::string
convert_regex_to_tcollect2( std::string rule )
{
    rule = replace_all( "*", ".*", rule );
    rule = replace_all( "?", ".", rule  );
    rule = replace_all( "[!", "[^", rule  );

    return rule;
}

// Callback to write tcollect function rule to file
static void
write_tcollect_function_rules( void*       userData,
                               const char* pattern,
                               bool        isExclude,
                               bool        isMangled )
{
    UTILS_BUG_ON( userData == NULL, "userData is NULL!" );
    UTILS_BUG_ON( pattern == NULL, "pattern is NULL!" );

    std::ofstream* stream = static_cast<std::ofstream*>( userData );
    *stream << " ':" << convert_regex_to_tcollect2( pattern )
            << "$' " << ( isExclude  ? "OFF" : "ON" ) << std::endl;
}

// Callback to write tcollect file rule to file
static void
write_tcollect_file_rules( void*       userData,
                           const char* pattern,
                           bool        isExclude,
                           bool        isMangled )
{
    UTILS_BUG_ON( userData == NULL, "userData is NULL!" );
    UTILS_BUG_ON( pattern == NULL, "pattern is NULL!" );

    std::ofstream* stream = static_cast<std::ofstream*>( userData );
    *stream << " '" << convert_regex_to_tcollect2( pattern )
            << ":' " << ( isExclude  ? "OFF" : "ON" ) << std::endl;
}
#endif // HAVE_BACKEND( SCOREP_COMPILER_INSTRUMENTATION_VT_INTEL )


std::string
SCOREP_Instrumenter_CompilerAdapter::getConfigToolFlag( SCOREP_Instrumenter_CmdLine& cmdLine,
                                                        const std::string&           inputFile )
{
#define FILTER_GCC_PLUGIN \
    if ( cmdLine.getVerbosity() >= 1 ) \
    { \
        std::ostringstream verbosity_arg; \
        verbosity_arg << " --compiler-arg=-fplugin-arg-scorep_instrument_function-verbosity="; \
        verbosity_arg << cmdLine.getVerbosity(); \
        flags += verbosity_arg.str(); \
    } \
    const std::vector<std::string>& filter_files = cmdLine.getInstrumentFilterFiles(); \
    for ( std::vector<std::string>::const_iterator file_it = filter_files.begin(); \
          file_it != filter_files.end(); \
          ++file_it ) \
    { \
        flags += " --compiler-arg=-fplugin-arg-scorep_instrument_function-filter="  + *file_it; \
    }

#define FILTER_INTEL \
    SCOREP_Filter * filter = SCOREP_Filter_New(); \
    std::string outfname; \
    bool        have_usable_filter = false; \
    \
    const std::vector<std::string>& filter_files = cmdLine.getInstrumentFilterFiles(); \
    for ( std::vector<std::string>::const_iterator file_it = filter_files.begin(); \
          file_it != filter_files.end(); \
          ++file_it ) \
    { \
        std::string fname = *file_it; \
        /* Parsing filter files. */ \
        SCOREP_ErrorCode err = SCOREP_Filter_ParseFile( filter,  fname.c_str() ); \
        if ( err != SCOREP_SUCCESS ) \
        { \
            std::cerr << "[Score-P] ERROR: Unable to parse filter file '" << fname << "' !" << std::endl; \
        } \
        else \
        { \
            outfname          += fname + "."; \
            have_usable_filter = true; \
        } \
    } \
         \
    if ( have_usable_filter ) \
    { \
        /* Using an unique temp file to avoid data races when calling scorep multiple times. */ \
        outfname += create_random_string() +  ".tcollect"; \
        /* Converting the aggregated filter data to tcollect format. */ \
        std::ofstream filter_file( outfname.c_str() ); \
        if ( filter_file.is_open() ) \
        { \
            SCOREP_Filter_ForAllFunctionRules( filter, write_tcollect_function_rules, &filter_file ); \
            SCOREP_Filter_ForAllFileRules( filter, write_tcollect_file_rules, &filter_file ); \
            filter_file.close(); \
        } \
        else \
        { \
            UTILS_ERROR_POSIX(  "Unable to open output filter specification file '%s'", \
                                outfname.c_str() ); \
        } \
                 \
        cmdLine.addTempFile( outfname ); \
        flags += " --compiler-arg=-tcollect-filter"; \
        flags += " --compiler-arg=" + outfname; \
        /* Add tcollect reporting flags. */ \
        flags += " --compiler-arg=-qopt-report-file=stderr"; \
        flags += " --compiler-arg=-qopt-report-phase=tcollect"; \
    }

    if ( !isEnabled() )
    {
        return " --no" + m_name;
    }

    std::string flags;

    if ( is_c_file( inputFile ) )
    {
#if HAVE_BACKEND( SCOREP_COMPILER_INSTRUMENTATION_CC_GCC_PLUGIN )
        FILTER_GCC_PLUGIN
#elif HAVE_BACKEND( SCOREP_COMPILER_INSTRUMENTATION_CC_VT_INTEL )
        FILTER_INTEL
#endif  /* SCOREP_BACKEND_COMPILER_CC_INTEL */
    }
    else if ( is_cpp_file( inputFile ) )
    {
#if HAVE_BACKEND( SCOREP_COMPILER_INSTRUMENTATION_CXX_GCC_PLUGIN )
        FILTER_GCC_PLUGIN
#elif HAVE_BACKEND( SCOREP_COMPILER_INSTRUMENTATION_CXX_VT_INTEL )
        FILTER_INTEL
#endif  /* SCOREP_BACKEND_COMPILER_CXX_INTEL */
    }
    else if ( is_fortran_file( inputFile ) )
    {
#if HAVE_BACKEND( SCOREP_COMPILER_INSTRUMENTATION_FC_GCC_PLUGIN )
        FILTER_GCC_PLUGIN
#elif HAVE_BACKEND( SCOREP_COMPILER_INSTRUMENTATION_FC_VT_INTEL )
        FILTER_INTEL
#endif  /* SCOREP_BACKEND_COMPILER_FC_INTEL */
    }

    return flags;

#undef FILTER_GCC_PLUGIN
#undef FILTER_INTEL
}

void
SCOREP_Instrumenter_CompilerAdapter::prelink( SCOREP_Instrumenter&         instrumenter,
                                              SCOREP_Instrumenter_CmdLine& cmdLine )
{
#if HAVE_BACKEND( SCOREP_COMPILER_INSTRUMENTATION_GCC_PLUGIN )
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
