/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013, 2015,
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

#ifndef SCOREP_CONFIG_MPP_HPP
#define SCOREP_CONFIG_MPP_HPP

/**
 * @file
 *
 * Collects information about available multiprocess paradigms
 */

#include <string>
#include <deque>
#include "SCOREP_Config_LibraryDependencies.hpp"

/* ***************************************************************************
* class SCOREP_Config_MppSystem
* ***************************************************************************/

/**
 * The class SCOREP_Config_MppSystem represents a multi-process paradigm inside the
 * scorep-config tool. It contains the paradigm specific data and algorithms.
 * This class is the basis from which specialized paradigm classes may
 * derive their own class.
 */
class SCOREP_Config_MppSystem
{
public:
    /**
     * Initializes the multi-process paradigm list.
     */
    static void
    init( void );

    /**
     * Destroys the multi-process paradigm list.
     */
    static void
    fini( void );

    /**
     * Prints standard help output for all multi-process paradigm.
     */
    static void
    printAll( void );

    /**
     * Checks whether the argument is one of the known multi-process paradigms,
     * and sets the current class member to this paradigm.
     * This implementation checks for the value after the '--mpp=' part.
     * @param arg  The argument which is checked.
     * @return True if this argument matches any module. False otherwise.
     */
    static bool
    checkAll( const std::string& arg );

    /**
     * Constructs a SCOREP_Config_MppSystem.
     * @param name     The name of the multi-process paradigm.
     */
    SCOREP_Config_MppSystem( const std::string& name );

    /**
     * Destroys the object.
     */
    virtual
    ~SCOREP_Config_MppSystem();

    /**
     * Adds required libraries of this paradigm to the list of libraries.
     * This implementation does nothing, but need to be overwritten by
     * a customized class.
     * @param libs The list of libs to which you may add other libs.
     * @param deps The library dependencies information structure.
     * @param withOnlineAccess Specifies whether online access is enabled or not
     */
    virtual void
    addLibs( std::deque<std::string>&           libs,
             SCOREP_Config_LibraryDependencies& deps,
             bool                               withOnlineAccess );

    /**
     * Adds required linker flags of this paradigm to the list of flags.
     * This implementation does nothing, but need to be overwritten by
     * a customized class.
     * @param ldflags      The linker flags to which you may modify or add new flags.
     * @param build_check  True '--build-check' was specified.
     * @param nvcc         True if compiler is nvcc.
     */
    virtual void
    addLdFlags( std::string& ldflags,
                bool         build_check,
                bool         nvcc );

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
     * Prints standard help output for this paradigm, based on the name.
     * Overwrite this functions if you need a different layout.
     */
    virtual void
    printHelp( void );

    /**
     * The name of the multi-process paradigm.
     */
    std::string m_name;

private:
    /**
     * List of available multi-process paradigms.
     */
    static std::deque<SCOREP_Config_MppSystem*> all;

public:
    /**
     * Points the currently selected multi-process paradigm.
     */
    static SCOREP_Config_MppSystem* current;
};

/* ***************************************************************************
* class SCOREP_Config_MockupMppSystem
* ***************************************************************************/

/**
 * This class is a place holder when using single process paradigm.
 */
class SCOREP_Config_MockupMppSystem : public SCOREP_Config_MppSystem
{
public:
    SCOREP_Config_MockupMppSystem();
    virtual void
    addLibs( std::deque<std::string>&           libs,
             SCOREP_Config_LibraryDependencies& deps,
             bool                               withOnlineAccess );
    virtual void
    getInitStructName( std::deque<std::string>& init_structs );
};

/* ***************************************************************************
* class SCOREP_Config_MpiMppSystem
* ***************************************************************************/

/**
 * This class represents the MPI implementation of the multi process paradigm.
 */
class SCOREP_Config_MpiMppSystem : public SCOREP_Config_MppSystem
{
public:
    SCOREP_Config_MpiMppSystem();
    virtual void
    addLibs( std::deque<std::string>&           libs,
             SCOREP_Config_LibraryDependencies& deps,
             bool                               withOnlineAccess );
    virtual void
    addLdFlags( std::string& ldflags,
                bool         build_check,
                bool         nvcc );
    virtual void
    getInitStructName( std::deque<std::string>& init_structs );
};

/* ***************************************************************************
* class SCOREP_Config_ShmemMppSystem
* ***************************************************************************/

/**
 * This class represents the SHMEM implementation of the multi process paradigm.
 */
class SCOREP_Config_ShmemMppSystem : public SCOREP_Config_MppSystem
{
public:
    SCOREP_Config_ShmemMppSystem();
    virtual void
    addLibs( std::deque<std::string>&           libs,
             SCOREP_Config_LibraryDependencies& deps,
             bool                               withOnlineAccess );
    virtual void
    addLdFlags( std::string& ldflags,
                bool         build_check,
                bool         nvcc );
    virtual void
    getInitStructName( std::deque<std::string>& init_structs );
};

#endif // SCOREP_CONFIG_MPP_HPP
