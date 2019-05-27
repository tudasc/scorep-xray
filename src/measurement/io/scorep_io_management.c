/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2017, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 * @brief
 */

#include <config.h>

#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include <scorep_environment.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Hashtab.h>
#include <SCOREP_IoManagement.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_Platform.h>
#include <SCOREP_Events.h>
#include <jenkins_hash.h>
#include <scorep_subsystem_management.h>
#include <scorep_substrates_definition.h>

#define SCOREP_DEBUG_MODULE_NAME IO_MANAGEMENT
#include <UTILS_Debug.h>

#define SCOREP_IO_FILE_HASHTABLE_SIZE 64

#define SCOREP_IO_HANDLE_HASHTABLE_POWER 6
#define SCOREP_IO_HANDLE_HASHTABLE_MASK hashmask( SCOREP_IO_HANDLE_HASHTABLE_POWER )
#define SCOREP_IO_HANDLE_HASHTABLE_SIZE hashsize( SCOREP_IO_HANDLE_HASHTABLE_POWER )

static SCOREP_Hashtab* io_file_handle_hashtable = NULL;

/** @brief Payload in every IoHandleHandle definition. */
typedef struct io_handle_payload
{
    /** @brief Hash value of the paradigm specific I/O handle */
    int64_t               hash;
    /** @brief Next handle entry in the active handle hash table */
    SCOREP_IoHandleHandle next;
} io_handle_payload;

/** @brief An entry in the current I/O handle stack. */
typedef struct io_handle_stack_entry
{
    struct io_handle_stack_entry* next;
    SCOREP_IoHandleHandle         handle;
    /** This handle is in its creation phase */
    bool                          in_creation;
    /** Counts how many sub-create routines are working on this creation.
     *  Only if this count is zero, the handle will be given to the adapter
     *  after a complete/pop. */
    uint32_t recursive_creation_counter;
} io_handle_stack_entry;

/** @brief context for one registered I/O paradigm. */
typedef struct io_mgmt_paradigm
{
    /** @brief The I/O paradigm definition */
    SCOREP_IoParadigm* definition;
    /** @brief The size for the paradigm specific I/O handle value
        (i.e., sizeof(int|FILE*|MPI_File)). */
    size_t                payload_size;
    /** @brief Hash table of all active I/O handles. */
    SCOREP_IoHandleHandle handles[ SCOREP_IO_HANDLE_HASHTABLE_SIZE ];
    /** @brief mutex to protect @a handles table for this I/O paradigm */
    SCOREP_Mutex          mutex;
} io_mgmt_paradigm;

/** @brief The per-location data, which holds the locations handle stack. */
typedef struct io_mgmt_location_data
{
    /** @brief This locations current handle stack. */
    io_handle_stack_entry* handle_stack;
    /** @brief unused handle stack entries */
    io_handle_stack_entry* unused_handle_stack_entries;
} io_mgmt_location_data;

static size_t io_mgmt_subsystem_id;

/** @brief All registered I/O paradigms. */
static io_mgmt_paradigm* io_paradigms[ SCOREP_NUM_IO_PARADIGMS ];

static inline io_mgmt_location_data*
get_location_data( void )
{
    io_mgmt_location_data* data =
        SCOREP_Location_GetSubsystemData( SCOREP_Location_GetCurrentCPULocation(),
                                          io_mgmt_subsystem_id );
    UTILS_BUG_ON( data == NULL, "No location data for location." );
    return data;
}

/** @brief Gets the top active I/O handle from the stack,
    or SCOREP_INVALID_IO_HANDLE if no handle is on the stack. */
static inline SCOREP_IoHandleHandle
handle_stack_top( io_mgmt_location_data* data )
{
    return data->handle_stack ? data->handle_stack->handle : SCOREP_INVALID_IO_HANDLE;
}

