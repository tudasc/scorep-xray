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
#include <sstream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <UTILS_Error.h>
#include <UTILS_IO.h>

#include <scorep_instrumenter.hpp>
#include <scorep_config_tool_backend.h>
#include <scorep_config_tool_mpi.h>
#include <scorep_instrumenter_utils.hpp>

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
SCOREP_Instrumenter::SCOREP_Instrumenter( SCOREP_Instrumenter_InstallData* install_data,
                                          SCOREP_Instrumenter_CmdLine*     command_line )
{
    m_install_data = install_data;
    m_command_line = command_line;
    m_temp_files   = "";
    m_input_files  = "";
}

SCOREP_Instrumenter::~SCOREP_Instrumenter ()
{
}

int
SCOREP_Instrumenter::Run( void )
{
    /* Because the sun compiler can only instrument Fortran files, check
       whether all source files can be instrumented */
    if ( m_command_line->isCompilerInstrumenting() )
    {
        prepare_compiler();
    }

    std::string object_files = "";
    m_input_files = m_command_line->getInputFiles();

    if ( m_command_line->isCompiling() )
    {
        /* Because Opari and PDT perform source code modifications, and store
           the modified source file with a different name, we get object files
           with a different name. To avoid this, we need to compile every
           source file separately and explicitly specify the output file name.
         */
        std::string current_file = "";
        std::string object_file  = "";
        size_t      old_pos      = 0;
        size_t      cur_pos      = 0;
        char*       cwd          = UTILS_IO_GetCwd( NULL, 0 );
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
            cur_pos = m_input_files.find( " ", old_pos );
            if ( old_pos < cur_pos ) // Discard a blank
            {
                current_file = m_input_files.substr( old_pos, cur_pos - old_pos );
                if ( is_source_file( current_file ) )
                {
                    /* Make sure, it has full path => Some compilers and
                       user instrumentation use the file name given to the compiler.
                       Thus, if we make all file names have full pathes, we get a
                       consistent input.
                       However, temporary files are without pathes. Thus, if a source
                       code instrumenter does not insert line directives, the result
                       may not contain path information anymore.
                     */
                    char* simplified = UTILS_IO_JoinPath( 2, cwd, current_file.c_str() );
                    if ( simplified )
                    {
                        UTILS_IO_SimplifyPath( simplified );
                        current_file = simplified;
                        free( simplified );
                    }

                    // Determine object file name
                    if ( ( !m_command_line->isLinking() ) &&
                         ( m_command_line->getOutputName() != "" ) &&
                         ( m_command_line->getInputFileNumber() == 1 ) )
                    {
                        object_file = m_command_line->getOutputName();
                    }
                    else
                    {
                        object_file = remove_extension(
                            remove_path( current_file ) ) + ".o";
                    }

                    /* Setup the config tool calls for the new input file. This
                       will already setup the compiler and user instrumentation
                       if desired
                     */
                    prepare_config_tool_calls( current_file );

                    /* If we create modified source, we must add the original
                       source directory to the include dirs, because local
                       files may be included
                     */
                    m_compiler_flags += " -I" + extract_path( current_file );

                    // If compiling and linking is performed in one step.
                    // The compiler leave no object file.
                    // Thus, we delete the object file, too.
                    if ( m_command_line->isLinking() )
                    {
                        m_temp_files += " " + object_file;
                    }

                    // Perform Opari instrumentation
                    if ( m_command_line->isOpariInstrumenting() )
                    {
                        current_file = instrument_opari( current_file );
                    }

                    // Perform PDT instrumentation
                    #ifdef HAVE_PDT
                    if ( m_command_line->isPdtInstrumenting() )
                    {
                        current_file = instrument_pdt( current_file );
                    }
                    #endif

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
        m_input_files = object_files;
    }

    if ( m_command_line->isLinking() )
    {
        prepare_config_tool_calls( "" );

        // Perform Opari instrumentation
        if ( m_command_line->isOpariInstrumenting() ||
             m_command_line->isOpenmpApplication() )
        {
            prepare_opari_linking();
        }

        // ----------------------- Link step
        link_step();

        #if HAVE( COBI )
        if ( m_command_line->isCobiInstrumenting() )
        {
            std::string output_name = m_command_line->getOutputName();
            std::string orig_name   = output_name + ".orig";
            if ( m_command_line->getVerbosity() >= 1 )
            {
                std::cout << "mv "
                          << output_name
                          << " " << orig_name << std::endl;
            }
            if ( !m_command_line->isDryRun() )
            {
                if ( rename( output_name.c_str(), orig_name.c_str() ) != 0 )
                {
                    UTILS_ERROR_POSIX( "Failed to rename binary" );
                    return EXIT_FAILURE;
                }
            }

            return invoke_cobi( orig_name, output_name );
        }
        #endif
    }

    clean_temp_files();

    return EXIT_SUCCESS;
}

/* ****************************************************************************
   Cleanup
******************************************************************************/
void
SCOREP_Instrumenter::clean_temp_files( void )
{
    if ( ( !m_command_line->hasKeepFiles() ) && ( m_temp_files != "" ) )
    {
        m_temp_files = "rm" + m_temp_files;
        execute_command( m_temp_files );
    }
}


/* ****************************************************************************
   Compilation
******************************************************************************/
void
SCOREP_Instrumenter::prepare_config_tool_calls( std::string input_file )
{
    std::string mode          = " --seq";
    std::string scorep_config = m_install_data->getScorepConfig();

    // Determine mode parameter
    if ( m_command_line->isMpiApplication() )
    {
        mode = ( m_command_line->isOpenmpApplication() ? " --hyb" : " --mpi" );
    }
    else if ( m_command_line->isOpenmpApplication() )
    {
        mode = " --omp";
    }

    if ( m_command_line->isCudaApplication() )
    {
        mode += " --cuda";
    }

    if ( !m_command_line->isCompilerInstrumenting() )
    {
        mode += " --nocompiler";
    }

    if ( m_command_line->isUserInstrumenting() )
    {
        mode += " --user";
    }

    if ( is_fortran_file( input_file ) )
    {
        mode += " --fortran";
    }

    // Generate calls
    m_compiler_flags = "`" + scorep_config + mode + " --cflags` ";
    m_linker_flags   = "`" + scorep_config + mode + " --ldflags` `" +
                       scorep_config + mode + " --libs` ";
    if ( m_command_line->isOpariInstrumenting() )
    {
        m_compiler_flags += "`" + m_install_data->getOpariConfig()
                            + " --cflags` ";
    }
}

void
SCOREP_Instrumenter::prepare_compiler( void )
{
    /* The sun compiler can only instrument Fortran files. Thus, any C/C++
       files are not instrumented. To avoid user confusion, the instrumenter
       aborts in case a C/C++ file should be compiler instrumented.
     */
#ifdef SCOREP_COMPILER_SUN
    if ( m_command_line->isCompiling() )
    {
        std::string current_file = "";
        std::string extension    = "";
        size_t      old_pos      = 0;
        size_t      cur_pos      = 0;
        while ( cur_pos != std::string::npos )
        {
            cur_pos = m_input_files.find( " ", old_pos );
            if ( old_pos < cur_pos ) // Discard a blank
            {
                current_file = m_input_files.substr( old_pos, cur_pos - old_pos );
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
#endif      // SCOREP_COMPILER_SUN
}

void
SCOREP_Instrumenter::invoke_opari( std::string input_file,
                                   std::string output_file )
{
    std::string command = m_install_data->getOpari() + " --tpd "
#ifdef SCOREP_COMPILER_CRAY
                          "--nosrc "
#endif
#ifdef OPARI_MANGLING_SCHEME
                          "--tpd-mangling=" OPARI_MANGLING_SCHEME " "
#endif
    ;
    if ( m_command_line->isPdtInstrumenting() && is_fortran_file( input_file ) )
    {
        command += "--nosrc ";
    }
    command += input_file + " " + output_file;

    execute_command( command );
}

void
SCOREP_Instrumenter::invoke_awk_script( std::string object_files,
                                        std::string output_file )
{
    std::string command = m_install_data->getNm() + " " +  object_files
                          + " | " + m_install_data->getAwk() + " -f "
                          + m_install_data->getOpariScript()
                          + " > " + output_file;
    execute_command( command );

    m_temp_files += " " + output_file;
}

void
SCOREP_Instrumenter::compile_init_file( std::string input_file,
                                        std::string output_file )
{
    std::string command = m_install_data->getCC()
                          + " -c " + input_file
                          + " `" + m_install_data->getOpariConfig()
                          + " --cflags` "
                          + " -o " + output_file;

    execute_command( command );

    m_temp_files += " " + output_file;
}

void
SCOREP_Instrumenter::compile_source_file( std::string input_file,
                                          std::string output_file )
{
    /* Construct command */
    std::string command = m_command_line->getCompilerName()
                          + " " + m_command_line->getFlagsBeforeLmpi()
                          + " " + m_compiler_flags
                          + " " + m_command_line->getFlagsAfterLmpi()
                          + " -c " + input_file
                          + " -o " + output_file;
    execute_command( command );
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

    std::string modified_file = remove_path( remove_extension( source_file )
                                             + ".opari"
                                             + extension );

    invoke_opari( source_file, modified_file );
    m_temp_files += " " + modified_file
                    + " " + remove_path( source_file + ".opari.inc" );
    return modified_file;
}

std::string
SCOREP_Instrumenter::instrument_pdt( std::string source_file )
{
    std::string extension = get_extension( source_file );
    if ( is_fortran_file( source_file ) )
    {
        std::transform( extension.begin(), extension.end(), extension.begin(), ::toupper );
    }
    std::string modified_file = remove_path( remove_extension( source_file ) +
                                             "_pdt" + extension );
    std::string       pdb_file = remove_path( remove_extension( source_file ) + ".pdb" );
    std::stringstream command;
    std::string       pdt_bin_path = m_install_data->getPdtBinPath();

    // Create database file
    if ( is_c_file( source_file ) )
    {
        command << pdt_bin_path << "/cparse " << source_file;
    }
    else if ( is_fortran_file( source_file ) )
    {
        command << pdt_bin_path << "/gfparse " << source_file;
    }
    else
    {
        command << pdt_bin_path << "/cxxparse " << source_file;
    }
    command << " " << m_command_line->getDefineFlags()
            << " " << m_command_line->getIncludeFlags()
            << " " << m_compiler_flags;
#ifdef _OPENMP
    if ( m_command_line->isOpenmpApplication() )
    {
        command << " -D_OPENMP=";
        command << _OPENMP;
    }
#endif

    if ( m_command_line->isMpiApplication() )
    {
        command << " -I" SCOREP_MPI_INCLUDE;
    }

    execute_command( command.str() );

    // instrument source
    command.str( "" );
    command << pdt_bin_path << "/tau_instrumentor "
            << pdb_file
            << " " << source_file
            << " " << m_command_line->getIncludeFlags()
            << " " << m_command_line->getDefineFlags()
            << " " << m_compiler_flags;
#ifdef _OPENMP
    if ( m_command_line->isOpenmpApplication() )
    {
        command << " -D_OPENMP=";
        command <<  _OPENMP;
    }
#endif
    command << " -o " << modified_file
            << " -spec " << m_install_data->getPdtConfigFile();
    command << " " << m_command_line->getPdtParams();

    if ( m_command_line->isMpiApplication() )
    {
        command << " -I" SCOREP_MPI_INCLUDE;
    }

    execute_command( command.str() );

    m_temp_files += " " + modified_file + " " + pdb_file;

    return modified_file;
}

/* ****************************************************************************
   Linking
******************************************************************************/

std::string
SCOREP_Instrumenter::get_library_files( void )
{
    std::string libraries   = m_command_line->getLibraries();
    std::string libdirs     = m_command_line->getLibDirs();
    std::string current_lib = "";
    std::string lib_files   = "";
    size_t      old_pos     = 0;
    size_t      cur_pos     = 0;

    while ( cur_pos != std::string::npos )
    {
        cur_pos = libraries.find( " ", old_pos );
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

void
SCOREP_Instrumenter::prepare_opari_linking( void )
{
    std::string output_name  = m_command_line->getOutputName();
    std::string current_file = "";
    std::string object_files = "";
    std::string init_source  = output_name + ".pomp_init.c";
    std::string init_object  = output_name + ".pomp_init.o";
    size_t      old_pos      = 0;
    size_t      cur_pos      = 0;

    // Compile list of library and object files
    while ( cur_pos != std::string::npos )
    {
        cur_pos = m_input_files.find( " ", old_pos );
        if ( old_pos < cur_pos ) // Discard a blank
        {
            current_file = m_input_files.substr( old_pos, cur_pos - old_pos );
            if ( is_object_file( current_file ) || is_library( current_file ) )
            {
                object_files += " " + current_file;
            }
        }
        // Setup for next file
        old_pos = cur_pos + 1;
    }
    object_files += get_library_files();

    // Create and compile the POMP2 init file.
    invoke_awk_script( object_files, init_source );
    compile_init_file( init_source, init_object );

    /* Add the object file for POMP2 initialization to the input files for linking.
       Prepend it to the front of input symbols, because some compilers do not
       find the POMP_Init_<ID> symbols (in libraries) if it is appended.
       See  ticket #627. */
    m_input_files = init_object + " " + m_input_files;
}

void
SCOREP_Instrumenter::link_step( void )
{
    std::string command = m_command_line->getCompilerName()
                          + " " + m_input_files
                          + " " + m_command_line->getFlagsBeforeLmpi()
                          + " " + m_linker_flags
                          + " " + m_command_line->getFlagsAfterLmpi();

    if ( m_command_line->getOutputName() != "" )
    {
        command += " -o " + m_command_line->getOutputName();
    }

    execute_command( command );
}

int
SCOREP_Instrumenter::invoke_cobi( std::string orig_name,
                                  std::string output_name )
{
    std::string adapter = m_install_data->getCobiConfigDir()
                          + "/SCOREP_Cobi_Adapter";
    std::string command;

    /* Define adapter definition file */
    if ( m_command_line->isMpiApplication() )
    {
        adapter += "Mpi";
    }
    if ( m_command_line->isOpenmpApplication() )
    {
        adapter += "Omp";
    }
    if ( !m_command_line->isMpiApplication() &&
         !m_command_line->isOpenmpApplication() )
    {
        adapter += "Serial";
    }
    adapter += ".xml";

    command = m_install_data->getCobi()
              + " -a " + adapter
              + " -b " + orig_name
              + " -f " + m_install_data->getCobiConfigDir() + "/SCOREP_Cobi_Filter.xml"
              + " -o " + output_name;

    execute_command( command );

    m_temp_files += " " + orig_name;

    return EXIT_SUCCESS;
}

void
SCOREP_Instrumenter::execute_command( std::string command )
{
    if ( m_command_line->getVerbosity() >= 1 )
    {
        std::cout << command << std::endl;
    }
    if ( !m_command_line->isDryRun() )
    {
        int return_value = system( command.c_str() );
        if ( return_value != 0 )
        {
            std::cerr << "Error executing: " << command << std::endl;
            exit( EXIT_FAILURE );
        }
    }
}
