#ifndef SILC_API_EVENTS_H
#define SILC_API_EVENTS_H

/**
 * @note there ist no final decision about taking the time stamp in the
 *       or in the measurement system!
 *       nonetheless I will add it here, removing it afterwards is simplier
 *       than adding it.
 *
 * Return the current wall-time.
 */
SILC_API_Time SILC_API_Wtime( void );

/**
 * Generate an enter event in the measurement system.
 *
 * @param regionHandle
 */
void SILC_API_Enter( SILC_API_Time*        timeStamp,
                     SILC_API_RegionHandle regionHandle );

/**
 * Generate an exit event in the measurement system.
 *
 * @param regionHandle
 */
void SILC_API_Exit( SILC_API_Time*        timeStamp,
                    SILC_API_RegionHandle regionHandle );

/**
 * Generate an mpi send event in the measurement system.
 *
 * @param regionHandle
 */
void SILC_API_MpiSend( SILC_API_Time*                 timeStamp,
                       SILC_API_LocationHandle        destinationHandle,
                       SILC_API_MPICommunicatorHandle commHandle,
                       uint32_t                       tag,
                       uint64_t                       bytesSent );

/**
 * Generate an mpi send event in the measurement system.
 *
 * @param regionHandle
 */
void SILC_API_MpiRecv( SILC_API_Time*                 timeStamp,
                       SILC_API_LocationHandle        sourceHandle,
                       SILC_API_MPICommunicatorHandle commHandle,
                       uint32_t                       tag,
                       uint64_t                       bytesReceived );

/**
 * Generate an mpi begin collective event in the measurement system.
 *
 * @param regionHandle
 * @param matchingId a location unique id to match the MpiEndColl
 * @param rootHandle root rank in collective operation,
 *                   or SILC_API_INVALID_LOCATION
 */
void SILC_API_MpiBeginColl( SILC_API_Time*                 timeStamp,
                            uint32_t                       matchingId,
                            SILC_API_RegionHandle          regionHandle,
                            SILC_API_MPICommunicatorHandle commHandle,
                            SILC_API_LocationHandle        rootHandle,
                            uint64_t                       bytesSent,
                            uint64_t                       bytesReceived );

/**
 * Generate an mpi end collective event in the measurement system.
 *
 * @param regionHandle
 * @param matchingId a location unique id to match the MpiBeginColl
 */
void SILC_API_MpiEndColl( SILC_API_Time* timeStamp,
                          uint32_t       matchingId );

#endif /* SILC_API_EVENTS_H */
