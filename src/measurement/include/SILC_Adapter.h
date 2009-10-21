#ifndef SILC_ADAPTER_H
#define SILC_ADAPTER_H

/**
 * @file    SILC_Adapter.h
 * @maintainer Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @brief Adapter management of the measurement system.
 *
 *
 *
 */

#include "SILC_Types.h"

/**
 * An adapter needs to provide numerous functions for the measurement system.
 * These are collected in this structure for easy handling.
 */
typedef struct SILC_Adapter
{
    /**
     * The type of the adapter
     */
    SILC_AdapterType adapter_type;

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
    SILC_ErrorCode ( * adapter_register )
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
    SILC_ErrorCode ( * adapter_init )
    (
        void
    );

    /**
     * Callback to register a location to the adapter.
     *
     */
    SILC_ErrorCode ( * adapter_init_location )
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
} SILC_Adapter;

#endif /* SILC_ADAPTER_H */
