/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
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

#include <iostream>
#include <string>

#include <scorep_utility/SCOREP_ParserBase.hpp>

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
class SCOREP_Instrumenter : public SCOREP_ParserBase
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
       Type for possible source code languages.
     */
    typedef enum
    {
        unknown_language,
        c_language,
        cpp_language,
        fortran_language,
        cuda_language
    } source_language_t;

    /**
       Type of the state of the command line parser. The parser starts in
       state scorep_parse_mode_param which means that arguments are interpreted
       as options of the wrapper tool. When the first argument is reached
       which has no leading dash it assumes that this is the compiler or
       linker command and changed to scorep_parse_mode_command. All further
       arguments are interpreted as arguments for the compiler/linker.
       The states scorep_parse_mode_output, and scorep_parse_mode_config are used
       to deal with arguments which reguire a value in a successive argument.
       Thus, if A user specifies a config file the state switches to
       scorep_parse_mode_config. The next argument is interpreted as the
       config file name. Then the state switches back to scorep_parse_mode_param.
       If the user command contains a '-o' the following argument is
       interpreted as the output file name and the state switches to
       scorep_parse_mode_output.
     */
    typedef enum
    {
        scorep_parse_mode_param,
        scorep_parse_mode_command,
        scorep_parse_mode_output,
        scorep_parse_mode_config,
        scorep_parse_mode_library,
        scorep_parse_mode_define,
        scorep_parse_mode_libdir,
        scorep_parse_mode_incdir
    } scorep_parse_mode_t;
    /* ****************************************************** Public methods */
public:

    /**
       Creates a new SCOREP_Instrumenter object.
     */
    SCOREP_Instrumenter();

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

    /**
       Parses the command line.
       @param argc The number of arguments.
       @param argv List of arguments. It assumes, that the first argument is
                   the tool name and the second argument is the action.
       @return SCOREP_SUCCESS if the parsing was successful. Else an error
               code is returned.
     */
    virtual SCOREP_Error_Code
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
       This function gives a (key, value) pair found in a configuration file and not
       processed by one of the former functions.
       @param key   The key
       @param value The value
     */
    virtual void
    SetValue( std::string key,
              std::string value );

    /* ***************************************************** Private methods */
