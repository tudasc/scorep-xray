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
 * Copyright (c) 2009-2012,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012, 2014
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_PROFILE_CONVERTER_H
#define SCOREP_PROFILE_CONVERTER_H

/**
 * @file
 *
 * Declares some converter defines.
 *
 */

/* ***************************************************************************************
   Defines
*****************************************************************************************/

/**
   @def SCOREP_PROFILE_POINTER2DATA( handle )
   Casts a pointer to node specific data type.
   @param handle The pointer which is casted to node specific data.
 */
#if SIZEOF_VOID_P == 8
#define SCOREP_PROFILE_POINTER2INT( ptr ) ( ( uint64_t )ptr )
#elif SIZEOF_VOID_P == 4
#define SCOREP_PROFILE_POINTER2INT( ptr ) ( ( uint32_t )ptr )
#else
#error Unsupported architecture. Only 32 bit and 64 bit architectures are supported.
#endif

/**
   @def SCOREP_PROFILE_DATA2POINTER( data )
   Casts a node specific data item to a region handle.
   @param data The node specific data that is casted to a pointer.
 */
#if SIZEOF_VOID_P == 8
#define SCOREP_PROFILE_INT2POINTER( data )   ( ( void* )( uint64_t )data )
#elif SIZEOF_VOID_P == 4
#define SCOREP_PROFILE_INT2POINTER( data )   ( ( void* )( uint32_t )data )
#else
#error Unsupported architecture. Only 32 bit and 64 bit architectures are supported.
#endif

#endif /* SCOREP_PROFILE_CONVERTER_H */
