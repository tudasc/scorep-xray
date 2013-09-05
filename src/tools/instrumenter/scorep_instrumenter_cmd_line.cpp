/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 *    RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
 *    Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 *    University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 *    Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 *    Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file       SCOREP_Instrumenter_CommandLine.cpp
 */

#include <config.h>
#include "scorep_instrumenter_cmd_line.hpp"
#include "scorep_instrumenter_adapter.hpp"
#include "scorep_instrumenter_utils.hpp"
#include <scorep_config_tool_mpi.h>
#include <scorep_config_tool_backend.h>

#include <iostream>
#include <stdlib.h>

extern void
print_help( void );

/* ****************************************************************************
   Main interface
******************************************************************************/
SCOREP_Instrumenter_CmdLine::SCOREP_Instrumenter_CmdLine( SCOREP_Instrumenter_InstallData& install_data )
    : m_install_data( install_data )
{
    /* Application types */
    m_is_mpi_application    = detect;
    m_is_openmp_application = detect;
    m_target_is_shared_lib  = false;

    /* Execution modes */
    m_is_compiling    = true; // Opposite recognized if no source files in input
    m_is_linking      = true; // Opposite recognized on existence of -c or -E flag
    m_no_compile_link = false;
    m_link_static     = detect;

    /* Input command elements */
    m_compiler_name     = "";
    m_flags_before_lmpi = "";
    m_flags_after_lmpi  = "";
    m_current_flags     = &m_flags_before_lmpi;
    m_include_flags     = "";
    m_define_flags      = "";
    m_output_name       = "";
    m_input_file_number = 0;
    m_input_files       = "";
    m_libraries         = "";
    m_libdirs           = "";
    m_lmpi_set          = false;

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
        }
    }

    check_parameter();
    if ( m_verbosity >= 2 )
    {
        print_parameter();
    }
}

bool
SCOREP_Instrumenter_CmdLine::isMpiApplication( void )
{
    return m_is_mpi_application == enabled;
}

