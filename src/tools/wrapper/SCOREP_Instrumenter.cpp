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
 * @file       SCOREP_Instrumenter.cpp
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 */

#include <config.h>
#include <algorithm>
#include <iostream>
#include <string>
#include <fstream>
#include <istream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <scorep_utility/SCOREP_Utils.h>
#include <scorep_utility/SCOREP_Error.h>

#include "SCOREP_Instrumenter.hpp"
#include "scorep_config_tool_backend.h"


void
print_help();

/* ****************************************************************************
   Compiler specific defines
******************************************************************************/
#ifdef SCOREP_COMPILER_INTEL
#define OPARI_MANGLING_SCHEME "intel"

#elif SCOREP_COMPILER_SUN
#define OPARI_MANGLING_SCHEME "sun"

#elif SCOREP_COMPILER_IBM
#define OPARI_MANGLING_SCHEME "ibm"

#elif SCOREP_COMPILER_PGI
#define OPARI_MANGLING_SCHEME "pgi"

#elif SCOREP_COMPILER_GNU
#define OPARI_MANGLING_SCHEME "gnu"

#elif SCOREP_COMPILER_CRAY
#define OPARI_MANGLING_SCHEME "cray"

#endif

/* ****************************************************************************
   Main interface
******************************************************************************/
SCOREP_Instrumenter::SCOREP_Instrumenter()
{
    /* Initialize values */
    compiler_instrumentation = enabled;
    opari_instrumentation    = detect;
    user_instrumentation     = disabled;
    mpi_instrumentation      = detect;
    pdt_instrumentation      = disabled;
    cobi_instrumentation     = disabled;

    is_mpi_application    = detect;
    is_openmp_application = detect;

    is_compiling = true; // Opposite recognized if no source files in input
    is_linking   = true; // Opposite recognized on existence of -c flag

    scorep_include_path = "";
    scorep_libs         = "";
    pdt_bin_path        = PDT;
    pdt_config_file     = PDT_CONFIG;
    input_file_number   = 0;
    define_flags        = "";
    include_flags       = "";
    temp_files          = "";

    compiler_instrumentation_flags = SCOREP_CFLAGS;
    c_compiler                     = SCOREP_CC;
    openmp_cflags                  = SCOREP_OPENMP_CFLAGS;
    nm                             = "`" OPARI_CONFIG " --nm`";
    awk                            =  "`" OPARI_CONFIG " --awk-cmd`";
    opari                          = OPARI;
    opari_script                   = "`" OPARI_CONFIG " --awk-script`";
    opari_config                   = OPARI_CONFIG;
    grep                           =  "`" OPARI_CONFIG " --egrep`";
    language                       = unknown_language;
    scorep_config                  = "";
    cobi                           = SCOREP_COBI_PATH;
    cobi_config_dir                = COBI_CONFIG_DIR;

    is_dry_run    = false;
    no_final_step = false;
    lmpi_set      = false;
    keep_files    = false;
}

SCOREP_Instrumenter::~SCOREP_Instrumenter ()
{
}

