/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2014, 2016,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_CONFIG_THREAD_HPP
#define SCOREP_CONFIG_THREAD_HPP

/**
 * @file
 *
 * Collects information about available threading systems.
 */

#include <string>
#include <deque>

#include "SCOREP_Config_LibraryDependencies.hpp"
#include "scorep_config_types.hpp"
#include "scorep_config_mutex.hpp"

/* **************************************************************************************
 * enum SCOREP_Config_ThreadSystemId
 * *************************************************************************************/

/**
 * Defines an identifier for every threading system.
 */
typedef enum
{
    SCOREP_CONFIG_THREAD_SYSTEM_ID_NONE,
    SCOREP_CONFIG_THREAD_SYSTEM_ID_OMP,
    SCOREP_CONFIG_THREAD_SYSTEM_ID_PTHREAD
} SCOREP_Config_ThreadSystemId;

/* **************************************************************************************
 * class SCOREP_Config_ThreadSystem
 * *************************************************************************************/

/**
 * The class SCOREP_Config_ThreadSystem represents a threading system inside the scorep-config
 * tool. It contains the threading system specific data and algorithms.
 * This class is the basis from which specialized threading systems may
 * derive their own class.
 */
class SCOREP_Config_ThreadSystem
{
public:
    /**
     * Initializes the threading system list.
     */
    static void
    init( void );

    /**
     * Destroys the threading system list.
     */
    static void
    fini( void );

    /**
     * Prints standard help output for all threading system.
     */
    static void
    printAll( void );

    /**
     * Checks whether the argument is one of the known threading system:variants,
     * and sets the current class member to this system.
     * This implementation checks for the value after the '--thread=' part.
     * @param arg  The argument which is checked.
     * @returns True if this argument matches any module. False otherwise.
     */
    static bool
    checkAll( const std::string& arg );

    /**
     * Constructs a SCOREP_Config_ThreadSystem.
     * @param name     The name of the threading system.
     * @param variant  The name of the measurement approach or threading system variant.
     * @param library  The name of the Score-P threading library. Please omit
     *                 the leading 'lib' which is prefixed automatically if needed.
     * @param mutexlib The name of the Score-P mutex implementation library. Please omit
     *                 the leading 'lib' which is prefixed automatically if needed.
     * @param id       The identifier of the threading system.
     */
    SCOREP_Config_ThreadSystem( const std::string&           name,
                                const std::string&           variant,
                                const std::string&           library,
                                SCOREP_Config_MutexId        mutexId,
                                SCOREP_Config_ThreadSystemId id );

    /**
     * Destroys the object.
     */
    virtual
    ~SCOREP_Config_ThreadSystem();

    /**
     * Adds required libraries of this threading system to the list of libraries.
     * This implementation adds the value stored in m_library. Overwrite this
     * function if you need more or different libraries.
     * @param libs The list of libs to which you may add other libs.
     * @param deps The library dependencies information structure.
     */
    virtual void
    addLibs( std::deque<std::string>&           libs,
             SCOREP_Config_LibraryDependencies& deps );

    /**
     * Overwrite this function if you want to do threading system specific modifications
     * to the compiler flags.
     * @param cflags      The compiler flags to which you may modify or add new flags.
     *                    This flags do not contain the include directories. For the
     *                    include flags use addIncFlags.
     * @param build_check Specifies whether --build-check was set.
     * @param language    Specifies the language of the compiler.
     * @param nvcc        True if compiler is nvcc.
     */
    virtual void
    addCFlags( std::string&           cflags,
               bool                   build_check,
               SCOREP_Config_Language language,
               bool                   nvcc );

    /**
     * Overwrite this function if you want to do threading system specific modifications
     * to the include flags. This function is also called, when the addCFlags()
     * function will be called.
     * @param incflags    The compiler flags to which you may modify or add new flags.
     * @param build_check Specifies whether --build-check was set.
     * @param language    Specifies the language of the compiler.
     * @param nvcc        True if compiler is nvcc.
     */
    virtual void
    addIncFlags( std::string&           incflags,
                 bool                   build_check,
                 SCOREP_Config_Language language,
                 bool                   nvcc );


