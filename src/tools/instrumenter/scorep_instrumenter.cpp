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
 * Copyright (c) 2009-2013, 2015, 2019,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * Copyright (c) 2016-2017,
 * Technische Universitaet Darmstadt, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 */

#include <config.h>
#include <scorep_config_tool_backend.h>
#include <scorep_config_tool_mpi.h>
#include <scorep_config_tool_shmem.h>
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

#include "scorep_instrumenter.hpp"
//#include "scorep_instrumenter_cobi.hpp"
#include "scorep_instrumenter_compiler.hpp"
#include "scorep_instrumenter_cuda.hpp"
#include "scorep_instrumenter_mpp.hpp"
#include "scorep_instrumenter_io.hpp"
#include "scorep_instrumenter_online_access.hpp"
#include "scorep_instrumenter_opari.hpp"
#include "scorep_instrumenter_preprocess.hpp"
#include "scorep_instrumenter_pdt.hpp"
#include "scorep_instrumenter_thread.hpp"
#include "scorep_instrumenter_user.hpp"
#include "scorep_instrumenter_openacc.hpp"
#include "scorep_instrumenter_opencl.hpp"
#include "scorep_instrumenter_memory.hpp"
#include "scorep_instrumenter_utils.hpp"
#include "scorep_instrumenter_mutex.hpp"
#include "scorep_instrumenter_linktime_wrapping.hpp"
#include "scorep_instrumenter_libwrap.hpp"

#include <scorep_tools_utils.hpp>

void
print_help();

/* ****************************************************************************
                                                                 public methods
 * ***************************************************************************/
SCOREP_Instrumenter::SCOREP_Instrumenter( SCOREP_Instrumenter_InstallData& install_data,
                                          SCOREP_Instrumenter_CmdLine&     command_line )
    : m_install_data( install_data ),
    m_command_line( command_line )
{
    /* Create adapters */
    //m_cobi_adapter       = new SCOREP_Instrumenter_CobiAdapter();
    m_compiler_adapter   = new SCOREP_Instrumenter_CompilerAdapter();
    m_cuda_adapter       = new SCOREP_Instrumenter_CudaAdapter();
    m_opari_adapter      = new SCOREP_Instrumenter_OpariAdapter();
    m_preprocess_adapter = new SCOREP_Instrumenter_PreprocessAdapter();
    m_pdt_adapter        = new SCOREP_Instrumenter_PdtAdapter();
    m_user_adapter       = new SCOREP_Instrumenter_UserAdapter();
    m_pthread_adapter    = new SCOREP_Instrumenter_PthreadAdapter();
    m_openacc_adapter    = new SCOREP_Instrumenter_OpenACCAdapter();
    m_opencl_adapter     = new SCOREP_Instrumenter_OpenCLAdapter();
    m_memory_adapter     = new SCOREP_Instrumenter_MemoryAdapter();
    new SCOREP_Instrumenter_LinktimeWrappingAdapter();
    new SCOREP_Instrumenter_OnlineAccess();
    new SCOREP_Instrumenter_LibwrapAdapter();

    /* pre-preprocess adapter order */
    m_preprocess_adapters.push_back( m_preprocess_adapter );

    /* pre-compile adapter order */
    m_precompile_adapters.push_back( m_compiler_adapter );
    m_precompile_adapters.push_back( m_opari_adapter );
    m_precompile_adapters.push_back( m_pdt_adapter );

    /* pre-link adapter order */
    m_prelink_adapters.push_back( m_compiler_adapter );
    m_prelink_adapters.push_back( m_opari_adapter );

    /* post-link adapter order */
    //m_postlink_adapters.push_back( m_cobi_adapter );

    /* Create paradigm groups */
    m_thread = new SCOREP_Instrumenter_Thread();
    m_mpp    = new SCOREP_Instrumenter_Mpp();
    m_io     = new SCOREP_Instrumenter_Io();

    /* Create mutex selector*/
    m_mutex = new SCOREP_Instrumenter_Mutex();
}

SCOREP_Instrumenter::~SCOREP_Instrumenter ()
{
    delete ( m_thread );
    SCOREP_Instrumenter_Adapter::destroyAll();
}

