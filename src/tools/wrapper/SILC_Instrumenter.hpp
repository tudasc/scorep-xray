/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 * @file       SILC_Instrumenter.hpp
 * @maintainer Daniel Lorenz  <d.lorenz@fz-juelich.de>
 *
 * @brief      Class to steer the instrumentation of the user code.
 */

#ifndef SILC_INSTRUMENTER_H_
#define SILC_INSTRUMENTER_H_

#include <iostream>
#include <string>


#include "SILC_Application.hpp"

/* ****************************************************************************
   Class SILC_Instrumenter
******************************************************************************/
/**
 *  @brief performes instrumentation stage
 *
 *  This class examines the available compiler settings and the type of
 *  instrumentation. Makes the necessary modifications to the user command
 *  for instrumentation and executed the user command.
 */
class SILC_Instrumenter : public SILC_Application
{
    /* ******************************************************* Private Types */
private:
    /**
       Type for the three values a adapter or paradigm configuration can have.
       Before a command can be executed, a decision must be made for all
       adapter and paradigms which are in detect state.
     */
    typedef enum
    {
        enabled,
        detect,
        disabled
    } instrumentation_usage_t;

    /**
       Type of the state of the command line parser. The parser starts in
       state silc_parse_mode_param which means that arguments are interpreted
       as options of the wrapper tool. When the first argument is reached
       which has no leading dash it assumes that this is the compiler or
       linker command and changed to silc_parse_mode_command. All further
       arguments are interpreted as arguments for the compiler/linker.
       The states silc_parse_mode_output, and silc_parse_mode_config are used
       to deal with arguments which reguire a value in a successive argument.
       Thus, if A user specifies a config file the state switches to
       silc_parse_mode_config. The next argument is interpreted as the
       config file name. Then the state switches back to silc_parse_mode_param.
       If the user command contains a '-o' the following argument is
       interpreted as the output file name and the state switches to
       silc_parse_mode_output.
     */
    typedef enum
    {
        silc_parse_mode_param,
        silc_parse_mode_command,
        silc_parse_mode_output,
        silc_parse_mode_config
    } silc_parse_mode_t;
    /* ****************************************************** Public methods */
public:

    /**
       Creates a new SILC_Instrumenter object.
     */
    SILC_Instrumenter();

    /**
       Destroys a SILC_Instrumenter object.
     */
    virtual ~
    SILC_Instrumenter();

    /**
       Performs the instrumentation of an application
     */
    virtual int
    Run();

    /**
       Parses the command line.
       @param argc The number of arguments.
       @param argv List of arguments. It assumes, that the first argument is
                   the tool name and the second argument is the action.
       @return SILC_SUCCESS if the parsing was successful. Else an error
               code is returned.
     */
    virtual SILC_Error_Code
    ParseCmdLine( int    argc,
                  char** argv );

    /**
       Prints the results from parsing the command line and parsing the
       configuration file to screen.
     */
    virtual void
    PrintParameter();

    /* *************************************************** Protected methods */
protected:
    /**
       This function is called from ReadConfigFile. It should set the
       compiler instrumentation flags.
       @param flags A string containing the instrumentation flags.
     */
    virtual void
    SetCompilerFlags( std::string flags );

    /**
       This function is called from ReadConfigFile. It should add one include
       directory.
       @param dir One directory name.
     */
    virtual void
    AddIncDir( std::string dir );

    /**
       This function is called from ReadConfigFile. It should add one library
       directory.
       @param dir One directory name.
     */
    virtual void
    AddLibDir( std::string dir );

    /**
       This function is called from ReadConfigFile. It should add libraries.
       The string is of format -l<name>.
       @param dir One or more libraries.
     */
    virtual void
    AddLib( std::string lib );

    /**
       This function is called from ReadConfigFile. It should set the C
       compiler.
       @param value Name of the C compiler.
     */
    virtual void
    SetCompiler( std::string value );

    /**
       This function is called from ReadConfigFile. It should set the nm.
       @param value nm
     */
    virtual void
    SetNm( std::string value );

    /**
       This function is called from ReadConfigFile. It should set the awk.
       @param value awk
     */
    virtual void
    SetAwk( std::string value );

    /**
       This function is called from ReadConfigFile. It should set Opari.
       @param value opari
     */
    virtual void
    SetOpari( std::string value );

    /* ***************************************************** Private methods */
private:

    /**
       Executes the modified user command.
       @returns the return value from the executed command.
     */
    int
    execute_command();

    /**
       Checks whether command line parameter parsing provided meaningful
       information, applies remaining detection decisions.
     */
    void
    check_parameter();