static inline void
handle_stack_push( io_mgmt_location_data* data,
                   SCOREP_IoHandleHandle  handle,
                   bool                   create )
{
    io_handle_stack_entry* elem = data->unused_handle_stack_entries;
    if ( elem == NULL )
    {
        elem = SCOREP_Memory_AllocForMisc( sizeof( *elem ) );
    }
    else
    {
        data->unused_handle_stack_entries = elem->next;
    }
    UTILS_ASSERT( elem );
    memset( elem, 0, sizeof( *elem ) );

    elem->handle       = handle;
    elem->in_creation  = create;
    elem->next         = data->handle_stack;
    data->handle_stack = elem;
}

static inline void
handle_stack_pop( io_mgmt_location_data* data )
{
    UTILS_BUG_ON( data->handle_stack == NULL, "Empty I/O handle stack." );

    io_handle_stack_entry* elem = data->handle_stack;

    data->handle_stack                = elem->next;
    elem->next                        = data->unused_handle_stack_entries;
    data->unused_handle_stack_entries = elem;
}

/** @brief Returns the memory address of the I/O paradigm value from a
    IoHandleHandle payload. */
static inline void*
payload_get_handle( io_handle_payload* payload )
{
    return payload + 1;
}

static inline SCOREP_IoHandleHandle*
get_handle_ref( SCOREP_IoParadigmType paradigm,
                const void*           ioHandle,
                uint32_t              hash,
                io_handle_payload**   entry )
{
    size_t handle_size = io_paradigms[ paradigm ]->payload_size;
    if ( hash == 0 )
    {
        hash = jenkins_hash( ioHandle, handle_size, 0 );
    }
    size_t index = hash & SCOREP_IO_HANDLE_HASHTABLE_MASK;

    SCOREP_IoHandleHandle* handle_iterator = &io_paradigms[ paradigm ]->handles[ index ];

    while ( *handle_iterator != SCOREP_INVALID_IO_HANDLE )
    {
        *entry = SCOREP_IoHandleHandle_GetPayload( *handle_iterator );

        UTILS_BUG_ON( !*entry, "Invalid payload for handle definition %u", *handle_iterator );

        if ( ( *entry )->hash == hash &&
             memcmp( payload_get_handle( *entry ), ioHandle, handle_size ) == 0 )
        {
            break;
        }
        handle_iterator = &( *entry )->next;
    }

    return handle_iterator;
}

static inline void
insert_handle( SCOREP_IoParadigmType paradigm,
               SCOREP_IoHandleHandle handle,
               const void*           ioHandle,
               uint32_t              hash )
{
    io_handle_payload*     entry;
    SCOREP_IoHandleHandle* handle_iterator = get_handle_ref( paradigm, ioHandle, hash, &entry );
    if ( *handle_iterator != SCOREP_INVALID_IO_HANDLE )
    {
        if ( SCOREP_Env_RunVerbose() )
        {
            fprintf( stderr, "[Score-P] warning: duplicate %s handle, previous handle not destroyed",
                     io_paradigms[ paradigm ]->definition->name );
        }

        /* drop duplicate from hash table */
        *handle_iterator = entry->next;
        entry->next      = SCOREP_INVALID_IO_HANDLE;
    }

    /* entry was overwritten by get_handle_ref, but hash value still set */
    entry = SCOREP_IoHandleHandle_GetPayload( handle );
    size_t index = entry->hash & SCOREP_IO_HANDLE_HASHTABLE_MASK;
    entry->next                                = io_paradigms[ paradigm ]->handles[ index ];
    io_paradigms[ paradigm ]->handles[ index ] = handle;
}