int
SCOREP_Instrumenter::Run( void )
{
    m_input_files = *m_command_line.getInputFiles();

    /* If no compiling or linking happens, e.g., because the command performs only
       dependency generation, execute the unmodified command */
    if ( m_command_line.doNothing() )
    {
        /* Construct command */
        std::string command = m_command_line.getCompilerName()
                              + " " + m_command_line.getFlagsBeforeInterpositionLib()
                              + " " + m_command_line.getFlagsAfterInterpositionLib()
                              + scorep_vector_to_string( m_input_files,
                                                         " ", "", " " );

        std::string output_name =  m_command_line.getOutputName();
        if ( output_name != "" )
        {
            command += " -o " + output_name;
        }
        executeCommand( command );

        return EXIT_SUCCESS;
    }

    if ( m_command_line.isCompiling() ||
         m_command_line.getPreprocessMode() != SCOREP_Instrumenter_CmdLine::DISABLE )
    {
        /* Because Opari and PDT perform source code modifications, and store
           the modified source file with a different name, we get object files
           with a different name. To avoid this, we need to compile every
           source file separately and explicitly specify the output file name.
         */
        std::vector<std::string> object_files;
        std::string              object_file = "";
        char*                    cwd_to_free = UTILS_IO_GetCwd( NULL, 0 );
        std::string              cwd         = ( cwd_to_free ? cwd_to_free : "" );
        cwd = backslash_special_chars( cwd );

        /* If the original command compile and link in one step,
           we need to split compilation and linking, because for Opari
           we need to run the script on the object files.
           Furthermore, if the user specifies multiple source files
           in the compile step, we need to compile them separately, because
           the sources are modified and, thus, the object file ends up with
           a different name. In this case we do not execute the last step.
         */
        for ( std::vector<std::string>::iterator current_file = m_input_files.begin();
              current_file != m_input_files.end();
              current_file++ )
        {
            if ( is_source_file( *current_file ) )
            {
                /* Make sure, it has full path => Some compilers and
                   user instrumentation use the file name given to the compiler.
                   Thus, if we make all file names have full paths, we get a
                   consistent input.
                   However, temporary files are without paths. Thus, if a source
                   code instrumenter does not insert line directives, the result
                   may not contain path information anymore.
                 */
                char* simplified = UTILS_IO_JoinPath( 2, cwd.c_str(),
                                                      current_file->c_str() );
                if ( simplified )
                {
                    UTILS_IO_SimplifyPath( simplified );
                    *current_file = simplified;
                    free( simplified );
                }

                // Determine object file name
                object_file = create_object_name( *current_file );

                /* Setup the config tool calls for the new input file. This
                   will already setup the compiler and user instrumentation
                   if desired
                 */
                prepare_config_tool_calls( *current_file );

                /* If compiling and linking is performed in one step.
                   The compiler leave no object file.
                   Thus, we delete the object file, too.
                 */
                if ( m_command_line.isLinking() )
                {
                    addTempFile( object_file );
                }

                /* If we create modified source, we must add the original
                     source directory to the include dirs, because local
                     files may be included
                 */
                std::string search_path                = extract_path( *current_file );
                std::string pdt_include_search_path    = " -I" + search_path;
                std::string opari2_include_search_path = " -I" +  search_path;
                if ( is_c_file( *current_file ) )
                {
                    #if SCOREP_BACKEND_HAVE_C_IQUOTE_SUPPORT
                    opari2_include_search_path = " -iquote " +  search_path;
                    #endif /* SCOREP_BACKEND_HAVE_IQUOTE_SUPPORT */
                }
                else if ( is_cpp_file( *current_file ) )
                {
                    #if SCOREP_BACKEND_HAVE_CXX_IQUOTE_SUPPORT
                    opari2_include_search_path = " -iquote " +  search_path;
                        #endif /* SCOREP_BACKEND_HAVE_IQUOTE_SUPPORT */
                }
                std::string compiler_flags_save = m_compiler_flags;

                // Perform preprocessing steps
                if ( m_command_line.getPreprocessMode() != SCOREP_Instrumenter_CmdLine::DISABLE &&
                     !is_assembler_file( *current_file ) )
                {
                    std::string orig_extension = get_extension( *current_file );
                    *current_file = preprocess( *current_file );
                    if ( m_command_line.getPreprocessMode() == SCOREP_Instrumenter_CmdLine::EXPLICIT_STEP )
                    {
                        std::string prep_file = m_command_line.getOutputName();
                        if ( m_command_line.isCompiling() )
                        {
                            prep_file = remove_extension( remove_path( *current_file ) )
                                        + ".prep"
                                        + orig_extension;
                            addTempFile( prep_file );
                        }
                        m_compiler_flags += opari2_include_search_path;
                        preprocess_source_file( *current_file, prep_file );
                        *current_file = prep_file;
                    }
                }
                m_compiler_flags = compiler_flags_save;

                // Perform compile step
                if ( m_command_line.isCompiling() )
                {
                    if ( m_pdt_adapter->isEnabled() )
                    {
                        m_compiler_flags += pdt_include_search_path;
                    }
                    else if ( m_opari_adapter->isEnabled() &&
                              m_command_line.getPreprocessMode() != SCOREP_Instrumenter_CmdLine::EXPLICIT_STEP )
                    {
                        m_compiler_flags += opari2_include_search_path;
                    }
                    *current_file = precompile( *current_file );

                    #if SCOREP_BACKEND_COMPILER_CRAY
                    if ( m_opari_adapter->isEnabled() &&
                         m_command_line.getCompilerName().find( "ftn" ) != std::string::npos )
                    {
                        m_compiler_flags += " -I.";
                    }
                    #endif

                    compile_source_file( *current_file, object_file );

                    // Add object file to the input file list for the link command
                    object_files.push_back( object_file );
                }
            }
            // If it is no source file, leave the file in the input list
            else
            {
                object_files.push_back( *current_file );
            }
        }
        free( cwd_to_free );

        // Replace sources by compiled by their object file names for the link command
        m_input_files = object_files;
    }

    if ( m_command_line.isLinking() )
    {
        SCOREP_Instrumenter_Adapter::checkAllObjects( *this );
        SCOREP_Instrumenter_Selector::checkAllObjects( *this );

        /* Some new adapters may be active now. */
        m_command_line.checkParameter();

        // Create the config tool calls for linking
        prepare_config_tool_calls( "" );

        // Perform pre-link instrumentation actions
        prelink();

        // Perform linking
        if ( !m_command_line.isTargetSharedLib() )
        {
            create_subsystem_initialization();
        }
        link_step();

        // Perform post-link instrumentation actions
        postlink();
    }

    clean_temp_files();

    return EXIT_SUCCESS;
}

