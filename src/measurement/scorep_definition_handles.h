/*
 * This file is part of the SCOREP project (http://www.scorep.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

#ifndef SCOREP_INTERNAL_DEFINITION_HANDLES_H
#define SCOREP_INTERNAL_DEFINITION_HANDLES_H



/**
 * @file       scorep_definition_handles.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include <SCOREP_Memory.h>


/**
 * Use this macro to define a definition struct.
 *
 * Usage:
 * @code
 *     SCOREP_DEFINE_DEFINITION_TYPE( Type ) {
 *         SCOREP_DEFINITION_HEADER( Type, uint32_t ); // must be first
 *         // definition specfic members
 *         :
 *     };
 * @endcode
 *
 * @see SCOREP_DEFINITION_HEADER
 */
#define SCOREP_DEFINE_DEFINITION_TYPE( Type ) \
    typedef struct SCOREP_ ## Type ## _Definition SCOREP_ ## Type ## _Definition; \
    struct SCOREP_ ## Type ## _Definition

/**
 * Provides a stub for the location struct header.
 * the sequence_number member is mostly use as the id for the local definitions
 *
 * @see SCOREP_DEFINE_DEFINITION_TYPE
 *
 * @note: This should be insync with the definition of @a scorep_any_definition
 */
#define SCOREP_DEFINITION_HEADER( Type )          \
    SCOREP_ ## Type ## Handle next;  \
    uint32_t sequence_number;                   \
    uint32_t hash_value

/**
 *  Dereferences a moveable memory pointer to the definition struct.
 *
 */
#define SCOREP_HANDLE_DEREF( handle, Type ) \
    ( SCOREP_MEMORY_DEREF_MOVABLE( handle, SCOREP_ ## Type ## _Definition* ) )

/**
 *  Extracts the ID out of an handle pointer.
 *
 *  @note: This is only the process local sequence number, which
 *         may happen to be the OTF2 definition id.
 *
 *  @note You can take also the address of this member with this macro
 */
#define SCOREP_HANDLE_TO_ID( handle, Type ) \
    ( SCOREP_HANDLE_DEREF( handle, Type )->sequence_number )

/**
 *  Extracts the hash value out of an handle pointer.
 *
 *  @note You can take also the address of this member with this macro
 */
#define SCOREP_HANDLE_GET_HASH( handle, Type ) \
    ( SCOREP_HANDLE_DEREF( handle, Type )->hash_value )

typedef struct scorep_any_definition scorep_any_definition;
typedef uint64_t                     scorep_any_handle;


struct scorep_any_definition
{
    scorep_any_handle next;
    uint32_t          sequence_number;
    uint32_t          hash_value;
};


#endif /* SCOREP_INTERNAL_DEFINITION_HANDLES_H */