    /**
       Evaluates one parameter when in output mode.
       @param arg The current argument
       @returns the parsing mode for the next parameter.
     */
    silc_parse_mode_t
    parse_output( std::string arg );

    /**
       Evaluates one parameter when in command mode.
       @param arg The current argument
       @returns the parsing mode for the next parameter.
     */
    silc_parse_mode_t
    parse_command( std::string arg );

    /**
       Evaluates one parameter when in parameter mode.
       @param arg The current argument
       @returns the parsing mode for the next parameter.
     */
    silc_parse_mode_t
    parse_parameter( std::string arg );

    /**
       Evaluates one parameter when in config mode.
       @param arg The current argument
       @returns the parsing mode for the next parameter.
     */
    silc_parse_mode_t
    parse_config( std::string arg );

    /**
       Performs necessary modifications to the command for enabling
       compiler instrumentation.
     */
    void
    prepare_compiler();

    /**
       Performs necessary modifications to the command for enabling
       manual user instrumentation.
     */
    void
    prepare_user();

    /**
       Performs necessary modifications to the command for enabling
       Opari instrumentation.
     */
    void
    prepare_opari();

    /**
       Tries to open a configuration file for instrumentation.
       @inFile Pointer to a stream which returns the open file.
       @returns SILC_SUCCESS if the file was successfully opened.
     */
    SILC_Error_Code
    open_config_file( std::ifstream* inFile );

    /**
       Reads the configuration data from a file.
       @returns SILC_SUCCESS if the file was successfully parsed.
     */
    SILC_Error_Code
    read_config_file();

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
        Retrieves the extension from a filename.
        @param filename  A filename
        @retuns the extension including the dot. If no extension is given
                (no dot appears in the filename) an empty string is returned.
     */
    std::string
    get_extension( std::string filename );

    /**
       Retrieves the basename from a filename. It removes the extension
       including the last dot from a filename.
       @param filename  A filename
       @retuns the basename. If no extension is given
               (no dot appears in the filename) the whole filename is returned.
     */
    std::string
    get_basename( std::string filename );

    /** Checks whether a file is a source file.
        @param filename A file name.
        @returns true if the file extension indicates a C/C++ or Fortran source
                 file.
     */
    bool
    is_source_file( std::string filename );

    /** Checks whether a file is an object file.
        @param filename A file name.
        @returns true if the file extension indicates an object file.
     */
    bool
    is_object_file( std::string filename );

    /* ***************************************************** Private members */
private:
    /* --------------------------------------------
       Flags for used adapters
       ------------------------------------------*/
    /**
       Specifies if compiler instrumentation is enabled. Default is enabled
     */
    instrumentation_usage_t compiler_instrumentation;

    /**
       Specifies if OPARI instrumentation is enabled. Default detect.
     */
    instrumentation_usage_t opari_instrumentation;

    /**
       Specifies if user instrumentation is enabled. Default is disabled.
     */
    instrumentation_usage_t user_instrumentation;

    /**
       Specifies if mpi wrappers are enabled. Default detect.
     */
    instrumentation_usage_t mpi_instrumentation;

    /* --------------------------------------------
       Flags for application type
       ------------------------------------------*/
    /**
       Specifies whether it is a MPI application.
     */
    instrumentation_usage_t is_mpi_application;

    /**
       Specifies whether it is an OpenMP application.
     */
    instrumentation_usage_t is_openmp_application;

    /* --------------------------------------------
       Work mode information
       ------------------------------------------*/
    /**
       True if compiling
     */
    bool is_compiling;

    /**
       True if linking
     */
    bool is_linking;

    /* --------------------------------------------
       Input command elements
       ------------------------------------------*/
    /**
        compiler/linker name
     */
    std::string compiler_name;

    /**
       compiler/linker flags
     */
    std::string compiler_flags;

    /**
       file name of the compiler/linker output
     */
    std::string output_name;

    /**
       input file names. Need to be separated because OPARI may
       perform source code modifications which take these as input and
       the original command needs the result from the OPRI output. Thus,
       they are then substituted by the OPRI output.
     */
    std::string input_files;

    /* --------------------------------------------
       Config file data
       ------------------------------------------*/
    /**
       Stores compiler instruemntation flags
     */
    std::string compiler_instrumentation_flags;

    /**
       Stores include path of SILC header files
     */
    std::string silc_include_path;

    /**
       Stores library path of SILC libraries
     */
    std::string silc_library_path;

    /**
       Stores external dependency libraries of the SILC library
     */
    std::string external_libs;

    /**
       C compiler
     */
    std::string c_compiler;

    /**
       nm
     */
    std::string nm;

    /**
       awk
     */
    std::string awk;

    /**
       Opari
     */
    std::string opari;
};

#endif /*SILC_INSTRUMENTER_H_*/
