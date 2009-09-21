#ifndef SILC_API_ADAPTER_H
#define SILC_API_ADAPTER_H

/**
 * An adapter needs to provide numerous functions for the measurement system.
 * These are collected in this structure for easy handling.
 */
typedef struct SILC_API_AdapterOps
{
    /**
     * The type od the adapter
     */
    SILC_API_AdapterType ao_type;

    /**
     * Name/Version/...
     */
    const char* ao_name;

    /**
     * Register the adapter.
     *
     * The main purpose is to allow the adapter to register config variables
     * to the system.
     *
     */
    SILC_ErrorCode ( *ao_register )( void );

    /**
     * Initialize the adapater for measurement.
     *
     * At this point all configure varialbes are set to there current
     * environment values.
     *
     */
    SILC_ErrorCode ( *ao_init )( void );

    /**
     * Initializes a location in the adapter and return a per-location data
     * pointer in the forst argument. This pointer can be queried with the
     * SILC_API_getLocationData.
     *
     */
    SILC_ErrorCode ( *ao_init_location )( void* /*, location id? */ );


    /**
     * Finalizes the per-location data from this adapter.
     *
     */
    void ( *ao_fini_location )( void* );

    /**
     * Finalizes the adapater for measurement.
     *
     */
    void ( *ao_fini )( void );

    /**
     * De-register the adapter.
     *
     */
    void ( *ao_deregister )( void );
} SILC_API_AdapterOps;

/**
 * get the per-location private data for the adapter type @a type
 *
 */
void* SILC_API_getLocationData( SILC_API_AdapterType );

#endif /* SILC_API_ADAPTER_H */
