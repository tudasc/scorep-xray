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
 * @file       SCOREP_Instrumenter.hpp
 * @maintainer Daniel Lorenz  <d.lorenz@fz-juelich.de>
 *
 * @brief      Class to steer the instrumentation of the user code.
 */

#ifndef SCOREP_INSTRUMENTER_H_
#define SCOREP_INSTRUMENTER_H_

#include <scorep_instrumenter_cmd_line.hpp>

#include <iostream>
#include <string>

/* ****************************************************************************
   Class SCOREP_Instrumenter
******************************************************************************/
/**
 *  @brief performes instrumentation stage
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
    SCOREP_Instrumenter( SCOREP_Instrumenter_InstallData* install_data,
                         SCOREP_Instrumenter_CmdLine*     command_line );

    /**
       Destroys a SCOREP_Instrumenter object.
     */
    virtual
    ~SCOREP_Instrumenter();

    /**
       Performs the instrumentation of an application
     */
    virtual int
    Run();

    /* ***************************************************** Private methods */
private:

    /**
       Executes the linking command. Aborts if an error occurs.
     */
    void
    link_step();

    /**
       Performs necessary modifications to the command for enabling
       compiler instrumentation.
     */
    void
    prepare_compiler();

    /**
       Instruments @a source_file with the Tau instrumentor.
       @param source_file File name of the source file, that is instrumented.
       @returns the file name of the instrumented source file.
     */
    std::string
    instrument_pdt( std::string source_file );

    /**
       Instruments @a source_file with Opari.
       @param source_file File name of the source file, that is instrumented.
       @returns the file name of the instrumented source file.
     */
    std::string
    instrument_opari( std::string source_file );

    /**
       Performs the necessary actions for linking Opari instrumented object
       files and libraries. Thus, it runs the awk script on the objects,
       creates the POMP2_Init Function, and compiles it.
     */
    void
    prepare_opari_linking();

    /**
       Constructs calls to the config tools.
       @param input_file The input file for which the calls are generated.
     */
    void
    prepare_config_tool_calls( std::string input_file );

    /**
       Invokes the opari tool to instrument a source file.
       @param input_file  Source file which is instrumented.
       @param output_file Filename for the instrumented source file
     */
    void
    invoke_opari( std::string input_file,
                  std::string output_file );

    /**
       Runs a script on a list of object files to generate the Pomp_Init
       function.
       @param object_files A list of space separated object file names.
       @param output_file  Filename for the generated source file.
     */
    void
    invoke_awk_script( std::string object_files,
                       std::string output_file );

    /**
       Compiles the generated source file.
       @param input_file  Source file which is compiled.
       @param output_file Filename for the obejct file.
     */
    void
    compile_init_file( std::string input_file,
                       std::string
                       output_file );

    /**
       Compiles a users source file. If the original command compile and
       link in one step, we need to split compilation and linking, because
       we need to run the script on the object files. Thus, we do already
       compile the source. It uses the compiler used by the user command,
       appends compiler flags given by the user.
       @param input_file  Source file which is compiled.
       @param output_file Filename for the obejct file.
     */
    void
    compile_source_file( std::string input_file,
                         std::string
                         output_file );

    /**
       Invoke the binary instrumenter Cobi. Instruments the binary
       @a orig_name. The instrumented file will be named @a output_name.
       @param orig_name   The name of the uninstrumented exectable
       @param output_name The name of the instrumented executable
       @return The return value of the cobi execution.
     */
    int
    invoke_cobi( std::string orig_name,
                 std::string output_name );

    /**
       Removes temorarily created files.
     */
    void
    clean_temp_files();

    /**
       Executes the command specified by @ command. It checks verbosity level
       and whether it is a dry run.
     */
    void
    execute_command( std::string command );

    /* ***************************************************** Private members */
private:

    /**
       Installation configuration data set.
     */
    SCOREP_Instrumenter_InstallData* m_install_data;

    /**
       The command line parser.
     */
    SCOREP_Instrumenter_CmdLine* m_command_line;

    /**
       input file names. Need to be separated because OPARI may
       perform source code modifications which take these as input and
       the original command needs the result from the OPRI output. Thus,
       they are then substituted by the OPRI output.
     */
    std::string m_input_files;

    /**
       A list of temorarily created files that are deleted at the end of a
       successful execution, if @a keep_files is false.
     */
    std::string m_temp_files;

    /**
       Additional compiler flgas added by the instrumenter
     */
    std::string m_compiler_flags;

    /**
       Additional linker flags added by the instrumenter
     */
    std::string m_linker_flags;
};
#endif /*SCOREP_INSTRUMENTER_H_*/
