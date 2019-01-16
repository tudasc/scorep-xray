/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014, 2016-2017,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013, 2015,
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
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_INSTRUMENTER_COMMAND_LINE_HPP
#define SCOREP_INSTRUMENTER_COMMAND_LINE_HPP

/**
 * @file
 *
 * @brief      Declares a class for parsing and evaluating a the command line
 *             and makes the results accessible.
 */

#include <string>
#include <vector>
#include "scorep_instrumenter_install_data.hpp"

/* ****************************************************************************
   class SCOREP_Instrumenter_CmdLine
******************************************************************************/

class SCOREP_Instrumenter_CmdLine
{
    /* ******************************************************* Private Types */
private:
    /**
       Type for the three values an adapter or paradigm configuration
       can have.  Before a command can be executed, a decision must be
       made for all adapters and paradigms which are in detect state.
     */
    typedef enum
    {
        enabled,
        detect,
        disabled
    } instrumentation_usage_t;

    /**
       Type of the state of the command line parser. The parser starts
       in state scorep_parse_mode_param which means that arguments are
       interpreted as options of the wrapper tool. When the first
       argument is reached which has no leading dash it assumes that
       this is the compiler or linker command. Then the parser chages to
       scorep_parse_mode_wrapper_option to check whether the next option(s)
       pass information to a compiler wrapper. On the first, option that
       does not belong to the wrapper itself, the mode changes to
       scorep_parse_mode_command. All further arguments are
       interpreted as arguments for the compiler/linker. The states
       scorep_parse_mode_output, and scorep_parse_mode_config are used
       to deal with arguments which require a value in a successive
       argument. Thus, if A user specifies a config file the state
       switches to scorep_parse_mode_config. The next argument is
       interpreted as the config file name. Then the state switches
       back to scorep_parse_mode_param. If the user command contains a
       '-o' the following argument is interpreted as the output file
       name and the state switches to scorep_parse_mode_output.
     */
    typedef enum
    {
        scorep_parse_mode_param,
        scorep_parse_mode_command,
        scorep_parse_mode_option_part,
        scorep_parse_mode_wrapper_option
    } scorep_parse_mode_t;

public:
    /**
       Type to define the state of the preprocessing.

       DISABLE means that any preprocessing is disabled, e.g.,
               because we are working on already preprocessed files.

       IN_COMPILE_STEP means that the compile step will also
               do preprocessing and we apply our preprocessing
               instrumentation, but do not need a separate preprocessing
               invocation of the compiler.

       EXPLICIT_STEP means that we perform an explicit preprocessing step.
               Either because the user command is only a preprocessing
               command, or because we need to preprocess files, e.g. before
               OAPRI2 instrumentation.
     */
    typedef enum
    {
        DISABLE,
        IN_COMPILE_STEP,
        EXPLICIT_STEP
    } scorep_preprocess_mode_t;

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

    /**
        @return m_path_to_binary
     */
    const std::string&
    getPathToBinary( void );
    std::string
    getPathToSrc( void ) const;

    void
    enableSeparatePreprocessingStep( void );
    scorep_preprocess_mode_t
    getPreprocessMode( void );
    bool
    isCompiling( void );
    bool
    isLinking( void );
    bool
    doNothing( void );
    std::string
    getCompilerName( void );
    std::string
    getFlagsBeforeInterpositionLib( void );
    std::string
    getFlagsAfterInterpositionLib( void );
    std::string
    getIncludeFlags( void );
    std::string
    getDefineFlags( void );
    std::string
    getOutputName( void );

    std::vector<std::string>*
    getInputFiles( void );
    int
    getInputFileNumber( void );
    bool
    isInterpositionLibSet( void );

#if SCOREP_BACKEND_COMPILER_INTEL
    bool
    isMmicSet( void );

#endif  /* SCOREP_BACKEND_COMPILER_INTEL */

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
       Returns the list of full file names for all libraries specified via -l flags.
     */
    std::string
    getLibraryFiles( bool allow_dynamic = true,
                     bool allow_static = true );