std::string
SCOREP_Instrumenter::getCompilerFlags( void )
{
    return m_compiler_flags;
}

std::vector<std::string>*
SCOREP_Instrumenter::getInputFiles( void )
{
    return &m_input_files;
}

void
SCOREP_Instrumenter::prependInputFile( const std::string& filename )
{
    m_input_files.insert( m_input_files.begin(), filename );
}

void
SCOREP_Instrumenter::appendInputFile( const std::string& filename )
{
    m_input_files.insert( m_input_files.end(), filename );
}

const std::string&
SCOREP_Instrumenter::getConfigBaseCall( void )
{
    return m_config_base;
}

/* ****************************************************************************
 *                                                              private methods
 * ***************************************************************************/

std::string
SCOREP_Instrumenter::create_object_name( const std::string& sourceFile )
{
    std::string object_file;
    if ( ( !m_command_line.isLinking() ) &&
         ( m_command_line.getOutputName() != "" ) &&
         ( m_command_line.getInputFileNumber() == 1 ) )
    {
        object_file = m_command_line.getOutputName();
    }
    else
    {
        object_file = remove_extension(
            remove_path( sourceFile ) ) + ".o";
        // If we are also linking, we want to add a timestamp
        // to the object file to avoid name clashes if two source files
        // from different directories have the same name.
        if ( m_command_line.isLinking() )
        {
            std::stringstream new_name;
            new_name << remove_extension( object_file )
                     << create_random_string()
                     << get_extension( object_file );
            object_file = new_name.str();
        }
    }
    return object_file;
}

void
SCOREP_Instrumenter::addTempFile( const std::string& filename )
{
    m_temp_files.push_back( filename );
}

void
SCOREP_Instrumenter::clean_temp_files( void )
{
    if ( ( !m_command_line.hasKeepFiles() ) && ( !m_temp_files.empty() ) )
    {
        executeCommand( scorep_vector_to_string( m_temp_files, "rm ", "", " " ) );
    }
}

void
SCOREP_Instrumenter::prepare_config_tool_calls( const std::string& input_file )
{
    std::string mode          = "";
    std::string target        = "";
    std::string scorep_config = m_install_data.getScorepConfig();

    mode += SCOREP_Instrumenter_Selector::getAllConfigToolFlags( m_command_line );
    mode += SCOREP_Instrumenter_Adapter::getAllConfigToolFlags( m_command_line );

    if ( m_command_line.enforceStaticLinking() )
    {
        mode += " --static";
    }
    else if ( m_command_line.enforceDynamicLinking() )
    {
        mode += " --dynamic";
    }

#if SCOREP_BACKEND_COMPILER_INTEL && ( HAVE( PLATFORM_MIC ) || HAVE( MIC_SUPPORT ) )
    if ( m_command_line.isMmicSet() )
    {
        target += " --target mic";
    }
#endif  /* SCOREP_BACKEND_COMPILER_INTEL && ( HAVE( PLATFORM_MIC ) || HAVE( MIC_SUPPORT ) ) */
    // Generate calls
    m_config_base = scorep_config + target + mode;
}

