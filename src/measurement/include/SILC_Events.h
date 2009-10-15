#ifndef SILC_EVENTS_H
#define SILC_EVENTS_H

/**
 * @note there ist no final decision about taking the time stamp in the
 *       or in the measurement system!
 *       nonetheless I will add it here, removing it afterwards is simplier
 *       than adding it.
 *
 * Return the current wall-time.
 */
SILC_Time SILC_Wtime( void );

/**
 * Generate an enter event in the measurement system.
 *
 * @param regionHandle
 */
void SILC_Enter( SILC_Time*        timeStamp,
                 SILC_RegionHandle regionHandle );

/**
 * Generate an exit event in the measurement system.
 *
 * @param regionHandle
 */
void SILC_Exit( SILC_Time*        timeStamp,
                SILC_RegionHandle regionHandle );

/**
 * Generate an mpi send event in the measurement system.
 *
 * @param regionHandle
 */
void SILC_MpiSend( SILC_Time*                 timeStamp,
                   SILC_LocationHandle        destinationHandle,
                   SILC_MPICommunicatorHandle commHandle,
                   uint32_t                   tag,
                   uint64_t                   bytesSent );

/**
 * Generate an mpi send event in the measurement system.
 *
 * @param regionHandle
 */
void SILC_MpiRecv( SILC_Time*                 timeStamp,
                   SILC_LocationHandle        sourceHandle,
                   SILC_MPICommunicatorHandle commHandle,
                   uint32_t                   tag,
                   uint64_t                   bytesReceived );

/**
 * Generate an mpi begin collective event in the measurement system.
 *
 * @param regionHandle
 * @param matchingId a location unique id to match the MpiEndColl
 * @param rootHandle root rank in collective operation,
 *                   or SILC_INVALID_LOCATION
 */
void SILC_MpiBeginColl( SILC_Time*                 timeStamp,
                        uint32_t                   matchingId,
                        SILC_RegionHandle          regionHandle,
                        SILC_MPICommunicatorHandle commHandle,
                        SILC_LocationHandle        rootHandle,
                        uint64_t                   bytesSent,
                        uint64_t                   bytesReceived );

/**
 * Generate an mpi end collective event in the measurement system.
 *
 * @param regionHandle
 * @param matchingId a location unique id to match the MpiBeginColl
 */
void SILC_MpiEndColl( SILC_Time* timeStamp,
                      uint32_t   matchingId );

/**
 * parameter based profiling events
 *
 */
void SILC_TriggerParameterInt64( SILC_ParameterHandle parameterHandle,
                                 int64_t              value );

void SILC_TriggerParameterDouble( SILC_ParameterHandle parameterHandle,
                                  double               value );

void SILC_TriggerParameterString( SILC_ParameterHandle parameterHandle,
                                  const char*          value );


#endif /* SILC_EVENTS_H */
