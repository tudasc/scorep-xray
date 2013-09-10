/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */

#ifndef SCOREP_POMP_COMMON_H
#define SCOREP_POMP_COMMON_H

/**
 * @file
 * @ingroup    POMP2
 *
 * @brief Declaration of global variables, used in several source files of the
          POMP2 adapter.
 */

#include <scorep/SCOREP_PublicTypes.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_RuntimeManagement.h>
#include <stdbool.h>

/** @ingroup POMP2
    @{
 */

/* **************************************************************************************
                                                              Defined in SCOREP_Pomp_User.c
****************************************************************************************/

/**
    Flag to indicate, whether POMP tracing is enable/disabled
 */
extern bool scorep_pomp_is_tracing_on;

/**
   Flag that indicates whether the POMP2 adapter is initialized.
 */
extern bool scorep_pomp_is_initialized;

/**
    Flag that indicates whether the POMP2 adapter is finalized
 */
extern bool scorep_pomp_is_finalized;

/**
   Lock to protect on-the-fly assignments.
 */
extern SCOREP_Mutex scorep_pomp_assign_lock;

/**
   @def SCOREP_POMP2_ENSURE_INITIALIZED
   Checks whether pomp adapter is initialized and if not initializes the measurement
   system.
 */
#define SCOREP_POMP2_ENSURE_INITIALIZED      \
    if ( !scorep_pomp_is_initialized )       \
    {                                        \
        SCOREP_InitMeasurement();                        \
    }

/**
   @def POMP2_HANDLE_UNITIALIZED_REGION( handle, ctc_string )
   Checks whether @a handle is initialized and initializes it if it is not.
   @param handle      A pointer to a POMP2_Region_handle.
   @param ctc_string  A string that contains the initialization information.
 */
#define SCOREP_POMP2_HANDLE_UNITIALIZED_REGION( handle, ctc_string ) \
    if ( *handle == NULL )                                           \
    {                                                                \
        SCOREP_MutexLock( scorep_pomp_assign_lock );                 \
        if ( *handle == NULL )                                       \
        {                                                            \
            POMP2_Assign_handle( handle, ctc_string );               \
        }                                                            \
        SCOREP_MutexUnlock( scorep_pomp_assign_lock );               \
    }

/** Adapter initialization function.
 */
SCOREP_ErrorCode
scorep_pomp_adapter_init( void );

/** Adapter finalization function.
 */
void
scorep_pomp_adapter_finalize( void );

/** @} */

#endif // SCOREP_POMP_COMMON_H
