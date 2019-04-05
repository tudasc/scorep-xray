/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2016-2017,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_INTERNAL_DEFINITIONS_H
#error "Do not include this header directly, use SCOREP_Definitions.h instead."
#endif

#ifndef SCOREP_PRIVATE_DEFINITIONS_IO_HANDLE_H
#define SCOREP_PRIVATE_DEFINITIONS_IO_HANDLE_H


/**
 * @file
 *
 *
 */



SCOREP_DEFINE_DEFINITION_TYPE( IoHandle )
{
    SCOREP_DEFINE_DEFINITION_HEADER( IoHandle );

    // Add SCOREP_IoFileHandle stuff from here on.
    SCOREP_StringHandle              name_handle;
    SCOREP_IoFileHandle              file_handle;
    SCOREP_IoParadigmType            io_paradigm_type;
    SCOREP_IoHandleFlag              flags;
    SCOREP_InterimCommunicatorHandle scope_handle;
    SCOREP_IoHandleHandle            parent_handle;
    uint32_t                         unify_key;

    // Additional parameters used by pre-created I/O handles
    SCOREP_IoAccessMode access_mode;
    SCOREP_IoStatusFlag status_flags;
    bool                is_completed;
};


/**
 * Associate a name, file, I/O paradigm and additional information with a process unique handle of an I/O file.
 *
 * @param name           The handle name.
 * @param file           The I/O file.
 * @param ioParadigmType I/O paradigm used for handling this file (e.g., POSIX I/O, MPI I/O).
 * @param flags          Additonal information about the handle, e.g., handle was pre-created.
 * @param scope          Group of locations that make use of this file handle.
 * @param parent         Reference to parent I/O handle in order to model hierarchical I/O operations.
 * @param unifyKey       A non-zero integer value to allow multiple definition of
 *                       the same structural equal I/O handles. I/O handles with value 0 never get unified.
 * @param sizeOfPayload  The size of the payload which the adapter requests
 *                       for this I/O handle.
 * @param[out] payload   Will be set to the memory location of the payload.
 * @param ...            SCOREP_IoAccessMode and SCOREP_IoStatusFlag in case
 *                       @a flags has @a SCOREP_IO_HANDLE_FLAG_PRE_CREATED set.
 *
 * @return A process unique handle of an I/O file handle to be used in I/O recording events.
 */
SCOREP_IoHandleHandle
SCOREP_Definitions_NewIoHandle( const char*                      name,
                                SCOREP_IoFileHandle              file,
                                SCOREP_IoParadigmType            ioParadigmType,
                                SCOREP_IoHandleFlag              flags,
                                SCOREP_InterimCommunicatorHandle scope,
                                SCOREP_IoHandleHandle            parent,
                                uint32_t                         unifyKey,
                                bool                             is_completed,
                                size_t                           sizeOfPayload,
                                void**                           payloadOut,
                                ... );

/**
 * Late associate an I/O file to an I/O handle.
 *
 * Only allowed once.
 */
void
SCOREP_IoHandleHandle_SetIoFile( SCOREP_IoHandleHandle handle,
                                 SCOREP_IoFileHandle   file );

SCOREP_IoFileHandle
SCOREP_IoHandleHandle_GetIoFile( SCOREP_IoHandleHandle handle );

void*
SCOREP_IoHandleHandle_GetPayload( SCOREP_IoHandleHandle handle );

void
scorep_definitions_unify_io_handle( SCOREP_IoHandleDef*                  definition,
                                    struct SCOREP_Allocator_PageManager* handlesPageManager );

void
scorep_definitions_rehash_io_handle( SCOREP_IoHandleDef* definition );


#endif /* SCOREP_PRIVATE_DEFINITIONS_IO_HANDLE_H */