void
SCOREP_IoMgmt_RegisterParadigm( SCOREP_IoParadigmType     paradigm,
                                SCOREP_IoParadigmClass    paradigmClass,
                                const char*               name,
                                SCOREP_IoParadigmFlag     paradigmFlags,
                                size_t                    payloadSize,
                                SCOREP_IoParadigmProperty propertySentinel,
                                ... )
{
    UTILS_BUG_ON( paradigm < 0 || paradigm >= SCOREP_INVALID_IO_PARADIGM_TYPE,
                  "Invalid I/O paradigm %d", paradigm );
    UTILS_BUG_ON( io_paradigms[ paradigm ], "Paradigm already registered" );

    io_paradigms[ paradigm ] = calloc( 1, sizeof( io_mgmt_paradigm ) );
    UTILS_ASSERT( io_paradigms[ paradigm ] );

    const char* identification = NULL;
    switch ( paradigm )
    {
#define SCOREP_IO_PARADIGM( upper, lower, id_name ) \
    case SCOREP_IO_PARADIGM_ ## upper: \
        identification = id_name; \
        break;
        SCOREP_IO_PARADIGMS
#undef SCOREP_IO_PARADIGM
    }

    io_paradigms[ paradigm ]->definition =
        SCOREP_Definitions_NewIoParadigm( paradigm,
                                          identification,
                                          name,
                                          paradigmClass,
                                          paradigmFlags );

    va_list va;
    va_start( va, propertySentinel );
    while ( propertySentinel != SCOREP_INVALID_IO_PARADIGM_PROPERTY )
    {
        const char* property_value = va_arg( va, const char* );

        SCOREP_Definitions_IoParadigmSetProperty(
            io_paradigms[ paradigm ]->definition,
            propertySentinel,
            SCOREP_Definitions_NewString( property_value ) );

        propertySentinel = va_arg( va, int );
    }
    va_end( va );

    io_paradigms[ paradigm ]->payload_size = payloadSize;

    SCOREP_MutexCreate( &io_paradigms[ paradigm ]->mutex );
}

void
SCOREP_IoMgmt_DeregisterParadigm( SCOREP_IoParadigmType paradigm )
{
    UTILS_BUG_ON( paradigm < 0 || paradigm >= SCOREP_INVALID_IO_PARADIGM_TYPE,
                  "Invalid I/O paradigm %d", paradigm );
    UTILS_BUG_ON( !io_paradigms[ paradigm ],
                  "Paradigm cannot be de-registered because it was never registered" );

    SCOREP_MutexDestroy( &io_paradigms[ paradigm ]->mutex );

    free( io_paradigms[ paradigm ] );

    io_paradigms[ paradigm ] = NULL;
}

uint32_t
SCOREP_IoMgmt_GetParadigmId( SCOREP_IoParadigmType paradigm )
{
    UTILS_BUG_ON( paradigm < 0 || paradigm >= SCOREP_INVALID_IO_PARADIGM_TYPE,
                  "Invalid I/O paradigm %d", paradigm );
    UTILS_BUG_ON( !io_paradigms[ paradigm ], "The given paradigm was not registered" );

    return io_paradigms[ paradigm ]->definition->sequence_number;
}

const char*
SCOREP_IoMgmt_GetParadigmName( SCOREP_IoParadigmType paradigm )
{
    UTILS_BUG_ON( paradigm < 0 || paradigm >= SCOREP_INVALID_IO_PARADIGM_TYPE,
                  "Invalid I/O paradigm %d", paradigm );
    UTILS_BUG_ON( !io_paradigms[ paradigm ], "The given paradigm was not registered" );

    return io_paradigms[ paradigm ]->definition->name;
}

