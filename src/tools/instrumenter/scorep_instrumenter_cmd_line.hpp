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
 * @file       scorep_instrumenter_command_line.hpp
 *
 * @brief      Declares a class for parsing and evaluating a the command line
 *             and makes the results accessable.
 */

#ifndef SCOREP_INSTRUMENTER_COMMAND_LINE_H_
#define SCOREP_INSTRUMENTER_COMMAND_LINE_H_


#include <string>
#include "scorep_instrumenter_install_data.hpp"

/* ****************************************************************************
   class SCOREP_Instrumenter_CmdLine
******************************************************************************/

class SCOREP_Instrumenter_CmdLine
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
        scorep_parse_mode_option_part
    } scorep_parse_mode_t;

    /* ****************************************************** Public methods */
public:

    /**
       Creates a new SCOREP_Instrumenter_CmdLine object.
       @param install_data Pointer to a installation configuration data
                           object.
     */
    SCOREP_Instrumenter_CmdLine( SCOREP_Instrumenter_InstallData& install_data );


    /**
       Destroys a SCOREP_Instrumenter_CmdLine object.
     */
    virtual
    ~SCOREP_Instrumenter_CmdLine();

    /**
       Parses the command line.
       @param argc The number of arguments.
       @param argv List of arguments. It assumes, that the first argument is
                   the tool name and the second argument is the action.
       @return SCOREP_SUCCESS if the parsing was successful. Else an error
               code is returned.
     */
    virtual void
    ParseCmdLine( int    argc,
                  char** argv );

    bool
    isNvccCompiler( void );
    bool
    isCompiling( void );
    bool
    isLinking( void );
    bool
    noCompileLink( void );
    std::string
    getCompilerName( void );
    std::string
    getFlagsBeforeLmpi( void );
    std::string
    getFlagsAfterLmpi( void );
    std::string
    getIncludeFlags( void );
    std::string
    getDefineFlags( void );
    std::string
    getOutputName( void );
    std::string
    getInputFiles( void );
    std::string
    getLibraries( void );
    std::string
    getLibDirs( void );
    int
    getInputFileNumber( void );
    bool
    isLmpiSet( void );
    bool
    isDryRun( void );
    bool
    hasKeepFiles( void );
    int
    getVerbosity( void );
    bool
    isBuildCheck( void );
    std::string
    getPdtParams( void );
    bool
    enforceStaticLinking( void );
    bool
    enforceDynamicLinking( void );

    /**
       Returns true if the link target is a shared library.
     */
    bool
    isTargetSharedLib( void );

    /**
       Returns the list of full filenames for all libraries specified via -l flags.
     */
    std::string
    getLibraryFiles( void );

#if defined( SCOREP_SHARED_BUILD )
    /**
     * Returns true if the user provided --no-as-needed on the command
     * line. Temporary feature to work around linking failures on
     * systems that have --as-needed as linker default. See ticket:385
     */
    bool
    getNoAsNeeded( void );

#endif

    SCOREP_Instrumenter_InstallData*
    getInstallData();

    /* ***************************************************** Private methods */
private:

    /**
       Prints the results from parsing the command line and parsing the
       configuration file to screen.
     */
    virtual void
    print_parameter( void );

    /**
       Checks whether command line parameter parsing provided meaningful
       information, applies remaining detection decisions.
     */
    void
    check_parameter( void );

    /**
       Evaluates one parameter when in output mode.
       @param arg The current argument
       @returns the parsing mode for the next parameter.
     */
    scorep_parse_mode_t
    parse_output( const std::string& arg );

    /**
       Evaluates one parameter when in command mode.
       @param current  The current argument
       @param next     The next argument
       @returns the parsing mode for the next parameter. If it is
                scorep_parse_mode_command the next parameter is
                regularly handled. If it is scorep_parse_mode_option_part
                the next parameter was a value specification for the current
                and is already processed.
     */
    scorep_parse_mode_t
    parse_command( const std::string& current,
                   const std::string& next );

    /**
       Evaluates one parameter when in parameter mode.
       @param arg The current argument
       @returns the parsing mode for the next parameter.
     */
    scorep_parse_mode_t
    parse_parameter( const std::string& arg );

    /**
       Processes a define parameter.
       @param arg The define argument.
     */
    void
    add_define( std::string arg );

    /* ***************************************************** Private members */
private:
    /**
       Pointer to the associated installation configration data
     */
    SCOREP_Instrumenter_InstallData& m_install_data;

    /* --------------------------------------------
       Work mode information
       ------------------------------------------*/
    /**
       True is the link target is a shared library.
     */
    bool m_target_is_shared_lib;

    /**
       True if compiling
     */
    bool m_is_compiling;

    /**
       True if linking
     */
    bool m_is_linking;

    /**
       True if the command should not be modified, e.g., because the command
       does only preprocessing or dependency generation
     */
    bool m_no_compile_link;

    /**
       Specification of static or dynamic linking of Score-P libraries into the
       application.
       Enabled means link static.
       Detect means use compiler default.
       Disabeld means link dynamic libraries if possible.
     */
    instrumentation_usage_t m_link_static;

    /* --------------------------------------------
       Input command elements
       ------------------------------------------*/
    /**
        compiler/linker name
     */
    std::string m_compiler_name;

    /**
       all compiler/linker flags, before an explicit -lmpi,
       except source files, object files, -c and -o options.
     */
    std::string m_flags_before_lmpi;

    /**
       all compiler/linker flags, after an explicit -lmpi,
       except source files, object files, -c and -o options.
     */
    std::string m_flags_after_lmpi;

    /**
       Pointers to @a m_flags_before_lmpi or @a m_flags_after_lmpi, depending
       whether -lmpi already occured.
     */
    std::string* m_current_flags;

    /**
       include flags
     */
    std::string m_include_flags;

    /**
       define flags
     */
    std::string m_define_flags;

    /**
       file name of the compiler/linker output
     */
    std::string m_output_name;

    /**
       library names
     */
    std::string m_libraries;

    /**
       libdirs
     */
    std::string m_libdirs;

    /**
       input file names. Need to be separated because OPARI may
       perform source code modifications which take these as input and
       the original command needs the result from the OPRI output. Thus,
       they are then substituted by the OPRI output.
     */
    std::string m_input_files;

    /**
       number of input file names.
     */
    int m_input_file_number;

    /**
       True, if -lmpi was specified.
     */
    bool m_lmpi_set;

    /* --------------------------------------------
       Instrumenter flags
       ------------------------------------------*/
    /**
       True, if the instrumentation is a dry run. It means that the commands
       are only printed to stdout but are not executed. The default is false.
     */
    bool m_is_dry_run;

    /**
       True, if temporary files should be kept. By default temprary files are
       deleted after successful instrumentation, but kept when the instrumenter
       aborts with an error.
     */
    bool m_keep_files;

    /**
       The level of verbosity. Currently, we know the levels:
       <ul>
       <li> 0 = No output
       <li> 1 = Print executed commands
       <li> 2 = Print executed commands plus further diagnostic output
       </ul>
     */
    int m_verbosity;

    /**
       True, if this is a build check is run. Does not assume to use data
       from an instrumented run, but from the build location.
     */
    bool m_is_build_check;

#if defined( SCOREP_SHARED_BUILD )
    /**
     * True if the user provided --no-as-needed on the command line.
     */
    bool m_no_as_needed;
#endif
};

#endif
