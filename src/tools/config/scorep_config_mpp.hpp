/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

#ifndef SCOREP_CONFIG_MPP_HPP
#define SCOREP_CONFIG_MPP_HPP

/**
 * @file src/tools/config/scorep_config_mpp.hpp
 * @status alpha
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
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
 * The class SCOREP_Config_MppSystem represents a multi process parodigm inside the
 * scorep-config tool. It contains the paradigm specific data and algorithms.
 * This class is the basis from which specialized paradigm classes may
 * derive their own class.
 */
class SCOREP_Config_MppSystem
{
public:
    /**
     * Constructs a SCOREP_Config_MppSystem.
     * @param name     The name of the multi process paradigm.
     */
    SCOREP_Config_MppSystem( std::string name );

    /**
     * Destroys the object.
     */
    virtual
    ~SCOREP_Config_MppSystem();

    /**
     * Prints standart help output for this paradigm, based on the name.
     * Overwrite this functions if you need a different layout.
     */
    virtual void
    printHelp( void );

    /**
     * Checks whether an pogram argument influences this module.
     * This implementation checks for the value after the '--mpp=' part.
     * @param arg  The argument which is checked.
     * @returns True if this argument matches this module. False otherwise.
     */
    virtual bool
    checkArgument( std::string system );

    /**
     * Adds required libraries of this paradigm to the list of libraries.
     * This implementation does nothing, but need to be overwritten by
     * a customized class.
     * @param libs The list of libs to which you may add other libs.
     * @param deps The library dependencies information structure.
     */
    virtual void
    addLibs( std::deque<std::string>&           libs,
             SCOREP_Config_LibraryDependencies& deps );

protected:
    /**
     * The name of the multi process paradigm.
     */
    std::string m_name;

public:
    /**
     * Points the the currently selected threading system.
     */
    static SCOREP_Config_MppSystem* current;
};

/* ***************************************************************************
* Threading system list
* ***************************************************************************/

/**
 * List of available threading systems.
 */
extern std::deque<SCOREP_Config_MppSystem*> scorep_mpp_systems;

/**
 * Initializes the threading system list.
 */
void
scorep_config_init_mpp_systems( void );

/**
 * Destroys the threading system list.
 */
void
scorep_config_final_mpp_systems( void );

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
             SCOREP_Config_LibraryDependencies& deps );
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
             SCOREP_Config_LibraryDependencies& deps );
};

#endif
