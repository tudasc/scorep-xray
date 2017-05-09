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
 * Copyright (c) 2009-2011,
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

#ifndef SCOREP_BITSTRING_H
#define SCOREP_BITSTRING_H

/**
 * @file
 *
 *
 * Declaration of functions for manipulation of bitstrings.
 */

#include <stdint.h>
#include <stdbool.h>

/**
   Sets the bit at @a index in @a bitstring. No validity checks for the index and the
   bitstring are performed.
   @param bitstring A bitstring
   @param index     The index of the bit. The index of the first bit is 0.
 */
void
SCOREP_Bitstring_Set( uint8_t* bitstring,
                      uint32_t index );

/**
   Checks whether the bit at @a index in @a bitstring is set.
   No validity checks for the index and the bitstring are performed.
   @param bitstring A bitstring
   @param index     The index of the bit. The index of the first bit is 0.
   @returns true if the bit at position @a index is set.
 */
bool
SCOREP_Bitstring_IsSet( uint8_t* bitstring,
                        uint32_t index );

/**
   Sets all bits of a bitstring to 0.
   @param bitstring A bitstring
   @param length    Number of bits in the bitstring.
 */
void
SCOREP_Bitstring_Clear( uint8_t* bitstring,
                        uint32_t length );

/**
   Sets all bits of a bitstring to 1.
   @param bitstring A bitstring
   @param length    Number of bits in the bitstring.
 */
void
SCOREP_Bitstring_SetAll( uint8_t* bitstring,
                         uint32_t length );

/**
   Calculates the number of bytes that needed for a bitstring of a given number of bits
   @param length    Number of bits in the bitstring.
   @returns the minimum number of bytes that are required for a bitstring of @a length
            bits.
 */
uint32_t
SCOREP_Bitstring_GetByteSize( uint32_t length );

#endif /* SCOREP_BITSTRING_H */
