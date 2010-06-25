#ifndef SILC_INTERNAL_DEFINITION_HANDLES_H
#define SILC_INTERNAL_DEFINITION_HANDLES_H

/*
 * This file is part of the SILC project (http://www.silc.de)
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


/**
 * @file       silc_definition_handles.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include <SILC_Memory.h>


#define SILC_MOVABLE_TYPE( type )                      \
    struct type ## _Movable                            \
    {                                                  \
        uint32_t page_id;                              \
        uint32_t offset;                               \
    }

/**
 * Use this macro to define a definition struct.
 *
 * Usage:
 * @code
 *     SILC_DEFINE_DEFINITION_TYPE_BEGIN( Type ) {
 *         SILC_DEFINITION_HEADER( Type, uint32_t ); // must be first
 *         // definition specfic members
 *         :
 *     };
 * @endcode
 *
 * @see SILC_DEFINITION_HEADER
 */
#define SILC_DEFINE_DEFINITION_TYPE( Type )            \
    SILC_MOVABLE_TYPE( SILC_ ## Type ## _Definition ); \
    struct SILC_ ## Type ## _Definition

/**
 * Provides a stub for the location struct header.
 * the sequence_number member is mostly use as the id for the local definitions
 *
 * @see SILC_DEFINE_DEFINITION_TYPE
 *
 * @note: This should be insync with the definition of @a silc_any_definition
 */
#define SILC_DEFINITION_HEADER( Type )          \
    SILC_ ## Type ## _Definition_Movable next;  \
    uint32_t sequence_number;                   \
    uint32_t hash_value

/**
 *  Dereferences a moveable memory pointer to the definition struct.
 *
 */
#define SILC_HANDLE_DEREF( handle, Type ) \
    ( SILC_MEMORY_DEREF_MOVABLE( handle, \
                                 SILC_ ## Type ## _Definition* ) )

/**
 *  Extracts the ID out of an handle pointer.
 *
 *  @note: This is only the process local sequence number, which
 *         may happen to be the OTF2 definition id.
 *
 *  @note You can take also the address of this member with this macro
 */
#define SILC_HANDLE_TO_ID( handle, Type ) \
    ( SILC_HANDLE_DEREF( handle, Type )->sequence_number )

/**
 *  Extracts the hash value out of an handle pointer.
 *
 *  @note You can take also the address of this member with this macro
 */
#define SILC_HANDLE_GET_HASH( handle, Type ) \
    ( SILC_HANDLE_DEREF( handle, Type )->hash_value )

typedef struct silc_any_definition         silc_any_definition;
typedef struct silc_any_definition_Movable silc_any_definition_Movable;
typedef silc_any_definition_Movable*       silc_any_handle;


SILC_MOVABLE_TYPE( silc_any_definition );
struct silc_any_definition
{
    silc_any_definition_Movable next;
    uint32_t                    sequence_number;
    uint32_t                    hash_value;
};


#endif /* SILC_INTERNAL_DEFINITION_HANDLES_H */
