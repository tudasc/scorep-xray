#ifndef SILC_PLATFORM_H
#define SILC_PLATFORM_H

/**
 * The platform specific wall-clock timestamp function.
 *
 * @scope internal to the measurement system.
 * @exported as SILC_API_Wtime
 *
 * @note The platform could also directly implement the SILC_API_Wtime()
 */
SILC_API_Time SILC_Platform_Wtime( void );

/**
 * The platform specific wall time resolution function.
 *
 */
SILC_API_Time SILC_Platform_ClockResolution( void );

/**
 * Gives the name of the running executable in a malloc'ed buffer.
 *
 */
char* SILC_Platform_ExecName( void );

/**
 * Gives the name of the running architecture/platform
 * in a malloc'ed buffer.
 *
 */
char* SILC_Platform_Name( void );

/**
 * Gives the directory of the global filesytem in a malloc'ed buffer.
 *
 */
char* SILC_Platform_GDir( void );

/**
 * Gives the directory of the local filesytem in a malloc'ed buffer.
 *
 */
char* SILC_Platform_LDir( void );

/**
 * Gives a unique id for the running machine (Ie. IP of the cluster node).
 *
 */
uint64_t SILC_Platform_NodeId( void );

/**
 * Gives the name of the running machine (Ie. hostname) in a malloc'ed buffer.
 *
 */
char* SILC_Platform_NodeName( void );

/**
 * Gives the number of cpus on the running machine.
 *
 * @question: is this the available/possible/allowed number of cpus?
 */
uint64_t SILC_Platform_NumCpus( void );

/**
 * Gives the number of nodes/machines.
 *
 */
uint64_t SILC_Platform_NumNodes( void );

#endif /* SILC_PLATFORM_H */