int
SCOREP_Instrumenter::Run()
{
    int ret_val = 0;

    if ( verbosity >= 2 )
    {
        PrintParameter();
    }

    /* First user and compiler instrumentation,  because we want to have
       the user instrumentation effects already present when we compile
       opari instrumented sources in the preparation step. The sources
       are already compiled in the preparation step if compiling and
       linking was done by the same user command. Because we must
       find the initialzation routines, compiling and linking must be splitted
       if Opari or PDT instrumentation is used.
     */
    if ( user_instrumentation == enabled )
    {
        prepare_user();
    }
    if ( compiler_instrumentation == enabled )
    {
        prepare_compiler();
    }

    std::string object_files = "";

    if ( is_compiling )
    {
        /* Because Opari and PDT perform source code modifications, and store the
           modified source file with a different name, we get object files with a
           different name. To avoid this, we need to compile every source file separately
            and explicitly specify the output file name.
         */
        std::string current_file = "";
        std::string object_file  = "";
        size_t      old_pos      = 0;
        size_t      cur_pos      = 0;
        char*       cwd          = SCOREP_IO_GetCwd( NULL, 0 );
        char*       cwd_to_free  = cwd;
        if ( !cwd )
        {
            /* ensure that cwd is non-NULL */
            cwd = ( char* )"";
        }

        /* If the original command compile and link in one step,
           we need to split compilation and linking, because for Opari
           we need to run the script on the object files.
           Furthermore, if the user specifies multiple source files
           in the compile step, we need to compile them separately, because
           the sources are modified and, thus, the object file ends up with
           a different name. In this case we do not execute the last step.
         */
        while ( cur_pos != std::string::npos )
        {
            cur_pos = input_files.find( " ", old_pos );
            if ( old_pos < cur_pos ) // Discard a blank
            {
                current_file = input_files.substr( old_pos, cur_pos - old_pos );
                if ( is_source_file( current_file ) )
                {
                    /* Make sure, it has full path => Some compilers and
                       user instrumentation use the file name given to the compiler.
                       Thus, if we make all file names have full pathes, we get a
                       consistent input. */
                    char* simplified = SCOREP_IO_JoinPath( 2, cwd, current_file.c_str() );
                    if ( simplified )
                    {
                        SCOREP_IO_SimplifyPath( simplified );
                        current_file = simplified;
                        free( simplified );
                    }

                    // Determine object file name
                    if ( ( !is_linking ) && ( output_name != "" ) )
                    {
                        object_file = output_name;
                    }
                    else
                    {
                        object_file = get_basename(
                            SCOREP_IO_GetWithoutPath( current_file.c_str() ) ) + ".o";
                    }

                    // If compiling and linking is performed in one step. The compiler leave no object file.
                    // Thus, we delete the object file, too.
                    if ( is_linking )
                    {
                        temp_files += " " + object_file;
                    }

                    // Perform PDT instrumentaion
                    #ifdef HAVE_PDT
                    if ( pdt_instrumentation == enabled )
                    {
                        current_file = instrument_pdt( current_file );
                    }
                    #endif

                    // Perform Opari instrumentation
                    if ( opari_instrumentation == enabled )
                    {
                        current_file = instrument_opari( current_file );
                    }

                    // Compile instrumented file
                    compile_source_file( current_file, object_file );

                    // Add object file to the input file list for the link command
                    object_files += " " + object_file;
                }
                // If it is no source file, leave the file in the input list
                else
                {
                    object_files += " " + current_file;
                }
            }
            // Setup for next file
            old_pos = cur_pos + 1;
        }
        free( cwd_to_free );

        // Replace sources by compiled by their object file names for the link command
        input_files = object_files;
    }

    if ( is_linking )
    {
        // Perform Opari instrumentation
        if ( opari_instrumentation == enabled )
        {
            prepare_opari_linking();
        }

        ret_val = link_step();

        if ( ret_val != EXIT_SUCCESS )
        {
            fprintf( stderr, "SCOREP: Error while linking\n" );
            return ret_val;
        }
    }

    #if HAVE( COBI )
    if ( cobi_instrumentation == enabled )
    {
        std::string orig_name = output_name + ".orig";
        if ( verbosity >= 1 )
        {
            std::cout << "mv " << output_name << " " << orig_name << std::endl;
        }
        if ( !is_dry_run )
        {
            if ( rename( output_name.c_str(), orig_name.c_str() ) != 0 )
            {
                SCOREP_ERROR_POSIX( "Failed to rename binary" );
                return EXIT_FAILURE;
            }
        }

        return invoke_cobi( orig_name );
    }
    #endif

    clean_temp_files();

    return EXIT_SUCCESS;
}

SCOREP_Error_Code
SCOREP_Instrumenter::ParseCmdLine( int    argc,
                                   char** argv )
{
    scorep_parse_mode_t mode = scorep_parse_mode_param;

    /* We must read the config file when the wrapper tool specific options
       are parsed, before the command options are parsed. Thus we must detect
       the point where the mode has changed to scorep_parse_mode_command.
     */
    bool              is_config_file_read = false;
    SCOREP_Error_Code ret_val;

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
            case scorep_parse_mode_output:
                mode = parse_output( argv[ i ] );
                break;
            case scorep_parse_mode_config:
                mode = parse_config( argv[ i ] );
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
        }

        if ( mode == scorep_parse_mode_command && !is_config_file_read )
        {
            if ( config_file != "" )
            {
                ret_val = ReadConfigFile( argv[ 0 ] );
            }
            is_config_file_read = true;
        }
    }

    check_parameter();
    prepare_config_tool_calls( argv[ 0 ] );

    return ret_val;
}

void
SCOREP_Instrumenter::PrintParameter()
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

    std::cout << std::endl << "Linked SCOREP library type: ";
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
}