void
SCOREP_Instrumenter::compile_source_file( const std::string& input_file,
                                          const std::string& output_file )
{
    std::string cflags = "--cflags";
    if ( is_cpp_file( input_file ) )
    {
        cflags = "--cxxflags";
    }
    else if ( is_fortran_file( input_file ) )
    {
        cflags = "--fflags";
    }

    /* Construct command */
    std::stringstream command;
    command << SCOREP_Instrumenter_InstallData::getCompilerEnvironmentVars();
    command << m_command_line.getCompilerName();
    command << " `" << m_config_base << " " <<  cflags << "`";
    command << " " << m_compiler_flags;
    command << " " << m_command_line.getFlagsBeforeInterpositionLib();
    command << " " << m_command_line.getFlagsAfterInterpositionLib();
    command << " -c " << input_file;
    command << " -o " << output_file;
    executeCommand( command.str() );
}

void
SCOREP_Instrumenter::preprocess_source_file( const std::string& source_file,
                                             const std::string& output_file )
{
    std::string command;

    // Determine language
    std::string language = "c";
    if ( is_cpp_file( source_file ) )
    {
        language = "c++";
    }
    else if ( is_fortran_file( source_file ) )
    {
        language = "fortran";
    }

    // Preprocess file
    command = SCOREP_Instrumenter_InstallData::getCompilerEnvironmentVars()
              + m_command_line.getCompilerName()
              + " " + m_command_line.getFlagsBeforeInterpositionLib()
              + " `" + getConfigBaseCall() + " --cppflags=" + language + "`"
              + " " + getCompilerFlags()
              + " " + m_command_line.getFlagsAfterInterpositionLib()
              + " " + source_file;

    if ( is_c_file( source_file ) )
    {
        command += " " + SCOREP_Instrumenter_InstallData::getCPreprocessingFlags( source_file,
                                                                                  output_file );
    }
    else if ( is_cpp_file( source_file ) )
    {
        command += " " +  SCOREP_Instrumenter_InstallData::getCxxPreprocessingFlags( source_file,
                                                                                     output_file );
    }

    else if ( is_fortran_file( source_file ) )
    {
        command += " " +  SCOREP_Instrumenter_InstallData::getFortranPreprocessingFlags( source_file,
                                                                                         output_file );
    }
    else if ( is_cuda_file( source_file ) )
    {
        command += " -E > " + output_file;
    }

    executeCommand( command );
}

std::string
SCOREP_Instrumenter::preprocess( std::string current_file )
{
    std::deque<SCOREP_Instrumenter_Adapter*>::iterator adapter;
    for ( adapter = m_preprocess_adapters.begin();
          adapter != m_preprocess_adapters.end();
          adapter++ )
    {
        if ( ( *adapter )->isEnabled() )
        {
            current_file = ( *adapter )->preprocess( *this,
                                                     m_command_line,
                                                     current_file );
        }
    }
    return current_file;
}

std::string
SCOREP_Instrumenter::precompile( std::string current_file )
{
    std::deque<SCOREP_Instrumenter_Adapter*>::iterator adapter;
    for ( adapter = m_precompile_adapters.begin();
          adapter != m_precompile_adapters.end();
          adapter++ )
    {
        if ( ( *adapter )->isEnabled() )
        {
            current_file = ( *adapter )->precompile( *this,
                                                     m_command_line,
                                                     current_file );
        }
    }
    return current_file;
}

void
SCOREP_Instrumenter::prelink( void )
{
    std::deque<SCOREP_Instrumenter_Adapter*>::iterator adapter;
    for ( adapter = m_prelink_adapters.begin();
          adapter != m_prelink_adapters.end();
          adapter++ )
    {
        if ( ( *adapter )->isEnabled() )
        {
            ( *adapter )->prelink( *this, m_command_line );
        }
    }
}

void
SCOREP_Instrumenter::postlink( void )
{
    std::deque<SCOREP_Instrumenter_Adapter*>::iterator adapter;
    for ( adapter = m_postlink_adapters.begin();
          adapter != m_postlink_adapters.end();
          adapter++ )
    {
        if ( ( *adapter )->isEnabled() )
        {
            ( *adapter )->postlink( *this, m_command_line );
        }
    }
}

