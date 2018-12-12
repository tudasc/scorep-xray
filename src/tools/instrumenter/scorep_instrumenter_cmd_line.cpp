/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2017,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013, 2015, 2018,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * Copyright (c) 2016,
 * Technische Universitaet Darmstadt, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 */

#include <config.h>
#include <scorep_config_tool_mpi.h>
#include <scorep_config_tool_backend.h>
#include "scorep_instrumenter_cmd_line.hpp"
#include "scorep_instrumenter_adapter.hpp"
#include "scorep_instrumenter_selector.hpp"
#include "scorep_instrumenter_utils.hpp"

#include <scorep_tools_utils.hpp>

#include <iostream>
#include <stdlib.h>

#include <UTILS_IO.h>

extern void
print_help( void );

/* ****************************************************************************
   Main interface
******************************************************************************/
SCOREP_Instrumenter_CmdLine::SCOREP_Instrumenter_CmdLine( SCOREP_Instrumenter_InstallData& install_data )
    : m_install_data( install_data )
{
    m_path_to_binary = "";

    /* Execution modes */
    m_target_is_shared_lib = false;
    m_preprocess_mode      = IN_COMPILE_STEP;
    m_is_compiling         = true; // Opposite recognized if no source files in input
    m_is_linking           = true; // Opposite recognized on existence of -c or -E flag
    m_do_nothing           = false;
    m_link_static          = detect;

    /* Input command elements */
    m_compiler_name                  = "";
    m_flags_before_interposition_lib = "";
    m_flags_after_interposition_lib  = "";
    m_current_flags                  = &m_flags_before_interposition_lib;
    m_include_flags                  = "";
    m_define_flags                   = "";
    m_output_name                    = "";
    m_interposition_lib_set          = false;
#if SCOREP_BACKEND_COMPILER_INTEL
    m_mmic_set = false;
#endif  /* SCOREP_BACKEND_COMPILER_INTEL */

    /* Instrumenter flags */
    m_is_dry_run     = false;
    m_keep_files     = false;
    m_verbosity      = 0;
    m_is_build_check = false;

#if defined( SCOREP_SHARED_BUILD )
    m_no_as_needed = false;
#endif
}

SCOREP_Instrumenter_CmdLine::~SCOREP_Instrumenter_CmdLine()
{
}

void
SCOREP_Instrumenter_CmdLine::ParseCmdLine( int    argc,
                                           char** argv )
{
    /* extract path to binary, used when in --build-check mode
       dont't use extract_path() we need the final slash
     */
    std::string            binary( argv[ 0 ] );
    std::string::size_type last_slash = binary.find_last_of( "/" );
    if ( last_slash != std::string::npos )
    {
        m_path_to_binary = binary.substr( 0, last_slash + 1 );
    }

    scorep_parse_mode_t mode = scorep_parse_mode_param;
    std::string         next = "";

    for ( int i = 1; i < argc; i++ )
    {
        next = ( i + 1 < argc ? argv[ i + 1 ] : "" );
        switch ( mode )
        {
            case scorep_parse_mode_param:
                mode = parse_parameter( argv[ i ] );
                break;
            case scorep_parse_mode_command:
                mode = parse_command( argv[ i ], next );
                break;
            case scorep_parse_mode_option_part:
                /* Skip this, it was already processed */
                mode = scorep_parse_mode_command;
                break;
            case scorep_parse_mode_wrapper_option:
                mode = parse_wrapper_option( argv[ i ], next );
        }
    }

    checkParameter();
    if ( m_verbosity >= 2 )
    {
        print_parameter();
    }

#if SCOREP_BACKEND_COMPILER_INTEL && HAVE( PLATFORM_MIC )
    if ( !m_mmic_set )
    {
        std::cerr << "[Score-P] ERROR: Could not detect '-mmic' flag\n"
                  << "                 Host compilation not supported by this installation" << std::endl;
        exit( EXIT_FAILURE );
    }
#endif /* SCOREP_BACKEND_COMPILER_INTEL */
#if SCOREP_BACKEND_COMPILER_INTEL && !HAVE( PLATFORM_MIC ) && !HAVE( MIC_SUPPORT )
    if ( m_mmic_set )
    {
        std::cerr << "[Score-P] ERROR: MIC compilation not supported by this installation" << std::endl;
        exit( EXIT_FAILURE );
    }
#endif
}

