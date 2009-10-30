#include <stdlib.h>
#include <stdio.h>


/**
 * @file        SILC_Events.c
 * @maintainer  Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @brief   Declaration of event recording functions to be used by the
 *          adapter layer.
 *
 *
 */


#include <SILC_Events.h>


#include "silc_types.h"


/**
 * Generate a region enter event in the measurement system.
 */
void
SILC_EnterRegion
(
    SILC_RegionHandle regionHandle
)
{
    char stringBuffer[ 16 ];

    fprintf( stderr, "%s: %s\n",
             __func__,
             silc_region_to_string( stringBuffer, sizeof( stringBuffer ),
                                    "%x", regionHandle ) );
}


/**
 * Generate a region exit event in the measurement system.
 */
void
SILC_ExitRegion
(
    SILC_RegionHandle regionHandle
)
{
    char stringBuffer[ 16 ];

    fprintf( stderr, "%s: %s\n",
             __func__,
             silc_region_to_string( stringBuffer, sizeof( stringBuffer ),
                                    "%x", regionHandle ) );
}


/**
 * Generate an mpi send event in the measurement system.
 */
void
SILC_MpiSend
(
    int32_t                    globalDestinationRank,
    SILC_MPICommunicatorHandle communicatorHandle,
    int32_t                    tag,
    int32_t                    bytesSent
)
{
    char stringBuffer[ 16 ];

    fprintf( stderr, "%s: %u %s %u %u\n",
             __func__,
             globalDestinationRank,
             silc_comm_to_string( stringBuffer, sizeof( stringBuffer ),
                                  "%x", communicatorHandle ),
             tag,
             bytesSent );
}


/**
 * Generate an mpi send event in the measurement system.
 */
void
SILC_MpiRecv
(
    int32_t                    globalSourceRank,
    SILC_MPICommunicatorHandle communicatorHandle,
    int32_t                    tag,
    int32_t                    bytesReceived
)
{
    char stringBuffer[ 16 ];

    fprintf( stderr, "%s: %u %s %u %u\n",
             __func__,
             globalSourceRank,
             silc_comm_to_string( stringBuffer, sizeof( stringBuffer ),
                                  "%x", communicatorHandle ),
             tag,
             bytesReceived );
}


/**
 * Generate an mpi collective event in the measurement system.
 */
void
SILC_MpiCollective
(
    SILC_RegionHandle          regionHandle,
    SILC_MPICommunicatorHandle communicatorHandle,
    int32_t                    globalRootRank,
    int32_t                    bytesSent,
    int32_t                    bytesReceived
)
{
    char stringBuffer[ 16 ];

    fprintf( stderr, "%s: %s %s %u %u %u\n",
             __func__,
             silc_region_to_string( stringBuffer, sizeof( stringBuffer ),
                                    "%x", regionHandle ),
             silc_comm_to_string( stringBuffer, sizeof( stringBuffer ),
                                  "%x", communicatorHandle ),
             globalRootRank,
             bytesSent,
             bytesReceived );
}


/**
 * Generate an OpenMP fork event in the measurement system.
 */
void
SILC_OmpFork
(
    SILC_RegionHandle regionHandle
)
{
    char stringBuffer[ 16 ];

    fprintf( stderr, "%s: %s\n",
             __func__,
             silc_region_to_string( stringBuffer, sizeof( stringBuffer ),
                                    "%x", regionHandle ) );
}


/**
 * Generate an OpenMP join event in the measurement system.
 */
void
SILC_OmpJoin
(
    SILC_RegionHandle regionHandle
)
{
    char stringBuffer[ 16 ];

    fprintf( stderr, "%s: %s\n",
             __func__,
             silc_region_to_string( stringBuffer, sizeof( stringBuffer ),
                                    "%x", regionHandle ) );
}


/**
 * Generate an OpenMP acquire lock event in the measurement system.
 */
void
SILC_OmpAcquireLock
(
    uint32_t lockId
)
{
    fprintf( stderr, "%s: %x\n", __func__, lockId );
}


/**
 * Generate an OpenMP release lock event in the measurement system.
 */
void
SILC_OmpReleaseLock
(
    uint32_t lockId
)
{
    fprintf( stderr, "%s: %x\n", __func__, lockId );
}


/**
 *
 */
void
SILC_ExitRegionOnException
(
    SILC_RegionHandle regionHandle
)
{
    char stringBuffer[ 16 ];

    fprintf( stderr, "%s: %s\n",
             __func__,
             silc_region_to_string( stringBuffer, sizeof( stringBuffer ),
                                    "%x", regionHandle ) );
}


/**
 *
 */
void
SILC_TriggerCounterInt64
(
    SILC_CounterHandle counterHandle,
    int64_t            value
)
{
    fprintf( stderr, "%s\n", __func__ );
}


/**
 *
 */
void
SILC_TriggerCounterDouble
(
    SILC_CounterHandle counterHandle,
    double             value
)
{
    fprintf( stderr, "%s\n", __func__ );
}


/**
 *
 */
void
SILC_TriggerMarker
(
    SILC_MarkerHandle markerHandle
)
{
    fprintf( stderr, "%s\n", __func__ );
}


/**
 *
 */
void
SILC_TriggerParameterInt64
(
    SILC_ParameterHandle parameterHandle,
    int64_t              value
)
{
    fprintf( stderr, "%s\n", __func__ );
}


/**
 *
 */
void
SILC_TriggerParameterDouble
(
    SILC_ParameterHandle parameterHandle,
    double               value
)
{
    fprintf( stderr, "%s\n", __func__ );
}


/**
 *
 */
void
SILC_TriggerParameterString
(
    SILC_ParameterHandle parameterHandle,
    const char*          value
)
{
    fprintf( stderr, "%s\n", __func__ );
}