bool
SCOREP_Instrumenter_CmdLine::isOpenmpApplication( void )
{
    return m_is_openmp_application == enabled;
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
SCOREP_Instrumenter_CmdLine::noCompileLink( void )
{
    return m_no_compile_link;
}

bool
SCOREP_Instrumenter_CmdLine::isNvccCompiler( void )
{
    return m_compiler_name == "nvcc";
}

std::string
SCOREP_Instrumenter_CmdLine::getCompilerName( void )
{
    return m_compiler_name;
}

std::string
SCOREP_Instrumenter_CmdLine::getFlagsBeforeLmpi( void )
{
    return m_flags_before_lmpi;
}

std::string
SCOREP_Instrumenter_CmdLine::getFlagsAfterLmpi( void )
{
    return m_flags_after_lmpi;
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

std::string
SCOREP_Instrumenter_CmdLine::getLibraries( void )
{
    return m_libraries;
}

std::string
SCOREP_Instrumenter_CmdLine::getLibDirs( void )
{
    return m_libdirs;
}

std::string
SCOREP_Instrumenter_CmdLine::getInputFiles( void )
{
    return m_input_files;
}

int
SCOREP_Instrumenter_CmdLine::getInputFileNumber( void )
{
    return m_input_file_number;
}

bool
SCOREP_Instrumenter_CmdLine::isLmpiSet( void )
{
    return m_lmpi_set;
}

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
SCOREP_Instrumenter_CmdLine::getLibraryFiles( void )
{
    std::string libraries   = getLibraries();
    std::string libdirs     = getLibDirs();
    std::string current_lib = "";
    std::string lib_files   = "";
    size_t      old_pos     = 0;
    size_t      cur_pos     = 0;

    while ( ( cur_pos = libraries.find( " ", old_pos ) ) != std::string::npos )
    {
        if ( old_pos < cur_pos ) // Discard a blank
        {
            current_lib = libraries.substr( old_pos, cur_pos - old_pos );
            lib_files  += " " + find_library( current_lib, libdirs, " " );
        }
        // Setup for next file
        old_pos = cur_pos + 1;
    }
    return lib_files;
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

/* ****************************************************************************
   private methods
******************************************************************************/

void
SCOREP_Instrumenter_CmdLine::print_parameter( void )
{
    std::cout << "\nEnabled instrumentation:";
    SCOREP_Instrumenter_Adapter::printEnabledAdapterList();
    std::cout << std::endl;

    std::cout << std::endl << "Linked SCOREP library type: ";
    if ( m_is_openmp_application == enabled )
    {
        if ( m_is_mpi_application == enabled )
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
        if ( m_is_mpi_application == enabled )
        {
            std::cout << "MPI" << std::endl;
        }
        else
        {
            std::cout << "serial" << std::endl;
        }
    }

    std::cout << "\nCompiler name: " << m_compiler_name << std::endl;
    std::cout << "Flags before -lmpi: " << m_flags_before_lmpi << std::endl;
    std::cout << "Flags after -lmpi: " << m_flags_after_lmpi << std::endl;
    std::cout << "Output file: " << m_output_name << std::endl;
    std::cout << "Input file(s): " << m_input_files << std::endl;
}

SCOREP_Instrumenter_CmdLine::scorep_parse_mode_t
SCOREP_Instrumenter_CmdLine::parse_parameter( const std::string& arg )
{
    if ( arg[ 0 ] != '-' )
    {
        /* Assume its the compiler/linker command. Maybe we want to add a
           validity check later */
        m_compiler_name = arg;
        return scorep_parse_mode_command;
    }

    /* Check for execution parameters */
    else if ( arg == "--dry-run" )
    {
        m_is_dry_run = true;
        return scorep_parse_mode_param;
    }

    else if ( arg == "--build-check" )
    {
        m_is_build_check = true;
        SCOREP_Instrumenter_Adapter::setAllBuildCheck();
        m_install_data.setBuildCheck();
        return scorep_parse_mode_param;
    }

    else if ( arg == "--keep-files" )
    {
        m_keep_files = true;
        return scorep_parse_mode_param;
    }

    /* Check for instrumentation methods */
    else if ( SCOREP_Instrumenter_Adapter::checkAllOption( arg ) )
    {
        return scorep_parse_mode_param;
    }

    /* Check for application type settings */
#if HAVE( BACKEND_MPI )
    else if ( arg == "--mpi" )
    {
        m_is_mpi_application = enabled;
        return scorep_parse_mode_param;
    }
#else
    else if ( arg == "--mpi" )
    {
        std::cerr << "ERROR: This Score-P installation does not support MPI.\n"
                  << std::endl;
        exit( EXIT_FAILURE );
    }
#endif
    else if ( arg == "--nompi" )
    {
        m_is_mpi_application = disabled;
        return scorep_parse_mode_param;
    }
    else if ( arg == "--openmp" )
    {
        m_is_openmp_application = enabled;
        return scorep_parse_mode_param;
    }
    else if ( arg == "--noopenmp" )
    {
        m_is_openmp_application = disabled;
        return scorep_parse_mode_param;
    }
    else if ( arg == "--help" || arg == "-h" )
    {
        print_help();
        exit( EXIT_SUCCESS );
    }

    /* Link options */
#if defined( SCOREP_STATIC_BUILD ) && defined( SCOREP_SHARED_BUILD )
    else if ( arg == "--static" )
    {
#if HAVE_LINK_FLAG_BSTATIC
        m_link_static = enabled;
        return scorep_parse_mode_param;
#else   // HAVE_LINK_FLAG_BSTATIC
        std::cerr << "The --static option is not supported with the used compiler."
                  << std::endl;
        exit( EXIT_FAILURE );
#endif  // HAVE_LINK_FLAG_BSTATIC
    }
    else if ( arg == "--dynamic" )
    {
#if HAVE_LINK_FLAG_BDYNAMIC
        m_link_static = disabled;
        return scorep_parse_mode_param;
#else   // HAVE_LINK_FLAG_BDYNAMIC
        std::cerr << "The --dynamic option is not supported with the used compiler."
                  << std::endl;
        exit( EXIT_FAILURE );
#endif  // HAVE_LINK_FLAG_BDYNAMIC
    }
#elif defined( SCOREP_STATIC_BUILD )
    else if ( arg == "--static" )
    {
        return scorep_parse_mode_param;
    }
    else if ( arg == "--dynamic" )
    {
        std::cerr << "Dynamic linking is not possible. This installation contains "
                  << "no shared Score-P libraries."
                  << std::endl;
        exit( EXIT_FAILURE );
    }
#elif defined( SCOREP_SHARED_BUILD )
    else if ( arg == "--static" )
    {
        std::cerr << "Static linking is not possible. This installation contains "
                  << "no static Score-P libraries."
                  << std::endl;
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
    else if ( arg.substr( 0, 8 ) == "--config" )
    {
        std::string config_file = arg.substr( 9, arg.length() - 9 );
        if ( config_file == "" )
        {
            std::cerr << "ERROR: No config file specified." << std::endl;
            exit( EXIT_FAILURE );
        }
        if ( m_install_data.readConfigFile( config_file ) != SCOREP_SUCCESS )
        {
            std::cerr << "ERROR: Failed to read config file." << std::endl;
            exit( EXIT_FAILURE );
        }
        return scorep_parse_mode_param;
    }
    else if ( arg.substr( 0, 9 ) == "--verbose" )
    {
        if ( arg.length() > 10 )
        {
            m_verbosity = atol( arg.substr( 10, arg.length() - 10 ).c_str() );
        }
        else
        {
            m_verbosity = 1;
        }
        return scorep_parse_mode_param;
    }
    else if ( arg.substr( 0, 2 ) == "-v" )
    {
        m_verbosity = 1;
        return scorep_parse_mode_param;
    }
#if defined( SCOREP_SHARED_BUILD )
    else if ( arg == "--no-as-needed" )
    {
        m_no_as_needed = true;
    }
#endif
    else
    {
        std::cerr << "ERROR: Unknown parameter: "
                  << arg
                  << ". Abort."
                  << std::endl;
        exit( EXIT_FAILURE );
    }

    /* Never executed but removes a warning with xl-compilers. */
    return scorep_parse_mode_param;
}

SCOREP_Instrumenter_CmdLine::scorep_parse_mode_t
SCOREP_Instrumenter_CmdLine::parse_command( const std::string& current,
                                            const std::string& next )
{
    scorep_parse_mode_t ret_val = scorep_parse_mode_command;

    /* Detect input files */
    if ( ( current[ 0 ] != '-' ) && is_library( current ) )
    {
        m_libraries += " " + current;
    }
    else if ( ( current[ 0 ] != '-' ) &&
              ( is_source_file( current ) || is_object_file( current ) ) )
    {
        m_input_files += " " + current;
        m_input_file_number++;
        return scorep_parse_mode_command;
    }

    else if ( current.substr( 0, 5 ) == "-lmpi" )
    {
        m_lmpi_set      = true;
        m_current_flags = &m_flags_after_lmpi;

        /* is_mpi_application can only be disabled, if --nompi was specified.
           In this case do not enable mpi wrappers.
         */
        if ( m_is_mpi_application != disabled )
        {
#if HAVE( BACKEND_MPI )
            m_is_mpi_application = enabled;
#else
            std::cerr << "ERROR: This installation does not support MPI." << std::endl;
#endif
        }
        m_libraries += " " + current;
    }
    else if ( current == "-c" )
    {
        m_is_linking = false;
        /* Do not add -c to the compiler options, because the instrumenter
           will add a -c during the compile step, anyway. */
        return scorep_parse_mode_command;
    }
    else if ( m_install_data.isPreprocessFlag( current ) )
    {
        m_no_compile_link = true;
        m_is_linking      = false;
        m_is_compiling    = false;
    }
    else if ( current == "-M" ) /* Generate dependencies */
    {
        m_no_compile_link = true;
        m_is_linking      = false;
        m_is_compiling    = false;
    }
    else if ( current == "-l" )
    {
        if ( ( next == "mpi" ) || ( next == "mpich.rts" ) )
        {
            m_lmpi_set      = true;
            m_current_flags = &m_flags_after_lmpi;
            /* is_mpi_application can only be disabled, if --nompi was specified.
               In this case do not enable mpi wrappers.
             */
            if ( m_is_mpi_application != disabled )
            {
                m_is_mpi_application = enabled;
            }
        }
        m_libraries += " -l" + next;
        ret_val      = scorep_parse_mode_option_part;
    }
    else if ( current == "-L" )
    {
        m_libdirs += " " + next;
        ret_val    = scorep_parse_mode_option_part;
    }
    else if ( current == "-D" )
    {
        /* The add_define function add the parameter to the parameter list,
           because, the value may need to be quoted and some characters
           baskslashed. Thus, we can not add the value as it is. */
        add_define( current + next );
        return scorep_parse_mode_option_part;
    }
    else if ( current == "-I" )
    {
        m_include_flags += " -I" + current;
        ret_val          = scorep_parse_mode_option_part;
    }
    else if ( current == "-o" )
    {
        /* Do not add the output name to parameter list, because the intermediate
           files may have a different name and having then an -o paramter in
           the parameter list makes trouble. */
        m_output_name = next;
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
        m_target_is_shared_lib = true;
    }
    else if ( m_install_data.isArgForOpenmp( current ) )
    {
        if ( m_is_openmp_application == detect )
        {
            m_is_openmp_application = enabled;
        }
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
               files may have a different name and having then an -o paramter in
               the parameter list makes trouble. */
            m_output_name = current.substr( 2, std::string::npos );
            return scorep_parse_mode_command;
        }
        else if ( current[ 1 ] == 'I' )
        {
            m_include_flags += " " + current;
        }
        else if ( current[ 1 ] == 'D' )
        {
            /* The add_define function add the parameter to the parameter list,
               because, the value may need to be quoted and some characters
               baskslashed. Thus, we can not add the value as it is. */
            add_define( current );
            return scorep_parse_mode_command;
        }
        else if ( current[ 1 ] == 'L' )
        {
            m_libdirs += " " + current.substr( 2, std::string::npos );
        }
        else if ( current[ 1 ] == 'l' )
        {
            if ( ( current == "-lmpi" ) || ( current == "-lmpich.rts" ) )
            {
                m_lmpi_set      = true;
                m_current_flags = &m_flags_after_lmpi;
                /* is_mpi_application can only be disabled, if --nompi was specified.
                   In this case do not enable mpi wrappers.
                 */
                if ( m_is_mpi_application != disabled )
                {
#if HAVE( BACKEND_MPI )
                    m_is_mpi_application = enabled;
#else
                    std::cerr << "ERROR: This installation does not support MPI."
                              << std::endl;
#endif
                }
            }
            m_libraries += " " + current;
        }
    }
    else if ( SCOREP_Instrumenter_Adapter::checkAllCommand( current, next ) )
    {
        ret_val = scorep_parse_mode_option_part;
    }

    /* In any case that not yet returned, save the flag */
    *m_current_flags += " " + current;

    /* If we already processed both, save the second, too */
    if ( ret_val == scorep_parse_mode_option_part )
    {
        *m_current_flags += " " + next;
    }

    return ret_val;
}

void
SCOREP_Instrumenter_CmdLine::add_define( std::string arg )
{
    /* we need to escape quotes since they get lost otherwise when calling
       system() */
    size_t pos = 0;
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

SCOREP_Instrumenter_CmdLine::scorep_parse_mode_t
SCOREP_Instrumenter_CmdLine::parse_output( const std::string& arg )
{
    m_output_name = arg;
    return scorep_parse_mode_command;
}

void
SCOREP_Instrumenter_CmdLine::check_parameter( void )
{
    SCOREP_Instrumenter_Adapter::checkAllDependencies();

    /* If is_mpi_application not manually specified, try a guess from the
       compiler name */
    if ( m_is_mpi_application == detect )
    {
        if ( m_compiler_name.substr( 0, 2 ) == "mp" )
        {
#if HAVE( BACKEND_MPI )
            m_is_mpi_application = enabled;
#else
            std::cerr << "ERROR: This installation does not support MPI."
                      << std::endl;
#endif
        }
        else
        {
            m_is_mpi_application = disabled;
        }
    }

    /* If openmp is not manuelly specified and no openmp flags found, it is
       probably not an openmp application. */
    if ( m_is_openmp_application == detect )
    {
        m_is_openmp_application = disabled;
    }

    /* Set mpi and opari instrumenatation if not done manually by the user */
    if ( m_is_openmp_application == enabled )
    {
        SCOREP_Instrumenter_Adapter::defaultOn( SCOREP_INSTRUMENTER_ADAPTER_OPARI );
    }

    SCOREP_Instrumenter_Adapter* adapter;
    adapter = SCOREP_Instrumenter_Adapter::getAdapter( SCOREP_INSTRUMENTER_ADAPTER_OPARI );
    if ( !adapter->isEnabled() && m_is_openmp_application == enabled )
    {
        std::cerr << "\n"
                  << "WARNING: You disabled OPARI2 instrumentation for an OpenMP\n"
                  << "         enabled application. The application will crash at runtime\n"
                  << "         if any event occurs inside a parallel region.\n"
                  << std::endl;
    }

    SCOREP_Instrumenter_Adapter::defaultOn( SCOREP_INSTRUMENTER_ADAPTER_CUDA );

    /* Default compiler adapter on */
    SCOREP_Instrumenter_Adapter::defaultOn( SCOREP_INSTRUMENTER_ADAPTER_COMPILER );

    /* Check default relations */
    SCOREP_Instrumenter_Adapter::checkAllDefaults();


    /* If this is a dry run, enable printing out commands, if it is not already */
    if ( m_is_dry_run && m_verbosity < 1 )
    {
        m_verbosity = 1;
    }

    if ( m_compiler_name == "" )
    {
        std::cerr << "ERROR: Could not identify compiler name." << std::endl;
        exit( EXIT_FAILURE );
    }

    if ( m_input_files == "" || m_input_file_number < 1 )
    {
        std::cerr << "WARNING: Found no input files." << std::endl;
    }

    /* If we want to instrument mpi applications with PDT we need to pass the
       include path to mpi.h to PDT. Thus, we can onyl support this compbination
       if we have this information. */
    adapter = SCOREP_Instrumenter_Adapter::getAdapter( SCOREP_INSTRUMENTER_ADAPTER_PDT );
    if ( ( adapter->isEnabled() ) &&
         ( m_is_mpi_application == enabled ) &&
         !SCOREP_HAVE_PDT_MPI_INSTRUMENTATION )
    {
        std::cerr << "Error: Your installation does not support PDT instrumentation for "
                  << "MPI applications." << std::endl;
        exit( EXIT_FAILURE );
    }
}
