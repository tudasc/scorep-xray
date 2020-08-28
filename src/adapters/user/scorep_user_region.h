/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012, 2019,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 *  @file
 *
 */

#ifndef SCOREP_USER_REGION_H
#define SCOREP_USER_REGION_H

#include <scorep/SCOREP_User_Types.h>

#include <stdint.h>
#include <stdbool.h>

UTILS_BEGIN_C_DECLS

#define SCOREP_FILTERED_USER_REGION ( ( void* )-1 )

void
scorep_user_region_init_c_cxx( SCOREP_User_RegionHandle*    handle,
                               const char**                 lastFileName,
                               SCOREP_SourceFileHandle*     lastFile,
                               const char*                  name,
                               const SCOREP_User_RegionType regionType,
                               const char*                  fileName,
                               const uint32_t               lineNo );

void
scorep_user_region_enter( const SCOREP_User_RegionHandle handle );

void
scorep_user_region_exit( const SCOREP_User_RegionHandle handle );

void
scorep_user_rewind_region_enter( const SCOREP_User_RegionHandle handle );

void
scorep_user_rewind_region_exit( const SCOREP_User_RegionHandle handle,
                                bool                           value );

void
scorep_user_region_by_name_begin( const char*                  name,
                                  const SCOREP_User_RegionType regionType,
                                  const char*                  fileName,
                                  const uint32_t               lineNo );

void
scorep_user_region_by_name_end( const char* name );


UTILS_END_C_DECLS

#endif /* SCOREP_USER_REGION_H */
