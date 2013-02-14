/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
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
 * @file       SCOREP_Instrumenter_CommandLine.cpp
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 */

#include <config.h>
#include <scorep_instrumenter_cmd_line.hpp>
#include <scorep_instrumenter_utils.hpp>
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
    /* Instrumentation methods */
    m_compiler_instrumentation = detect;
    m_opari_instrumentation    = detect;
    m_pomp_instrumentation     = detect;
    m_user_instrumentation     = detect;
    m_mpi_instrumentation      = detect;
    m_pdt_instrumentation      = disabled;
    m_cobi_instrumentation     = disabled;

    /* Appplication types */
    m_is_mpi_application    = detect;
    m_is_openmp_application = detect;
    m_is_cuda_application   = detect;
    m_target_is_shared_lib  = false;

    /* Execution modes */
    m_is_compiling = true; // Opposite recognized if no source files in input
    m_is_linking   = true; // Opposite recognized on existence of -c flag
    m_link_static  = detect;

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
    m_pdt_params        = "";

    /* Instrumenter flags */
    m_is_dry_run     = false;
    m_keep_files     = false;
    m_verbosity      = 0;
    m_is_build_check = false;
}

SCOREP_Instrumenter_CmdLine::~SCOREP_Instrumenter_CmdLine()
{
}

