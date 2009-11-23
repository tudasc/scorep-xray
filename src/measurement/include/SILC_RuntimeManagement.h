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


#ifndef SILC_RUNTIMEMANAGEMENT_H
#define SILC_RUNTIMEMANAGEMENT_H


/**
 * @file        SILC_RuntimeManagement.h
 * @maintainer  Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @brief   Declaration of runtime management functions to be used by the
 *          adapter layer.
 *
 */


/**
 * @defgroup SILC_RuntimeManagement SILC Runtime Management
 *
 * Before the adapters can use the @ref SILC_Definitions definitions interface
 * and the @ref SILC_Events event interface the measurement system needs to be
 * initialized. Therefore we require that one (arbitrary) adapter calls
 * SILC_InitMeasurement(). There the initialization of all adapter in use is
 * triggered. Adapters may also explicitly finalize the measurement system by
 * calling SILC_FinalizeMeasurement() which is usually implicitly done. During
 * measurement the recording of events can be dis- or enabled or temporarily
 * interrupted (see the todos).
 *
 * @todo Periscope interface function. Milestone 2
 *
 * @todo SILC_EnableRecording() and SILC_DisableRecording(). Specify what will
 * happen internally. Will the dis/enabling be proccess-wise or will there be
 * a global synchronization (if possible from inside the measurement). Global
 * synchronization can be demanded from the user. Process-local dis/enabling
 * will render the traces useless for Scalasca analysis.
 */
/*@{*/


#include <stdbool.h>


/**
 * Indicates if the measurement system is initialized, i.e. if
 * SILC_InitMeasurement() has been executed.
 *
 * @return true if measurement system is initialized, false otherwise.
 */
bool
SILC_IsInitialized
(
);


/**
 * Initialize the measurement system from the adapter layer. This function
 * needs to be called at least once by an (arbitrary) adapter before any other
 * measurement API function is called. Calling other API functions before is
 * seen as undefined behaviour. The first call to this function triggers the
 * initialization of all adapters in use.
 *
 * For performance reasons the adapter should keep track of it's
 * initialization status and call this function only once.
 *
 * Calling this function several times does no harm to the measurement system.
 *
 * Each arising error leads to a fatal abortion of the program.
 *
 * @note The MPI adapter needs special treatment, see
 * SILC_InitMeasurementMPI().
 *
 * @see SILC_FinalizeMeasurement()
 */
void
SILC_InitMeasurement
(
);


/**
 * Finalize the measurement system. This function @e may be called from the
 * adapter layer (or implicitly by at_exit). Calling other API functions
 * afterwards is seen as undefined behaviour.
 *
 * Calling this function several times does no harm to the measurement system.
 *
 * @see SILC_InitMeasurement()
 */
void
SILC_FinalizeMeasurement
(
);


/**
 * Special initialization of the measurement system when using MPI. This
 * function must be called only after a successful call to PMPI_Init.
 *
 */
void
SILC_InitMeasurementMPI
(
);


/**
 * Enable event recording for this process. This is a noop if
 * SILC_RecordingEnabled() equals true.
 *
 */
void
SILC_EnableRecording
(
);


/**
 * Disable event recording for this process. This is a noop if
 * SILC_RecordingEnabled() equals false.
 *
 */
void
SILC_DisableRecording
(
);


/**
 * Predicate indicating if the process is recording events or not.
 *
 * @return True if the process is recording, false otherwise.
 */
bool
SILC_RecordingEnabled
(
);


/*@}*/


#endif /* SILC_RUNTIMEMANAGEMENT_H */
