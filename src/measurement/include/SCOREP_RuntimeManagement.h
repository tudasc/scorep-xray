/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen University, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


#ifndef SCOREP_RUNTIMEMANAGEMENT_H
#define SCOREP_RUNTIMEMANAGEMENT_H


/**
 * @file        SCOREP_RuntimeManagement.h
 * @maintainer  Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @brief   Declaration of runtime management functions to be used by the
 *          adapter layer.
 *
 */


/**
 * @defgroup SCOREP_RuntimeManagement SCOREP Runtime Management
 *
 * Before the adapters can use the @ref SCOREP_Definitions definitions interface
 * and the @ref SCOREP_Events event interface the measurement system needs to be
 * initialized. Therefore we require that one (arbitrary) adapter calls
 * SCOREP_InitMeasurement(). There the initialization of all adapter in use is
 * triggered. Adapters may also explicitly finalize the measurement system by
 * calling SCOREP_FinalizeMeasurement() which is usually implicitly done. During
 * measurement the recording of events can be dis- or enabled or temporarily
 * interrupted (see the todos).
 *
 * @todo Periscope interface function. Milestone 2
 *
 * @todo SCOREP_EnableRecording() and SCOREP_DisableRecording(). Specify what will
 * happen internally. Will the dis/enabling be proccess-wise or will there be
 * a global synchronization (if possible from inside the measurement). Global
 * synchronization can be demanded from the user. Process-local dis/enabling
 * will render the traces useless for Scalasca analysis.
 */
/*@{*/


#include <stdbool.h>


/**
 * Indicates if the measurement system is initialized, i.e. if
 * SCOREP_InitMeasurement() has been executed.
 *
 * @return true if measurement system is initialized, false otherwise.
 */
bool
SCOREP_IsInitialized();


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
 * SCOREP_InitMeasurementMPI().
 *
 * @see SCOREP_FinalizeMeasurement()
 */
void
SCOREP_InitMeasurement();



/**
 * Finalize the measurement system. This function @e may be called from the
 * adapter layer (or implicitly by at_exit). Calling other API functions
 * afterwards is seen as undefined behaviour.
 *
 * Calling this function several times does no harm to the measurement system.
 *
 * @see SCOREP_InitMeasurement()
 */
void
SCOREP_FinalizeMeasurement();


/**
 * Special initialization of the measurement system when using MPI. This
 * function must be called only after a successful call to PMPI_Init().
 *
 */
void
SCOREP_InitMeasurementMPI( int rank );

/**
 * Registers an exit handler. The OpenMP implementation on JUMP uses
 * its own exit handler. This exit handler is registered after our
 * Score-P exit handler in the test cases. Causing segmentation faults
 * during finalization, due to invalid TPD variables. Thus, we register
 * an exit handler in the join event, which ensures that the Score-P
 * finalization can access threadprivate variables.
 */
void
SCOREP_RegisterExitHandler();

/**
 * Special initialization of the measurement system when using MPI. This
 * function must be called directly after a successful call to PMPI_Init().
 *
 */
void
SCOREP_OnPMPI_Init();


/**
 * Special finalization of the measurement system when using MPI. This
 * function must be called before the actual PMPI_Finalize() call.
 *
 */
void
SCOREP_FinalizeMeasurementMPI();


/**
 * Special finalization of the measurement system when using MPI. This
 * function must be called directly after the actual PMPI_Finalize() call.
 *
 */
void
SCOREP_OnPMPI_Finalize();


/**
 * Enable event recording for this process. This is a noop if
 * SCOREP_RecordingEnabled() equals true.
 *
 */
void
SCOREP_EnableRecording();


/**
 * Disable event recording for this process. This is a noop if
 * SCOREP_RecordingEnabled() equals false.
 *
 */
void
SCOREP_DisableRecording();


/**
 * Predicate indicating if the process is recording events or not.
 *
 * @return True if the process is recording, false otherwise.
 */
bool
SCOREP_RecordingEnabled();


typedef int ( *SCOREP_ExitCallback )( void );

void
SCOREP_RegisterExitCallback( SCOREP_ExitCallback exitCallback );

/*@}*/


#endif /* SCOREP_RUNTIMEMANAGEMENT_H */