/* ****************************************************************************
   Command line parsing
******************************************************************************/
SCOREP_Instrumenter::scorep_parse_mode_t
SCOREP_Instrumenter::parse_parameter( std::string arg )
{
    if ( arg[ 0 ] != '-' )
    {
        /* Assume its the compiler/linker command. Maybe we want to add a
           validity check later */
        compiler_name = arg;
        return scorep_parse_mode_command;
    }

    /* Check for execution parameters */
    else if ( arg == "--dry-run" )
    {
        is_dry_run = true;
        return scorep_parse_mode_param;
    }

    else if ( arg == "--keep-files" )
    {
        keep_files = true;
        return scorep_parse_mode_param;
    }

    /* Check for instrumenatation settings */
    else if ( arg == "--compiler" )
    {
        compiler_instrumentation = enabled;
        return scorep_parse_mode_param;
    }
    else if ( arg == "--nocompiler" )
    {
        compiler_instrumentation = disabled;
        return scorep_parse_mode_param;
    }
    else if ( arg == "--opari" )
    {
        opari_instrumentation = enabled;
        return scorep_parse_mode_param;
    }
    else if ( arg == "--noopari" )
    {
        opari_instrumentation = disabled;
        if ( is_openmp_application == detect )
        {
            is_openmp_application = disabled;
        }
        return scorep_parse_mode_param;
    }
    else if ( arg == "--user" )
    {
        user_instrumentation = enabled;
        return scorep_parse_mode_param;
    }
    else if ( arg == "--nouser" )
    {
        user_instrumentation = disabled;
        return scorep_parse_mode_param;
    }
    else if ( arg == "--mpi" )
    {
        mpi_instrumentation = enabled;
        is_mpi_application  = enabled;
        return scorep_parse_mode_param;
    }
    else if ( arg == "--nompi" )
    {
        is_mpi_application  = disabled;
        mpi_instrumentation = disabled;
        return scorep_parse_mode_param;
    }
#ifdef HAVE_PDT
    else if ( arg == "--pdt" )
    {
        pdt_instrumentation = enabled;
        return scorep_parse_mode_param;
    }
    else if ( arg == "--nopdt" )
    {
        pdt_instrumentation = disabled;
        return scorep_parse_mode_param;
    }
#endif
#if HAVE( COBI )
    else if ( arg == "--cobi" )
    {
        cobi_instrumentation = enabled;
        return scorep_parse_mode_param;
    }
    else if ( arg == "--nocobi" )
    {
        cobi_instrumentation = disabled;
        return scorep_parse_mode_param;
    }
#endif

    /* Check for application type settings */
    else if ( arg == "--openmp_support" )
    {
        is_openmp_application = enabled;
        return scorep_parse_mode_param;
    }
    else if ( arg == "--noopenmp_support" )
    {
        is_openmp_application = disabled;
        return scorep_parse_mode_param;
    }
    else if ( arg == "--help" || arg == "-h" )
    {
        print_help();
        exit( EXIT_SUCCESS );
    }
    /* Misc parameters */
    else if ( arg == "--version" )
    {
        printf( PACKAGE_STRING "\n" );
        exit( EXIT_SUCCESS );
    }
    else if ( CheckForCommonArg( arg ) )
    {
        return scorep_parse_mode_param;
    }
    else
    {
        std::cerr << "ERROR: Unknown parameter: " << arg << ". Abort." << std::endl;
        exit( EXIT_FAILURE );
    }

    /* Never executed but removes a warning with xl-compilers. */
    return scorep_parse_mode_param;
}