void
SCOREP_Instrumenter::create_subsystem_initialization( void )
{
    // Create initialization source
    std::string init_source = m_command_line.getOutputName() + ".scorep_init.c";
    {
        std::stringstream command;
        command << getConfigBaseCall() << " --adapter-init > " << init_source;
        executeCommand( command.str() );
        addTempFile( init_source );
    }

    // Compile initialization source file
    {
        std::string       init_object  = remove_extension( init_source ) + ".o";
        std::string       target_flags = "";
        std::stringstream command;

#if SCOREP_BACKEND_COMPILER_INTEL && ( HAVE( PLATFORM_MIC ) || HAVE( MIC_SUPPORT ) )
        if ( m_command_line.isMmicSet() )
        {
            target_flags += " -mmic";
        }
#endif  /* SCOREP_BACKEND_COMPILER_INTEL && ( HAVE( PLATFORM_MIC ) || HAVE( MIC_SUPPORT ) ) */

        command << m_install_data.getCC()
                << target_flags
                << " -c " << init_source
                << " -o " << init_object;
        executeCommand( command.str() );
        addTempFile( init_object );
        prependInputFile( init_object );
    }
}

void
SCOREP_Instrumenter::link_step( void )
{
    std::string linker_prefix = " " LIBDIR_FLAG_WL;
    if ( m_cuda_adapter->isNvcc() )
    {
        linker_prefix = " -Xlinker=";
    }

    std::string libs_prefix;
    std::string libs_suffix;
#if defined( SCOREP_SHARED_BUILD )
    /* temporary, see ticket:385 */
    if ( m_command_line.getNoAsNeeded() )
    {
        libs_prefix = linker_prefix +  "--no-as-needed";
        libs_suffix = linker_prefix +  "--as-needed";
    }
#endif

    std::stringstream command;
    command << SCOREP_Instrumenter_InstallData::getCompilerEnvironmentVars();
    command << m_command_line.getCompilerName();
    command << scorep_vector_to_string( m_input_files, " ", "", " " );
#if HAVE_BACKEND( COMPILER_CONSTRUCTOR_SUPPORT )
    if ( !m_command_line.isTargetSharedLib() )
    {
        command << " `" << m_config_base << " --constructor`";
    }
#endif
    command << " `" << m_config_base << " --ldflags`";
    command << " " << m_command_line.getFlagsBeforeInterpositionLib();

    std::string linker_start_group_flags;
    std::string linker_end_group_flags;
    // nvcc uses its own pair of start-/end-group flags,
    // if scorep adds another one, we might get into trouble with nested groups,
    // therefore, skip our start-/end-group if the compiler is nvcc
    if ( !( m_cuda_adapter->isNvcc() ) )
    {
        linker_start_group_flags = linker_prefix + "-start-group";
        linker_end_group_flags   = linker_prefix + "-end-group";
    }
#if SCOREP_BACKEND_HAVE_LINKER_START_END_GROUP
    command << libs_prefix << linker_start_group_flags << " `" << m_config_base << " --event-libs`";
    command << " " << m_command_line.getFlagsAfterInterpositionLib() << libs_suffix;
#else
    command << libs_prefix << " `" << m_config_base << " --event-libs`" << libs_suffix;
    command << " " << m_command_line.getFlagsAfterInterpositionLib();
#endif
#if SCOREP_BACKEND_HAVE_LINKER_START_END_GROUP
    command << libs_prefix << " `" << m_config_base << " --mgmt-libs`" << linker_end_group_flags << libs_suffix;
#else
    command << libs_prefix << " `" << m_config_base << " --mgmt-libs`" << libs_suffix;
#endif
    if ( m_command_line.getOutputName() != "" )
    {
        /* nvcc requires a space between -o and the output name. */
        command << " -o " << m_command_line.getOutputName();
    }

    executeCommand( command.str() );
}

void
SCOREP_Instrumenter::executeCommand( const std::string& orig_command )
{
    std::string command( orig_command );
    if ( m_command_line.getVerbosity() >= 1 )
    {
        std::cerr << orig_command << std::endl;

        /* --dry-run implies --verbose */
        if ( m_command_line.isDryRun() )
        {
            return;
        }

        /* Let the shell do its job and show us all executed commands. */
        command = "PS4=' Executing: '; set -x; " + command;
    }

    int return_value = system( command.c_str() );
    if ( return_value != 0 )
    {
        std::cerr << "[Score-P] ERROR: Execution failed: " << orig_command << std::endl;
        exit( EXIT_FAILURE );
    }
}
