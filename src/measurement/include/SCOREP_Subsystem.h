/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 *    RWTH Aachen University, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


#ifndef SCOREP_SUBSYSTEM_H
#define SCOREP_SUBSYSTEM_H


/**
 * @file    src/measurement/include/SCOREP_Subsystem.h
 * @maintainer Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @brief Subsystem management of the measurement system.
 * @status alpha
 *
 */


#include <SCOREP_ErrorCodes.h>
#include <SCOREP_Types.h>
#include <SCOREP_Location.h>

/**
 * @defgroup SCOREP_Subsystem SCOREP Subsystem Management

 * Subsystem Management will be handled by callback functions, which will be
   called at defined places from the measurement system. In the particul
   callbacks the subsystem is only allowed to make specific actions.

 * Each subsystem exports these callbacks in a object of type @ref SCOREP_Subsystem
   wich is describted in this module.

 * All @ref SCOREP_Subsystem objects are referenced by an array inside the
   measurement system, so the measurement system does not need to know
   explicitly which subsystems are included at compile time.

 * @{
 */


/**
 * A subsystem can provide numerous callbacks for the measurement system.
 * These are collected in this structure for easy handling.
 */
typedef struct SCOREP_Subsystem
{
    /**
     * The type of the adapter
     */
    //SCOREP_AdapterType adapter_type;

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
     * The subsystem gets a unique ID assigned, provided as the parameter
     * of this callback.
     */
    SCOREP_ErrorCode ( * subsystem_register )( size_t );

    /**
     * Initialize the subsystem for measurement.
     *
     * At this point all configure variables are set to there current
     * environment values. The subsystem can also do calls to the definition
     * interface from this point on.
     *
     */
    SCOREP_ErrorCode ( * subsystem_init )( void );

    /**
     * Callback to register a location to the subsystem.
     *
     */
    SCOREP_ErrorCode ( * subsystem_init_location )( struct SCOREP_Location* );

    /**
     * Finalizes the per-location data from this subsystem.
     *
     */
    void ( * subsystem_finalize_location )( struct SCOREP_Location* );

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
} SCOREP_Subsystem;


/*
 * @}
 */


#endif /* SCOREP_SUBSYSTEM_H */