SCOREP_Instrumenter::scorep_parse_mode_t
SCOREP_Instrumenter::parse_command( std::string arg )
{
    if ( ( arg[ 0 ] != '-' ) &&
         ( is_source_file( arg ) || is_object_file( arg ) || is_library( arg ) ) )
    {
        /* Assume it is a input file */
        input_files += " " + arg;
        input_file_number++;
        if ( is_c_file( arg ) )
        {
            language = c_language;
        }
        else if ( is_cpp_file( arg ) )
        {
            language = cpp_language;
        }
        else if ( is_fortran_file( arg ) )
        {
            language = fortran_language;
        }
        return scorep_parse_mode_command;
    }
    else if ( arg == "-lmpi" )
    {
        lmpi_set = true;
        /* is_mpi_application can only be disabled, if --nompi was specified. In this case
           do not enable mpi wrappers.
         */
        if ( is_mpi_application != disabled )
        {
            is_mpi_application = enabled;
        }
        /* We must append the -lmpi after our flags, else our mpi wrappers are not
           used. Thus, do not store this flag in the flag list, but return.
         */
        return scorep_parse_mode_command;
    }
    else if ( arg == "-c" )
    {
        is_linking = false;
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
    /* Check for OpenMP flags. The compiler's OpenMP flag is detected during configure
       time. Unfortunately, newer intel compiler versions support the gnu-like
       -fopenmp in addition. In this case the configure test detects -fopenmp as the
       OpenMP flag. Thus, we hardcode support for the standard -openmp flag for intel
       compilers.
     */
#ifdef SCOREP_COMPILER_INTEL
    else if ( ( arg == "-openmp" ) || ( arg == openmp_cflags ) )
#else
    else if ( arg == openmp_cflags )
#endif
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
    else if ( arg[ 1 ] == 'o' )
    {
        output_name = arg.substr( 2, std::string::npos );
    }
    else if ( arg[ 1 ] == 'I' )
    {
        include_flags += " " + arg;
    }
    else if ( arg[ 1 ] == 'D' )
    {
        add_define( arg );
        return scorep_parse_mode_command;
    }

    /* In any case that not yet returned, save the flag */
    compiler_flags += " " + arg;
    return scorep_parse_mode_command;
}

void
SCOREP_Instrumenter::add_define( std::string arg )
{
    // we need to escape quotes since they get lost otherwise when calling system()
    size_t pos = 0;
    while ( ( pos = arg.find( '"', pos ) ) != std::string::npos )
    {
        arg.insert( pos, 1, '\\' );
        pos += 2;
    }

    /* Because enclosing quotes may disappear, we must always enclose the argument of
       with quotes */
    pos =  arg.find( '=', 0 );
    if ( pos !=  std::string::npos )
    {
        arg.insert( pos + 1, 1, '\"' );
        arg.append( 1, '\"' );
    }

    define_flags += " " + arg;
}

SCOREP_Instrumenter::scorep_parse_mode_t
SCOREP_Instrumenter::parse_output( std::string arg )
{
    output_name = arg;
    return scorep_parse_mode_command;
}

void
SCOREP_Instrumenter::check_parameter()
{
    /* If is_mpi_application not manually specified, try a guess from the
       compiler name */
    if ( is_mpi_application == detect )
    {
        if ( compiler_name.substr( 0, 2 ) == "mp" )
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

    /* Check pdt dependencies */
    if ( pdt_instrumentation == enabled )
    {
        user_instrumentation     = enabled;  // Needed to activate the inserted macros.
        compiler_instrumentation = disabled; // Avoid double instrumentation.
    }

    /* Check pdt dependencies */
    if ( cobi_instrumentation == enabled )
    {
        compiler_instrumentation = disabled; // Avoid double instrumentation.
    }

    /* If this is a dry run, enable printing out commands, if it is not already */
    if ( is_dry_run && verbosity < 1 )
    {
        verbosity = 1;
    }

    if ( compiler_name == "" )
    {
        std::cout << "ERROR: Could not identify compiler name." << std::endl;
        abort();
    }
    /*
       if ( output_name != "" && !is_linking && input_file_number > 1 )
       {
        std::cerr << "ERROR: Can not specify -o with multiple files if only"
                  << " compiling or preprocessing." << std::endl;
       }
     */
    if ( input_files == "" || input_file_number < 1 )
    {
        std::cout << "WARNING: Found no input files." << std::endl;
    }
}

SCOREP_Instrumenter::scorep_parse_mode_t
SCOREP_Instrumenter::parse_config( std::string arg )
{
    config_file = arg;
    return scorep_parse_mode_param;
}

SCOREP_Instrumenter::scorep_parse_mode_t
SCOREP_Instrumenter::parse_library( std::string arg )
{
    if ( arg == "mpi" )
    {
        lmpi_set = true;
        /* is_mpi_application can only be disabled, if --nompi was specified. In this case
           do not enable mpi wrappers.
         */
        if ( is_mpi_application != disabled )
        {
            is_mpi_application = enabled;
        }
        /* We must append the -lmpi after our flags, else our mpi wrappers are not
           used. Thus, do not store this flag in the flag list, but return.
         */
    }
    else
    {
        compiler_flags += " -l" + arg;
    }
    return scorep_parse_mode_command;
}

SCOREP_Instrumenter::scorep_parse_mode_t
SCOREP_Instrumenter::parse_define( std::string arg )
{
    add_define( "-D" + arg );
    return scorep_parse_mode_command;
}

SCOREP_Instrumenter::scorep_parse_mode_t
SCOREP_Instrumenter::parse_incdir( std::string arg )
{
    include_flags  += " -I" + arg;
    compiler_flags += " -I" + arg;
    return scorep_parse_mode_command;
}

SCOREP_Instrumenter::scorep_parse_mode_t
SCOREP_Instrumenter::parse_libdir( std::string arg )
{
    compiler_flags += " -L" + arg;
    return scorep_parse_mode_command;
}

/* ****************************************************************************
   Config file parsing
******************************************************************************/

void
SCOREP_Instrumenter::AddIncDir( std::string dir )
{
}

void
SCOREP_Instrumenter::AddLibDir( std::string dir )
{
}

void
SCOREP_Instrumenter::AddLib( std::string lib )
{
}

void
SCOREP_Instrumenter::set_pdt_path( std::string pdt )
{
    if ( pdt == "yes" )
    {
        char* path = SCOREP_GetExecutablePath( "tau_instrumentor" );
        if ( path != NULL )
        {
            pdt_bin_path = path;
            free( path );
        }
        else
        {
            std::cout << "ERROR: Unable to find PDT binaries.\n";
            abort();
        }
    }
    else if ( pdt == "no" )
    {
        return;
    }
    else
    {
        pdt_bin_path = pdt;
    }
}

void
SCOREP_Instrumenter::SetValue( std::string key,
                               std::string value )
{
    if ( key == "EGREP" && value != "" )
    {
        grep = value;
    }

    else if ( key == "OPARI_CONFIG" && value != "" )
    {
        nm           = "`" + value + " --nm`";
        awk          = "`" + value + " --awk-cmd`";
        opari_script = "`" + value + " --awk-script`";
        grep         = "`" + value + " --egrep`";
        opari_config = value;
    }
    else if ( key == "PDT" && value != "" )
    {
        set_pdt_path( value );
    }
    else if ( key == "OPENMP_CFLAGS" && value != "" )
    {
        openmp_cflags = value;
    }
    else if ( key == "OPARI" && value != "" )
    {
        opari = value;
    }
    else if ( key == "CC"  && value != "" )
    {
        c_compiler = value;
    }
    else if ( key == "COMPILER_INSTRUMENTATION_CPPFLAGS" && value != "" )
    {
        compiler_instrumentation_flags = value;
    }
    else if ( key == "PDT_CONFIG" && value != "" )
    {
        pdt_config_file = value;
    }
    else if ( key == "SCOREP_CONFIG" && value != "" )
    {
        scorep_config = value;
    }
    else if ( key == "COBI_CONFIG_DIR" && value != "" )
    {
        cobi_config_dir = value;
    }
}

/* ****************************************************************************
   Helper functions for file name manipulation and analysis
******************************************************************************/

std::string
SCOREP_Instrumenter::get_extension( std::string filename )
{
    int pos = filename.rfind( "." );
    if ( pos == std::string::npos )
    {
        return "";
    }
    return filename.substr( pos );
}

std::string
SCOREP_Instrumenter::get_basename( std::string filename )
{
    int pos = filename.rfind( "." );
    if ( pos == std::string::npos )
    {
        return filename;
    }
    return filename.substr( 0, pos );
}

bool
SCOREP_Instrumenter::is_fortran_file( std::string filename )
{
    std::string extension = get_extension( filename );
    if ( extension == "" )
    {
        return false;
    }
    #define SCOREP_CHECK_EXT( ext ) if ( extension == ext ) return true
    SCOREP_CHECK_EXT( ".f" );
    SCOREP_CHECK_EXT( ".F" );
    SCOREP_CHECK_EXT( ".f90" );
    SCOREP_CHECK_EXT( ".F90" );
    SCOREP_CHECK_EXT( ".fpp" );
    SCOREP_CHECK_EXT( ".FPP" );
    SCOREP_CHECK_EXT( ".For" );
    SCOREP_CHECK_EXT( ".FOR" );
    SCOREP_CHECK_EXT( ".Ftn" );
    SCOREP_CHECK_EXT( ".FTN" );
    SCOREP_CHECK_EXT( ".f95" );
    SCOREP_CHECK_EXT( ".F95" );
    SCOREP_CHECK_EXT( ".f03" );
    SCOREP_CHECK_EXT( ".F03" );
    SCOREP_CHECK_EXT( ".f08" );
    SCOREP_CHECK_EXT( ".F08" );
    #undef SCOREP_CHECK_EXT
    return false;
}

bool
SCOREP_Instrumenter::is_c_file( std::string filename )
{
    std::string extension = get_extension( filename );
    if ( extension == "" )
    {
        return false;
    }
    #define SCOREP_CHECK_EXT( ext ) if ( extension == ext ) return true
    SCOREP_CHECK_EXT( ".c" );
    SCOREP_CHECK_EXT( ".C" );
    #undef SCOREP_CHECK_EXT
    return false;
}

bool
SCOREP_Instrumenter::is_cpp_file( std::string filename )
{
    std::string extension = get_extension( filename );
    if ( extension == "" )
    {
        return false;
    }
    #define SCOREP_CHECK_EXT( ext ) if ( extension == ext ) return true
    SCOREP_CHECK_EXT( ".cpp" );
    SCOREP_CHECK_EXT( ".CPP" );
    SCOREP_CHECK_EXT( ".cxx" );
    SCOREP_CHECK_EXT( ".CXX" );
    SCOREP_CHECK_EXT( ".cc" );
    SCOREP_CHECK_EXT( ".CC" );
    #undef SCOREP_CHECK_EXT
    return false;
}

bool
SCOREP_Instrumenter::is_source_file( std::string filename )
{
    return is_c_file( filename ) ||
           is_cpp_file( filename ) ||
           is_fortran_file( filename );
}

bool
SCOREP_Instrumenter::is_object_file( std::string filename )
{
    std::string extension = get_extension( filename );
    if ( extension == "" )
    {
        return false;
    }
    if ( extension == ".o" )
    {
        return true;
    }
    return false;
}

bool
SCOREP_Instrumenter::is_library( std::string filename )
{
    std::string extension = get_extension( filename );
    if ( extension == "" )
    {
        return false;
    }
    if ( extension == ".a" )
    {
        return true;
    }
    if ( extension == ".so" )
    {
        return true;
    }
    if ( extension.find( ".a." ) != std::string::npos )
    {
        return true;
    }
    if ( extension.find( ".so." ) != std::string::npos )
    {
        return true;
    }
    return false;
}


/* ****************************************************************************
   Cleanup
******************************************************************************/
void
SCOREP_Instrumenter::clean_temp_files()
{
    if ( ( !keep_files ) && ( temp_files != "" ) )
    {
        temp_files = "rm" + temp_files;
        if ( verbosity >= 1 )
        {
            std::cout << temp_files << std::endl;
        }
        if ( !is_dry_run )
        {
            if ( system( temp_files.c_str() ) != 0 )
            {
                if ( verbosity < 1 )
                {
                    std::cout << "Error executing: " << temp_files << std::endl;
                }
                abort();
            }
        }
    }
}


/* ****************************************************************************
   Compilation
******************************************************************************/
void
SCOREP_Instrumenter::prepare_config_tool_calls( std::string arg )
{
    std::string mode = " --seq";
    if ( scorep_config == "" )
    {
        char* path = SCOREP_GetExecutablePath( arg.c_str() );

        // Determine config tool path
        if ( path != NULL )
        {
            scorep_config = path;
            free( path );
        }
        else
        {
            std::cout << "ERROR: Unable to find Score-P config tool.\n";
            abort();
        }
        scorep_config += "/scorep-config";
    }

    // Determine mode parameter
    if ( is_mpi_application == enabled )
    {
        mode = ( is_openmp_application == enabled ? " --hyb" : " --mpi" );
    }
    else if ( is_openmp_application == enabled )
    {
        mode = " --omp";
    }

    // Generate calls
    scorep_include_path = "`" + scorep_config + mode + " --inc` ";
    scorep_libs         = "`" + scorep_config + mode + " --libs` ";
    if ( opari_instrumentation == enabled )
    {
        scorep_include_path += "`" + opari_config + " --cflags` ";
    }

    // Handle manual -lmpi flag
    if ( lmpi_set )
    {
        scorep_libs += "-lmpi ";
    }
}

void
SCOREP_Instrumenter::prepare_compiler()
{
    compiler_flags += " -g " + compiler_instrumentation_flags;

    /* The sun compiler can only instrument Fortran files. Thus, any C/C++ files
       are not instrumented. To avoid user confusion, the instrumneter aborts in
       case a C/C++ file should be compiler instumented.
     */
#ifdef SCOREP_COMPILER_SUN
    if ( is_compiling )
    {
        std::string current_file = "";
        std::string extension    = "";
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
                    abort();
                }
            }
            // Setup for next file
            old_pos = cur_pos + 1;
        }
    }
