/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2017,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 * @brief The I/O management provides an interface to:
 *  - Create/Duplicate/Destroy Score-P I/O handles
 *  - Manage files and their handles
 *  - Store these handles in a hash table by paradigm handles (as key)
 *  - Create a parentship between handles of multiple paradigms by a stack
 *
 * Before using these functions, its required to register the I/O paradigm !
 *
 * See tests in $SCOREP_ROOT/test/io_management/ to get an idea how
 * this interface can be utilized.
 *
 */
#ifndef SCOREP_IO_MANAGEMENT_H
#define SCOREP_IO_MANAGEMENT_H
#include <stdbool.h>
#include <SCOREP_DefinitionHandles.h>

/**
 * Matching ID handling.
 *
 * @{
 */

typedef enum SCOREP_IoBlockingMatchingId
{
    SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_POSIX = 1,
    SCOREP_BLOCKING_IO_OPERATION_MATCHING_ID_ISOC  = 2,

    SCOREP_INVALID_BLOCKING_IO_OPERATION_MATCHING_ID /**< For internal use only. */
} SCOREP_IoBlockingMatchingId;

/**
 * @}
 */

/**
 *  I/O paradigm registration.
 *
 * @{
 */

/**
 * @brief Registers an I/O paradigm.
 *
 * @param paradigm      The I/O paradigm to register.
 * @param paradigmClass The class of this paradigm.
 * @param name          The human readable name of this paradigm.
 * @param paradigmFlags Any flags for this paradigm.
 * @param ioHandleSize  The size of the handle that is used in the paradigm
 *                      e.g. POSIX I/O: sizeof(int)
 * @param propertySentinel, ... (SCOREP_IoParadigmProperty, const char*) tuples
 *                              terminated with SCOREP_INVALID_IO_PARADIGM_PROPERTY.
 */

void
SCOREP_IoMgmt_RegisterParadigm( SCOREP_IoParadigmType     paradigm,
                                SCOREP_IoParadigmClass    paradigmClass,
                                const char*               name,
                                SCOREP_IoParadigmFlag     paradigmFlags,
                                size_t                    ioHandleSize,
                                SCOREP_IoParadigmProperty propertySentinel,
                                ... );

/**
 * @brief Deregisters a registered I/O paradigm.
 *
 * @param paradigm      The I/O paradigm to deregister.
 *
 */

void
SCOREP_IoMgmt_DeregisterParadigm( SCOREP_IoParadigmType paradigm );


/**
 * @brief Get a unique number for a registered I/O paradigm.
 *
 * @param paradigm      The I/O paradigm to query.
 * @return              The sequence ID.
 *
 */
uint32_t
SCOREP_IoMgmt_GetParadigmId( SCOREP_IoParadigmType paradigm );

/**
 * @brief Get the disaply name for a registered I/O paradigm.
 *
 * @param paradigm      The I/O paradigm to query.
 * @return              The display name.
 *
 */
const char*
SCOREP_IoMgmt_GetParadigmName( SCOREP_IoParadigmType paradigm );

/**
 * @}
 */

/**
 *  I/O File management.
 *
 * @{
 */

/**
 * @brief Determines the absolute path of a given file and creates the Score-P
 *        file I/O handle. This is internally stored and returned.
 *        In the case the file does not exists, SCOREP_INVALID_IO_FILE is returned.
 *
 * @param pathname      The path to a file or directory.
 *
 * @return the Score-P file handle that refers to filename
 */

SCOREP_IoFileHandle
SCOREP_IoMgmt_GetIoFileHandle( const char* pathname );

/**
 * @brief Returns the absolute path as C string of a Score-P I/O handle.
 *
 * @param handle        The Score-P I/O handle.
 *
 * @return file or path as C string (only a reference)
 */

const char*
SCOREP_IoMgmt_GetIoFile( SCOREP_IoHandleHandle handle );

/**
 * @}
 */