void
SCOREP_IoMgmt_CreatePreCreatedHandle( SCOREP_IoParadigmType            paradigm,
                                      SCOREP_IoFileHandle              file,
                                      SCOREP_IoHandleFlag              flags,
                                      SCOREP_IoAccessMode              accessMode,
                                      SCOREP_IoStatusFlag              statusFlags,
                                      SCOREP_InterimCommunicatorHandle scope,
                                      uint32_t                         unifyKey,
                                      const char*                      name,
                                      const void*                      ioHandle )
{
    UTILS_BUG_ON( paradigm < 0 || paradigm >= SCOREP_INVALID_IO_PARADIGM_TYPE,
                  "Invalid I/O paradigm %d", paradigm );
    UTILS_BUG_ON( !io_paradigms[ paradigm ], "The given paradigm was not registered" );
    UTILS_BUG_ON( ~flags & SCOREP_IO_HANDLE_FLAG_PRE_CREATED,
                  "Attempt to create a non-pre-created I/O handle" );

    io_handle_payload* payload;
    size_t             payload_size = sizeof( *payload ) + io_paradigms[ paradigm ]->payload_size;

    SCOREP_IoHandleHandle handle = SCOREP_Definitions_NewIoHandle( name,
                                                                   file,
                                                                   paradigm,
                                                                   flags,
                                                                   scope,
                                                                   SCOREP_INVALID_IO_HANDLE,
                                                                   unifyKey,
                                                                   true,
                                                                   payload_size,
                                                                   ( void** )&payload,
                                                                   accessMode,
                                                                   statusFlags );
    payload->next = SCOREP_INVALID_IO_HANDLE;
    payload->hash = jenkins_hash( ioHandle, io_paradigms[ paradigm ]->payload_size, 0 );
    memcpy( payload_get_handle( payload ), ioHandle, io_paradigms[ paradigm ]->payload_size );
    size_t index = payload->hash & SCOREP_IO_HANDLE_HASHTABLE_MASK;

    SCOREP_MutexLock( io_paradigms[ paradigm ]->mutex );

    /* do we need to check for duplicates? */
    payload->next                              = io_paradigms[ paradigm ]->handles[ index ];
    io_paradigms[ paradigm ]->handles[ index ] = handle;

    SCOREP_MutexUnlock( io_paradigms[ paradigm ]->mutex );
}

void
SCOREP_IoMgmt_BeginHandleCreation( SCOREP_IoParadigmType            paradigm,
                                   SCOREP_IoHandleFlag              flags,
                                   SCOREP_InterimCommunicatorHandle scope,
                                   uint32_t                         unifyKey,
                                   const char*                      name )
{
    UTILS_BUG_ON( paradigm < 0 || paradigm >= SCOREP_INVALID_IO_PARADIGM_TYPE,
                  "Invalid I/O paradigm %d", paradigm );
    UTILS_BUG_ON( !io_paradigms[ paradigm ], "The given paradigm was not registered" );
    UTILS_BUG_ON( flags & SCOREP_IO_HANDLE_FLAG_PRE_CREATED,
                  "Attempt to create a pre-created I/O handle" );

    io_mgmt_location_data* data = get_location_data();

    /* Check if we are in a recursive create */
    if ( data->handle_stack && data->handle_stack->in_creation )
    {
        SCOREP_IoHandleDef* top_handle_def = SCOREP_LOCAL_HANDLE_DEREF( data->handle_stack->handle, IoHandle );
        if ( paradigm == top_handle_def->io_paradigm_type )
        {
            /* This is a recursive create, increment the counter and return */
            data->handle_stack->recursive_creation_counter++;
            return;
        }
    }

    io_handle_payload* payload;
    size_t             payload_size = sizeof( *payload ) + io_paradigms[ paradigm ]->payload_size;

    SCOREP_IoHandleHandle parent = handle_stack_top( data );

    SCOREP_IoHandleHandle handle = SCOREP_Definitions_NewIoHandle( name,
                                                                   SCOREP_INVALID_IO_FILE,
                                                                   paradigm,
                                                                   flags,
                                                                   scope,
                                                                   parent,
                                                                   unifyKey,
                                                                   false,
                                                                   payload_size,
                                                                   ( void** )&payload );
    payload->hash = 0;
    payload->next = SCOREP_INVALID_IO_HANDLE;

    handle_stack_push( data, handle, true );

    SCOREP_CALL_SUBSTRATE_MGMT( IoParadigmEnter, IO_PARADIGM_ENTER,
                                ( SCOREP_Location_GetCurrentCPULocation(), paradigm ) );
}

