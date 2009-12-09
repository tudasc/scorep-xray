/*
 * This file is part of the SILC project (http://www.silc.de)
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
#include <SILC_Timing.h>
#include <OTF2_EvtWriter.h>


#include "silc_runtime_management.h"
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

    fprintf( stderr, "%s: Reg:%s\n",
             __func__,
             silc_region_to_string( stringBuffer, sizeof( stringBuffer ),
                                    "%x", regionHandle ) );

    OTF2_EvtWriter_Enter( local_event_writer, NULL,
                          SILC_GetWallClockTime(),
                          regionHandle );
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

    fprintf( stderr, "%s: Reg:%s\n",
             __func__,
             silc_region_to_string( stringBuffer, sizeof( stringBuffer ),
                                    "%x", regionHandle ) );

    OTF2_EvtWriter_Leave( local_event_writer, NULL,
                          SILC_GetWallClockTime(),
                          regionHandle );
}


/**
 * Generate an mpi send event in the measurement system.
 */
void
SILC_MpiSend
(
    uint32_t                   globalDestinationRank,
    SILC_MPICommunicatorHandle communicatorHandle,
    uint32_t                   tag,
    uint64_t                   bytesSent
)
{
    char stringBuffer[ 16 ];

    fprintf( stderr, "%s: Dst:%u Comm:%s Tag:%u Bytes:%llu\n",
             __func__,
             globalDestinationRank,
             silc_comm_to_string( stringBuffer, sizeof( stringBuffer ),
                                  "%x", communicatorHandle ),
             tag,
             ( unsigned long long )bytesSent );
}


/**
 * Generate an mpi recv event in the measurement system.
 */
void
SILC_MpiRecv
(
    uint32_t                   globalSourceRank,
    SILC_MPICommunicatorHandle communicatorHandle,
    uint32_t                   tag,
    uint64_t                   bytesReceived
)
{
    char stringBuffer[ 16 ];

    fprintf( stderr, "%s: Src:%u Comm:%s Tag:%u Bytes:%llu\n",
             __func__,
             globalSourceRank,
             silc_comm_to_string( stringBuffer, sizeof( stringBuffer ),
                                  "%x", communicatorHandle ),
             tag,
             ( unsigned long long )bytesReceived );
}


/**
 * Generate an mpi collective event in the measurement system.
 */
void
SILC_MpiCollective
(
    SILC_RegionHandle          regionHandle,
    SILC_MPICommunicatorHandle communicatorHandle,
    uint32_t                   globalRootRank,
    uint64_t                   bytesSent,
    uint64_t                   bytesReceived
)
{
    char stringBuffer[ 3 ][ 16 ];

    fprintf( stderr, "%s: Reg:%s Comm:%s Root:%s Sent:%llu Recv:%llu\n",
             __func__,
             silc_region_to_string( stringBuffer[ 0 ], sizeof( stringBuffer[ 0 ] ),
                                    "%x", regionHandle ),
             silc_comm_to_string( stringBuffer[ 1 ], sizeof( stringBuffer[ 1 ] ),
                                  "%x", communicatorHandle ),
             silc_uint32_to_string( stringBuffer[ 2 ], sizeof( stringBuffer[ 2 ] ),
                                    "%x", globalRootRank, SILC_INVALID_ROOT_RANK ),
             ( unsigned long long )bytesSent,
             ( unsigned long long )bytesReceived );
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

    fprintf( stderr, "%s: Reg:%s\n",
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

    fprintf( stderr, "%s: Reg:%s\n",
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
    fprintf( stderr, "%s: Lock:%x\n", __func__, lockId );
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
    fprintf( stderr, "%s: Lock:%x\n", __func__, lockId );
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
