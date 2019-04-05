/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2014-2017,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_INSTRUMENTER_ADAPTER_HPP
#define SCOREP_INSTRUMENTER_ADAPTER_HPP

/**
 * @file
 *
 * Defines the base class for an instrumentation method.
 */

#include <string>
#include <map>
#include <deque>
#include <stdint.h>

/* **************************************************************************************
 * Typedefs
 * *************************************************************************************/

/**
 * Defines an identifier for every explicit instrumentation approach.
 */
typedef enum
{
    SCOREP_INSTRUMENTER_ADAPTER_COBI,
    SCOREP_INSTRUMENTER_ADAPTER_COMPILER,
    SCOREP_INSTRUMENTER_ADAPTER_CUDA,
    SCOREP_INSTRUMENTER_ADAPTER_ONLINE_ACCESS,
    SCOREP_INSTRUMENTER_ADAPTER_OPARI,
    SCOREP_INSTRUMENTER_ADAPTER_PDT,
    SCOREP_INSTRUMENTER_ADAPTER_PREPROCESS,
    SCOREP_INSTRUMENTER_ADAPTER_USER,
    SCOREP_INSTRUMENTER_ADAPTER_PTHREAD,
    SCOREP_INSTRUMENTER_ADAPTER_OPENCL,
    SCOREP_INSTRUMENTER_ADAPTER_OPENACC,
    SCOREP_INSTRUMENTER_ADAPTER_MEMORY,
    SCOREP_INSTRUMENTER_ADAPTER_LINKTIME_WRAPPING,
    SCOREP_INSTRUMENTER_ADAPTER_LIBWRAP,

    /* Should stay last element in enum */
    SCOREP_INSTRUMENTER_ADAPTER_NUM
} SCOREP_Instrumenter_AdapterId;

/**
 * Type for creating adapter lists.
 */
typedef std::deque<SCOREP_Instrumenter_AdapterId> SCOREP_Instrumenter_DependencyList;


class SCOREP_Instrumenter_Adapter;
class SCOREP_Instrumenter;
class SCOREP_Instrumenter_CmdLine;

/**
 * Type to list available adapters.
 */
typedef std::map<SCOREP_Instrumenter_AdapterId, SCOREP_Instrumenter_Adapter*>
    SCOREP_Instrumenter_AdapterList;

/* **************************************************************************************
 * class SCOREP_Instrumenter_Adapter
 * *************************************************************************************/

/**
 * The class SCOREP_Instrumenter_Adapter represents a adapter inside the
 + scorep instrumenter. It contains the adapter specific data and algorithms
 * for the scorep instrumenter. This class is the basis from which specialized adapters
 * may derive their own class.
 */

class SCOREP_Instrumenter_Adapter
{
    /* ------------------------------------------------------------------------- types */
protected:
    /**
     * Type for the three values a adapter or paradigm configuration can have.
     * Before a command can be executed, a decision must be made for all
     * adapter and paradigms which are in detect state.
     */
    typedef enum
    {
        enabled,
        detect,
        disabled
    } instrumentation_usage_t;

    /* ----------------------------------------------------------------------- methods */
public:
    /**
     * Creates a SCOREP_Instrumenter_Adapter instance.
     * @param id    The identifier for this instrumentation method.
     * @param name  The name of the instrumentation method.
     */
    SCOREP_Instrumenter_Adapter( SCOREP_Instrumenter_AdapterId id,
                                 const std::string&            name );

    /**
     * Deletes a SCOREP_Instrumenter_Adapter instance.
     */
    virtual
    ~SCOREP_Instrumenter_Adapter();

    /**
     * Returns the identifier of the instrumentation method.
     */
    SCOREP_Instrumenter_AdapterId
    getId( void );

    /**
     * Ensures that required other adapters are enabled and that no
     * conflicting adapters are enabled.
     */
    virtual void
    checkDependencies( void );

    /**
     * Evaluate default relationships to other adapters.
     */
    virtual void
    checkDefaults( void );

    /**
     * Does this adapter make use of the instrumentation filter?
     */
    virtual bool
    supportInstrumentFilters( void ) const;

    /**
     * Prints the default help output for this adapter.
     */
    virtual void
    printHelp( void );

