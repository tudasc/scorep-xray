/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 *    RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
 *    Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 *    University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 *    Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

#ifndef SCOREP_INTERNAL_DEFINITIONS_H
#error "Do not include this header directly, use SCOREP_Definitions.h instead."
#endif

#ifndef SCOREP_PRIVATE_DEFINITIONS_COMMUNICATOR_H
#define SCOREP_PRIVATE_DEFINITIONS_COMMUNICATOR_H


/**
 * @file
 *
 *
 */

#include <stdarg.h>

#include <scorep/SCOREP_PublicTypes.h>
#include <SCOREP_DefinitionHandles.h>
#include <SCOREP_Memory.h>


SCOREP_DEFINE_DEFINITION_TYPE( InterimCommunicator )
{
    SCOREP_DEFINE_DEFINITION_HEADER( InterimCommunicator );

    SCOREP_StringHandle              name_handle;
    SCOREP_InterimCommunicatorHandle parent_handle;
    SCOREP_ParadigmType              paradigm_type;
};


SCOREP_DEFINE_DEFINITION_TYPE( Communicator )
{
    SCOREP_DEFINE_DEFINITION_HEADER( Communicator );

    SCOREP_GroupHandle        group_handle;
    SCOREP_StringHandle       name_handle;
    SCOREP_CommunicatorHandle parent_handle;
};


void
scorep_definitions_create_interim_communicator_counter_lock( void );


void
scorep_definitions_destroy_interim_communicator_counter_lock( void );


typedef uint32_t
( *scorep_definitions_init_payload_fn )( void*,
                                         uint32_t,
                                         va_list );


typedef bool
( *scorep_definitions_equal_payloads_fn )( const void*,
                                           const void* );

/**
 *  Creates a new interim communicator definition in the definition memory pool
 *  of the given location and maintains it in the given @a manager_entry.
 *  The definition can be augmented via the @a sizeOfPayload argument. This number
 *  of bytes is additionally allocated and a pointer to it will be returned
 *  via @a payloadOut.
 *  If the manager entry does has a hash table allocated and an @a init_payload_fn
 *  function pointer is given, this function is called to initialize the
 *  payload before checking for an already existing definition. Any additional
 *  arguments to this function is passed as an @a va_list to the @a init_payload_fn
 *  call. The @a init_payload_fn can also update the hash value of the definition.
 *  The current hash value is given as the second argument, the new hash value should
 *  be returned.
 *  When searching for an existing definition, the @a equal_payloads_fn function
 *  is used to compare the payloads of two definitions.
 *  If the definition was already known, @a *payloadOut will be NULL, else
 *  it points to the payload of the new definition.
 */
SCOREP_InterimCommunicatorHandle
SCOREP_Definitions_NewInterimCommunicatorInLocation(
    SCOREP_Location*                     location,
    SCOREP_InterimCommunicatorHandle     parentComm,
    SCOREP_ParadigmType                  paradigmType,
    scorep_definitions_init_payload_fn   init_payload_fn,
    scorep_definitions_equal_payloads_fn equal_payloads_fn,
    scorep_definitions_manager_entry*    manager_entry,
    size_t                               sizeOfPayload,
    void**                               payloadOut,
    ... );


SCOREP_InterimCommunicatorHandle
SCOREP_InterimCommunicatorHandle_GetParent( SCOREP_InterimCommunicatorHandle commHandle );


SCOREP_CommunicatorHandle
SCOREP_Definitions_NewCommunicator( SCOREP_GroupHandle        group_handle,
                                    const char*               name,
                                    SCOREP_CommunicatorHandle parent_handle );


SCOREP_CommunicatorHandle
SCOREP_Definitions_NewUnifiedCommunicator( SCOREP_GroupHandle        group_handle,
                                           const char*               name,
                                           SCOREP_CommunicatorHandle parent_handle );


void
scorep_definitions_unify_communicator( SCOREP_CommunicatorDef*       definition,
                                       SCOREP_Allocator_PageManager* handlesPageManager );


#endif /* SCOREP_PRIVATE_DEFINITIONS_COMMUNICATOR_H */