const std::string&
SCOREP_Instrumenter_CmdLine::getPathToBinary( void )
{
    return m_path_to_binary;
}

void
SCOREP_Instrumenter_CmdLine::enableSeparatePreprocessingStep( void )
{
    m_preprocess_mode = EXPLICIT_STEP;
}

std::string
SCOREP_Instrumenter_CmdLine::getPathToSrc( void ) const
{
    char* path_c_str = UTILS_IO_JoinPath( 2, m_path_to_binary.c_str(),
                                          AFS_PACKAGE_SRCDIR );
    const std::string path_to_src = path_c_str;
    free( path_c_str );
    return path_to_src;
}

SCOREP_Instrumenter_CmdLine::scorep_preprocess_mode_t
SCOREP_Instrumenter_CmdLine::getPreprocessMode( void )
{
    return m_preprocess_mode;
}

bool
SCOREP_Instrumenter_CmdLine::isCompiling( void )
{
    return m_is_compiling;
}

bool
SCOREP_Instrumenter_CmdLine::isLinking( void )
{
    return m_is_linking;
}

bool
SCOREP_Instrumenter_CmdLine::doNothing( void )
{
    return m_do_nothing || 0 == m_input_files.size();
}

std::string
SCOREP_Instrumenter_CmdLine::getCompilerName( void )
{
    return m_compiler_name;
}

std::string
SCOREP_Instrumenter_CmdLine::getFlagsBeforeInterpositionLib( void )
{
    return m_flags_before_interposition_lib;
}

std::string
SCOREP_Instrumenter_CmdLine::getFlagsAfterInterpositionLib( void )
{
    return m_flags_after_interposition_lib;
}

std::string
SCOREP_Instrumenter_CmdLine::getIncludeFlags( void )
{
    return m_include_flags;
}

std::string
SCOREP_Instrumenter_CmdLine::getDefineFlags( void )
{
    return m_define_flags;
}

std::string
SCOREP_Instrumenter_CmdLine::getOutputName( void )
{
    return m_output_name;
}

std::vector<std::string>*
SCOREP_Instrumenter_CmdLine::getInputFiles( void )
{
    return &m_input_files;
}

int
SCOREP_Instrumenter_CmdLine::getInputFileNumber( void )
{
    return m_input_files.size();
}

bool
SCOREP_Instrumenter_CmdLine::isInterpositionLibSet( void )
{
    return m_interposition_lib_set;
}

#if SCOREP_BACKEND_COMPILER_INTEL
bool
SCOREP_Instrumenter_CmdLine::isMmicSet( void )
{
    return m_mmic_set;
}
#endif  /* SCOREP_BACKEND_COMPILER_INTEL */

bool
SCOREP_Instrumenter_CmdLine::isDryRun( void )
{
    return m_is_dry_run;
}

bool
SCOREP_Instrumenter_CmdLine::hasKeepFiles( void )
{
    return m_keep_files;
}

int
SCOREP_Instrumenter_CmdLine::getVerbosity( void )
{
    return m_verbosity;
}

bool
SCOREP_Instrumenter_CmdLine::isBuildCheck( void )
{
    return m_is_build_check;
}

bool
SCOREP_Instrumenter_CmdLine::enforceStaticLinking( void )
{
    return m_link_static == enabled;
}

bool
SCOREP_Instrumenter_CmdLine::enforceDynamicLinking( void )
{
    return m_link_static == disabled;
}

bool
SCOREP_Instrumenter_CmdLine::isTargetSharedLib( void )
{
    return m_target_is_shared_lib;
}

std::string
SCOREP_Instrumenter_CmdLine::getLibraryFiles( bool allow_dynamic,
                                              bool allow_static )
{
    std::string lib_files = "";

    for ( std::vector<std::string>::iterator current_lib = m_libraries.begin();
          current_lib != m_libraries.end();
          current_lib++ )
    {
        lib_files += " " + backslash_special_chars( find_library( *current_lib, m_libdirs, allow_static, allow_dynamic, true ) );
    }
    return lib_files;
}

const std::vector<std::string>&
SCOREP_Instrumenter_CmdLine::getInstrumentFilterFiles( void ) const
{
    return m_filters;
}

#if defined( SCOREP_SHARED_BUILD )
bool
SCOREP_Instrumenter_CmdLine::getNoAsNeeded( void )
{
    return m_no_as_needed;
}
#endif

SCOREP_Instrumenter_InstallData*
SCOREP_Instrumenter_CmdLine::getInstallData()
{
    return &m_install_data;
}