    /**
     * Evaluates an scorep option for this adapter.
     * By default, it enables the adapter if it encounters a parameter that matches
     * '--name' and disables the adapter if it encounters a parameter that matches
     * '--noname'. hereby you need to replace 'name' by the name of the adapter.
     * If an '=' follows the '--name' parameter, the string after the '=' character
     * is stored in m_params.
     * Overwrite this function if you need to process different or more parameters.
     * @param arg  The option that is evaluated.
     * @returns true, if this argument was known to this adapter.
     */
    virtual bool
    checkOption( const std::string& arg );

    /**
     * Returns whether this adapter is enabled.
     */
    virtual bool
    isEnabled( void ) const;

    /**
     * This function performs pre-preprocessing instrumentation activities. In
     * case only a preprocessor is invoked.
     * The default does nothing.
     * Overwrite this function if your adapter should perform any pre-preprocessing
     * activities. Do not forget to add the adapter to the pre-preprocess adapter list
     * in the instrumenter.
     * This function is not meant to set compiler flags. If you need to add some
     * compiler flags, please add them to the scorep-config output.
     * @param instrumenter  The instrumenter object.
     * @param cmdLine       The command line object.
     * @param source_file   The file name of the file that is going to be compiled.
     * @returns the name of the processed source file that should be used for
     *          further processing.
     */
    virtual std::string
    preprocess( SCOREP_Instrumenter&         instrumenter,
                SCOREP_Instrumenter_CmdLine& cmdLine,
                const std::string&           source_file );

    /**
     * This function performs pre-compiling instrumentation activities.
     * This operations may be applied to already preprocessed files.
     * The default does nothing.
     * Overwrite this function if your adapter should perform any pre-compiling
     * activities. Do not forget to add the adapter to the pre-compile adapter list
     * in the instrumenter.
     * This function is not meant to set compiler flags. If you need to add some
     * compiler flags, please add them to the scorep-config output.
     * @param instrumenter  The instrumenter object.
     * @param cmdLine       The command line object.
     * @param source_file   The file name of the file that is going to be compiled.
     * @returns the name of the processed source file that should be used for
     *          further processing.
     */
    virtual std::string
    precompile( SCOREP_Instrumenter&         instrumenter,
                SCOREP_Instrumenter_CmdLine& cmdLine,
                const std::string&           source_file );

    /**
     * This function performs pre-linking instrumentation activities.
     * The default does nothing.
     * Overwrite this function if your adapter should perform any pre-linking
     * activities. Do not forget to add the adapter to the pre-link adapter list
     * in the instrumenter.
     * This function is not meant to set linker flags. If you need to add some
     * linker flags, please add them to the scorep-config output.
     * @param instrumenter  The instrumenter object.
     * @param cmdLine       The command line object.
     */
    virtual void
    prelink( SCOREP_Instrumenter&         instrumenter,
             SCOREP_Instrumenter_CmdLine& cmdLine );

    /**
     * This function performs post-linking instrumentation activities.
     * The default does nothing.
     * Overwrite this function if your adapter should perform any post-linking
     * activities. Do not forget to add the adapter to the post-link adapter list
     * in the instrumenter.
     * @param instrumenter  The instrumenter object.
     * @param cmdLine       The command line object.
     */
    virtual void
    postlink( SCOREP_Instrumenter&         instrumenter,
              SCOREP_Instrumenter_CmdLine& cmdLine );

    /**
     * Returns the config tool flags. This flag must contain a leading space if it
     * is not empty. This function is called no matter whether the adapter is
     * enabled or not.
     * The default implementation returns the name of the adapter preceded by a
     * space and two dash (' --'). Overwrite this function, if you want to change
     * this behavior, e.g., if you do not pass anything at all.
     */
    virtual std::string
    getConfigToolFlag( SCOREP_Instrumenter_CmdLine& cmdLine );

    /**
     * Changes the behavior of the adapter to use not the installed files, but the
     * files in the source/build location. The default implementation does nothing.
     * Overwrite this function, if your adapter depends on files that have a different
     * location when installed than where built.
     */
    virtual void
    setBuildCheck( SCOREP_Instrumenter_CmdLine& cmdLine );