SCOREP_IoHandleHandle
SCOREP_IoMgmt_CompleteHandleCreation( SCOREP_IoParadigmType paradigm,
                                      SCOREP_IoFileHandle   file,
                                      const void*           ioHandle )
{
    UTILS_BUG_ON( paradigm < 0 || paradigm >= SCOREP_INVALID_IO_PARADIGM_TYPE,
                  "Invalid I/O paradigm %d", paradigm );
    UTILS_BUG_ON( !io_paradigms[ paradigm ], "The given paradigm was not registered" );
    UTILS_BUG_ON( file == SCOREP_INVALID_IO_FILE, "Invalid file handle given" );
    UTILS_BUG_ON( !ioHandle, "Invalid I/O paradigm handle reference" );

    io_mgmt_location_data* data = get_location_data();

    SCOREP_IoHandleHandle handle = handle_stack_top( data );
    UTILS_BUG_ON( handle == SCOREP_INVALID_IO_HANDLE,
                  "No I/O handle on current stack to finalize!" );

    if ( data->handle_stack->in_creation && data->handle_stack->recursive_creation_counter > 0 )
    {
        --data->handle_stack->recursive_creation_counter;
        /* todo: check, if we can take the file handle from the lowest depth */
        return SCOREP_INVALID_IO_HANDLE;
    }

    handle_stack_pop( data );

    io_handle_payload* entry = SCOREP_IoHandleHandle_GetPayload( handle );
    entry->hash = jenkins_hash( ioHandle, io_paradigms[ paradigm ]->payload_size, 0 );
    memcpy( payload_get_handle( entry ), ioHandle, io_paradigms[ paradigm ]->payload_size );

    SCOREP_MutexLock( io_paradigms[ paradigm ]->mutex );

    insert_handle( paradigm, handle, payload_get_handle( entry ), entry->hash );

    SCOREP_MutexUnlock( io_paradigms[ paradigm ]->mutex );

    SCOREP_IoHandleHandle_SetIoFile( handle, file );

    SCOREP_CALL_SUBSTRATE_MGMT( IoParadigmLeave, IO_PARADIGM_LEAVE,
                                ( SCOREP_Location_GetCurrentCPULocation(), paradigm ) );

    return handle;
}

void
SCOREP_IoMgmt_BeginHandleDuplication( SCOREP_IoParadigmType paradigm,
                                      SCOREP_IoHandleHandle srcHandle,
                                      uint32_t              unifyKey )
{
    UTILS_BUG_ON( srcHandle == SCOREP_INVALID_IO_HANDLE,
                  "Given handle is invalid" );

    io_mgmt_location_data* data = get_location_data();

    /* Check if we are in a recursive create */
    if ( data->handle_stack && data->handle_stack->in_creation )
    {
        SCOREP_IoHandleDef* top_handle_def = SCOREP_LOCAL_HANDLE_DEREF( data->handle_stack->handle, IoHandle );
        if ( paradigm == top_handle_def->io_paradigm_type )
        {
            /* This is a recursive create, increment the counter and return */
            data->handle_stack->recursive_creation_counter++;
            return;
        }
    }

    io_handle_payload* payload;
    size_t             payload_size = sizeof( *payload ) + io_paradigms[ paradigm ]->payload_size;

    SCOREP_IoHandleDef* src_def = SCOREP_LOCAL_HANDLE_DEREF( srcHandle, IoHandle );

    SCOREP_IoHandleHandle handle = SCOREP_Definitions_NewIoHandle( SCOREP_StringHandle_Get( src_def->name_handle ),
                                                                   src_def->file_handle,       /* Just store, may get overridden at completion */
                                                                   src_def->io_paradigm_type,
                                                                   SCOREP_IO_HANDLE_FLAG_NONE, /* The new handle won't inherit the handle flags (e.g., SCOREP_IO_HANDLE_FLAG_PRE_CREATED) */
                                                                   src_def->scope_handle,
                                                                   src_def->parent_handle,
                                                                   unifyKey,
                                                                   false,
                                                                   payload_size,
                                                                   ( void** )&payload );
    payload->hash = 0;
    payload->next = SCOREP_INVALID_IO_HANDLE;

    handle_stack_push( data, handle, true );

    SCOREP_CALL_SUBSTRATE_MGMT( IoParadigmEnter, IO_PARADIGM_ENTER,
                                ( SCOREP_Location_GetCurrentCPULocation(), paradigm ) );
}

