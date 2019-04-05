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
 * Copyright (c) 2009-2014,
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
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * @brief      Class to steer the instrumentation of the user code.
 */

#ifndef SCOREP_INSTRUMENTER_HPP
#define SCOREP_INSTRUMENTER_HPP

#include "scorep_instrumenter_cmd_line.hpp"

#include <iostream>
#include <string>
#include <deque>
#include <vector>

class SCOREP_Instrumenter_Adapter;
//class SCOREP_Instrumenter_CobiAdapter;
class SCOREP_Instrumenter_CompilerAdapter;
class SCOREP_Instrumenter_CudaAdapter;
class SCOREP_Instrumenter_OpariAdapter;
class SCOREP_Instrumenter_PreprocessAdapter;
class SCOREP_Instrumenter_PdtAdapter;
class SCOREP_Instrumenter_UserAdapter;
class SCOREP_Instrumenter_PthreadAdapter;
class SCOREP_Instrumenter_OpenCLAdapter;
class SCOREP_Instrumenter_OpenACCAdapter;
class SCOREP_Instrumenter_MemoryAdapter;

class SCOREP_Instrumenter_Thread;
class SCOREP_Instrumenter_Mpp;
class SCOREP_Instrumenter_Io;

class SCOREP_Instrumenter_Mutex;

/* ****************************************************************************
   Class SCOREP_Instrumenter
******************************************************************************/
/**
 *  @brief performs instrumentation stage
 *
 *  This class examines the available compiler settings and the type of
 *  instrumentation. Makes the necessary modifications to the user command
 *  for instrumentation and executed the user command.
 */
class SCOREP_Instrumenter
{
    /* ****************************************************** Public methods */
public:

    /**
       Creates a new SCOREP_Instrumenter object.
     */
    SCOREP_Instrumenter( SCOREP_Instrumenter_InstallData& install_data,
                         SCOREP_Instrumenter_CmdLine&     command_line );

    /**
       Destroys a SCOREP_Instrumenter object.
     */
    virtual
    ~SCOREP_Instrumenter();

    /**
       Performs the instrumentation of an application
     */
    virtual int
    Run( void );

    /**
       Executes the command specified by @ command. It checks verbosity level
       and whether it is a dry run.
     */
    void
    executeCommand( const std::string& command );

    void
    addTempFile( const std::string& filename );

    std::string
    getCompilerFlags( void );

    std::vector<std::string>*
    getInputFiles( void );

    void
    prependInputFile( const std::string& filename );

    void
    appendInputFile( const std::string& filename );

    const std::string&
    getConfigBaseCall( void );

    SCOREP_Instrumenter_CmdLine&
    getCommandLine( void )
    {
        return m_command_line;
    };

    /* ***************************************************** Private methods */
private:

    /**
       Executes the linking command. Aborts if an error occurs.
     */
    void
    link_step( void );

    /**
       Constructs calls to the config tools.
       @param input_file The input file for which the calls are generated.
     */
    void
    prepare_config_tool_calls( const std::string& input_file );

    /**
       Invokes the preprocessor on a source file.
       @param input_file  Source file which is compiled.
       @param output_file Filename for the object file.
     */
    void
    preprocess_source_file( const std::string& input_file,
                            const std::string& output_file );

    /**
       Compiles a users source file. If the original command compile and
       link in one step, we need to split compilation and linking, because
       we need to run the script on the object files. Thus, we do already
       compile the source. It uses the compiler used by the user command,
       appends compiler flags given by the user.
       @param input_file  Source file which is compiled.
       @param output_file Filename for the object file.
     */
    void
    compile_source_file( const std::string& input_file,
                         const std::string& output_file );

    /**
       Removes temporarily created files.
     */
    void
    clean_temp_files( void );

    /**
       Invokes the adapters' pre-preprocess step on the current file.
       @param current_file The currently processed file.
       @returns The filename of the source file that should be compiled.
     */
    std::string
    preprocess( std::string current_file );

    /**
       Invokes the adapters' precompile step on the current file.
       @param current_file The currently processed file.
       @returns The filename of the source file that should be compiled.
     */
    std::string
    precompile( std::string current_file );

    /**
       Invokes the adapters' precompile step.
     */
    void
    prelink( void );

    /**
       Invokes the adapters' postcompile step.
     */
    void
    postlink( void );

    /**
       Creates and compiles a source file which initializes the
       used Score-P subsystems.
     */
    void
    create_subsystem_initialization( void );

    /**
       Creates the object file name.
     */
    std::string
    create_object_name( const std::string& sourceFile );

    /* ***************************************************** Private members */
private:

    /**
       Installation configuration data set.
     */
    SCOREP_Instrumenter_InstallData& m_install_data;

    /**
       The command line parser.
     */
    SCOREP_Instrumenter_CmdLine& m_command_line;

    /**
       input file names. Need to be separated because OPARI may
       perform source code modifications which take these as input and
       the original command needs the result from the OPARI output. Thus,
       they are then substituted by the OPARI output.
     */
    std::vector<std::string> m_input_files;

    /**
       A list of temporarily created files that are deleted at the end of a
       successful execution, if @a keep_files is false.
     */
    std::vector<std::string> m_temp_files;

    /**
       The base config call without action parameter.
     */
    std::string m_config_base;

    /**
       Additional compiler flags added by the instrumenter
     */
    std::string m_compiler_flags;

    //SCOREP_Instrumenter_CobiAdapter*       m_cobi_adapter;
    SCOREP_Instrumenter_CompilerAdapter*   m_compiler_adapter;
    SCOREP_Instrumenter_CudaAdapter*       m_cuda_adapter;
    SCOREP_Instrumenter_OpariAdapter*      m_opari_adapter;
    SCOREP_Instrumenter_PreprocessAdapter* m_preprocess_adapter;
    SCOREP_Instrumenter_PdtAdapter*        m_pdt_adapter;
    SCOREP_Instrumenter_UserAdapter*       m_user_adapter;
    SCOREP_Instrumenter_PthreadAdapter*    m_pthread_adapter;
    SCOREP_Instrumenter_OpenACCAdapter*    m_openacc_adapter;
    SCOREP_Instrumenter_OpenCLAdapter*     m_opencl_adapter;
    SCOREP_Instrumenter_MemoryAdapter*     m_memory_adapter;

    SCOREP_Instrumenter_Thread* m_thread;
    SCOREP_Instrumenter_Mpp*    m_mpp;
    SCOREP_Instrumenter_Io*     m_io;

    SCOREP_Instrumenter_Mutex* m_mutex;

    std::deque<SCOREP_Instrumenter_Adapter*> m_preprocess_adapters;
    std::deque<SCOREP_Instrumenter_Adapter*> m_precompile_adapters;
    std::deque<SCOREP_Instrumenter_Adapter*> m_prelink_adapters;
    std::deque<SCOREP_Instrumenter_Adapter*> m_postlink_adapters;
};
#endif // SCOREP_INSTRUMENTER_HPP