void
SCOREP_Instrumenter_CmdLine::removeUserArg( const std::string& arg )
{
    // Remove all instances before the interposition lib
    m_flags_before_interposition_lib =
        remove_string_from_list( m_flags_before_interposition_lib, arg, ' ' );

    //Remove all instances after the interposition lib
    m_flags_after_interposition_lib =
        remove_string_from_list( m_flags_after_interposition_lib, arg, ' ' );
}

/* ****************************************************************************
   private methods
******************************************************************************/

void
SCOREP_Instrumenter_CmdLine::print_parameter( void )
{
    std::cerr << "[Score-P]        Enabled instrumentation:";
    SCOREP_Instrumenter_Adapter::printEnabledAdapterList();
    std::cerr << "\n";

    std::cerr << "[Score-P]             Selected paradigms:";
    SCOREP_Instrumenter_Selector::printSelectedParadigms();
    std::cerr << "\n";

    std::cerr << "[Score-P]                  Compiler name: " << m_compiler_name << "\n";
    std::cerr << "[Score-P] Flags before interposition lib: " << m_flags_before_interposition_lib << "\n";
    std::cerr << "[Score-P]  Flags after interposition lib: " << m_flags_after_interposition_lib << "\n";
    std::cerr << "[Score-P]                    Output file: " << m_output_name << "\n";
    std::cerr << "[Score-P]                  Input file(s): "
              << scorep_vector_to_string( m_input_files, "", "", ", " )
              << "\n";
}

