/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2015,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013, 2015, 2017,
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

#include <stdio.h>


/**
 * @defgroup SCOREP_Subsystem SCOREP Subsystem Management

 * Subsystem Management will be handled by callback functions, which will be
   called at defined places from the measurement system. In the particular
   callbacks the subsystem is only allowed to make specific actions.

 * Each subsystem exports these callbacks in a object of type @ref SCOREP_Subsystem
   which is described in this module.

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
     *  The next 6 callbacks notify the subsystem about the lifetime of the whole
     *  measurement:
     *
     *  1. init
     *  2. begin
     *  3. init_mpp
     *  4. synchronize(BEGIN) or synchronize(BEGIN_MPP)
     *  5. synchronize(END)
     *  6. end
     *  7. finalize
     */

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
     * SCOREP_InitMeasurement() (currently SCOREP_Location_ActivateInitLocations()).
     * It is ok to use malloc. If you need to realloc during measurement,
     * please consider not to use malloc.
     */
    SCOREP_ErrorCode ( * subsystem_init )( void );

    /**
     * Notifies the subsystems that Score-P enters the measurement phase.
     */
    SCOREP_ErrorCode ( * subsystem_begin )( void );

    /**
     * Notifies the subsystems that Score-P initialized the MPP. This is called
     * in both MPP and non-MPP case.
     */
    SCOREP_ErrorCode ( * subsystem_init_mpp )( void );

    /**
     * Notifies the subsystems that Score-P synchronizes itself.
     */
    void ( * subsystem_synchronize )( SCOREP_SynchronizationMode syncMode );

    /**
     * Notifies the subsystems that Score-P leaves the measurement phase.
     *
     * Called in reverse order in the subsystem list.
     */
    void ( * subsystem_end )( void );

    /**
     * Finalizes the subsystem for measurement.
     *
     * Called in reverse order in the subsystem list.
     */
    void ( * subsystem_finalize )( void );

    /**
     *  The next 4 callbacks notify the subsystem about the lifetime of a
     *  location. For CPU locations:
     *
     *  - init_location
     *  - activate_cpu_location(MGMT)                  \
     *  - activate_cpu_location(EVENTS)                |
     *  - deactivate_cpu_location(PAUSE)   \ possible  | at least once
     *  - activate_cpu_location(PAUSE)     /           |
     *  - deactivate_cpu_location(EVENTS)              |
     *  - deactivate_cpu_location(MGMT)                /
     *  - finalize_location
     *
     *  For none-CPU locations:
     *
     *  1. init_location
     *  2. finalize_location
     */

    /**
     * Callback to be notified about a location creation. This takes place
     * during measurement, except for the initial location/master thread.
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
     * Callback to be notified about a CPU location will get activated.
     *
     * Activation comes in two phases. Resulting in two calls to this callback,
     * differentiated by the @a phase parameter.
     *
     * (1) SCOREP_CPU_LOCATION_PHASE_MGMT::
     *     A general notification about the intention to activate the location.
     *     No events are allowed, just maintenance stuff should be done with
     *     CPU the location.
     * (2) SCOREP_CPU_LOCATION_PHASE_EVENTS::
     *     The location was successfully activated. Events can now be triggered.
     *
     * Additionally, a CPU location can be in a PAUSE phase where no events are
     * allowed.
     *     SCOREP_CPU_LOCATION_PHASE_PAUSE::
     *     Location goes into an PAUSE phase were no events are allowed.
     *
     * This callback is always called by the CPU thread which owns this location.
     * I.e., location == SCOREP_Location_GetCurrentCPULocation.
     *
     * @param location           The location for the current CPU.
     * @param parentLocation     The creator of this location.  Only valid in
     *                           the SCOREP_CPU_LOCATION_PHASE_MGMT phase.
     * @param forkSequenceCount  The fork sequence count. Only valid in the
     *                           SCOREP_CPU_LOCATION_PHASE_MGMT phase.
     * @param phase              The phase.
     */
    SCOREP_ErrorCode ( * subsystem_activate_cpu_location )( struct SCOREP_Location* location,
                                                            struct SCOREP_Location* parentLocation,
                                                            uint32_t                forkSequenceCount,
                                                            SCOREP_CPULocationPhase phase );

    /**
     * Callback to be notified about a CPU location will get deactivated.
     *
     * Activation comes in two phases. Resulting in two calls to this callback,
     * differentiated by the @a phase parameter.
     *
     * (1) SCOREP_CPU_LOCATION_PHASE_EVENTS::
     *     That the location will be deactivated. Events are still allowed in
     *     this callback.
     * (2) SCOREP_CPU_LOCATION_PHASE_MGMT::
     *     The location was successfully deactivated. No events are allowed.
     *
     * Additionally, a CPU location can be in a PAUSE phase where no events are
     * allowed.
     *     SCOREP_CPU_LOCATION_PHASE_PAUSE::
     *     Location goes into an PAUSE phase were no events are allowed.
     *
     * This callback is always called by the CPU thread which owns this location.
     * I.e., location == SCOREP_Location_GetCurrentCPULocation.
     *
     * Called in reverse order in the subsystem list.
     *
     * @param location           The location for the current CPU.
     * @param parentLocation     The creator of this location.  Only valid in
     *                           the SCOREP_CPU_LOCATION_PHASE_MGMT phase.
     * @param phase              The phase.
     */
    void ( * subsystem_deactivate_cpu_location )( struct SCOREP_Location* location,
                                                  struct SCOREP_Location* parentLocation,
                                                  SCOREP_CPULocationPhase phase );

    /**
     * Finalizes the per-location data from this subsystem.
     *
     * Called in reverse order in the subsystem list.
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
     * De-register the subsystem.
     *
     * Called in reverse order in the subsystem list.
     */
    void ( * subsystem_deregister )( void );

    /**
     * Dump subsystem information and configs to file.
     *
     * Called during experiment directory creation.
     */
    void ( * subsystem_dump_manifest )( FILE*       manifestFile,
                                        const char* relativeSourceDir,
                                        const char* targetDir );
} SCOREP_Subsystem;


/*
 * @}
 */


#endif /* SCOREP_SUBSYSTEM_H */