void
SCOREP_Instrumenter_CmdLine::ParseCmdLine( int    argc,
                                           char** argv )
{
    scorep_parse_mode_t mode = scorep_parse_mode_param;

    for ( int i = 1; i < argc; i++ )
    {
        switch ( mode )
        {
            case scorep_parse_mode_param:
                mode = parse_parameter( argv[ i ] );
                break;
            case scorep_parse_mode_command:
                mode = parse_command( argv[ i ] );
                break;
            case scorep_parse_mode_option_part:
                mode = parse_option_part( argv[ i ] );
                break;
            case scorep_parse_mode_output:
                mode = parse_output( argv[ i ] );
                break;
            case scorep_parse_mode_library:
                mode = parse_library( argv[ i ] );
                break;
            case scorep_parse_mode_define:
                mode = parse_define( argv[ i ] );
                break;
            case scorep_parse_mode_incdir:
                mode = parse_incdir( argv[ i ] );
                break;
            case scorep_parse_mode_libdir:
                mode = parse_libdir( argv[ i ] );
                break;
            case scorep_parse_mode_fortran_form:
                mode = parse_fortran_form( argv[ i ] );
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
SCOREP_Instrumenter_CmdLine::isCompilerInstrumenting( void )
{
    return m_compiler_instrumentation == enabled;
}

bool
SCOREP_Instrumenter_CmdLine::isOpariInstrumenting( void )
{
    return m_opari_instrumentation == enabled;
}

bool
SCOREP_Instrumenter_CmdLine::isUserInstrumenting( void )
{
    return m_user_instrumentation == enabled;
}

bool
SCOREP_Instrumenter_CmdLine::isMpiInstrumenting( void )
{
    return m_mpi_instrumentation == enabled;
}

bool
SCOREP_Instrumenter_CmdLine::isPdtInstrumenting( void )
{
    return m_pdt_instrumentation == enabled;
}

bool
SCOREP_Instrumenter_CmdLine::isCobiInstrumenting( void )
{
    return m_cobi_instrumentation == enabled;
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
SCOREP_Instrumenter_CmdLine::isCudaApplication( void )
{
    return m_is_cuda_application == enabled;
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

std::string
SCOREP_Instrumenter_CmdLine::getPdtParams( void )
{
    return m_pdt_params;
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

/* ****************************************************************************
   private methods
******************************************************************************/

void
SCOREP_Instrumenter_CmdLine::print_parameter( void )
{
    std::cout << "\nEnabled instrumentation:";
    if ( m_compiler_instrumentation == enabled )
    {
        std::cout << " compiler";
    }
    if ( m_opari_instrumentation == enabled )
    {
        std::cout << " opari";
    }
    if ( m_user_instrumentation == enabled )
    {
        std::cout << " user";
    }
    if ( m_mpi_instrumentation == enabled )
    {
        std::cout << " mpi";
    }
    if ( m_pdt_instrumentation == enabled )
    {
        std::cout << " pdt";
    }
    if ( m_cobi_instrumentation == enabled )
    {
        std::cout << " cobi";
    }

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
SCOREP_Instrumenter_CmdLine::parse_parameter( std::string arg )
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
        m_install_data.setBuildCheck();
        return scorep_parse_mode_param;
    }

    else if ( arg == "--keep-files" )
    {
        m_keep_files = true;
        return scorep_parse_mode_param;
    }

    /* Check for instrumenatation settings */
    else if ( arg == "--compiler" )
    {
        m_compiler_instrumentation = enabled;
        return scorep_parse_mode_param;
    }
    else if ( arg == "--nocompiler" )
    {
        m_compiler_instrumentation = disabled;
        return scorep_parse_mode_param;
    }
    else if ( arg.substr( 0, 7 ) == "--opari" )
    {
        m_opari_instrumentation = enabled;
        m_install_data.setOpariParams( get_tool_params( arg, 7 ) );
        return scorep_parse_mode_param;
    }
    else if ( arg == "--noopari" )
    {
        m_opari_instrumentation = disabled;
        return scorep_parse_mode_param;
    }
    else if ( arg == "--pomp" )
    {
        m_pomp_instrumentation = enabled;
        return scorep_parse_mode_param;
    }
    else if ( arg == "--nopomp" )
    {
        m_pomp_instrumentation = disabled;
        return scorep_parse_mode_param;
    }
    else if ( arg == "--user" )
    {
        m_user_instrumentation = enabled;
        return scorep_parse_mode_param;
    }
    else if ( arg == "--nouser" )
    {
        m_user_instrumentation = disabled;
        return scorep_parse_mode_param;
    }
    else if ( arg == "--mpi" )
    {
        m_mpi_instrumentation = enabled;
        m_is_mpi_application  = enabled;
        return scorep_parse_mode_param;
    }
    else if ( arg == "--nompi" )
    {
        m_is_mpi_application  = disabled;
        m_mpi_instrumentation = disabled;
        return scorep_parse_mode_param;
    }
    else if ( arg == "--cuda" )
    {
#if HAVE_BACKEND( CUDA )
        m_is_cuda_application = enabled;
        return scorep_parse_mode_param;
#else
        std::cerr << "ERROR: Cuda is not supported by this installation."
                  << std::endl;
        exit( EXIT_FAILURE );
#endif
    }
    else if ( arg == "--nocuda" )
    {
        m_is_cuda_application = disabled;
        return scorep_parse_mode_param;
    }
    else if ( arg.substr( 0, 5 ) == "--pdt" )
    {
#ifdef HAVE_PDT
        m_pdt_instrumentation = enabled;
        m_pdt_params          = get_tool_params( arg, 5 );
        return scorep_parse_mode_param;
#else
        std::cerr << "ERROR: PDT instrumentation is not supported by this installation."
                  << std::endl;
        exit( EXIT_FAILURE );
#endif
    }
    else if ( arg == "--nopdt" )
    {
        m_pdt_instrumentation = disabled;
        return scorep_parse_mode_param;
    }
    else if ( arg.substr( 0, 6 ) == "--cobi" )
    {
#if HAVE( COBI )
        m_cobi_instrumentation = enabled;
        m_install_data.setCobiParams( get_tool_params( arg, 6 ) );
        return scorep_parse_mode_param;
#else
        std::cerr << "ERROR: Binary instrumentation with Cobi is not supported\n"
                  << "       by this installation."
                  << std::endl;
        exit( EXIT_FAILURE );
#endif
    }
    else if ( arg == "--nocobi" )
    {
        m_cobi_instrumentation = disabled;
        return scorep_parse_mode_param;
    }

    /* Check for application type settings */
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
SCOREP_Instrumenter_CmdLine::parse_command( std::string arg )
{
    if ( ( arg[ 0 ] != '-' ) && is_library( arg ) )
    {
        m_libraries += " " + arg;
    }
    else if ( ( arg[ 0 ] != '-' ) &&
              ( is_source_file( arg ) || is_object_file( arg ) ) )
    {
        m_input_files += " " + arg;
        m_input_file_number++;
        if ( is_cuda_file( arg ) )
        {
            m_is_cuda_application = enabled;
        }
        return scorep_parse_mode_command;
    }
    else if ( arg.substr( 0, 5 ) == "-lmpi" )
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
        m_libraries += arg;
    }
    else if ( arg == "-c" )
    {
        m_is_linking = false;
        /* Do not add -c to the compiler options, because the instrumenter
           will add a -c during the compile step, anyway. */
        return scorep_parse_mode_command;
    }
    else if ( arg == "-l" )
    {
        return scorep_parse_mode_library;
    }
    else if ( arg == "-L" )
    {
        return scorep_parse_mode_libdir;
    }
    else if ( arg == "-D" )
    {
        return scorep_parse_mode_define;
    }
    else if ( arg == "-I" )
    {
        return scorep_parse_mode_incdir;
    }
    else if ( arg == "-o" )
    {
        return scorep_parse_mode_output;
    }
    else if ( arg == "-MF" )
    {
        *m_current_flags += " " + arg;
        return scorep_parse_mode_option_part;
    }
    else if ( arg == "-MT" )
    {
        *m_current_flags += " " + arg;
        return scorep_parse_mode_option_part;
    }
    else if (  m_install_data.isArgForShared( arg ) )
    {
        m_target_is_shared_lib = true;
    }
    else if ( m_install_data.isArgForOpenmp( arg ) )
    {
        if ( m_is_openmp_application == detect )
        {
            m_is_openmp_application = enabled;
        }
        if ( m_opari_instrumentation == detect )
        {
            m_opari_instrumentation = enabled;
        }
    }

    /* Check whether free form or fixed form is explicitly enabled. */
    else if ( m_install_data.isArgForFreeform( arg ) )
    {
        m_install_data.setOpariFortranForm( true );
    }
    else if (  m_install_data.isArgForFixedform( arg ) )
    {
        m_install_data.setOpariFortranForm( false );
    }
    else if ( arg == "-f" )
    {
        *m_current_flags += " " + arg;
        return scorep_parse_mode_fortran_form;
    }

    /* Check standard parameters */
    else if ( arg[ 1 ] == 'o' )
    {
        m_output_name = arg.substr( 2, std::string::npos );
    }
    else if ( arg[ 1 ] == 'I' )
    {
        m_include_flags += " " + arg;
    }
    else if ( arg[ 1 ] == 'D' )
    {
        add_define( arg );
        return scorep_parse_mode_command;
    }
    else if ( arg[ 1 ] == 'L' )
    {
        m_libdirs += " " + arg.substr( 2, std::string::npos );
    }
    else if ( arg[ 1 ] == 'l' )
    {
        m_libraries += " " + arg;
    }

    /* In any case that not yet returned, save the flag */
    *m_current_flags += " " + arg;
    return scorep_parse_mode_command;
}

SCOREP_Instrumenter_CmdLine::scorep_parse_mode_t
SCOREP_Instrumenter_CmdLine::parse_option_part( std::string arg )
{
    *m_current_flags += " " + arg;
    return scorep_parse_mode_command;
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

    /* Because enclosing quotes may disappear, we must always enclose the
       argument of with quotes */
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
SCOREP_Instrumenter_CmdLine::parse_output( std::string arg )
{
    m_output_name = arg;
    return scorep_parse_mode_command;
}

void
SCOREP_Instrumenter_CmdLine::check_parameter( void )
{
    /* If is_mpi_application not manually specified, try a guess from the
       compiler name */
    if ( m_is_mpi_application == detect )
    {
        if ( m_compiler_name.substr( 0, 2 ) == "mp" )
        {
            m_is_mpi_application = enabled;
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
    if ( m_opari_instrumentation == detect )
    {
        m_opari_instrumentation = m_is_openmp_application;
    }
    if ( m_opari_instrumentation == disabled &&
         m_is_openmp_application == enabled )
    {
        std::cerr << "\n"
                  << "WARNING: You disabled OPARI2 instrumentation for an OpenMP\n"
                  << "         enabled application. The application will crash at runtime\n"
                  << "         if any event occurs inside a parallel region.\n"
                  << std::endl;
    }

    if ( m_mpi_instrumentation == detect )
    {
        m_mpi_instrumentation = m_is_mpi_application;
    }

    /* Check interference between POMP and Opari instrumentation */
    if ( m_opari_instrumentation == enabled &&
         m_pomp_instrumentation == disabled )
    {
        m_install_data.setOpariParams( "--disable=region" );
    }

    if ( m_opari_instrumentation == disabled &&
         m_pomp_instrumentation == enabled )
    {
        m_opari_instrumentation = enabled;
        m_install_data.setOpariParams( "--disable=omp" );
    }

    /* Check pdt dependencies */
    if ( m_pdt_instrumentation == enabled )
    {
        if ( m_user_instrumentation == disabled )
        {
            std::cerr << "ERROR: You must not combine --pdt with --nouser.\n"
                      << "       PDT instrumentation inserts user instrumentation macros\n"
                      << "       into the source code. Thus, it implicitly enables user\n"
                      << "       instrumentation.\n"
                      << std::endl;
            exit( EXIT_FAILURE );
        }
        m_user_instrumentation = enabled;      // Needed to activate the inserted macros.
    }

    /* Evaluate the default user instrumentation */
    if ( m_user_instrumentation == detect )
    {
        m_user_instrumentation = disabled;
    }

    /* Evaluate the default compiler instrumentation. By default use compiler
       instrumentation if no other generic functions instrumentation was
       selected by the user. */
    if ( m_compiler_instrumentation == detect )
    {
        if ( m_cobi_instrumentation == enabled ||
             m_pdt_instrumentation == enabled )
        {
            m_compiler_instrumentation = disabled;
        }
        else
        {
            m_compiler_instrumentation = enabled;
        }
    }

    /* Evaluate whether we have a cuda application */
    if ( m_is_cuda_application == detect )
    {
        if ( m_compiler_name.substr( 0, 2 ) == "nv" )
        {
            m_is_cuda_application = enabled;
        }
        else
        {
            m_is_cuda_application = disabled;
        }
    }

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
    /*
       if ( output_name != "" && !is_linking && input_file_number > 1 )
       {
        std::cerr << "ERROR: Can not specify -o with multiple files if only"
                  << " compiling or preprocessing." << std::endl;
       }
     */
    if ( m_input_files == "" || m_input_file_number < 1 )
    {
        std::cerr << "WARNING: Found no input files." << std::endl;
    }

    /* If we want to instrument mpi applications with PDT we need to pass the
       include path to mpi.h to PDT. Thus, we can onyl support this compbination
       if we have this information. */
    if ( ( m_pdt_instrumentation == enabled ) &&
         ( m_is_mpi_application == enabled ) &&
         !SCOREP_HAVE_PDT_MPI_INSTRUMENTATION )
    {
        std::cerr << "Error: Your installation does not support PDT instrumentation for "
                  << "MPI applications." << std::endl;
        exit( EXIT_FAILURE );
    }
}

SCOREP_Instrumenter_CmdLine::scorep_parse_mode_t
SCOREP_Instrumenter_CmdLine::parse_library( std::string arg )
{
    m_libraries += " -l" + arg;
    if ( arg.substr( 0, 3 )  == "mpi" )
    {
        m_lmpi_set = true;
        /* is_mpi_application can only be disabled, if --nompi was specified.
           In this case, do not enable mpi wrappers.
         */
        if ( m_is_mpi_application != disabled )
        {
            m_is_mpi_application = enabled;
        }
    }

    *m_current_flags += " -l" + arg;
    return scorep_parse_mode_command;
}

SCOREP_Instrumenter_CmdLine::scorep_parse_mode_t
SCOREP_Instrumenter_CmdLine::parse_define( std::string arg )
{
    add_define( "-D" + arg );
    return scorep_parse_mode_command;
}

SCOREP_Instrumenter_CmdLine::scorep_parse_mode_t
SCOREP_Instrumenter_CmdLine::parse_incdir( std::string arg )
{
    m_include_flags  += " -I" + arg;
    *m_current_flags += " -I" + arg;
    return scorep_parse_mode_command;
}

SCOREP_Instrumenter_CmdLine::scorep_parse_mode_t
SCOREP_Instrumenter_CmdLine::parse_libdir( std::string arg )
{
    *m_current_flags += " -L" + arg;
    m_libdirs        += " " + arg;
    return scorep_parse_mode_command;
}

std::string
SCOREP_Instrumenter_CmdLine::get_tool_params( std::string arg, size_t pos )
{
    if ( arg.length() <= pos + 1 )
    {
        return "";
    }

    if ( arg[ pos ] !=  '=' )
    {
        std::cerr << "ERROR: Unknown paramter: " << arg << std::endl;
        std::cerr << "You may specify " << arg.substr( 0, pos )
                  << " or " << arg.substr( 0, pos ) << "=\"<parameter-list>\"" << std::endl;
        exit( EXIT_FAILURE );
    }
    return arg.substr( pos + 1, std::string::npos );
}

SCOREP_Instrumenter_CmdLine::scorep_parse_mode_t
SCOREP_Instrumenter_CmdLine::parse_fortran_form( std::string arg )
{
    if ( m_install_data.isArgForFreeform( "-f" + arg ) )
    {
        m_install_data.setOpariFortranForm( true );
    }
    else if ( m_install_data.isArgForFixedform( "-f" + arg ) )
    {
        m_install_data.setOpariFortranForm( false );
    }
    else
    {
        return parse_command( arg );
    }
    *m_current_flags += " " + arg;
    return scorep_parse_mode_command;
}