    /**
     * Processes a key value pair from a config file. The default implementation
     * does nothing.
     * Overwrite this function if your adapter can be configured by a config
     * file that a user passes in via the '--config' option.
     * @param key   The key that is specified.
     * @param value The value of this key.
     */
    virtual void
    setConfigValue( const std::string& key,
                    const std::string& value );

    /**
     * This function is called during command line parsing during parsing of the
     * user command. The default implementation ignores all arguments.
     * Overwrite this function if the adapter's behavior is influenced by options
     * of the compiler/link command.
     * @param current The current argument under evaluation from the command line.
     * @param next    The next argument of the command line. Sometimes, the current
     *                argument indicates that the next argument contains some value
     *                that belongs together.
     * @returns This function should return true, if the next parameter and the
     *          current parameter belong together and the next parameter was already
     *          processed by this call. Thus, the iteration for the next parameter
     *          is skipped.
     */
    virtual bool
    checkCommand( const std::string& current,
                  const std::string& next );

    /**
     * Checks the compiler name, which might affect the automatic adapter selection
     * or adapter configuration.
     * The default implementation does nothing. Overwrite this function, if your
     * adapter is affected by the compiler name.
     * @param compiler  The name of the compiler as it appears in the user command.
     */
    virtual void
    checkCompilerName( const std::string& compiler );

    /**
     * Called before the link step in all paradigms to perform object file based
     * checks.
     * @param instrumenter  Pointer to the instrumenter instance.
     */
    virtual void
    checkObjects( SCOREP_Instrumenter& instrumenter );

    /**
     * Return true, if the given library needs to be interposed
     */
    virtual bool
    isInterpositionLibrary( const std::string& libraryName );

protected:
    /**
     * If the instrumentation approach represented by this adapter is not
     * supported by this installation. This function should be called by the
     * constructor. It sets some variables to make sure that this adapter
     * cannot be enabled and prints meaning error message if it is.
     * Furthermore, it modifies the help output.
     */
    void
    unsupported( void );

    /**
     * Prints an error message that this installation does not support this
     * instrumentation approach and aborts.
     */
    void
    error_unsupported( void );

    /* ----------------------------------------------------------------------- members */
protected:
    /**
     * Stores the name of the instrumentation method.
     */
    std::string m_name;

    /**
     * Stores ids of adapters which are required by this adapter to work.
     * If the required adapters are not enabled, it will automatically
     * enable them if not explicitly disabled by the user. If the
     * required adapter was disabled by the user, the instrumenter will
     * quit with an error message.
     */
    SCOREP_Instrumenter_DependencyList m_requires;

    /**
     * Stores ids of adapters which are required by this adapter to work.
     * If none of the adapters in the list is enabled, the adapter can
     * not be enabled. The instrumenter prints a warning that it was
     * not possible to enable the adapter.
     */
    SCOREP_Instrumenter_DependencyList m_prerequisites;

    /**
     * Stores ids of adapters which must bot be used together with this adapter.
     */
    SCOREP_Instrumenter_DependencyList m_conflicts;

    /**
     * Stores ids of adapters which are enabled by default, if this adapter is enabled.
     */
    SCOREP_Instrumenter_DependencyList m_default_on;

    /**
     * Stores ids of adapters which are disabled by default, if this adapter is enabled.
     */
    SCOREP_Instrumenter_DependencyList m_default_off;

    /**
     * Stores whether this adapter is enabled.
     */
    instrumentation_usage_t m_usage;

    /**
     * Stores extra parameters for the adapter.
     */
    std::string m_params;

    /**
     * Set this to true if you want to use the possibility to pass extra parameters to
     * the tool.
     */
    bool m_use_params;

    /**
     * Stores whether this instrumentation approach is supported by this installation.
     */
    bool m_unsupported;

private:
    /**
     * Stores the instrumentation method identifier for this instance.
     */
    SCOREP_Instrumenter_AdapterId m_id;

    /* ----------------------------------------------------------------------- static */
public:
    /**
     * Deletes all adapters and empties the adapter list.
     */
    static void
    destroyAll( void );

    /**
     * Print the help information for all adapters.
     */
    static void
    printAll( void );

