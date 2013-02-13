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

#ifndef SCOREP_POMP_VARIABLES_H
#define SCOREP_POMP_VARIABLES_H

/**
 * @file       SCOREP_Pomp_Variables.h
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     alpha
 * @ingroup    POMP2
 *
 * @brief Declaration of global variables, used in several source files of the
          POMP2 adapter.
 */

#include <scorep/SCOREP_PublicTypes.h>
#include <SCOREP_Mutex.h>
#include <stdbool.h>

/** @ingroup POMP2
    @{
 */

/* **************************************************************************************
                                                              Defined in SCOREP_Pomp_User.c
****************************************************************************************/

/**
   Contains the region handle for all implicit barriers
 */
extern SCOREP_RegionHandle scorep_pomp_implicit_barrier_region;

/**
    Flag to indicate, wether POMP tracing is enable/disabled
 */
extern bool scorep_pomp_is_tracing_on;

/**
   Flag that indicates whether the POMP2 adapter is initialized.
 */
extern bool scorep_pomp_is_initialized;

/**
    Flag that indicates wether the POMP2 adapter is finalized
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
        POMP2_Init();                        \
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


/** @} */

#endif // SCOREP_POMP_VARIABLES_H