SCOREP_Instrumenter_CmdLine::scorep_parse_mode_t
SCOREP_Instrumenter_CmdLine::parse_parameter( const std::string& arg )
{
    if ( arg[ 0 ] != '-' )
    {
        /* Assume its the compiler/linker command. Maybe we want to add a
           validity check later */
        m_compiler_name = arg;
        SCOREP_Instrumenter_Adapter::checkAllCompilerName( arg );
        SCOREP_Instrumenter_Selector::checkAllCompilerName( arg );
        return scorep_parse_mode_wrapper_option;
    }

    /* Check for execution parameters */
    else if ( arg == "--dry-run" )
    {
        m_is_dry_run = true;
        return scorep_parse_mode_param;
    }

    else if ( arg == "--build-check" )
    {
        if ( m_path_to_binary == "" )
        {
            std::cerr << "[Score-P] ERROR: Using '--build-check' requires calling scorep not via $PATH" << std::endl;
            exit( EXIT_FAILURE );
        }
        m_is_build_check = true;
        SCOREP_Instrumenter_Adapter::setAllBuildCheck( *this );
        m_install_data.setBuildCheck( *this );
        return scorep_parse_mode_param;
    }

    else if ( arg == "--keep-files" )
    {
        m_keep_files = true;
        return scorep_parse_mode_param;
    }

    else if ( arg == "--help" || arg == "-h" )
    {
        print_help();
        exit( EXIT_SUCCESS );
    }

    else if ( arg == "--disable-preprocessing" )
    {
        m_preprocess_mode = DISABLE;
        return scorep_parse_mode_param;
    }

    else if ( arg.substr( 0, 20 ) == "--instrument-filter=" )
    {
        std::string filter_file_name = arg.substr( 20 );

        if ( filter_file_name == "" )
        {
            std::cerr << "[Score-P] ERROR: No filter file specified" << std::endl;
            exit( EXIT_FAILURE );
        }

        if ( !exists_file( filter_file_name ) )
        {
            std::cerr << "[Score-P] ERROR: Filter file does not exists: "
                      << "'" << filter_file_name << "'" << std::endl;
            exit( EXIT_FAILURE );
        }

        m_filters.push_back( filter_file_name );
        return scorep_parse_mode_param;
    }

    /* Check for instrumentation and paradigms */
    else if ( SCOREP_Instrumenter_Adapter::checkAllOption( arg ) )
    {
        return scorep_parse_mode_param;
    }

    else if ( SCOREP_Instrumenter_Selector::checkAllOption( arg ) )
    {
        return scorep_parse_mode_param;
    }

    /* Link options */
#if defined( SCOREP_STATIC_BUILD ) && defined( SCOREP_SHARED_BUILD )
    else if ( arg == "--static" )
    {
        m_link_static = enabled;
        return scorep_parse_mode_param;
    }
    else if ( arg == "--dynamic" )
    {
        m_link_static = disabled;
        return scorep_parse_mode_param;
    }
#elif defined( SCOREP_STATIC_BUILD )
    else if ( arg == "--static" )
    {
        return scorep_parse_mode_param;
    }
    else if ( arg == "--dynamic" )
    {
        std::cerr << "[Score-P] ERROR: Dynamic linking is not possible\n"
                  << "                 This installation contains no shared Score-P libraries" << std::endl;
        exit( EXIT_FAILURE );
    }
#elif defined( SCOREP_SHARED_BUILD )
    else if ( arg == "--static" )
    {
        std::cerr << "[Score-P] ERROR: Static linking is not possible\n"
                  << "                 This installation contains no static Score-P libraries" << std::endl;
        exit( EXIT_FAILURE );
    }
    else if ( arg == "--dynamic" )
    {
        return scorep_parse_mode_param;
    }
#endif

    /* Misc parameters */
    else if ( arg == "--version" )
    {
        std::cout << PACKAGE_STRING << std::endl;
        exit( EXIT_SUCCESS );
    }
    else if ( ( arg.length() >= 9 ) && ( arg.substr( 0, 9 ) == "--config=" ) )
    {
        if ( arg.length() < 10 )
        {
            std::cerr << "[Score-P] ERROR: No config file specified" << std::endl;
            exit( EXIT_FAILURE );
        }
        std::string config_file = arg.substr( 9 );
        if ( m_install_data.readConfigFile( config_file ) != SCOREP_SUCCESS )
        {
            std::cerr << "[Score-P] ERROR: Failed to read config file "
                      << "'" << config_file << "'" << std::endl;
            exit( EXIT_FAILURE );
        }
        return scorep_parse_mode_param;
    }
    else if ( arg == "--verbose" )
    {
        m_verbosity = 1;
        return scorep_parse_mode_param;
    }
    else if ( ( arg.length() >= 10 ) && ( arg.substr( 0, 10 ) == "--verbose=" ) )
    {
        if ( arg.length() < 11 )
        {
            std::cerr << "[Score-P] ERROR: No verbosity value specified" << std::endl;
            exit( EXIT_FAILURE );
        }
        m_verbosity = atol( arg.substr( 10 ).c_str() );
        if ( m_verbosity < 0 )
        {
            std::cerr << "[Score-P] ERROR: Invalid verbosity value: '" << m_verbosity << "'" << std::endl;
            exit( EXIT_FAILURE );
        }
        return scorep_parse_mode_param;
    }
    else if ( arg == "-v" )
    {
        m_verbosity = 1;
        return scorep_parse_mode_param;
    }
    else if ( arg == "--no-as-needed" )
    {
#if defined( SCOREP_SHARED_BUILD )
        m_no_as_needed = true;
#else
        std::cerr << "[Score-P] WARNING: Option '--no-as-needed' makes no sense for static-only Score-P "
                  << "builds and will be ignored" << std::endl;
#endif
    }
    else
    {
        std::cerr << "[Score-P] ERROR: Unknown parameter: '" << arg << "'" << std::endl;
        exit( EXIT_FAILURE );
    }

    /* Never executed but removes a warning with xl-compilers. */
    return scorep_parse_mode_param;
}

SCOREP_Instrumenter_CmdLine::scorep_parse_mode_t
SCOREP_Instrumenter_CmdLine::parse_wrapper_option( const std::string& current,
                                                   const std::string& next )
{
    if ( SCOREP_Instrumenter_Selector::checkAllWrapperOption( current, next ) )
    {
        m_compiler_name += " " + current;
        return scorep_parse_mode_wrapper_option;
    }
    return parse_command( current, next );
}

