/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2015, 2017,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_INSTRUMENTER_PARADIGM_HPP
#define SCOREP_INSTRUMENTER_PARADIGM_HPP

/**
 * @file
 *
 * Defines the base class for an paradigm.
 */

#include <string>
#include <stdint.h>

#include "scorep_instrumenter_adapter.hpp"

class SCOREP_Instrumenter;
class SCOREP_Instrumenter_Selector;

/* **************************************************************************************
 * class SCOREP_Instrumenter_Paradigm
 * *************************************************************************************/

/**
 * The class SCOREP_Instrumenter_Paradigm represents a paradigm inside the
 + scorep instrumenter. It contains the paradigm specific data and algorithms
 * for the scorep instrumenter. This class is the basis from which specialized
 * paradigms may derive their own class.
 */

class SCOREP_Instrumenter_Paradigm
{
    /* ----------------------------------------------------------------------- methods */
public:
    /**
     * Creates a SCOREP_Instrumenter_Paradigm instance.
     * @param selector    The selector for the paradigm group of this paradigm.
     * @param name        The name of the paradigm.
     * @param variant     The name of the paradigm variant or implementation.
     * @param description Description of the paradigm which is displayed on the help
     *                    output.
     */
    SCOREP_Instrumenter_Paradigm( SCOREP_Instrumenter_Selector* selector,
                                  const std::string&            name,
                                  const std::string&            variant,
                                  const std::string&            description );

    /**
     * Deletes a SCOREP_Instrumenter_Paradigm instance.
     */
    virtual
    ~SCOREP_Instrumenter_Paradigm();

    /**
     * Ensures that required adapters are enabled and that no
     * conflicting adapters are enabled.
     */
    virtual void
    checkDependencies( void );

    /**
     * Evaluate default relationships to the adapters.
     */
    virtual void
    checkDefaults( void );

    /**
     * Does this paradigm make use of the instrumentation filter?
     */
    virtual bool
    supportInstrumentFilters( void ) const;

    /**
     * Prints the default help output.
     */
    virtual void
    printHelp( void );

    /**
     * Evaluates an scorep option for this paradigm.
     * Overwrite this function if you need to process different or more parameters.
     * @param arg  The option that is evaluated.
     * @returns true, if this argument selects this paradigm.
     */
    virtual bool
    checkOption( const std::string& arg );

    /**
     * Returns the FQ name of this paradigm.
     */
    virtual std::string
    getName( void );

    /**
     * Returns the name as used for the selection of the scorep-config tool.
     */
    virtual std::string
    getConfigName( void );

    /**
     * This function is called during command line parsing during parsing of the
     * user command. The default implementation ignores all arguments.
     * Overwrite this function if the paradigm's behavior is influenced by options
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
     * Processes a key value pair from a config file. The default implementation
     * does nothing.
     * Overwrite this function if your paradigm can be configured by a config
     * file that a user passes in via the '--config' option.
     * @param key   The key that is specified.
     * @param value The value of this key.
     */
    virtual void
    setConfigValue( const std::string& key,
                    const std::string& value );

    /**
     * Checks the compiler name, which might affect the automatic paradigm selection
     * or paradigm configuration.
     * The default implementation does nothing. Overwrite this function, if your
     * paradigm is affected by the compiler name.
     * @param compiler  The name of the compiler as it appears in the user command.
     */
    virtual void
    checkCompilerName( const std::string& compiler );

    /**
     * Returns whether the paradigm is supported.
     */
    bool
    isSupported( void );

    /**
     * Called before the link step in all paradigms to perform object file based
     * checks.
     * @param instrumenter  Pointer to the instrumenter instance.
     */
    virtual void
    checkObjects( SCOREP_Instrumenter& instrumenter );

    /**
     * This function is called during the command line parsing of the user
     * command. It decides whether options following the compiler name
     * belong to a wrapper, or to a build command.
     * It is only called, if the detection or selection of this paradigm,
     * suggested that this paradigm will be selected.
     * @param current The current argument that is evaluated.
     * @param next    The next parameter.
     * @returns true if the current argument is a wrapper option.
     */
    virtual bool
    checkWrapperOption( const std::string& current,
                        const std::string& next );

    /**
     * Return true, if the given library needs to be interposed
     */
    virtual bool
    isInterpositionLibrary( const std::string& libraryName );

protected:
    /**
     * If the paradigm is not supported by this installation. This function
     * should be called by the constructor. It sets some variables to make sure
     * that this paradigm cannot be selected and prints meaning error message if it is.
     * Furthermore, it modifies the help output.
     */
    void
    unsupported( void );

    /* ----------------------------------------------------------------------- members */
protected:
    /**
     * Stores the name of the paradigm.
     */
    std::string m_name;

    /**
     * Stores the variant of the paradigm.
     */
    std::string m_variant;

    /**
     * Stores the description.
     */
    std::string m_description;

    /**
     * Stores ids of adapters which are required by this paradigm.
     */
    SCOREP_Instrumenter_DependencyList m_requires;

    /**
     * Stores ids of adapters which must not be used together with this paradigm.
     */
    SCOREP_Instrumenter_DependencyList m_conflicts;

    /**
     * Stores ids of adapters which are enabled by default, if this paradigm
     * is enabled.
     */
    SCOREP_Instrumenter_DependencyList m_default_on;

    /**
     * Stores ids of adapters which are disabled by default, if this paradigm
     * is enabled.
     */
    SCOREP_Instrumenter_DependencyList m_default_off;

    /**
     * Stores whether this instrumentation approach is supported by this installation.
     */
    bool m_unsupported;

    /**
     * Stores a pointer to the selector.
     */
    SCOREP_Instrumenter_Selector* m_selector;
};

#endif // SCOREP_INSTRUMENTER_PARADIGM_HPP
