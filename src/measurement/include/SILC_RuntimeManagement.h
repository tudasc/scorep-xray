#ifndef SILC_RUNTIMEMANAGEMENT_H
#define SILC_RUNTIMEMANAGEMENT_H


/**
 * @file    SILC_RuntimeManagement.h
 * @author  Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 * @date    Started Wed Sep  2 18:44:31 2009
 *
 * @brief Declaration of runtime management functions to be used by the
 *        adapter layer.
 *
 */


/**
 * The adapter might provide a callback of this type to be initialized by the
 * measuremt system.
 */
typedef void ( *InitAdapterCallback )();


/**
 * Initialize the measurement system from the adapter layer. This function
 * needs to be called at least once by every adapter in use before any other
 * API function is called. Calling other API functions before is seen as
 * undefined behaviour. For performance reasons the adapter should keep track
 * of it's initialization status and call this function only once. The adapter
 * might provide a callback that is triggered within this function. If this
 * function is called several times from the same adapter, the callback is
 * triggered several times too. Calling this function several times does no
 * harm to the measurement system.
 *
 * @param initAdapterCallback A pointer to a function or NULL.
 *
 */
void SILC_InitMeasurement( InitAdapterCallback initAdapterCallback );

/**
 * Globaly disable and enable event recording.
 *
 * These functions do not guarantee to succeed in it's operation. The
 * measurement system may still need full control to the event generation.
 */
void SILC_EnableRecording( void );
void SILC_DisableRecording( void );
bool SILC_RecordingEnabled( void );

/**
 * Supend the measurement in order to obtain data or reconfigure it. Has no
 * effect if measurement is supended. Details need to be specified.
 *
 */
void SILC_SuspendMeasurement();


/**
 * Resume measurement after it was suspended. Has no effect if called when
 * measurement is active resp. resumed. Details need to be specified.
 *
 */
void SILC_ResumeMeasurement();


#endif /* SILC_RUNTIMEMANAGEMENT_H */