SCOREP_IoHandleHandle
SCOREP_IoMgmt_CompleteHandleDuplication( SCOREP_IoParadigmType paradigm,
                                         SCOREP_IoFileHandle   file,
                                         const void*           ioHandle )
{
    io_mgmt_location_data* data = get_location_data();

    SCOREP_IoHandleHandle handle = handle_stack_top( data );

    UTILS_BUG_ON( handle == SCOREP_INVALID_IO_HANDLE,
                  "No I/O handle on current stack to finalize!" );

    if ( data->handle_stack->in_creation && data->handle_stack->recursive_creation_counter > 0 )
    {
        --data->handle_stack->recursive_creation_counter;
        /* todo: check, if we can take the file handle from the lowest depth */
        return SCOREP_INVALID_IO_HANDLE;
    }

    handle_stack_pop( data );

    if ( file == SCOREP_INVALID_IO_FILE )
    {
        /* file was stored in begin, but still not completed */
        file = SCOREP_IoHandleHandle_GetIoFile( handle );
    }

    io_handle_payload* entry = SCOREP_IoHandleHandle_GetPayload( handle );
    entry->hash = jenkins_hash( ioHandle, io_paradigms[ paradigm ]->payload_size, 0 );
    memcpy( payload_get_handle( entry ), ioHandle, io_paradigms[ paradigm ]->payload_size );

    SCOREP_MutexLock( io_paradigms[ paradigm ]->mutex );

    insert_handle( paradigm, handle, payload_get_handle( entry ), entry->hash );

    SCOREP_MutexUnlock( io_paradigms[ paradigm ]->mutex );

    SCOREP_IoHandleHandle_SetIoFile( handle, file );

    SCOREP_CALL_SUBSTRATE_MGMT( IoParadigmLeave, IO_PARADIGM_LEAVE,
                                ( SCOREP_Location_GetCurrentCPULocation(), paradigm ) );

    return handle;
}

void
SCOREP_IoMgmt_DropIncompleteHandle( void )
{
    io_mgmt_location_data* data = get_location_data();

    SCOREP_IoHandleHandle handle = handle_stack_top( data );
    handle_stack_pop( data );

    SCOREP_IoHandleDef* handle_def = SCOREP_LOCAL_HANDLE_DEREF( handle, IoHandle );
    SCOREP_CALL_SUBSTRATE_MGMT( IoParadigmLeave, IO_PARADIGM_LEAVE,
                                ( SCOREP_Location_GetCurrentCPULocation(),
                                  handle_def->io_paradigm_type ) );
}

void
SCOREP_IoMgmt_DestroyHandle( SCOREP_IoHandleHandle handle )
{
    /* Nothing to do yet. */
}

SCOREP_IoHandleHandle
SCOREP_IoMgmt_RemoveHandle( SCOREP_IoParadigmType paradigm,
                            const void*           ioHandle )
{
    io_mgmt_location_data* data = get_location_data();

    SCOREP_MutexLock( io_paradigms[ paradigm ]->mutex );

    io_handle_payload*     entry;
    SCOREP_IoHandleHandle* handle_iterator = get_handle_ref( paradigm, ioHandle, 0, &entry );
    if ( *handle_iterator == SCOREP_INVALID_IO_HANDLE )
    {
        UTILS_WARNING( "[Paradigm: %d] Could not find I/O handle in hashtable", paradigm );

        SCOREP_MutexUnlock( io_paradigms[ paradigm ]->mutex );

        return SCOREP_INVALID_IO_HANDLE;
    }
    SCOREP_IoHandleHandle old_handle = *handle_iterator;
    *handle_iterator = entry->next;
    entry->next      = SCOREP_INVALID_IO_HANDLE;

    SCOREP_MutexUnlock( io_paradigms[ paradigm ]->mutex );

    return old_handle;
}

