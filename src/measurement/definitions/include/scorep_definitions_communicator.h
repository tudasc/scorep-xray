/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2016-2017,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013, 2017,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
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


SCOREP_DEFINE_DEFINITION_TYPE( InterimCommunicator )
{
    SCOREP_DEFINE_DEFINITION_HEADER( InterimCommunicator );

    SCOREP_StringHandle              name_handle;
    SCOREP_InterimCommunicatorHandle parent_handle;
    SCOREP_ParadigmType              paradigm_type;

    /* counter for RMA windows created with this comm */
    uint32_t rma_window_creation_counter;
};


SCOREP_DEFINE_DEFINITION_TYPE( Communicator )
{
    SCOREP_DEFINE_DEFINITION_HEADER( Communicator );

    SCOREP_GroupHandle        group_handle;
    SCOREP_StringHandle       name_handle;
    SCOREP_CommunicatorHandle parent_handle;
    uint32_t                  unify_key;
};


void
scorep_definitions_create_interim_communicator_counter_lock( void );


void
scorep_definitions_destroy_interim_communicator_counter_lock( void );


uint32_t
scorep_definitions_interim_communicator_get_rma_window_creation_counter( SCOREP_InterimCommunicatorHandle communicatorHandle );


/**
 * Associate a MPI communicator with a process unique communicator handle.
 *
 * The creator of an @a InterimCommunicator definition must convert this in a
 * pre-unify hook to a @a Communicator definition, and assign the result
 * to the @a unified member of the @a InterimCommunicator definition.
 *
 * @param parentComm    A possible parent communicator.
 * @param paradigmType  The paradigm of the adapter which defines this
 *                      communicator.
 * @param sizeOfPayload The size of the payload which the adapter requests
 *                      for this communicator.
 * @param[out] payload  Will be set to the memory location of the payload.
 *
 * @return A process unique communicator handle.
 *
 */
SCOREP_InterimCommunicatorHandle
SCOREP_Definitions_NewInterimCommunicator( SCOREP_InterimCommunicatorHandle parentComm,
                                           SCOREP_ParadigmType              paradigmType,
                                           size_t                           sizeOfPayload,
                                           void**                           payload );


typedef uint32_t
( * scorep_definitions_init_payload_fn )( void*,
                                          uint32_t,
                                          va_list );


typedef bool
( * scorep_definitions_equal_payloads_fn )( const void*,
                                            const void* );

struct SCOREP_Location;

/**
 *  Creates a new interim communicator definition either in the definition
 *  memory pool of the given @a location argument or in the common pool. Locking
 *  is only done when @a location points to NULL.
 *  It is maintained in the given (non-null) @a managerEntry argument.
 *  The definition can be augmented via the @a sizeOfPayload argument. This number
 *  of bytes is additionally allocated and a pointer to it will be returned
 *  via @a payloadOut.
 *  If the manager entry does has a hash table allocated and an @a initPayloadFn
 *  function pointer is given, this function is called to initialize the
 *  payload before checking for an already existing definition. Any additional
 *  arguments to this function is passed as an @a va_list to the @a initPayloadFn
 *  call. The @a initPayloadFn can also update the hash value of the definition.
 *  The current hash value is given as the second argument, the new hash value should
 *  be returned.
 *  When searching for an existing definition, the @a equalPayloadsFn function
 *  is used to compare the payloads of two definitions.
 *  If the definition was already known, @a *payloadOut will be NULL, else
 *  it points to the payload of the new definition.
 *
 *  @todo List valid combination of location/managerEntry/initPayloadFn/equalPayloadsFn
 *
 *  Common usages:
 *  (1) Use a location shared (i.e., per-process) definition pool but avoid duplicates:
 *      - @a location: NULL, to get approriate locking
 *      - @a managerEntry:: a per-process @a scorep_definitions_manager_entry object and allocate
 *        a hash table for it (see scorep_definitions_manager_entry_alloc_hash_table).
 *      - @a initPayloadFn and @a equalPayloadsFn:: appropriate callbacks
 *        to initialize and test for equality of the payloads
 *      - @a sizeOfPayload:: the size of the paload struct
 *      - @a payloadOut:: pointer to payload, if NULL on return the definition already existed
 *      - @a ...:: an arbitrary number of arguments which are passed on to @a initPayloadFn
 *  (2) Use a per-location, (mostly) lockfree, definition pool:
 *      - @a location: the current location
 *      - @a managerEntry:: a location private @a scorep_definitions_manager_entry object
 *      - (optional) avoid duplicates like in usage (1)
 */
SCOREP_InterimCommunicatorHandle
SCOREP_Definitions_NewInterimCommunicatorCustom(
    struct SCOREP_Location*              location,
    scorep_definitions_manager_entry*    managerEntry,
    scorep_definitions_init_payload_fn   initPayloadFn,
    scorep_definitions_equal_payloads_fn equalPayloadsFn,
    SCOREP_InterimCommunicatorHandle     parentComm,
    SCOREP_ParadigmType                  paradigmType,
    size_t                               sizeOfPayload,
    void**                               payloadOut,
    ... );


SCOREP_InterimCommunicatorHandle
SCOREP_InterimCommunicatorHandle_GetParent( SCOREP_InterimCommunicatorHandle commHandle );


/**
 * Get access to the payload from a communicator definition.
 */
void*
SCOREP_InterimCommunicatorHandle_GetPayload( SCOREP_InterimCommunicatorHandle handle );


/**
 * Set the name of the communicator to @a name, but only if it wasn't done before.
 * If @a name is NULL, it is assigned the empty string.
 *
 *  @param commHandle  The comm in question.
 *  @param name        The name for this communicator.
 *
 */
void
SCOREP_InterimCommunicatorHandle_SetName( SCOREP_InterimCommunicatorHandle commHandle,
                                          const char*                      name );


/**
 *  Creates a communicator definition.
 *
 *  @note  These handle cannot be used in any event functions.
 *
 *  @param group     The group of locations which are members of this communicator.
 *  @param name      The name of this communicator. May be @a SCOREP_INVALID_STRING.
 *                   In this case, the unification of will take an arbitrary
 *                   non-SCOREP_INVALID_STRING value when merging structural
 *                   equal communicators. If it is still @a NULL after the
 *                   unification, it is assigned the empty string. The parameter
 *                   is not of type 'const char*' to ease converting InterimComm
 *                   handles to Comm handles, as the former already have a
 *                   StringHandle member.
 *  @param parent    The parent.
 *  @param unifyKey  An arbitrary integer value to allow multiple definition of
 *                   the same structural equal communicators.
 *
 *  @return The handle for this new communicator.
 */
SCOREP_CommunicatorHandle
SCOREP_Definitions_NewCommunicator( SCOREP_GroupHandle        group,
                                    SCOREP_StringHandle       name,
                                    SCOREP_CommunicatorHandle parent,
                                    uint32_t                  unifyKey );


void
scorep_definitions_unify_communicator( SCOREP_CommunicatorDef*              definition,
                                       struct SCOREP_Allocator_PageManager* handlesPageManager );


#endif /* SCOREP_PRIVATE_DEFINITIONS_COMMUNICATOR_H */