#endif // SCOREP_COMPILER_SUN
}

void
SCOREP_Instrumenter::prepare_user()
{
    #ifdef SCOREP_COMPILER_IBM
    if ( language == fortran_language )
    {
        define_flags += " -WF,-DSCOREP_USER_ENABLE=1";
        return;
    }
    #endif // SCOREP_COMPILER_IBM

    define_flags += " -DSCOREP_USER_ENABLE=1";
}

void
SCOREP_Instrumenter::invoke_opari( std::string input_file,
                                   std::string output_file )
{
    std::string command = opari + " --tpd "
#ifdef SCOREP_COMPILER_CRAY
                          "--nosrc "
#endif
#ifdef OPARI_MANGLING_SCHEME
                          "--tpd-mangling=" OPARI_MANGLING_SCHEME " "
#endif
                          + input_file
                          + " " + output_file;
    if ( verbosity >= 1 )
    {
        std::cout << command << std::endl;
    }
    if ( !is_dry_run )
    {
        if ( system( command.c_str() ) != 0 )
        {
            if ( verbosity < 1 )
            {
                std::cout << "Error executing: " << command << std::endl;
            }
            abort();
        }
    }
}

void
SCOREP_Instrumenter::invoke_awk_script( std::string object_files,
                                        std::string output_file )
{
    std::string command = nm + " " +  object_files
                          + " | grep -E -i \"T \\.{0,1}_{0,2}pomp2_init_regions\" | "
                          + awk + " -f " + opari_script
                          + " > " + output_file;
    if ( verbosity >= 1 )
    {
        std::cout << command << std::endl;
    }
    if ( !is_dry_run )
    {
        if ( system( command.c_str() ) != 0 )
        {
            if ( verbosity < 1 )
            {
                std::cout << "Error executing: " << command << std::endl;
            }
            abort();
        }
    }

    temp_files += " " + output_file;
}

