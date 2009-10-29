/** @file SILC_Adapter_Init.h

    Defines the functions needed on the adapter side, during the initialization process.
    For each adapter, an instance of type SILC_Adapter_InitFunctions is required by the
    measurement system.

    The measurement system calls three adapter functions during initializtaion. First,
    SILC_Adapter_Register is called, which should retrieve the configuration from the
    measurement system. Second, SILC_Adapter_Init is called, which should initialize
    the adapter itself. Finally, SILC_Adapter_InitLocation is called.
 */


/** Function prototype of the register function of the adapters. In this function
    an adapter should retrieve the necessary configuration data from the measurement
    system.
 */
typedef void ( *SILC_Adapter_RegisterFunc )();

/** Function prototype pf the initialization function of the adapters. It should
    initialize the adapter data structures.
 */
typedef void ( *SILC_Adapter_InitFunc )();

/** Function prototype pf the initialize location function.
 */
typedef void ( *SILC_Adapter_InitLocationFunc )();


/** struct which contains the function definitions for adapter initialization for one
    adapter.
 */
typedef struct
{
    SILC_Adapter_RegisterFunc     SILC_Adapter_Register;
    SILC_Adapter_InitFunc         SILC_Adapter_Init;
    SILC_Adapter_InitLocationFunc SILC_Adapter_InitLocation;
} SILC_Adapter_InitFunctions;
