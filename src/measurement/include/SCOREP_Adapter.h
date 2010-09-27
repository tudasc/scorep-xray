/*
 * This file is part of the SCOREP project (http://www.scorep.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


#ifndef SCOREP_ADAPTER_H
#define SCOREP_ADAPTER_H


/**
 * @file    SCOREP_Adapter.h
 * @maintainer Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @brief Adapter management of the measurement system.
 * @status alpha
 *
 */


#include <scorep_utility/SCOREP_Error.h>
#include <SCOREP_Types.h>


/**
 * @defgroup SCOREP_Adapter SCOREP Adapter Management

 * Adapter Management will be handled by callback functions, which will be
   called at defined places from the measurement system. In the particul
   callbacks the adapter is only allowed to make specific actions.

 * Each adapter exports these callbacks in a object of type @ref SCOREP_Adapter
   wich is describted in this module.

 * All @ref SCOREP_Adapter objects are referenced by an array inside the
   measurement system, so the measurement system does not need to know
   explicitly which adapters are included at compile time.

 * @{
 */


/**
 * An adapter needs to provide numerous functions for the measurement system.
 * These are collected in this structure for easy handling.
 */
typedef struct SCOREP_Adapter
{
    /**
     * The type of the adapter
     */
    SCOREP_AdapterType adapter_type;

    /**
     * Name/Version/...
     */
    const char* adapter_name;

    /**
     * Register the adapter.
     *
     * The main purpose is to allow the adapter to register config variables
     * to the system.
     *
     */
    SCOREP_Error_Code ( * adapter_register )
    (
        void
    );

    /**
     * Initialize the adapter for measurement.
     *
     * At this point all configure variables are set to there current
     * environment values. The adapter can also do calls to the definition
     * interface from this point on.
     *
     */
    SCOREP_Error_Code ( * adapter_init )
    (
        void
    );

    /**
     * Callback to register a location to the adapter.
     *
     */
    SCOREP_Error_Code ( * adapter_init_location )
    (
        void
    );

    /**
     * Finalizes the per-location data from this adapter.
     *
     */
    void ( *adapter_finalize_location )
    (
        void*
    );

    /**
     * Finalizes the adapter for measurement.
     *
     */
    void ( *adapter_finalize )
    (
        void
    );

    /**
     * De-register the adapter.
     *
     */
    void ( *adapter_deregister )
    (
        void
    );
} SCOREP_Adapter;


/*
 * @}
 */


#endif /* SCOREP_ADAPTER_H */