void
SCOREP_Instrumenter::compile_init_file( std::string input_file,
                                        std::string output_file )
{
    std::string command = c_compiler
                          + " -c " + input_file
                          + " `" + opari_config + " --cflags` "
                          + " -o " + output_file;
    if ( verbosity >= 1 )
    {
        std::cout << command << std::endl;
    }
    if ( !is_dry_run )
    {
        if ( system( command.c_str() ) != 0 )
        {
            if ( verbosity < 1 )
            {
                std::cout << "Error executing: " << command << std::endl;
            }
            abort();
        }
    }

    temp_files += " " + output_file;
}

void
SCOREP_Instrumenter::compile_source_file( std::string input_file,
                                          std::string output_file )
{
    /* If the we have only one source file and do not link, then the user could
       specify object file name.
     */
    if ( ( input_file_number == 1 ) && !is_linking && ( output_name != "" ) )
    {
        output_file = output_name;
    }

    /* Construct command */
    std::string command = compiler_name + " "
                          + scorep_include_path
                          + " -c " + input_file
                          + compiler_flags + define_flags
                          + " -o " + output_file;

    if ( verbosity >= 1 )
    {
        std::cout << command << std::endl;
    }

    /* Execute compilation */
    if ( !is_dry_run )
    {
        if ( system( command.c_str() ) != 0 )
        {
            if ( verbosity < 1 )
            {
                std::cout << "Error executing: " << command << std::endl;
            }
            abort();
        }
    }
}

