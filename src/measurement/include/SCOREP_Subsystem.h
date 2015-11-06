/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013, 2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


#ifndef SCOREP_SUBSYSTEM_H
#define SCOREP_SUBSYSTEM_H


/**
 * @file
 *
 * @brief Subsystem management of the measurement system.
 *
 */


#include <SCOREP_ErrorCodes.h>
#include <SCOREP_Types.h>

#include <stddef.h>


/**
 * @defgroup SCOREP_Subsystem SCOREP Subsystem Management

 * Subsystem Management will be handled by callback functions, which will be
   called at defined places from the measurement system. In the particul
   callbacks the subsystem is only allowed to make specific actions.

 * Each subsystem exports these callbacks in a object of type @ref SCOREP_Subsystem
   wich is describted in this module.

 * @ref SCOREP_Subsystem objects are referenced by an array inside the
   measurement system, so the measurement system does not need to know
   explicitly which subsystems are included at compile time.

 * @{
 */

/**
 * This type defines predefined commands for adapter control.
 */
typedef enum
{
    /**
     * Enables event recording in an adapter.
     */
    SCOREP_SUBSYSTEM_COMMAND_ENABLE,

    /**
     * Disables event recording in an adapter.
     */
    SCOREP_SUBSYSTEM_COMMAND_DISABLE,

    /**
     * Offset for adapter specific commands.
     */
    SCOREP_SUBSYSTEM_COMMAND_CUSTOM_BASE
} SCOREP_Subsystem_Command;


struct SCOREP_Location;

/**
 * A subsystem can provide numerous callbacks for the measurement system.
 * These are collected in this structure for easy handling.
 */
typedef struct SCOREP_Subsystem
{
    /**
     * The paradigm type of the adapter
     */
    //SCOREP_ParadigmType paradigm_type;

    /**
     * Name/Version/...
     */
    const char* subsystem_name;

    /**
     * Register the subsystem.
     *
     * The main purpose is to allow the subsystem to register config variables
     * to the system.
     *
     * @param uniqueId The subsystem gets a unique ID assigned, provided as
     * the parameter of this callback.
     */
    SCOREP_ErrorCode ( * subsystem_register )( size_t uniqueId );

    /**
     * Callback to be notified about the initialization of your subsystem. This
     * takes place during measurement initialization.
     *
     * It is safe to assume single-threaded mode.
     * At this point all configure variables are set to their current
     * environment values. Subsystem of parallel paradigms should register
     * the paradigm in this callback, calls to the definition interface can
     * be done from this point on too. You are not supposed to access
     * SCOREP_Location objects even if your subsystem created them. First
     * time to access SCOREP_Location objects is in subsystem_init_location.
     * It is NOT ok to trigger a subsystem_init_location callback from a
     * subsystem_init one. For locations created during initialization the
     * subsystem_init_location step will be performed at the end of
     * SCOREP_InitMeasurement() (currently SCOREP_Thread_ActivateMaster()).
     * It is ok to use malloc. If you need to realloc during measurement,
     * please consider not to use malloc.
     */
    SCOREP_ErrorCode ( * subsystem_init )( void );

    /**
     * Callback to be notified about a location creation.
     *
     * It is not safe to assume single-threaded mode.
     *
     * @param newLocation Partially valid SCOREP_Location object. Subsystem and
     * substrate specific parts of the object need to be initialized here.
     * @param parentLocation The parent of @a newLocation. Is NULL for the
     * initial/master thread.
     * @return SCOREP_ErrorCode SCOREP_SUCCESS if successful.
     */
    SCOREP_ErrorCode ( * subsystem_init_location )( struct SCOREP_Location* newLocation,
                                                    struct SCOREP_Location* parentLocation );

    /**
     * Finalizes the per-location data from this subsystem.
     *
     * @param location The location object that is going to be finalized.
     */
    void ( * subsystem_finalize_location )( struct SCOREP_Location* location );

    /**
     * Called before the unification starts.
     *
     */
    SCOREP_ErrorCode ( * subsystem_pre_unify )( void );

    /**
     * Called after the unification.
     *
     */
    SCOREP_ErrorCode ( * subsystem_post_unify )( void );

    /**
     * Finalizes the subsystem for measurement.
     *
     */
    void ( * subsystem_finalize )( void );

    /**
     * De-register the subsystem.
     *
     */
    void ( * subsystem_deregister )( void );

    /**
     * Allows measurement control over the adapter.
     */
    void ( * subsystem_control )( SCOREP_Subsystem_Command command );
} SCOREP_Subsystem;


/*
 * @}
 */


#endif /* SCOREP_SUBSYSTEM_H */