    /**
     * Let all adapters evaluate a scorep option.
     * @param arg  The scorep option that is evaluated.
     * @returns   true if one adapter knew this argument.
     */
    static bool
    checkAllOption( const std::string& arg );

    /**
     * Check the dependencies of all adapters.
     */
    static void
    checkAllDependencies( void );

    /**
     * Check the default relationships of all adapters.
     */
    static void
    checkAllDefaults( void );

    /**
     * Returns true if any of the selected adapters support the instrument filter.
     */
    static bool
    supportAnyInstrumentFilters( void );

    /**
     * Prints a space separated list of the names of all enabled adapters.
     */
    static void
    printEnabledAdapterList( void );

    /**
     * Returns the config tool flags for all adapters. It has a leading space.
     */
    static std::string
    getAllConfigToolFlags( SCOREP_Instrumenter_CmdLine& cmdLine );

    /**
     * Call the setBuildCheck function in all adapters.
     */
    static void
    setAllBuildCheck( SCOREP_Instrumenter_CmdLine& cmdLine );

    /**
     * Let all adapters check for the @a key/@a value pair.
     * @param key   The key that is specified.
     * @param value The value of this key.
     */
    static void
    setAllConfigValue( const std::string& key,
                       const std::string& value );

    /**
     * Let all adapters evaluate an argument form the command.
     * @param current The current argument that is evaluated.
     * @param next    The next parameter.
     * @returns true if the next parameter was already processed.
     */
    static bool
    checkAllCommand( const std::string& current,
                     const std::string& next );

    /**
     * Let all adapters evaluate the compiler name.
     * @param compiler  The name of the compiler as it appears in the user command.
     */
    static void
    checkAllCompilerName( const std::string& compiler );

    /**
     * Let all paradigms check the object files.
     * @param instrumenter The instrumenter instance.
     */
    static void
    checkAllObjects( SCOREP_Instrumenter& instrumenter );

    /**
     * Return true, if the given library needs to be interposed
     */
    static bool
    isAllInterpositionLibrary( const std::string& libraryName );

    /**
     * Returns the adapter, specified by the @a id. This function may not
     * work during the adapter construction.
     * @param id The identifier of the adapter you want to get.
     */
    static SCOREP_Instrumenter_Adapter*
    getAdapter( SCOREP_Instrumenter_AdapterId id );

    /**
     * Checks whether adapter enabling violated earlier settings and
     * enables it. If earlier settings are violated, it terminates with an error.
     * @caller name of the adapter or paradigm that requires this adapter. The name
     *         is only used in error messages.
     * @param  id The identifier of the adapter you require.
     */
    static void
    require( const std::string&            caller,
             SCOREP_Instrumenter_AdapterId id );

    /**
     * Checks whether adapter enabling violated earlier settings and
     * disables it. If earlier settings are violated, it terminates with an error.
     * @caller name of the adapter or paradigm that requires this adapter. The name
     *         is only used in error messages.
     * @param  id The identifier of the adapter you require.
     */
    static void
    conflict( const std::string&            caller,
              SCOREP_Instrumenter_AdapterId id );

    /**
     * This function is called if the selected paradigm enables this instrumentation
     * approach by default. It checks whether the user has made a different selection
     * and if not, enables this adapter and checks its dependencies.
     * @param id The identifier of the adapter you require.
     */
    static void
    defaultOn( SCOREP_Instrumenter_AdapterId id );

    /**
     * This function is called if the selected paradigm disables this instrumentation
     * approach by default. It checks whether the user has made a different selection
     * and if not, enables this adapter and checks its dependencies.
     * @param id The identifier of the adapter you require.
     */
    static void
    defaultOff( SCOREP_Instrumenter_AdapterId id );

    /**
     * Prints help for one dependency list.
     * @param list  Pointer to the list which we want to print.
     * @param entry String that is prepended to the list if the list is not empty.
     */
    static std::string
    getDepList( const SCOREP_Instrumenter_DependencyList& list,
                const std::string&                        entry );

private:
    /**
     * List of all adapters. They can be found at the index of their identifier.
     */
    static SCOREP_Instrumenter_AdapterList m_adapter_list;
};

#endif // SCOREP_INSTRUMENTER_ADAPTER_HPP
