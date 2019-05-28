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
 * Copyright (c) 2009-2011, 2014, 2019,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_OPARI2_USER_REGIONS_H
#define SCOREP_OPARI2_USER_REGIONS_H

/**
 * @file
 *
 * @ingroup OPARI2
 *
 * @brief Declares functionality for interpretation of opari2 region strings.
 */

#include <SCOREP_Mutex.h>
#include <SCOREP_Opari2_Region_Info.h>
#include <pomp2_user_region_info.h>

/**
 * SCOREP_Opari2_RegionType
 * @ingroup OPARI2
 * @{
 *
 */

/**
   Lock to protect on-the-fly assignments.
 */
extern SCOREP_Mutex scorep_opari2_user_assign_lock;

/**
   @def SCOREP_OPARI2_USER_HANDLE_UNINITIALIZED_REGION( handle, ctc_string )
   Checks whether @a handle is initialized and initializes it if it is not.
   @param handle      A pointer to a POMP2_Region_handle.
   @param ctc_string  A string that contains the initialization information.
 */
#define SCOREP_OPARI2_USER_HANDLE_UNINITIALIZED_REGION( handle, ctc_string ) \
    if ( *handle == NULL )                                                 \
    {                                                                      \
        POMP2_USER_Assign_handle( handle, ctc_string );                    \
    }

/** Struct which contains all data for a user region. */
typedef struct
{
    SCOREP_Opari2_Region genericInfo;
    SCOREP_RegionHandle  regionHandle;
    char*                name;
} SCOREP_Opari2_User_Region;

/** @} */

#endif /* SCOREP_OPARI2_USER_REGIONS_H */