    /**
     * Overwrite this function if you want to do adapter specific modifications
     * to the linker flags.
     * @param ldflags  the linker flags to which you may modify or add new flags.
     * @param nvcc     True if compiler is nvcc.
     */
    virtual void
    addLdFlags( std::string& ldflags,
                bool         nvcc );

    /**
     * Checks if an explicit choice has been made for the locking mechanism
     *  - If the mutex selection has been explicitly changed from the default mockup,
     *    the choice stays valid
     *  - If either "none" is chosen explicitly or is still the default, the threading
     *    system changes it to its own default, ensuring a valid locking for a threading
     *    case.
     */
    virtual SCOREP_Config_MutexId
    validateDependencies();

    /**
     * Returns the threading system identifier.
     */
    SCOREP_Config_ThreadSystemId
    getId( void );

    /**
     * Adds the name(s) of the SCOREP_Subsystem struct(s) associated with this
     * adapter. To be implemented by derived-classes. These names need to match
     * the SCOREP_Subsystem struct(s) variable name(s) used in the measurement
     * system.
     */
    virtual void
    getInitStructName( std::deque<std::string>& init_structs ) = 0;

protected:
    /**
     * Prints standard help output for this threading system, based on the name.
     * Overwrite this functions if you need a different layout.
     */
    virtual void
    printHelp( void );

    /**
     * The name of the threading system.
     */
    std::string m_name;

    /**
     * The name of the variant.
     */
    std::string m_variant;

    /**
     * The library name.
     */
    std::string m_library;

    /**
     * The name of the default parameter for the mutex choice.
     */
    SCOREP_Config_MutexId m_mutexId;

private:
    /**
     * The identifier for the thread system type.
     */
    SCOREP_Config_ThreadSystemId m_id;

public:
    /**
     * Points the currently selected threading system.
     */
    static SCOREP_Config_ThreadSystem* current;

private:
    /**
     * List of available threading systems.
     */
    static std::deque<SCOREP_Config_ThreadSystem*> m_all;
};


/* **************************************************************************************
 * class SCOREP_Config_MockupThreadSystem
 * *************************************************************************************/

/**
 * This class represents a mockup thread system, used for single-threaded
 * applications.
 */
class SCOREP_Config_MockupThreadSystem : public SCOREP_Config_ThreadSystem
{
public:
    SCOREP_Config_MockupThreadSystem();
    virtual void
    getInitStructName( std::deque<std::string>& init_structs );
};


/* **************************************************************************************
 * class SCOREP_Config_OmpThreadSystem
 * *************************************************************************************/

/**
 * This class represents the POMP2-based implementation for OpenMP threads which
 * uses the copyin structure for the TPD variable.
 */
class SCOREP_Config_OmpThreadSystem : public SCOREP_Config_ThreadSystem
{
public:
    SCOREP_Config_OmpThreadSystem();
    virtual void
    addLibs( std::deque<std::string>&           libs,
             SCOREP_Config_LibraryDependencies& deps );
    virtual void
    addCFlags( std::string&           cflags,
               bool                   build_check,
               SCOREP_Config_Language language,
               bool                   nvcc );

    virtual void
    addIncFlags( std::string&           incflags,
                 bool                   build_check,
                 SCOREP_Config_Language language,
                 bool                   nvcc );
    virtual void
    getInitStructName( std::deque<std::string>& init_structs );
};

/* ****************************************************************************
* class SCOREP_Config_PthreadThreadSystem
* ****************************************************************************/

/**
 * This class represents the Pthread backend
 */
class SCOREP_Config_PthreadThreadSystem : public SCOREP_Config_ThreadSystem
{
public:
    SCOREP_Config_PthreadThreadSystem();
    virtual void
    addLibs( std::deque<std::string>&           libs,
             SCOREP_Config_LibraryDependencies& deps );
    virtual void
    addLdFlags( std::string& ldflags,
                bool         nvcc );
    virtual void
    getInitStructName( std::deque<std::string>& init_structs );
};

#endif // SCOREP_CONFIG_THREAD_HPP
