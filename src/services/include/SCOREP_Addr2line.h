/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2021-2022,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_ADDR2LINE_H
#define SCOREP_ADDR2LINE_H

/**
 * @file
 *
 * @brief Provide addr2line functionality.
 */

#include <stdbool.h>
#include <stdint.h>


/**
 * Initialize addr2line before usage of remaining SCOREP_Addr2Line_*
 * functions. Intended to be called at measurement initialization. It
 * collects the shared objects loaded at this time for addr2line
 * lookup. As measurement initialization is usually triggered by a
 * ctor, this would be the load-time objects only. Multiple calls do
 * no harm.
 *
 * @note Shared objects introduced via dlopen are handled via
 * rtdl-audit, if available and active. See scorep_libaudit.c.
 * @note The init function is not thread safe.
 */
void
SCOREP_Addr2line_Initialize( void );


/**
 * Free resources that were allocated during measurement.
 */
void
SCOREP_Addr2line_Finalize( void );


#if HAVE( SCOREP_ADDR2LINE )


/**
 * Shared object token returned if shared object lookup failed.
 */
#define SCOREP_ADDR2LINE_SO_OBJECT_INVALID_TOKEN 0


/**
 * The lookup functions below expect a program counter address. Use
 * this function to convert a given return address to a program
 * counter address.
 */
static inline uintptr_t
SCOREP_Addr2line_ConvertRetAddr2PrgCntAddr( uintptr_t returnAddr )
{
#if HAVE( CPU_INSTRUCTION_SET_PPC64 )
    return returnAddr - 4;
#elif HAVE( CPU_INSTRUCTION_SET_X86_64 )
    return returnAddr - 1;
#elif HAVE( CPU_INSTRUCTION_SET_AARCH64 )
    return returnAddr - 4;
#else
#error Unknown instruction set
#endif
}


/**
 * Lookup @a programCounterAddr in the set of currently loaded shared objects
 * (so). Indicate search result by setting @a soHandle to NULL and @a
 * soToken to SCOREP_ADDR2LINE_SO_OBJECT_INVALID_TOKEN if @a programCounterAddr
 * isn't found. Otherwise the combination of @a soHandle and
 * offset=programCounterAddr-soBaseAddr can be used later on to retrieve source
 * code information via SCOREP_Addr2line_SoLookupAddr() and
 * SCOREP_Addr2line_SoLookupAddrRange().
 * @a soFileName contains the shared object's path, either absolute or
 * relative (relative only seen for dlopened objects where a relative
 * path was provided to dlopen).
 * For dlopened shared objects, @a soToken contains a unique
 * identifier that can be used to efficiently query if the shared
 * object is still loaded, e.g., if @a programCounterAddr is cached and used
 * later on, see SCOREP_Addr2line_SoStillLoaded(). dlclosing and
 * dl-re-opening will provide a new token as @a soBaseAddr most likely
 * changed. All loadtime shared object return the same token
 * (SCOREP_ADDR2LINE_LT_OBJECT_TOKEN) as they won't be dlclosed during
 * normal program execution.
 *
 * @note SCOREP_Addr2line_LookupSo() is async-signal-safe.
 */
void
SCOREP_Addr2line_LookupSo( uintptr_t    programCounterAddr,
                           /* shared object OUT parameters */
                           void**       soHandle,
                           const char** soFileName,
                           uintptr_t*   soBaseAddr,
                           uint16_t*    soToken );


/**
 * Calls to SCOREP_Addr2line_LookupSo() allow for caching @a soHandle
 * and @a offset=programCounterAddr-soBaseAddr. This pair can be used here to
 * retrieve source code information. @a sclFound indicates if the
 * (bfd) lookup was successful, in which case the other scl* out
 * parameters are populated accordingly. The so* out parameters are
 * populated even if @a sclFound returns false.
 *
 * See SCOREP_Addr2line_LookupSo() for @a soToken.
 */
void
SCOREP_Addr2line_SoLookupAddr( uintptr_t    offset,
                               void*        soHandle,
                               /* shared object OUT parameters */
                               const char** soFileName,
                               uint16_t*    soToken,
                               /* Source code location OUT parameters */
                               bool*        sclFound,
                               const char** sclFileName,
                               const char** sclFunctionName,
                               unsigned*    sclLineNo );


/**
 * Similar to SCOREP_Addr2line_SoLookupAddr(), but try to match @a
 * beginOffset and @a endOffset with @a sclBeginLineNo and @a
 * sclEndLineNo, assuming that @a beginOffset and @a endOffset belong
 * to the same bfd_lookup section. @a sclFileName, @a sclFunctionName
 * and @a sclBeginLineNo are determined using @a beginOffset, only @a
 * sclEndLineNo is looked up via @a endOffset. Success and failure of
 * the two lookups is returned in @a sclFoundBegin and @a sclFoundEnd.
 *
 * See SCOREP_Addr2line_LookupSo() for @a soToken.
 */
