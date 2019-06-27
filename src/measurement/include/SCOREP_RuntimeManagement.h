/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2015,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013, 2018,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */


#ifndef SCOREP_RUNTIMEMANAGEMENT_H
#define SCOREP_RUNTIMEMANAGEMENT_H


/**
 * @file
 *
 * @brief   Declaration of runtime management functions to be used by the
 *          adapter layer.
 *
 */


UTILS_BEGIN_C_DECLS

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
#include <stdint.h>

#include <SCOREP_DefinitionHandles.h>

/**
 * Indicates if the measurement system is initialized, i.e. if
 * SCOREP_InitMeasurement() has been executed.
 *
 * @return true if measurement system is initialized, false otherwise.
 */
bool
SCOREP_IsInitialized( void );


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
 * SCOREP_InitMppMeasurement().
 *
 * @see SCOREP_FinalizeMeasurement()
 */
void
SCOREP_InitMeasurement( void );


/**
 * Same as @s SCOREP_InitMeasurement but also provides the command arguments
 * to the measurement.
 */
void
SCOREP_InitMeasurementWithArgs( int   argc,
                                char* argv[] );


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
SCOREP_FinalizeMeasurement( void );


/**
 * Special initialization of the measurement system when using MPI. This
 * function must be called only after a successful call to PMPI_Init().
 *
 */
void
SCOREP_InitMppMeasurement( void );

/**
 * Registers an exit handler. The OpenMP implementation on JUMP uses
 * its own exit handler. This exit handler is registered after our
 * Score-P exit handler in the test cases. Causing segmentation faults
 * during finalization, due to invalid TPD variables. Thus, we register
 * an exit handler in the join event, which ensures that the Score-P
 * finalization can access threadprivate variables.
 */
void
SCOREP_RegisterExitHandler( void );

/**
 * Special finalization of the measurement system when using MPI. This
 * function must be called before the actual PMPI_Finalize() call.
 *
 */
void
SCOREP_FinalizeMppMeasurement( void );


/**
 * Enable event recording for this process. This is a noop if
 * SCOREP_RecordingEnabled() equals true.
 */
void
SCOREP_EnableRecording( void );


/**
 * Disable event recording for this process. This is a noop if
 * SCOREP_RecordingEnabled() equals false.
 */
void
SCOREP_DisableRecording( void );


/**
 * Predicate indicating if the process is recording events or not.
 *
 * @return True if the process is recording, false otherwise.
 */
bool
SCOREP_RecordingEnabled( void );

/**
 * Sets whether recording is enabled or disabled by default at measurement start.
 * Has only effect, when set during initialization.
 * @param enabled  If true, recording is enabled at program start until it is
 *                 explicitly disabled. If false, recording is disabled at
 *                 program start, until it is explicitly enabled.
 */
void
SCOREP_SetDefaultRecordingMode( bool enabled );


typedef int ( * SCOREP_ExitCallback )( void );

void
SCOREP_RegisterExitCallback( SCOREP_ExitCallback exitCallback );

/**
 * Get handle of location group.
 * @return Returns handle of current location group.
 */
SCOREP_LocationGroupHandle
SCOREP_GetLocationGroup( void );

/**
 * Get the initial timestamp of this process which denotes the begin of the
 * epoch.
 */
uint64_t
SCOREP_GetBeginEpoch( void );

/**
 * Get the final timestamp of this process which denotes the end of the
 * epoch.
 */
uint64_t
SCOREP_GetEndEpoch( void );

/**
 * Set an indication that the application aborted.
 */
void
SCOREP_SetAbortFlag( void );


/**
 *  Indicates that the unwinding mode is active.
 *
 */
#if HAVE( UNWINDING_SUPPORT )

extern bool scorep_is_unwinding_enabled;
#define SCOREP_IsUnwindingEnabled() scorep_is_unwinding_enabled

#else

#define SCOREP_IsUnwindingEnabled() 0

#endif

/**
 * Returns the full path to the executable, if available, otherwise 'PROGRAM'.
 * If the executable name is an executable file @a executableNameIsFile equals true.
 */
const char*
SCOREP_GetExecutableName( bool* executableNameIsFile );

/**
 * Returns the process-local region handle that represents the application.
 * Note that this handle might not be unique across all ranks for MPMD programs.
 */
SCOREP_RegionHandle
SCOREP_GetProgramRegion( void );

/*@}*/


UTILS_END_C_DECLS


#endif /* SCOREP_RUNTIMEMANAGEMENT_H */