void
SCOREP_IoMgmt_ReinsertHandle( SCOREP_IoParadigmType paradigm,
                              SCOREP_IoHandleHandle handle )
{
    UTILS_BUG_ON( paradigm < 0 || paradigm >= SCOREP_INVALID_IO_PARADIGM_TYPE,
                  "Invalid I/O paradigm %d", paradigm );
    UTILS_BUG_ON( !io_paradigms[ paradigm ], "The given paradigm was not registered" );

    io_handle_payload* entry = SCOREP_IoHandleHandle_GetPayload( handle );

    UTILS_BUG_ON( entry->hash == 0, "Reinserted I/O handle without initialized hash value" );

    SCOREP_MutexLock( io_paradigms[ paradigm ]->mutex );

    insert_handle( paradigm, handle, payload_get_handle( entry ), entry->hash );

    SCOREP_MutexUnlock( io_paradigms[ paradigm ]->mutex );
}

void
SCOREP_IoMgmt_PushHandle( SCOREP_IoHandleHandle handle )
{
    io_mgmt_location_data* data = get_location_data();

    if ( handle != SCOREP_INVALID_IO_HANDLE )
    {
        handle_stack_push( data, handle, false );

        SCOREP_IoHandleDef* handle_def = SCOREP_LOCAL_HANDLE_DEREF( handle, IoHandle );
        SCOREP_CALL_SUBSTRATE_MGMT( IoParadigmEnter, IO_PARADIGM_ENTER,
                                    ( SCOREP_Location_GetCurrentCPULocation(),
                                      handle_def->io_paradigm_type ) );
    }
}

SCOREP_IoHandleHandle
SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IoParadigmType paradigm,
                                const void*           ioHandle )
{
    io_mgmt_location_data* data = get_location_data();

    SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_GetIoHandle( paradigm,
                                                              ioHandle );
    if ( handle != SCOREP_INVALID_IO_HANDLE )
    {
        handle_stack_push( data, handle, false );

        SCOREP_CALL_SUBSTRATE_MGMT( IoParadigmEnter, IO_PARADIGM_ENTER,
                                    ( SCOREP_Location_GetCurrentCPULocation(),
                                      paradigm ) );
    }

    return handle;
}

void
SCOREP_IoMgmt_PopHandle( SCOREP_IoHandleHandle handle )
{
    io_mgmt_location_data* data = get_location_data();

    if ( handle != SCOREP_INVALID_IO_HANDLE )
    {
        UTILS_BUG_ON( handle_stack_top( data ) != handle,
                      "Requested I/O handle was not at top of the stack." );
        handle_stack_pop( data );

        SCOREP_IoHandleDef* handle_def = SCOREP_LOCAL_HANDLE_DEREF( handle, IoHandle );
        SCOREP_CALL_SUBSTRATE_MGMT( IoParadigmLeave, IO_PARADIGM_LEAVE,
                                    ( SCOREP_Location_GetCurrentCPULocation(),
                                      handle_def->io_paradigm_type ) );
    }
}

SCOREP_IoHandleHandle
SCOREP_IoMgmt_GetIoHandle( SCOREP_IoParadigmType paradigm,
                           const void*           ioHandle )
{
    UTILS_BUG_ON( paradigm < 0 || paradigm >= SCOREP_INVALID_IO_PARADIGM_TYPE,
                  "Invalid I/O paradigm %d", paradigm );
    UTILS_BUG_ON( !io_paradigms[ paradigm ], "The given paradigm was not registered" );

    SCOREP_MutexLock( io_paradigms[ paradigm ]->mutex );

    io_handle_payload*     entry;
    SCOREP_IoHandleHandle* handle_iterator = get_handle_ref( paradigm, ioHandle, 0, &entry );
    if ( *handle_iterator == SCOREP_INVALID_IO_HANDLE )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_IO_MANAGEMENT,
                            "[Paradigm: %d] Could not find I/O handle in hashtable",
                            paradigm );
    }

    SCOREP_MutexUnlock( io_paradigms[ paradigm ]->mutex );

    return *handle_iterator;
}

