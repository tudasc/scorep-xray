/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_CONFIG_THREAD_HPP
#define SCOREP_CONFIG_THREAD_HPP

/**
 * @file scorep_config_thread.hpp
 *
 * Collects information about available threading systems.
 */

#include <string>
#include <deque>

#include "SCOREP_Config_LibraryDependencies.hpp"

/* **************************************************************************************
 * enum SCOREP_Config_ThreadSystemId
 * *************************************************************************************/

/**
 * Defines an identifier for every threading system.
 */
typedef enum
{
    SCOREP_CONFIG_THREAD_SYSTEM_ID_NONE,
    SCOREP_CONFIG_THREAD_SYSTEM_ID_POMP_TPD
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
     * Constructs a SCOREP_Config_ThreadSystem.
     * @param name     The name of the threading system.
     * @param variant  The name of the measurement approach or threading system variant.
     * @param library  The name of the Score-P threading library. Please omit
     *                 the leading 'lib' which is prefixed automatically if needed.
     * @param mutexlib The name of the Score-P mutex implementation library. Please omit
     *                 the leading 'lib' which is prefixed automatically if needed.
     * @param id       The identifier of the threading system.
     */
    SCOREP_Config_ThreadSystem( std::string                  name,
                                std::string                  variant,
                                std::string                  library,
                                std::string                  mutexlib,
                                SCOREP_Config_ThreadSystemId id );

    /**
     * Destroys the object.
     */
    virtual
    ~SCOREP_Config_ThreadSystem();

    /**
     * Prints standart help output for this threading system, based on the name.
     * Overwrite this functions if you need a different layout.
     */
    virtual void
    printHelp( void );

    /**
     * Checks whether an pogram argument influences this module.
     * This implementation checks for the value after the '--thread=' part.
     * @param arg  The argument which is checked.
     * @returns True if this argument is matches this module. False otherwise.
     */
    virtual bool
    checkArgument( std::string system );

    /**
     * Adds required libraries of this threaing system to the list of libraries.
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
     * @param fortran     True if the source file is a fortran file.
     * @param nvcc        True if compiler is nvcc.
     */
    virtual void
    addCFlags( std::string& cflags,
               bool         build_check,
               bool         fortran,
               bool         nvcc );

    /**
     * Overwrite this function if you want to do threading system specific modifications
     * to the include flags.
     * @param incflags    The compiler flags to which you may modify or add new flags.
     * @param build_check Specifies whether --build-check was set.
     * @param nvcc        True if compiler is nvcc.
     */
    virtual void
    addIncFlags( std::string& incflags,
                 bool         build_check,
                 bool         nvcc );

    /**
     * Returns the threading system identifier.
     */
    SCOREP_Config_ThreadSystemId
    getId( void );

protected:
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
     * The name of the mutex library.
     */
    std::string m_mutexlib;

private:
    /**
     * The identifier for the thread system type.
     */
    SCOREP_Config_ThreadSystemId m_id;

public:
    /**
     * Points the the currently selected threading system.
     */
    static SCOREP_Config_ThreadSystem* current;
};

/* **************************************************************************************
 * Threading system list
 * *************************************************************************************/

/**
 * List of available threading systems.
 */
extern std::deque<SCOREP_Config_ThreadSystem*> scorep_thread_systems;

/**
 * Initializes the threading system list.
 */
void
scorep_config_init_thread_systems( void );

/**
 * Destroys the threading system list.
 */
void
scorep_config_final_thread_systems( void );


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
    addLibs( std::deque<std::string>&           libs,
             SCOREP_Config_LibraryDependencies& deps );
};

/* **************************************************************************************
 * class SCOREP_Config_PompTpdThreadSystem
 * *************************************************************************************/

/**
 * This class represents the POMP2-based implementation for OpenMP threads which
 * uses the copyin structure for the TPD variable.
 */
class SCOREP_Config_PompTpdThreadSystem : public SCOREP_Config_ThreadSystem
{
public:
    SCOREP_Config_PompTpdThreadSystem();
    virtual void
    addLibs( std::deque<std::string>&           libs,
             SCOREP_Config_LibraryDependencies& deps );
    virtual void
    addCFlags( std::string& cflags,
               bool         build_check,
               bool         fortran,
               bool         nvcc );

    virtual void
    addIncFlags( std::string& incflags,
                 bool         build_check,
                 bool         nvcc );
};

#endif
