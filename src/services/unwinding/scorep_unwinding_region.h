/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_UNWINDING_REGION_H
#define SCOREP_UNWINDING_REGION_H


#include "scorep_unwinding_mgmt.h"


/** Inserts a new region into the known regions set.
 *
 *  Caller must ensure that this region is not known already.
 *
 *  @param unwindData  The unwindData which holds the known regions
 *  @param start     The start address of the region (inclusive)
 *  @param end       The end address of the region (exclusive)
 *  @param name      Name of the region
 *
 *  @return New region
 */
scorep_unwinding_region*
scorep_unwinding_region_insert( SCOREP_Unwinding_CpuLocationData* unwindData,
                                uint64_t                          start,
                                uint64_t                          end,
                                const char*                       name );


/** Finds a known region by an address inside the range of the region.
 *
 *  @param unwindData  The unwindData which holds the known regions
 *  @param addr      The address to search the region for
 *
 *  @return Found region, or @a NULL of not found
 */
scorep_unwinding_region*
scorep_unwinding_region_find( SCOREP_Unwinding_CpuLocationData* unwindData,
                              uint64_t                          addr );


/** Function type for the cleanup argument of @a scorep_unwinding_region_clear. */
typedef void ( * scorep_unwinding_region_cleanup )( scorep_unwinding_region* node,
                                                    void*                    arg );

/** Clears all known regions and apply a custom operation before tha fact
 *
 *  @param unwindData  The unwindData which holds the known regions
 *  @param cleanup   Opeation to be performed before region is cleared
 *  @param arg       Argument to @p cleanup
 */
void
scorep_unwinding_region_clear( SCOREP_Unwinding_CpuLocationData* unwindData,
                               scorep_unwinding_region_cleanup   cleanup,
                               void*                             arg );


#endif /* SCOREP_UNWINDING_REGION_H */
