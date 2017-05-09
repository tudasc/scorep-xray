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

/**
 * @file
 *
 *
 * Implementation of functions for manipulation of bitstrings.
 */

#include <config.h>
#include <SCOREP_Bitstring.h>

#include <string.h>

void
SCOREP_Bitstring_Set( uint8_t* bitstring, uint32_t index )
{
    bitstring[ index / 8 ] |= ( ( uint8_t )0x80 ) >> ( index % 8 );
}

bool
SCOREP_Bitstring_IsSet( uint8_t* bitstring, uint32_t index )
{
    return bitstring[ index / 8 ] & ( ( ( uint8_t )0x80 ) >> ( index % 8 ) );
}

void
SCOREP_Bitstring_Clear( uint8_t* bitstring, uint32_t length )
{
    memset( bitstring, 0x00, ( length + 7 ) / 8 );
}

void
SCOREP_Bitstring_SetAll( uint8_t* bitstring, uint32_t length )
{
    memset( bitstring, 0xFF, ( length + 7 ) / 8 );
}

uint32_t
SCOREP_Bitstring_GetByteSize( uint32_t length )
{
    return ( length + 7 ) / 8;
}