/**
 * @brief Creates and registers a pre-created IoHandle.
 *
 * @param paradigm      The I/O paradigm to operate in.
 * @param file          The file handle for the I/O handle.
 * @param flags         The I/O flags for this handle.
 * @param accessMode    The access mode for an I/O handle in subsequent I/O operations.
 * @param statusFlags   The additional status flags for the I/O handle
 * @param scope         The scope (collective context) for this handle.
 * @param unifyKey      An arbitrary unification key, zero values prohibits
 *                      unification.
 * @param name          The name this handle.
 * @param ioHandle      The paradigm specific I/O handle reference.
 *
 * @{
 */

void
SCOREP_IoMgmt_CreatePreCreatedHandle( SCOREP_IoParadigmType            paradigm,
                                      SCOREP_IoFileHandle              file,
                                      SCOREP_IoHandleFlag              flags,
                                      SCOREP_IoAccessMode              accessMode,
                                      SCOREP_IoStatusFlag              statusFlags,
                                      SCOREP_InterimCommunicatorHandle scope,
                                      uint32_t                         unifyKey,
                                      const char*                      name,
                                      const void*                      ioHandle );

/**
 * @}
 */

/**
 * Creating/Duplicating Score-P I/O handles.
 *
 * To create or duplicate a Score-P I/O handle, there are 2 steps required:
 *
 * 1. Initialization by SCOREP_IoMgmt_BeginHandle[Creation | Duplication]
 *    Call it before e.g. open, dup, ...
 * 2. Completion by SCOREP_IoMgmt_CompleteHandle[Creation | Duplication]
 *    Call it after a successful execution of e.g. open, dup, ...
 *
 * If a creation/duplication needs to be canceled after the initialization,
 * SCOREP_IoMgmt_DropIncompleteHandle can be called.
 *
 * When the lifetime of the handle ends, it is required to destroy the handle
 * by SCOREP_IoMgmt_DestroyHandle. Call it after a successful close operation.
 *
 *  @{
 */

/**
 * @brief Initializes the creation of a Score-P I/O handle.
 *
 * @param paradigm      The I/O paradigm to operate in.
 * @param flags         The I/O flags for this handle.
 * @param scope         The scope (collective context) for this handle.
 * @param unifyKey      An arbitrary unification key, zero values prohibits
 *                      unification.
 * @param name          The name for this handle.
 */

void
SCOREP_IoMgmt_BeginHandleCreation( SCOREP_IoParadigmType            paradigm,
                                   SCOREP_IoHandleFlag              flags,
                                   SCOREP_InterimCommunicatorHandle scope,
                                   uint32_t                         unifyKey,
                                   const char*                      name );

/**
 * @brief Completes the creation of a Score-P I/O handle.
 *
 * @param paradigm      The I/O paradigm to operate in.
 * @param path          Path or file that was opened or created
 * @param ioHandle      The paradigm specific I/O handle reference.
 *
 * @return The completed Score-P I/O handle
 */

SCOREP_IoHandleHandle
SCOREP_IoMgmt_CompleteHandleCreation( SCOREP_IoParadigmType paradigm,
                                      SCOREP_IoFileHandle   file,
                                      const void*           ioHandle );

/**
 * @brief Initializes the duplication of a Score-P I/O handle.
 *
 * @param paradigm      The I/O paradigm to operate in.
 * @param srcHandle     The handle to duplicate.
 * @param unifyKey      An arbitrary unification key, zero values prohibits
 *                      unification.
 *
 * @return the Score-P I/O handle or @a SCOREP_INVALID_IO_HANDLE.
 */

void
SCOREP_IoMgmt_BeginHandleDuplication( SCOREP_IoParadigmType paradigm,
                                      SCOREP_IoHandleHandle srcHandle,
                                      uint32_t              unifyKey );

/**
 * @brief Completes the duplication of a Score-P I/O handle.
 *
 * @param paradigm      The I/O paradigm to operate in.
 * @param path          New file path for the duplicated handle.
 *                      If NULL is given, the file path of the old handle is used.
 * @param ioHandle      The paradigm specific I/O handle reference.
 *
 * @return the Score-P I/O handle
 */