std::string
SCOREP_Instrumenter::instrument_opari( std::string source_file )
{
    /* For Fortran source files, the extension must be in upper case to use the
       C-Preporcessor */
    std::string extension = get_extension( source_file );
    if ( is_fortran_file( source_file ) )
    {
        std::transform( extension.begin(), extension.end(), extension.begin(), ::toupper );
    }

    std::string modified_file = get_basename( source_file )
                                + ".opari"
                                + extension;

    invoke_opari( source_file, modified_file );
    temp_files += " " + modified_file + " " + source_file + ".opari.inc";
    return modified_file;
}

std::string
SCOREP_Instrumenter::remove_path( std::string full_path )
{
    size_t pos = full_path.rfind( "/" );
    if ( pos == std::string::npos )
    {
        return full_path;
    }
    else
    {
        return full_path.substr( pos + 1, std::string::npos );
    }
}

std::string
SCOREP_Instrumenter::instrument_pdt( std::string source_file )
{
    std::string extension     = get_extension( source_file );
    std::string modified_file = get_basename( source_file ) + "_pdt" + extension;
    std::string pdb_file      = remove_path( get_basename( source_file ) + ".pdb" );
    std::string command       = "";
    int         return_value  = 0;

    // Create database file
    if ( is_c_file( source_file ) )
    {
        command = pdt_bin_path + "/cparse " + source_file;
    }
    else if ( is_fortran_file( source_file ) )
    {
        command = pdt_bin_path + "/gfparse " + source_file;
    }
    else
    {
        command = pdt_bin_path + "/cxxparse " + source_file;
    }
    command += define_flags + include_flags + " " + scorep_include_path;

    if ( verbosity >= 1 )
    {
        std::cout << command << std::endl;
    }
    if ( !is_dry_run )
    {
        return_value = system( command.c_str() );
        if ( return_value != 0 )
        {
            std::cerr << "Failed to create PDT database file." << std::endl;
            exit( EXIT_FAILURE );
        }
    }

    // instrument source
    command = pdt_bin_path + "/tau_instrumentor "
              + pdb_file + " "
              + source_file
              + compiler_flags + define_flags
              + " -o " + modified_file
              + " -spec " + pdt_config_file;

    if ( is_openmp_application == enabled )
    {
        command += " -D_OPENMP";
    }

    if ( verbosity >= 1 )
    {
        std::cout << command << std::endl;
    }
    if ( !is_dry_run )
    {
        return_value = system( command.c_str() );
        if ( return_value != 0 )
        {
            std::cerr << "PDT instrumentation failed." << std::endl;
            exit( EXIT_FAILURE );
        }
    }

    temp_files += " " + modified_file + " " + pdb_file;

    return modified_file;
}