SCOREP_IoFileHandle
SCOREP_IoMgmt_GetIoFileHandle( const char* pathname )
{
    char* resolved_filename = realpath( pathname, NULL );

    if ( !resolved_filename )
    {
        return SCOREP_INVALID_IO_FILE;
    }

    UTILS_BUG_ON( !io_file_handle_hashtable, "Hashtable is not initialized for storing %s", pathname );

    size_t                hash_hint;
    SCOREP_Hashtab_Entry* entry = SCOREP_Hashtab_Find( io_file_handle_hashtable,
                                                       ( void* )resolved_filename,
                                                       &hash_hint );
    if ( entry != NULL )
    {
        free( resolved_filename );
        return entry->value.handle;
    }

    SCOREP_IoFileHandle file_handle = SCOREP_Definitions_NewIoFile( resolved_filename,
                                                                    SCOREP_INVALID_SYSTEM_TREE_NODE );

    SCOREP_Hashtab_InsertHandle( io_file_handle_hashtable,
                                 ( void* )resolved_filename,
                                 file_handle,
                                 &hash_hint );

    return file_handle;
}

const char*
SCOREP_IoMgmt_GetIoFile( SCOREP_IoHandleHandle handle )
{
    SCOREP_IoFileHandle fh = SCOREP_IoHandleHandle_GetIoFile( handle );
    return SCOREP_Definitions_GetIoFileName( fh );
}

static SCOREP_ErrorCode
io_mgmt_subsystem_pre_unify( void )
{
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( &scorep_local_definition_manager,
                                                         IoFile,
                                                         io_file )
    {
        const char* filename = SCOREP_StringHandle_Get( definition->file_name_handle );

        SCOREP_MountInfo* mnt_info = SCOREP_Platform_GetMountInfo( filename );

        definition->scope = SCOREP_Platform_GetTreeNodeHandle( mnt_info );

        SCOREP_Platform_AddMountInfoProperties( handle, mnt_info );
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();

    return SCOREP_SUCCESS;
}

static SCOREP_ErrorCode
io_mgmt_subsystem_register( size_t subsystemId )
{
    io_mgmt_subsystem_id = subsystemId;

    return SCOREP_SUCCESS;
}

static SCOREP_ErrorCode
io_mgmt_subsystem_init( void )
{
    io_file_handle_hashtable = SCOREP_Hashtab_CreateSize( SCOREP_IO_FILE_HASHTABLE_SIZE,
                                                          &SCOREP_Hashtab_HashString,
                                                          &SCOREP_Hashtab_CompareStrings );

    return SCOREP_Platform_MountInfoInitialize();
}

static void
io_mgmt_subsystem_finalize( void )
{
    SCOREP_Hashtab_FreeAll( io_file_handle_hashtable,
                            &SCOREP_Hashtab_DeleteFree,
                            &SCOREP_Hashtab_DeleteNone );

    SCOREP_Platform_MountInfoFinalize();
}



/** Initializes the location specific data */
static SCOREP_ErrorCode
io_mgmt_subsystem_init_location( SCOREP_Location* location,
                                 SCOREP_Location* parent )
{
    if ( SCOREP_Location_GetType( location ) != SCOREP_LOCATION_TYPE_CPU_THREAD )
    {
        return SCOREP_SUCCESS;
    }

    io_mgmt_location_data* location_data =
        SCOREP_Location_AllocForMisc( location, sizeof( *location_data ) );
    memset( location_data, 0, sizeof( *location_data ) );

    SCOREP_Location_SetSubsystemData( location, io_mgmt_subsystem_id, location_data );

    return SCOREP_SUCCESS;
}

const SCOREP_Subsystem SCOREP_Subsystem_IoManagement =
{
    .subsystem_name          = "I/O Management",
    .subsystem_register      = &io_mgmt_subsystem_register,
    .subsystem_init          = &io_mgmt_subsystem_init,
    .subsystem_pre_unify     = &io_mgmt_subsystem_pre_unify,
    .subsystem_finalize      = &io_mgmt_subsystem_finalize,
    .subsystem_init_location = &io_mgmt_subsystem_init_location,
};
