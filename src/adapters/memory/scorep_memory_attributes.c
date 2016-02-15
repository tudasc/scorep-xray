/**
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2016,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */

/**
 * Implementation of internal functions for memory attributes.
 */

#include <config.h>

#include "scorep_memory_mgmt.h"
#include "scorep_memory_attributes.h"

#include <SCOREP_Events.h>
#include <SCOREP_Memory.h>
#include <SCOREP_Definitions.h>


#define SCOREP_DEBUG_MODULE_NAME MEMORY
#include <UTILS_Debug.h>
#include <UTILS_Error.h>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

/**
 * Memory attribute handles
 */
static SCOREP_AttributeHandle attribute_allocation_size;     /**< memory to allocate */
static SCOREP_AttributeHandle attribute_deallocation_size;   /**< memory to deallocate */
static SCOREP_AttributeHandle attribute_argument_address;    /**< argument address of realloc or free */
static SCOREP_AttributeHandle attribute_result_address;      /**< return address of malloc, calloc or realloc */


void
scorep_memory_attributes_init( void )
{
    attribute_allocation_size   = SCOREP_AllocMetric_GetAllocationSizeAttribute();
    attribute_deallocation_size = SCOREP_AllocMetric_GetDeallocationSizeAttribute();

    attribute_argument_address = SCOREP_Definitions_NewAttribute(
        "ARGUMENT_ADDRESS",
        "Address argument",
        SCOREP_ATTRIBUTE_TYPE_UINT64 );

    attribute_result_address = SCOREP_Definitions_NewAttribute(
        "RESULT_ADDRESS",
        "Result of allocation",
        SCOREP_ATTRIBUTE_TYPE_UINT64 );
}

void
scorep_memory_attributes_add_enter_alloc_size( size_t memorySize )
{
    UTILS_DEBUG_ENTRY( "%zu", memorySize );

    uint64_t mem_size = ( uint64_t )memorySize;
    SCOREP_AddAttribute( attribute_allocation_size, &mem_size );

    UTILS_DEBUG_EXIT();
}

void
scorep_memory_attributes_add_exit_dealloc_size( size_t memorySize )
{
    UTILS_DEBUG_ENTRY( "%zu", memorySize );

    uint64_t mem_size = ( uint64_t )memorySize;
    SCOREP_AddAttribute( attribute_deallocation_size, &mem_size );

    UTILS_DEBUG_EXIT();
}

void
scorep_memory_attributes_add_enter_argument_address( uint64_t argumentAddress )
{
    UTILS_DEBUG_ENTRY( "%p", ( void* )argumentAddress );

    SCOREP_AddAttribute( attribute_argument_address, &argumentAddress );

    UTILS_DEBUG_EXIT();
}

void
scorep_memory_attributes_add_exit_return_address( uint64_t resultAddress )
{
    UTILS_DEBUG_ENTRY( "%p", ( void* )resultAddress );

    SCOREP_AddAttribute( attribute_result_address, &resultAddress );

    UTILS_DEBUG_EXIT();
}
