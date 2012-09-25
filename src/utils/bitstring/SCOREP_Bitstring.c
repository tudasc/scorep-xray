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

/**
 * @file SCOREP_Bitstring.c
 *
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @status alpha
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