private:

    /**
       This function processes a setting of the PDT path from config file.
       @param pdt A string containing the binary directory of the PDT installation.
     */
    void
    set_pdt_path( std::string pdt );

    /**
       Executes the linking command.
       @returns the return value from the executed command.
     */
    int
    link_step();

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
    scorep_parse_mode_t
    parse_output( std::string arg );

    /**
       Evaluates one parameter when in command mode.
       @param arg The current argument
       @returns the parsing mode for the next parameter.
     */
    scorep_parse_mode_t
    parse_command( std::string arg );

    /**
       Evaluates one parameter when in parameter mode.
       @param arg The current argument
       @returns the parsing mode for the next parameter.
     */
    scorep_parse_mode_t
    parse_parameter( std::string arg );

    /**
       Evaluates one parameter when in config mode.
       @param arg The current argument
       @returns the parsing mode for the next parameter.
     */
    scorep_parse_mode_t
    parse_config( std::string arg );

    /**
       Evaluates one parameter when in libray mode.
       @param arg The current argument
       @returns the parsing mode for the next parameter.
     */
    scorep_parse_mode_t
    parse_library( std::string arg );

    /**
       Evaluates one parameter when in define mode.
       @param arg The current argument
       @returns the parsing mode for the next parameter.
     */
    scorep_parse_mode_t
    parse_define( std::string arg );

    /**
       Evaluates one parameter when in incdir mode.
       @param arg The current argument
       @returns the parsing mode for the next parameter.
     */
    scorep_parse_mode_t
    parse_incdir( std::string arg );

    /**
       Evaluates one parameter when in libdir mode.
       @param arg The current argument
       @returns the parsing mode for the next parameter.
     */
    scorep_parse_mode_t
    parse_libdir( std::string arg );

    /**
       Processes a define parameter.
       @param arg The define argument.
     */
    void
    add_define( std::string arg );

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
       Performs the necessary actions for linking Opari instrumented object files and
       libraries. Thus, it runs the awk script on the objects, creates the POMP2_Init
       Function, and compiles it.
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
       Tries to open a configuration file for instrumentation.
       @inFile Pointer to a stream which returns the open file.
       @returns SCOREP_SUCCESS if the file was successfully opened.
     */
    SCOREP_Error_Code
    open_config_file( std::ifstream* inFile );

    /**
       Reads the configuration data from a file.
       @returns SCOREP_SUCCESS if the file was successfully parsed.
     */
    SCOREP_Error_Code
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

    /** Checks whether a file is a Fortran source file.
        @param filename A file name.
        @returns true if the file extension indicates Fortran source file.
     */
    bool
    is_fortran_file( std::string filename );

    /** Checks whether a file is a C source file.
        @param filename A file name.
        @returns true if the file extension indicates C source file.
     */
    bool
    is_c_file( std::string filename );

    /** Checks whether a file is a C++ source file.
        @param filename A file name.
        @returns true if the file extension indicates C++ source file.
     */
    bool
    is_cpp_file( std::string filename );

    /** Checks whether a file is a CUDA source file.
        @param filename A file name.
        @returns true if the file extension indicates CUDA source file.
     */
    bool
    is_cuda_file( std::string filename );

    /** Checks whether a file is an object file.
        @param filename A file name.
        @returns true if the file extension indicates an object file.
     */
    bool
    is_object_file( std::string filename );

    /** Checks whether a file is a library.
        @param filename A file name.
        @returns true if the file extension indicates an object file.
     */
    bool
    is_library( std::string filename );

    /**
       Removes everything before the first slash from @a full_path.
       @param a file with its full path.
       @returns The filename without its path.
     */
    std::string
    remove_path( std::string full_path );

    /**
       Invoke the binary instrumenter Cobi. Instruments the binary @a orig_name. The
       instrumented file will be named @a output_name.
       @param orig_name The name of the uninstrumented exectable
       @return The return value of the cobi execution.
     */
    int
    invoke_cobi( std::string orig_name );

    /**
       Removes temorarily created files.
     */
    void
    clean_temp_files();

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

    /**
       Specifies if pdt instrumentation enabled. Default disabled. If it is enabled,
       it will automatically enable the user adapter and disable the compiler
       instrumentation.
     */
    instrumentation_usage_t pdt_instrumentation;

    /**
       Specifies if binary instrumentation with Cobi is enabled.
     */
    instrumentation_usage_t cobi_instrumentation;

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

    /**
       Specifies whether it is an CUDA application.
     */
    instrumentation_usage_t is_cuda_application;

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
       all compiler/linker flags, except defines.
     */
    std::string compiler_flags;

    /**
       include flags
     */
    std::string include_flags;

    /**
       define flags
     */
    std::string define_flags;

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

    /**
       number of input file names.
     */
    int input_file_number;

    /**
       Source code language of input files.
     */
    source_language_t language;

    /**
       True, if -lmpi was specified.
     */
    bool lmpi_set;

    /**
         The argv[0] value , to get the own binary
     */
    std::string scorep_bin;

    /* --------------------------------------------
       Config file data
       ------------------------------------------*/
    /**
       Stores compiler instruemntation flags
     */
    std::string compiler_instrumentation_flags;

    /**
       Stores C compiler OpenMP flags
     */
    std::string openmp_cflags;

    /**
       Stores additional Score-P flags
     */
    std::string scorep_flags;

    /**
       Stores SCOREP library linking flags
     */
    std::string scorep_libs;

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

    /**
       grep
     */
    std::string grep;

    /**
       cobi
     */
    std::string cobi;

    /**
       awk script used to generate functions for initialization of Opari
       instrumented regions.
     */
    std::string opari_script;

    /**
       Opari2 config tool
     */
    std::string opari_config;

    /**
        Path to PDT binaries.
     */
    std::string pdt_bin_path;

    /**
        PDT instrumentation specificaion file.
     */
    std::string pdt_config_file;

    /**
        Name of the scorep_config tool.
     */
    std::string scorep_config;

    /**
        Directory of the Cobi configuration files
     */
    std::string cobi_config_dir;

    /* --------------------------------------------
       Execution flags
       ------------------------------------------*/
    /**
       True, if the instrumentation is a dry run. It means that the commands are
       only printed to stdout but are not executed. The default is false.
     */
    bool is_dry_run;

    /**
       In case we compile multiple source files that need preprocessing with Opari,
       all files are already compiled during the Opari preprocssing. This flag indicate
       that the final command execution step is omitted.
     */
    bool no_final_step;

    /* --------------------------------------------
       Clean up information
       ------------------------------------------*/
    /**
       True, if temporary files should be kept. By default temprary files are deleted
       after successful instrumentation, but kept when the instrumenter aborts with an
       error.
     */
    bool keep_files;

    /**
       A list of temorarily created files that are deleted at the end of a successful
       execution, if @a keep_files is false.
     */
    std::string temp_files;
};

#endif /*SCOREP_INSTRUMENTER_H_*/