SCOREP_Instrumenter_CmdLine::scorep_parse_mode_t
SCOREP_Instrumenter_CmdLine::parse_command( const std::string& current,
                                            const std::string& next )
{
    scorep_parse_mode_t ret_val = scorep_parse_mode_command;
    if ( SCOREP_Instrumenter_Adapter::checkAllCommand( current, next ) )
    {
        ret_val = scorep_parse_mode_option_part;
    }
    if ( SCOREP_Instrumenter_Selector::checkAllCommand( current, next ) )
    {
        ret_val = scorep_parse_mode_option_part;
    }

    /* Detect input files */
    if ( ( current[ 0 ] != '-' ) && is_library( current ) )
    {
        /* extract library name */
        if ( is_interposition_library( get_library_name( current ) ) )
        {
            m_interposition_lib_set = true;
            m_current_flags         = &m_flags_after_interposition_lib;
        }
        add_library( current );
    }
    else if ( ( current[ 0 ] != '-' ) &&
              ( is_source_file( current ) || is_object_file( current ) ) )
    {
        add_input_file( current );
        return scorep_parse_mode_command;
    }
    else if ( ( current[ 0 ] != '-' ) && is_header_file( current ) )
    {
        /* Ignore header files in command */
        return scorep_parse_mode_command;
    }
#if SCOREP_BACKEND_COMPILER_INTEL
    else if ( current == "-mmic" )
    {
        m_mmic_set = true;
    }
#endif  /* SCOREP_BACKEND_COMPILER_INTEL */
    else if ( current == "-c" )
    {
        m_is_linking = false;
        /* Do add -c to the compiler options, even if the instrumenter
           will add a -c during the compile step, anyway.
           However, if we are generating dependencies or do only preprocessing
           a missing -c do harm.
         */
    }
    else if ( m_install_data.isPreprocessFlag( current ) )
    {
        m_preprocess_mode = EXPLICIT_STEP;
        m_is_linking      = false;
        m_is_compiling    = false;
    }
    else if ( current == "-M" ) /* Generate dependencies */
    {
        m_do_nothing      = true;
        m_preprocess_mode = DISABLE;
        m_is_linking      = false;
        m_is_compiling    = false;
    }
    else if ( current == "-l" )
    {
        if ( is_interposition_library( next ) )
        {
            m_interposition_lib_set = true;
            m_current_flags         = &m_flags_after_interposition_lib;
        }
        else if ( next == "c" )
        {
            m_flags_after_interposition_lib += " -lc";
            add_library( "-lc" );
            return scorep_parse_mode_option_part;
        }
        add_library( "-l" + next );
        ret_val = scorep_parse_mode_option_part;
    }
    else if ( current == "-lc" )
    {
        m_flags_after_interposition_lib += " -lc";
        add_library( "-lc" );
        return scorep_parse_mode_command;
    }
    else if ( current == "-L" )
    {
        add_library_path( next );
        ret_val = scorep_parse_mode_option_part;
    }
    else if ( current == "-D" )
    {
        /* The add_define function add the parameter to the parameter list,
           because, the value may need to be quoted and some characters
           baskslashed. Thus, we cannot add the value as it is. */
        add_define( current + next );
        return scorep_parse_mode_option_part;
    }
    else if ( current == "-I" )
    {
        add_include_path( next );
        ret_val = scorep_parse_mode_option_part;
    }
    else if ( current == "-o" )
    {
        /* Do not add the output name to parameter list, because the intermediate
           files may have a different name and having then an -o parameter in
           the parameter list makes trouble. */
        set_output_file( next );
        return scorep_parse_mode_option_part;
    }
    else if ( current == "-MF" )
    {
        ret_val = scorep_parse_mode_option_part;
    }
    else if ( current == "-MT" )
    {
        ret_val = scorep_parse_mode_option_part;
    }
    else if (  m_install_data.isArgForShared( current ) )
    {
#ifndef SCOREP_SHARED_BUILD
        std::cerr << "[Score-P] ERROR: It is not possible to build instrumented shared "
                  << "libraries with a statically build Score-P\n"
                  << "                 You need a Score-P installation that was build with "
                  << "shared libraries (--enable-shared)" << std::endl;
        exit( EXIT_FAILURE );
#endif
        m_target_is_shared_lib = true;
    }

    /* Some stupid compilers have options starting with -o that do not
       specify an output filename */
    else if ( m_install_data.isArgWithO( current ) )
    {
        *m_current_flags += " " + current;
        return scorep_parse_mode_command;
    }

    else if ( current[ 0 ] == '-' )
    {
        /* Check standard parameters */
        if ( current[ 1 ] == 'o' )
        {
            /* Do not add the output name to parameter list, because the intermediate
               files may have a different name and having then an -o parameter in
               the parameter list makes trouble. */
            set_output_file( current.substr( 2 ) );
            return scorep_parse_mode_command;
        }
        else if ( current[ 1 ] == 'I' )
        {
            add_include_path( current.substr( 2 ) );
        }
        else if ( current[ 1 ] == 'D' )
        {
            /* The add_define function add the parameter to the parameter list,
               because, the value may need to be quoted and some characters
               baskslashed. Thus, we cannot add the value as it is. */
            add_define( current );
            return scorep_parse_mode_command;
        }
        else if ( current[ 1 ] == 'L' )
        {
            add_library_path( current.substr( 2 ) );
        }
        else if ( current[ 1 ] == 'l' )
        {
            if ( is_interposition_library( current.substr( 2 ) ) )
            {
                m_interposition_lib_set = true;
                m_current_flags         = &m_flags_after_interposition_lib;
            }
            add_library( current );
        }
    }

    if ( m_install_data.isCompositeArg( current, next ) )
    {
        ret_val = scorep_parse_mode_option_part;
    }

    /* In any case that not yet returned, save the flag */
    *m_current_flags += " " + backslash_special_chars( current );

    /* If we already processed both, save the second, too */
    if ( ret_val == scorep_parse_mode_option_part )
    {
        *m_current_flags += " " + backslash_special_chars( next );
    }

    return ret_val;
}