    const std::vector<std::string>&
    getInstrumentFilterFiles( void ) const;

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

    /**
     * Removes @a argument given by the user from on the command line from any further
     * invocations of the compiler. If @a argument is not present in the command line,
     * this function does nothing.
     * @param argument  The flag that is removed from the command line.
     */
    void
    removeUserArg( const std::string& argument );

    /**
       Checks whether command line parameter parsing provided meaningful
       information, applies remaining detection decisions.
     */
    void
    checkParameter( void );

    /* ***************************************************** Private methods */
private:

    /**
       Prints the results from parsing the command line and parsing the
       configuration file to screen.
     */
    virtual void
    print_parameter( void );

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
       Evaluates one parameter when in wrapper option mode
       @param current  The current argument
       @param next     The next argument
       @returns the parsing mode for the next parameter.
     */
    scorep_parse_mode_t
    parse_wrapper_option( const std::string& current,
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

    /**
       Adds a file name to the list of input files.
       @param input_file The file name of the input file.
     */
    void
    add_input_file( const std::string& input_file );

    /**
       Sets the output file name.
       @param output_file The file name of the output file.
     */
    void
    set_output_file( const std::string& output_file );

    /**
       Adds a include path to the list of include paths.
       @param include_path The new include path.
     */
    void
    add_include_path( const std::string& inlcude_path );

    /**
       Adds a library path to the list of library paths.
       @param library_path  The new library path.
     */
    void
    add_library_path( const std::string& library_path );

    /**
       Adds a library to the list of libraries.
       @param library  The new library.
     */
    void
    add_library( const std::string& library );

    /**
       Checks whether the library name is a library we want to interpose.
       @param libraryName  The name of the library.
     */
    bool
    is_interposition_library( const std::string& libraryName );

    /* ***************************************************** Private members */
private:
    /**
       Path to this binary. Extracted from argv[0] and includes the final slash.
       Empty string if the binary was called via $PATH.
     */
    std::string m_path_to_binary;

    /**
       Reference to the associated installation configuration data
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
       True, if we need to perform an explicit preprocessing step
     */
    scorep_preprocess_mode_t m_preprocess_mode;

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
       does only dependency generation
     */
    bool m_do_nothing;

    /**
       Specification of static or dynamic linking of Score-P libraries into the
       application.
       Enabled:  link static.
       Detect:   use compiler default.
       Disabled: link dynamic libraries if possible.
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
       all compiler/linker flags, before an explicit interposition lib (e.g., -lmpi, -lshmem),
       except source files, object files, -c and -o options.
     */
    std::string m_flags_before_interposition_lib;

    /**
       all compiler/linker flags, after an explicit interposition lib (e.g., -lmpi, -lshmem),
       except source files, object files, -c and -o options.
     */
    std::string m_flags_after_interposition_lib;

    /**
       Pointers to @a m_flags_before_interposition_lib or @a m_flags_after_interposition_lib, depending
       whether an interposition lib (e.g. -lmpi) already occurred.
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
    std::vector<std::string> m_libraries;

    /**
       libdirs
     */
    std::vector<std::string> m_libdirs;

    /**
       input file names
     */
    std::vector<std::string> m_input_files;

    /**
       instrument filter files
     */
    std::vector<std::string> m_filters;

    /**
       number of input file names.
     */
    //int m_input_file_number;

    /**
       True, if an interposition lib (e.g., -lmpi) was specified.
     */
    bool m_interposition_lib_set;

#if SCOREP_BACKEND_COMPILER_INTEL
    /**
       True, if -mmic was specified.
     */
    bool m_mmic_set;
#endif  /* SCOREP_BACKEND_COMPILER_INTEL */

    /* --------------------------------------------
       Instrumenter flags
       ------------------------------------------*/
    /**
       True, if the instrumentation is a dry run. It means that the commands
       are only printed to stdout but are not executed. The default is false.
     */
    bool m_is_dry_run;

    /**
       True, if temporary files should be kept. By default temporary files are
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

#endif // SCOREP_INSTRUMENTER_COMMAND_LINE_HPP
