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
 *         SILC_DEFINITION_HEADER( Type ); // must be first
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
 * @see SILC_DEFINE_DEFINITION_TYPE
 */
#define SILC_DEFINITION_HEADER( Type )        \
    SILC_ ## Type ## Definition_Movable next; \
    uint32_t id

/**
 *  Extracts the ID out of an handle pointer.
 *
 * The handle pointer needs to be a non-zero pointer.
 */
#define SILC_HANDLE_TO_ID( handle, Type ) \
    ( ( SILC_MEMORY_DEREF_MOVABLE( ( handle ), \
                                   SILC_ ## Type ## _Definition* ) )->id )

typedef struct silc_any_definition         silc_any_definition;
typedef struct silc_any_definition_Movable silc_any_definition_Movable;
typedef silc_any_definition_Movable*       silc_any_handle;


SILC_MOVABLE_TYPE( silc_any_definition );
struct silc_any_definition
{
    silc_any_definition_Movable next;
    uint32_t                    id;
};


#endif /* SILC_INTERNAL_DEFINITION_HANDLES_H */