void
SCOREP_Instrumenter_CmdLine::add_define( std::string arg )
{
    /* we need to escape quotes since they get lost otherwise when calling
       system() */
    std::string::size_type pos = 0;
    while ( ( pos = arg.find( '"', pos ) ) != std::string::npos )
    {
        arg.insert( pos, 1, '\\' );
        pos += 2;
    }

    /* Because enclosing quotes may have disappeared, we must always enclose the
       argument with quotes */
    pos =  arg.find( '=', 0 );
    if ( pos !=  std::string::npos )
    {
        arg.insert( pos + 1, 1, '\"' );
        arg.append( 1, '\"' );
    }

    *m_current_flags += " " + arg;
    m_define_flags   += " " + arg;
}

void
SCOREP_Instrumenter_CmdLine::checkParameter( void )
{
    /* Check dependencies */
    SCOREP_Instrumenter_Selector::checkDependencies();
    SCOREP_Instrumenter_Adapter::checkAllDependencies();

    /* Check default relations */
    SCOREP_Instrumenter_Selector::checkDefaults();
    SCOREP_Instrumenter_Adapter::checkAllDefaults();
    SCOREP_Instrumenter_Adapter::defaultOn( SCOREP_INSTRUMENTER_ADAPTER_COMPILER );

    /* Check whether the selected paradigms are supported */
    SCOREP_Instrumenter_Selector::checkAllSupported();

    /* Print a warning if an instrumentation filter was passed, but no active adapter
     * or paradigm supports the filter */
    if ( m_filters.size() > 0
         && ( !SCOREP_Instrumenter_Selector::supportInstrumentFilters()
              && !SCOREP_Instrumenter_Adapter::supportAnyInstrumentFilters() ) )
    {
        std::cerr << "[Score-P] WARNING: Instrument filter(s) will be ignored" << std::endl;
    }

    /* If this is a dry run, enable printing out commands, if it is not already */
    if ( m_is_dry_run && m_verbosity < 1 )
    {
        m_verbosity = 1;
    }

    if ( m_compiler_name == "" )
    {
        std::cerr << "[Score-P] ERROR: Could not identify compiler name" << std::endl;
        exit( EXIT_FAILURE );
    }

    if ( getInputFileNumber() < 1 )
    {
        std::cerr << "[Score-P] WARNING: Found no input files" << std::endl;
    }
}

void
SCOREP_Instrumenter_CmdLine::add_input_file( const std::string& input_file )
{
    m_input_files.push_back( backslash_special_chars( input_file ) );
}

void
SCOREP_Instrumenter_CmdLine::set_output_file( const std::string& output_file )
{
    m_output_name = backslash_special_chars( output_file );
}

void
SCOREP_Instrumenter_CmdLine::add_include_path( const std::string& include_path )
{
    m_include_flags += " -I" + backslash_special_chars( include_path );
}

void
SCOREP_Instrumenter_CmdLine::add_library_path( const std::string& library_path )
{
    m_libdirs.push_back( library_path );
}

void
SCOREP_Instrumenter_CmdLine::add_library( const std::string& library )
{
    m_libraries.push_back( library );
}

bool
SCOREP_Instrumenter_CmdLine::is_interposition_library( const std::string& libraryName )
{
    return SCOREP_Instrumenter_Adapter::isAllInterpositionLibrary( libraryName )
           || SCOREP_Instrumenter_Selector::isAllInterpositionLibrary( libraryName );
}
