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

#ifndef SCOREP_MEMORY_ATTRIBUTES_H
#define SCOREP_MEMORY_ATTRIBUTES_H

/**
 * Declaration of internal functions for memory attributes.
 */

#include <SCOREP_Events.h>

/**
 * init memory attributes.
 */
void
scorep_memory_attributes_init( void );

void
scorep_memory_attributes_add_enter_alloc_size( size_t memorySize );

void
scorep_memory_attributes_add_exit_dealloc_size( size_t memorySize );

void
scorep_memory_attributes_add_enter_argument_address( uint64_t argumentAddress );

/**
 * Writes memory reference attributes to an attribute list.
 *
 * @param returnAddress return address of malloc, calloc, realloc or new
 */
void
scorep_memory_attributes_add_exit_return_address( uint64_t returnAddress );

#endif /* SCOREP_MEMORY_ATTRIBUTES_H */