void
SCOREP_Addr2line_SoLookupAddrRange( uintptr_t    beginOffset,
                                    uintptr_t    endOffset,
                                    void*        soHandle,
                                    /* shared object OUT parameters */
                                    const char** soFileName,
                                    uint16_t*    soToken,
                                    /* Source code location OUT parameters */
                                    bool*        sclFoundBegin,
                                    bool*        sclFoundEnd,
                                    const char** sclFileName,
                                    const char** sclFunctionName,
                                    unsigned*    sclBeginLineNo,
                                    unsigned*    sclEndLineNo );


/**
 * Lookup @a programCounterAddr in the set of currently loaded shared objects.
 * Indicate search result in @a soHandle/@a soToken
 * (NULL/SCOREP_ADDR2LINE_SO_OBJECT_INVALID_TOKEN if @a programCounterAddr not
 * found in the set of currently loaded shared objects), and @a soFound
 * (true if bfd lookup was successful). Populate corresponding so* and
 * scl* out parameters accordingly.
 *
 * @note If a shared object is loaded again after being unloaded
 * completely, @a soBaseAddr might have changed. I.e., if you cache @a
 * programCounterAddr, make sure associated data is still valid via @a soToken
 * and SCOREP_Addr2line_SoStillLoaded() or by getting notified about
 * unloading a shared object, see
 * SCOREP_Addr2line_RegisterObjcloseCb().
 *
 * See SCOREP_Addr2line_LookupSo() for @a soToken.
 */
void
SCOREP_Addr2line_LookupAddr( uintptr_t    programCounterAddr,
                             /* shared object OUT parameters */
                             void**       soHandle,
                             const char** soFileName,
                             uintptr_t*   soBaseAddr,
                             uint16_t*    soToken,
                             /* Source code location OUT parameters */
                             bool*        sclFound,
                             const char** sclFileName,
                             const char** sclFunctionName,
                             unsigned*    sclLineNo );



/**
 * Similar to SCOREP_Addr2line_LookupAddr(), but try to match @a
 * beginProgramCounterAddr and @a endProgramCounterAddr with @a sclBeginLineNo
 * and @a sclEndLineNo, assuming that @a beginProgramCounterAddr and @a endAddr
 * belong to the same bfd_lookup section. @a sclFileName, @a sclFunctionName and
 * @a sclBeginLineNo are determined using @a beginProgramCounterAddr, only @a
 * sclEndLineNo is looked up via @a endAddr. Success and failure of
 * the two lookups is returned in @a sclFoundBegin and @a sclFoundEnd.
 *
 * See SCOREP_Addr2line_LookupSo() for @a soToken.
 */
void
SCOREP_Addr2line_LookupAddrRange( uintptr_t    beginProgramCounterAddr,
                                  uintptr_t    endProgramCounterAddr,
                                  /* shared object OUT parameters */
                                  void**       soHandle,
                                  const char** soFileName,
                                  uintptr_t*   soBaseAddr,
                                  uint16_t*    soToken,
                                  /* Source code location OUT parameters */
                                  bool*        sclFoundBegin,
                                  bool*        sclFoundEnd,
                                  const char** sclFileName,
                                  const char** sclFunctionName,
                                  unsigned*    sclBeginLineNo,
                                  unsigned*    sclEndLineNo );


/**
 * Callback type to be used in SCOREP_Addr2line_RegisterObjcloseCb().
 */
typedef void ( * SCOREP_Addr2line_ObjcloseCb )( void*       soHandle,
                                                const char* soFileName,
                                                uintptr_t   soBaseAddr,
                                                uint16_t    soToken );


/**
 * Register @a cb callback to be triggered when run-time loaded shared
 * objects are dlclosed. All available information about the shared
 * object is provided via the callback's arguments. This gives the
 * opportunity to update cached addresses (that might be invalid once
 * the shared object is closed).
 *
 * @note See SCOREP_Addr2line_SoStillLoaded() for a different way to
 * check if addresses corresponding to a soToken are still valid.
 */
void
SCOREP_Addr2line_RegisterObjcloseCb( SCOREP_Addr2line_ObjcloseCb cb );


/**
 * Shared object token to identify load time shared objects (the ones
 * that live throughout the entire program's runtime).
 */
#define SCOREP_ADDR2LINE_LT_OBJECT_TOKEN UINT16_MAX


/**
 * Check if addresses of a shared object with token @a soToken are
 * still valid, i.e., if the shared object hasn't been dlclosed since
 * a call to any function that provided @a soToken.  Always returns
 * true for load-time shared objects.
 *
 * @note See SCOREP_Addr2line_RegisterObjcloseCb() for a different
 * way to check if addresses corresponding to a soToken are still
 * valid.
 * @note For performance reasons. the lookup is done using a bitset of
 * fixed size, thus no more than MAX_OBJOPEN_CALLS_TRACKED dlopen
 * calls that provide symbols are tracked. We abort if this number is
 * exceeded.
 */
static inline bool
SCOREP_Addr2line_SoStillLoaded( uint16_t soToken )
{
    if ( soToken == SCOREP_ADDR2LINE_LT_OBJECT_TOKEN )
    {
        return true;
    }
    bool scorep_addr2line_so_still_loaded( uint16_t soToken );
    return scorep_addr2line_so_still_loaded( soToken );
}


#endif /* HAVE( SCOREP_ADDR2LINE ) */

#endif /* SCOREP_ADDR2LINE_H */
