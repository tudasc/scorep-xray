/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2014,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_CONFIG_MUTEX_HPP
#define SCOREP_CONFIG_MUTEX_HPP

/**
 * @file
 *
 * Collects information about available threading systems.
 */

#include <string>
#include <deque>

#include "SCOREP_Config_LibraryDependencies.hpp"
#include "scorep_config_types.hpp"

/* **************************************************************************************
 * enum SCOREP_Config_MutexId
 * *************************************************************************************/

/**
 * Defines an identifier for every locking system.
 */
typedef enum
{
    SCOREP_CONFIG_MUTEX_ID_NONE,
    SCOREP_CONFIG_MUTEX_ID_OMP,
    SCOREP_CONFIG_MUTEX_ID_PTHREAD,
    SCOREP_CONFIG_MUTEX_ID_PTHREAD_WRAP,
    SCOREP_CONFIG_MUTEX_ID_PTHREAD_SPINLOCK,
} SCOREP_Config_MutexId;

/* **************************************************************************************
 * class SCOREP_Config_Mutex
 * *************************************************************************************/

/**
 * The class SCOREP_Config_Mutex represents a mutex selection inside the scorep-config
 * tool.
 */
class SCOREP_Config_Mutex
{
public:
    /**
     * Initializes the mutex list.
     */
    static void
    init( void );

    /**
     * Destroys the mutex list.
     */
    static void
    fini( void );

    /**
     * Prints standard help output for all mutex types.
     */
    static void
    printAll( void );

    /**
     * Checks whether the argument is one of the known mutex types,
     * and sets the current class member current to this type.
     * This implementation checks for the value after the '--mutex=' part.
     * @param arg  The argument which is checked.
     * @returns True if this argument matches any module. False otherwise.
     */
    static bool
    checkAll( const std::string& arg );

    /**
     * Selects the mutex system for a matching ID.
     * @param mutexId  The ID to select.
     */
    static void
    select( SCOREP_Config_MutexId id );

    /**
     * Constructs a SCOREP_Config_Mutex.
     * @param name     The name of the mutex.
     * @param variant  The name of the locking variant.
     * @param library  The name of the Score-P mutex library. Please omit
     *                 the leading 'lib' which is prefixed automatically if needed.
     * @param id       The identifier of the mutex system.
     */
    SCOREP_Config_Mutex( const std::string&    name,
                         const std::string&    variant,
                         const std::string&    library,
                         SCOREP_Config_MutexId id );

    /**
     * Destroys the object.
     */
    virtual
    ~SCOREP_Config_Mutex();

    /**
     * Prints standard help output for this mutex system, based on the name.
     * Overwrite this functions if you need a different layout.
     */
    virtual void
    printHelp( void );

    /**
     * Checks whether an pogram argument influences this module.
     * This implementation checks for the value after the '--mutex=' part.
     * @param arg  The argument which is checked.
     * @returns True if this argument is matches this module. False otherwise.
     */
    virtual bool
    checkArgument( const std::string& system );

    /**
     * Adds required libraries of this mutex to the list of libraries.
     * This implementation adds the value stored in m_library. Overwrite this
     * function if you need more or different libraries.
     * @param libs The list of libs to which you may add other libs.
     * @param deps The library dependencies information structure.
     */
    virtual void
    addLibs( std::deque<std::string>&           libs,
             SCOREP_Config_LibraryDependencies& deps );

    /**
     * Overwrite this function if you want to do system specific modifications
     * to the compiler flags.
     * @param cflags      The compiler flags to which you may modify or add new flags.
     *                    This flags do not contain the include directories. For the
     *                    include flags use addIncFlags.
     * @param build_check Specifies whether --build-check was set.
     * @param language     Specifies whether it is a C, C++ or Fortran compiler.
     * @param nvcc        True if compiler is nvcc.
     */
    virtual void
    addCFlags( std::string&           cflags,
               bool                   build_check,
               SCOREP_Config_Language language,
               bool                   nvcc );

    /**
     * Overwrite this function if you want to do system specific modifications
     * to the include flags.
     * @param incflags    The compiler flags to which you may modify or add new flags.
     * @param build_check Specifies whether --build-check was set.
     * @param language     Specifies whether it is a C, C++ or Fortran compiler.
     * @param nvcc        True if compiler is nvcc.
     */
    virtual void
    addIncFlags( std::string&           incflags,
                 bool                   build_check,
                 SCOREP_Config_Language language,
                 bool                   nvcc );

    /**
     * Returns the mutex identifier.
     */
    SCOREP_Config_MutexId
    getId( void );

protected:
    /**
     * The name of the mutex.
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


private:
    /**
     * The identifier for the mutex type.
     */
    SCOREP_Config_MutexId m_id;

public:
    /**
     * Points the currently selected mutex.
     */
    static SCOREP_Config_Mutex* current;

private:
    /**
     * List of available mutex types.
     */
    static std::deque<SCOREP_Config_Mutex*> all;
};

/* **************************************************************************************
 * class SCOREP_Config_MutexMockup
 * *************************************************************************************/

/**
 * This class represents a mockup, used for single-threaded
 * applications.
 */
class SCOREP_Config_MutexMockup : public SCOREP_Config_Mutex
{
public:
    SCOREP_Config_MutexMockup();
};

/* **************************************************************************************
 * class SCOREP_Config_MutexOmp
 * *************************************************************************************/

/**
 * This class represents the OpenMP-based locking system.
 */
class SCOREP_Config_MutexOmp : public SCOREP_Config_Mutex
{
public:
    SCOREP_Config_MutexOmp();
};

/* **************************************************************************************
 * class SCOREP_Config_MutexPthread
 * *************************************************************************************/

/**
 * This class represents the Pthread-based mutex locking system.
 */
class SCOREP_Config_MutexPthread : public SCOREP_Config_Mutex
{
public:
    SCOREP_Config_MutexPthread();
};

/* **************************************************************************************
 * class SCOREP_Config_MutexPthreadWrap
 * *************************************************************************************/

/**
 * This class represents the Pthread-based mutex locking system for library wrapping.
 */
class SCOREP_Config_MutexPthreadWrap : public SCOREP_Config_Mutex
{
public:
    SCOREP_Config_MutexPthreadWrap();
};

/* **************************************************************************************
 * class SCOREP_Config_MutexPthreadSpinlock
 * *************************************************************************************/

/**
 * This class represents the Pthread-based spin locking system.
 */
class SCOREP_Config_MutexPthreadSpinlock : public SCOREP_Config_Mutex
{
public:
    SCOREP_Config_MutexPthreadSpinlock();
};


#endif // SCOREP_CONFIG_MUTEX_HPP