SCOREP_IoHandleHandle
SCOREP_IoMgmt_CompleteHandleDuplication( SCOREP_IoParadigmType paradigm,
                                         SCOREP_IoFileHandle   file,
                                         const void*           ioHandle );

/**
 * @brief Cancels the current handle creation or duplication of a Score-P I/O handle.
 */

void
SCOREP_IoMgmt_DropIncompleteHandle( void );

/**
 * @brief Destroys a Score-P handle.
 *
 * @param srcHandle     The handle to destroy.
 */

void
SCOREP_IoMgmt_DestroyHandle( SCOREP_IoHandleHandle handle );

/**
 * @}
 */

/**
 * Adding, getting and removing active Score-P I/O handles to the internal hash table.
 * This hash table stores all Score-P I/O handles of all I/O adapters.
 * Typically, the key is handle of the I/O paradigm e.g. the fd in POSIX and
 * the value is the created or duplicated Score-P I/O handle.
 *
 * @{
 */

/**
 * @brief Removes the handle from the active I/O handle table.
 *
 * Call this before the actual action (i.e., close, fclose, nc_close, ...).
 *
 * @param paradigm      The I/O paradigm to operate in.
 * @param ioHandle      The paradigm specific I/O handle reference.
 *
 * @return the Score-P I/O handle or @a SCOREP_INVALID_IO_HANDLE
 */

SCOREP_IoHandleHandle
SCOREP_IoMgmt_RemoveHandle( SCOREP_IoParadigmType paradigm,
                            const void*           ioHandle );

/**
 * @brief Inserts an existing I/O handle into the hash table.
 *
 * @param paradigm      The I/O paradigm to operate in.
 * @param handle        The Score-P I/O handle to insert into the hash table.
 *
 */
void
SCOREP_IoMgmt_ReinsertHandle( SCOREP_IoParadigmType paradigm,
                              SCOREP_IoHandleHandle handle );

/**
 * @brief Queries an active Score-P I/O handle for the I/O handle table.
 *
 * @param paradigm      The I/O paradigm to operate in.
 * @param ioHandle      The paradigm specific I/O handle reference.
 *
 * @return the Score-P I/O handle or @a SCOREP_INVALID_IO_HANDL
 */

SCOREP_IoHandleHandle
SCOREP_IoMgmt_GetIoHandle( SCOREP_IoParadigmType paradigm,
                           const void*           ioHandle );

/**
 * @}
 */

/**
 * I/O handle stack.
 *
 * Following functions push/pop Score-P I/O handles to an internal stack
 * in order to connect multiple I/O layers.
 *
 * Call these functions in wrappers of e.g. write, read, seek or close operation.
 * Calling these functions in conjunction with a duplication or creation
 * is not allowed.
 *
 * @{
 */

/**
 * @brief Gets and pushes an existing I/O handle to the stack, if it is known.
 *
 * Call in a wrapper operating on an I/O handle, before doing the actual action.
 *
 * @param paradigm      The I/O paradigm to operate in.
 * @param ioHandle      The paradigm specific I/O handle reference.
 *
 * @return the Score-P I/O handle or @a SCOREP_INVALID_IO_HANDLE
 */
SCOREP_IoHandleHandle
SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IoParadigmType paradigm,
                                const void*           ioHandle );

/**
 * @brief Pushes an existing I/O handle on the stack, if it is known.
 *
 * Call in a wrapper operating on an I/O handle, before doing the actual action.
 *
 * @param handle        The Score-P I/O handle to push on the stack.
 */
void
SCOREP_IoMgmt_PushHandle( SCOREP_IoHandleHandle handle );

/**
 * @brief Pops an existing I/O handle from the stack.
 *
 * Call in a wrapper operating on an I/O handle, after doing the actual action.
 *
 * @param handle        The Score-P I/O handle to remove from the stack.
 *                      @a SCOREP_INVALID_IO_HANDLE is valid.
 */
void
SCOREP_IoMgmt_PopHandle( SCOREP_IoHandleHandle handle );

/**
 * @}
 */

/**
 * @}
 */
#endif /* SCOREP_IO_MANAGEMENT_H */