/* ****************************************************************************
   Linking
******************************************************************************/

void
SCOREP_Instrumenter::prepare_opari_linking()
{
    std::string current_file = "";
    std::string object_files = "";
    std::string init_source  = output_name + ".pomp_init.c";
    std::string init_object  = output_name + ".pomp_init.o";
    size_t      old_pos      = 0;
    size_t      cur_pos      = 0;

    // Compile list of library and object files
    while ( cur_pos != std::string::npos )
    {
        cur_pos = input_files.find( " ", old_pos );
        if ( old_pos < cur_pos ) // Discard a blank
        {
            current_file = input_files.substr( old_pos, cur_pos - old_pos );
            if ( is_object_file( current_file ) || is_library( current_file ) )
            {
                object_files += " " + current_file;
            }
        }
        // Setup for next file
        old_pos = cur_pos + 1;
    }

    // Create and compile the POMP2 init file.
    invoke_awk_script( object_files, init_source );
    compile_init_file( init_source, init_object );

    // Add the object file for POMP2 initialization to the input files for linking.
    input_files += " " + init_object;
}

int
SCOREP_Instrumenter::link_step()
{
    std::string command = compiler_name
                          + " " + input_files
                          + " " + scorep_libs
                          + " " + compiler_flags
                          + " " + scorep_libs;

    if ( output_name != "" )
    {
        command += " -o " + output_name;
    }

    if ( verbosity >= 1 )
    {
        std::cout << command << std::endl;
    }

    if ( !is_dry_run )
    {
        return system( command.c_str() );
    }
    return EXIT_SUCCESS;
}

int
SCOREP_Instrumenter::invoke_cobi( std::string orig_name )
{
    std::string adapter = cobi_config_dir + "/SCOREP_Cobi_Adapter";
    std::string command;

    /* Define adapter definition file */
    if ( is_mpi_application == enabled )
    {
        adapter += "Mpi";
    }
    if ( is_openmp_application == enabled )
    {
        adapter += "Omp";
    }
    if ( ( is_mpi_application == disabled ) && ( is_openmp_application == disabled ) )
    {
        adapter += "Serial";
    }
    adapter += ".xml";

    command = cobi
              + " -a " + adapter
              + " -b " + orig_name
              + " -f " + cobi_config_dir + "/SCOREP_Cobi_Filter.xml"
              + " -o " + output_name;

    if ( verbosity >= 1 )
    {
        std::cout << command << std::endl;
    }
    if ( !is_dry_run )
    {
        return system( command.c_str() );
    }

    temp_files += " " + orig_name;

    return EXIT_SUCCESS;
}
