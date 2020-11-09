#ifndef SCOREP_ATOMIC_H
#define SCOREP_ATOMIC_H

/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2019-2020,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 * Wrap gcc atomic builtins for following types:
 * void*, bool, char, [u]int(8|16|32|64)_t, [u]intptr_t.
 * For documentation, please see
 * https://gcc.gnu.org/onlinedocs/gcc-8.2.0/gcc/_005f_005fatomic-Builtins.html#g_t_005f_005fatomic-Builtins
 * If CC does not support gcc atomic builtins, use precompiled architecture-
 * specific implementations. In this case atomics are not inlined.
 */


#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* This file is used during configure where it is included from
 * src/utils/atomic/SCOREP_Atomic.inc.c which is compiled to determine
 * 'gcc atomic builtins' support. At configure time, the following block
 * must not be evaluated, but it is needed at make time. */
#if defined( HAVE_CONFIG_H )

#if HAVE( SCOREP_GCC_ATOMIC_BUILTINS )
#define STATIC_INLINE static inline
#else /* !SCOREP_GCC_ATOMIC_BUILTINS */
#define STATIC_INLINE
#endif /* !SCOREP_GCC_ATOMIC_BUILTINS */

/* define SCOREP_CPU_RELAX to be a 'PAUSE' instruction to improve the performance of spin-wait loops, if available. */
#if defined( ASM_INLINE )
#if HAVE( SCOREP_CPU_INSTRUCTION_SET_X86_64 )
#define SCOREP_CPU_RELAX ASM_INLINE volatile ( "pause" )
#elif HAVE( SCOREP_CPU_INSTRUCTION_SET_AARCH_64 )
#define SCOREP_CPU_RELAX ASM_INLINE volatile ( "yield" )
#endif
#endif /* defined( ASM_INLINE ) */

#if !defined( SCOREP_CPU_RELAX )
#define SCOREP_CPU_RELAX ( ( void )0 )
#endif /* !defined( SCOREP_CPU_RELAX ) */

#endif /* defined( HAVE_CONFIG_H ) */

typedef enum SCOREP_Atomic_Memorder
{
    SCOREP_ATOMIC_RELAXED = 0,
    SCOREP_ATOMIC_CONSUME,
    SCOREP_ATOMIC_ACQUIRE,
    SCOREP_ATOMIC_RELEASE,
    SCOREP_ATOMIC_ACQUIRE_RELEASE,
    SCOREP_ATOMIC_SEQUENTIAL_CONSISTENT
} SCOREP_Atomic_Memorder;

STATIC_INLINE bool
SCOREP_Atomic_TestAndSet( void*                  ptr,
                          SCOREP_Atomic_Memorder memorder );

STATIC_INLINE void
SCOREP_Atomic_clear( bool*                  ptr,
                     SCOREP_Atomic_Memorder memorder );

STATIC_INLINE void
SCOREP_Atomic_ThreadFence( SCOREP_Atomic_Memorder memorder );

/* Cannot wrap __atomic_always_lock_free as argument size is required
 * to be a compile time constant. */
/* STATIC_INLINE bool
 * SCOREP_Atomic_AlwaysLockFree( size_t size, void* ptr ); */

/* Cannot wrap __atomic_is_lock_free as it generates an undefined
 * reference error. */
/* STATIC_INLINE bool
 * SCOREP_Atomic_IsLockFree( size_t size, void* ptr ); */

STATIC_INLINE void*
SCOREP_Atomic_LoadN_void_ptr( void**                 ptr,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE void
SCOREP_Atomic_StoreN_void_ptr( void**                 ptr,
                               void*                  val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE void*
SCOREP_Atomic_ExchangeN_void_ptr( void**                 ptr,
                                  void*                  val,
                                  SCOREP_Atomic_Memorder memorder );

STATIC_INLINE bool
SCOREP_Atomic_CompareExchangeN_void_ptr( void**                 ptr,
                                         void**                 expected,
                                         void*                  desired,
                                         bool                   weak,
                                         SCOREP_Atomic_Memorder successMemorder,
                                         SCOREP_Atomic_Memorder failureMemorder );

STATIC_INLINE void*
SCOREP_Atomic_AddFetch_void_ptr( void**                 ptr,
                                 void*                  val,
                                 SCOREP_Atomic_Memorder memorder );

STATIC_INLINE void*
SCOREP_Atomic_SubFetch_void_ptr( void**                 ptr,
                                 void*                  val,
                                 SCOREP_Atomic_Memorder memorder );

STATIC_INLINE void*
SCOREP_Atomic_AndFetch_void_ptr( void**                 ptr,
                                 void*                  val,
                                 SCOREP_Atomic_Memorder memorder );

STATIC_INLINE void*
SCOREP_Atomic_XorFetch_void_ptr( void**                 ptr,
                                 void*                  val,
                                 SCOREP_Atomic_Memorder memorder );

STATIC_INLINE void*
SCOREP_Atomic_OrFetch_void_ptr( void**                 ptr,
                                void*                  val,
                                SCOREP_Atomic_Memorder memorder );

STATIC_INLINE void*
SCOREP_Atomic_NandFetch_void_ptr( void**                 ptr,
                                  void*                  val,
                                  SCOREP_Atomic_Memorder memorder );

STATIC_INLINE void*
SCOREP_Atomic_FetchAdd_void_ptr( void**                 ptr,
                                 void*                  val,
                                 SCOREP_Atomic_Memorder memorder );

STATIC_INLINE void*
SCOREP_Atomic_FetchSub_void_ptr( void**                 ptr,
                                 void*                  val,
                                 SCOREP_Atomic_Memorder memorder );

STATIC_INLINE void*
SCOREP_Atomic_FetchAnd_void_ptr( void**                 ptr,
                                 void*                  val,
                                 SCOREP_Atomic_Memorder memorder );

STATIC_INLINE void*
SCOREP_Atomic_FetchXor_void_ptr( void**                 ptr,
                                 void*                  val,
                                 SCOREP_Atomic_Memorder memorder );

STATIC_INLINE void*
SCOREP_Atomic_FetchOr_void_ptr( void**                 ptr,
                                void*                  val,
                                SCOREP_Atomic_Memorder memorder );

STATIC_INLINE void*
SCOREP_Atomic_FetchNand_void_ptr( void**                 ptr,
                                  void*                  val,
                                  SCOREP_Atomic_Memorder memorder );

STATIC_INLINE bool
SCOREP_Atomic_LoadN_bool( bool*                  ptr,
                          SCOREP_Atomic_Memorder memorder );

STATIC_INLINE void
SCOREP_Atomic_StoreN_bool( bool*                  ptr,
                           bool                   val,
                           SCOREP_Atomic_Memorder memorder );

STATIC_INLINE bool
SCOREP_Atomic_ExchangeN_bool( bool*                  ptr,
                              bool                   val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE bool
SCOREP_Atomic_CompareExchangeN_bool( bool*                  ptr,
                                     bool*                  expected,
                                     bool                   desired,
                                     bool                   weak,
                                     SCOREP_Atomic_Memorder successMemorder,
                                     SCOREP_Atomic_Memorder failureMemorder );

/* Omit fetch* and *fetch operations for type bool as they cause
* compile errors with some gcc compilers (at least with 7.3). */

STATIC_INLINE char
SCOREP_Atomic_LoadN_char( char*                  ptr,
                          SCOREP_Atomic_Memorder memorder );

STATIC_INLINE void
SCOREP_Atomic_StoreN_char( char*                  ptr,
                           char                   val,
                           SCOREP_Atomic_Memorder memorder );

STATIC_INLINE char
SCOREP_Atomic_ExchangeN_char( char*                  ptr,
                              char                   val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE bool
SCOREP_Atomic_CompareExchangeN_char( char*                  ptr,
                                     char*                  expected,
                                     char                   desired,
                                     bool                   weak,
                                     SCOREP_Atomic_Memorder successMemorder,
                                     SCOREP_Atomic_Memorder failureMemorder );

STATIC_INLINE char
SCOREP_Atomic_AddFetch_char( char*                  ptr,
                             char                   val,
                             SCOREP_Atomic_Memorder memorder );

STATIC_INLINE char
SCOREP_Atomic_SubFetch_char( char*                  ptr,
                             char                   val,
                             SCOREP_Atomic_Memorder memorder );

STATIC_INLINE char
SCOREP_Atomic_AndFetch_char( char*                  ptr,
                             char                   val,
                             SCOREP_Atomic_Memorder memorder );

STATIC_INLINE char
SCOREP_Atomic_XorFetch_char( char*                  ptr,
                             char                   val,
                             SCOREP_Atomic_Memorder memorder );

STATIC_INLINE char
SCOREP_Atomic_OrFetch_char( char*                  ptr,
                            char                   val,
                            SCOREP_Atomic_Memorder memorder );

STATIC_INLINE char
SCOREP_Atomic_NandFetch_char( char*                  ptr,
                              char                   val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE char
SCOREP_Atomic_FetchAdd_char( char*                  ptr,
                             char                   val,
                             SCOREP_Atomic_Memorder memorder );

STATIC_INLINE char
SCOREP_Atomic_FetchSub_char( char*                  ptr,
                             char                   val,
                             SCOREP_Atomic_Memorder memorder );

STATIC_INLINE char
SCOREP_Atomic_FetchAnd_char( char*                  ptr,
                             char                   val,
                             SCOREP_Atomic_Memorder memorder );

STATIC_INLINE char
SCOREP_Atomic_FetchXor_char( char*                  ptr,
                             char                   val,
                             SCOREP_Atomic_Memorder memorder );

STATIC_INLINE char
SCOREP_Atomic_FetchOr_char( char*                  ptr,
                            char                   val,
                            SCOREP_Atomic_Memorder memorder );

STATIC_INLINE char
SCOREP_Atomic_FetchNand_char( char*                  ptr,
                              char                   val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint8_t
SCOREP_Atomic_LoadN_uint8( uint8_t*               ptr,
                           SCOREP_Atomic_Memorder memorder );

STATIC_INLINE void
SCOREP_Atomic_StoreN_uint8( uint8_t*               ptr,
                            uint8_t                val,
                            SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint8_t
SCOREP_Atomic_ExchangeN_uint8( uint8_t*               ptr,
                               uint8_t                val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE bool
SCOREP_Atomic_CompareExchangeN_uint8( uint8_t*               ptr,
                                      uint8_t*               expected,
                                      uint8_t                desired,
                                      bool                   weak,
                                      SCOREP_Atomic_Memorder successMemorder,
                                      SCOREP_Atomic_Memorder failureMemorder );

STATIC_INLINE uint8_t
SCOREP_Atomic_AddFetch_uint8( uint8_t*               ptr,
                              uint8_t                val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint8_t
SCOREP_Atomic_SubFetch_uint8( uint8_t*               ptr,
                              uint8_t                val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint8_t
SCOREP_Atomic_AndFetch_uint8( uint8_t*               ptr,
                              uint8_t                val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint8_t
SCOREP_Atomic_XorFetch_uint8( uint8_t*               ptr,
                              uint8_t                val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint8_t
SCOREP_Atomic_OrFetch_uint8( uint8_t*               ptr,
                             uint8_t                val,
                             SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint8_t
SCOREP_Atomic_NandFetch_uint8( uint8_t*               ptr,
                               uint8_t                val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint8_t
SCOREP_Atomic_FetchAdd_uint8( uint8_t*               ptr,
                              uint8_t                val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint8_t
SCOREP_Atomic_FetchSub_uint8( uint8_t*               ptr,
                              uint8_t                val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint8_t
SCOREP_Atomic_FetchAnd_uint8( uint8_t*               ptr,
                              uint8_t                val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint8_t
SCOREP_Atomic_FetchXor_uint8( uint8_t*               ptr,
                              uint8_t                val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint8_t
SCOREP_Atomic_FetchOr_uint8( uint8_t*               ptr,
                             uint8_t                val,
                             SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint8_t
SCOREP_Atomic_FetchNand_uint8( uint8_t*               ptr,
                               uint8_t                val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint16_t
SCOREP_Atomic_LoadN_uint16( uint16_t*              ptr,
                            SCOREP_Atomic_Memorder memorder );

STATIC_INLINE void
SCOREP_Atomic_StoreN_uint16( uint16_t*              ptr,
                             uint16_t               val,
                             SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint16_t
SCOREP_Atomic_ExchangeN_uint16( uint16_t*              ptr,
                                uint16_t               val,
                                SCOREP_Atomic_Memorder memorder );

STATIC_INLINE bool
SCOREP_Atomic_CompareExchangeN_uint16( uint16_t*              ptr,
                                       uint16_t*              expected,
                                       uint16_t               desired,
                                       bool                   weak,
                                       SCOREP_Atomic_Memorder successMemorder,
                                       SCOREP_Atomic_Memorder failureMemorder );

STATIC_INLINE uint16_t
SCOREP_Atomic_AddFetch_uint16( uint16_t*              ptr,
                               uint16_t               val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint16_t
SCOREP_Atomic_SubFetch_uint16( uint16_t*              ptr,
                               uint16_t               val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint16_t
SCOREP_Atomic_AndFetch_uint16( uint16_t*              ptr,
                               uint16_t               val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint16_t
SCOREP_Atomic_XorFetch_uint16( uint16_t*              ptr,
                               uint16_t               val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint16_t
SCOREP_Atomic_OrFetch_uint16( uint16_t*              ptr,
                              uint16_t               val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint16_t
SCOREP_Atomic_NandFetch_uint16( uint16_t*              ptr,
                                uint16_t               val,
                                SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint16_t
SCOREP_Atomic_FetchAdd_uint16( uint16_t*              ptr,
                               uint16_t               val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint16_t
SCOREP_Atomic_FetchSub_uint16( uint16_t*              ptr,
                               uint16_t               val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint16_t
SCOREP_Atomic_FetchAnd_uint16( uint16_t*              ptr,
                               uint16_t               val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint16_t
SCOREP_Atomic_FetchXor_uint16( uint16_t*              ptr,
                               uint16_t               val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint16_t
SCOREP_Atomic_FetchOr_uint16( uint16_t*              ptr,
                              uint16_t               val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint16_t
SCOREP_Atomic_FetchNand_uint16( uint16_t*              ptr,
                                uint16_t               val,
                                SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint32_t
SCOREP_Atomic_LoadN_uint32( uint32_t*              ptr,
                            SCOREP_Atomic_Memorder memorder );

STATIC_INLINE void
SCOREP_Atomic_StoreN_uint32( uint32_t*              ptr,
                             uint32_t               val,
                             SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint32_t
SCOREP_Atomic_ExchangeN_uint32( uint32_t*              ptr,
                                uint32_t               val,
                                SCOREP_Atomic_Memorder memorder );

STATIC_INLINE bool
SCOREP_Atomic_CompareExchangeN_uint32( uint32_t*              ptr,
                                       uint32_t*              expected,
                                       uint32_t               desired,
                                       bool                   weak,
                                       SCOREP_Atomic_Memorder successMemorder,
                                       SCOREP_Atomic_Memorder failureMemorder );

STATIC_INLINE uint32_t
SCOREP_Atomic_AddFetch_uint32( uint32_t*              ptr,
                               uint32_t               val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint32_t
SCOREP_Atomic_SubFetch_uint32( uint32_t*              ptr,
                               uint32_t               val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint32_t
SCOREP_Atomic_AndFetch_uint32( uint32_t*              ptr,
                               uint32_t               val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint32_t
SCOREP_Atomic_XorFetch_uint32( uint32_t*              ptr,
                               uint32_t               val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint32_t
SCOREP_Atomic_OrFetch_uint32( uint32_t*              ptr,
                              uint32_t               val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint32_t
SCOREP_Atomic_NandFetch_uint32( uint32_t*              ptr,
                                uint32_t               val,
                                SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint32_t
SCOREP_Atomic_FetchAdd_uint32( uint32_t*              ptr,
                               uint32_t               val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint32_t
SCOREP_Atomic_FetchSub_uint32( uint32_t*              ptr,
                               uint32_t               val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint32_t
SCOREP_Atomic_FetchAnd_uint32( uint32_t*              ptr,
                               uint32_t               val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint32_t
SCOREP_Atomic_FetchXor_uint32( uint32_t*              ptr,
                               uint32_t               val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint32_t
SCOREP_Atomic_FetchOr_uint32( uint32_t*              ptr,
                              uint32_t               val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint32_t
SCOREP_Atomic_FetchNand_uint32( uint32_t*              ptr,
                                uint32_t               val,
                                SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint64_t
SCOREP_Atomic_LoadN_uint64( uint64_t*              ptr,
                            SCOREP_Atomic_Memorder memorder );

STATIC_INLINE void
SCOREP_Atomic_StoreN_uint64( uint64_t*              ptr,
                             uint64_t               val,
                             SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint64_t
SCOREP_Atomic_ExchangeN_uint64( uint64_t*              ptr,
                                uint64_t               val,
                                SCOREP_Atomic_Memorder memorder );

STATIC_INLINE bool
SCOREP_Atomic_CompareExchangeN_uint64( uint64_t*              ptr,
                                       uint64_t*              expected,
                                       uint64_t               desired,
                                       bool                   weak,
                                       SCOREP_Atomic_Memorder successMemorder,
                                       SCOREP_Atomic_Memorder failureMemorder );

STATIC_INLINE uint64_t
SCOREP_Atomic_AddFetch_uint64( uint64_t*              ptr,
                               uint64_t               val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint64_t
SCOREP_Atomic_SubFetch_uint64( uint64_t*              ptr,
                               uint64_t               val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint64_t
SCOREP_Atomic_AndFetch_uint64( uint64_t*              ptr,
                               uint64_t               val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint64_t
SCOREP_Atomic_XorFetch_uint64( uint64_t*              ptr,
                               uint64_t               val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint64_t
SCOREP_Atomic_OrFetch_uint64( uint64_t*              ptr,
                              uint64_t               val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint64_t
SCOREP_Atomic_NandFetch_uint64( uint64_t*              ptr,
                                uint64_t               val,
                                SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint64_t
SCOREP_Atomic_FetchAdd_uint64( uint64_t*              ptr,
                               uint64_t               val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint64_t
SCOREP_Atomic_FetchSub_uint64( uint64_t*              ptr,
                               uint64_t               val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint64_t
SCOREP_Atomic_FetchAnd_uint64( uint64_t*              ptr,
                               uint64_t               val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint64_t
SCOREP_Atomic_FetchXor_uint64( uint64_t*              ptr,
                               uint64_t               val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint64_t
SCOREP_Atomic_FetchOr_uint64( uint64_t*              ptr,
                              uint64_t               val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uint64_t
SCOREP_Atomic_FetchNand_uint64( uint64_t*              ptr,
                                uint64_t               val,
                                SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uintptr_t
SCOREP_Atomic_LoadN_uintptr( uintptr_t*             ptr,
                             SCOREP_Atomic_Memorder memorder );

STATIC_INLINE void
SCOREP_Atomic_StoreN_uintptr( uintptr_t*             ptr,
                              uintptr_t              val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uintptr_t
SCOREP_Atomic_ExchangeN_uintptr( uintptr_t*             ptr,
                                 uintptr_t              val,
                                 SCOREP_Atomic_Memorder memorder );

STATIC_INLINE bool
SCOREP_Atomic_CompareExchangeN_uintptr( uintptr_t*             ptr,
                                        uintptr_t*             expected,
                                        uintptr_t              desired,
                                        bool                   weak,
                                        SCOREP_Atomic_Memorder successMemorder,
                                        SCOREP_Atomic_Memorder failureMemorder );

STATIC_INLINE uintptr_t
SCOREP_Atomic_AddFetch_uintptr( uintptr_t*             ptr,
                                uintptr_t              val,
                                SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uintptr_t
SCOREP_Atomic_SubFetch_uintptr( uintptr_t*             ptr,
                                uintptr_t              val,
                                SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uintptr_t
SCOREP_Atomic_AndFetch_uintptr( uintptr_t*             ptr,
                                uintptr_t              val,
                                SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uintptr_t
SCOREP_Atomic_XorFetch_uintptr( uintptr_t*             ptr,
                                uintptr_t              val,
                                SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uintptr_t
SCOREP_Atomic_OrFetch_uintptr( uintptr_t*             ptr,
                               uintptr_t              val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uintptr_t
SCOREP_Atomic_NandFetch_uintptr( uintptr_t*             ptr,
                                 uintptr_t              val,
                                 SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uintptr_t
SCOREP_Atomic_FetchAdd_uintptr( uintptr_t*             ptr,
                                uintptr_t              val,
                                SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uintptr_t
SCOREP_Atomic_FetchSub_uintptr( uintptr_t*             ptr,
                                uintptr_t              val,
                                SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uintptr_t
SCOREP_Atomic_FetchAnd_uintptr( uintptr_t*             ptr,
                                uintptr_t              val,
                                SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uintptr_t
SCOREP_Atomic_FetchXor_uintptr( uintptr_t*             ptr,
                                uintptr_t              val,
                                SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uintptr_t
SCOREP_Atomic_FetchOr_uintptr( uintptr_t*             ptr,
                               uintptr_t              val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE uintptr_t
SCOREP_Atomic_FetchNand_uintptr( uintptr_t*             ptr,
                                 uintptr_t              val,
                                 SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int8_t
SCOREP_Atomic_LoadN_int8( int8_t*                ptr,
                          SCOREP_Atomic_Memorder memorder );

STATIC_INLINE void
SCOREP_Atomic_StoreN_int8( int8_t*                ptr,
                           int8_t                 val,
                           SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int8_t
SCOREP_Atomic_ExchangeN_int8( int8_t*                ptr,
                              int8_t                 val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE bool
SCOREP_Atomic_CompareExchangeN_int8( int8_t*                ptr,
                                     int8_t*                expected,
                                     int8_t                 desired,
                                     bool                   weak,
                                     SCOREP_Atomic_Memorder successMemorder,
                                     SCOREP_Atomic_Memorder failureMemorder );

STATIC_INLINE int8_t
SCOREP_Atomic_AddFetch_int8( int8_t*                ptr,
                             int8_t                 val,
                             SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int8_t
SCOREP_Atomic_SubFetch_int8( int8_t*                ptr,
                             int8_t                 val,
                             SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int8_t
SCOREP_Atomic_AndFetch_int8( int8_t*                ptr,
                             int8_t                 val,
                             SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int8_t
SCOREP_Atomic_XorFetch_int8( int8_t*                ptr,
                             int8_t                 val,
                             SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int8_t
SCOREP_Atomic_OrFetch_int8( int8_t*                ptr,
                            int8_t                 val,
                            SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int8_t
SCOREP_Atomic_NandFetch_int8( int8_t*                ptr,
                              int8_t                 val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int8_t
SCOREP_Atomic_FetchAdd_int8( int8_t*                ptr,
                             int8_t                 val,
                             SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int8_t
SCOREP_Atomic_FetchSub_int8( int8_t*                ptr,
                             int8_t                 val,
                             SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int8_t
SCOREP_Atomic_FetchAnd_int8( int8_t*                ptr,
                             int8_t                 val,
                             SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int8_t
SCOREP_Atomic_FetchXor_int8( int8_t*                ptr,
                             int8_t                 val,
                             SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int8_t
SCOREP_Atomic_FetchOr_int8( int8_t*                ptr,
                            int8_t                 val,
                            SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int8_t
SCOREP_Atomic_FetchNand_int8( int8_t*                ptr,
                              int8_t                 val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int16_t
SCOREP_Atomic_LoadN_int16( int16_t*               ptr,
                           SCOREP_Atomic_Memorder memorder );

STATIC_INLINE void
SCOREP_Atomic_StoreN_int16( int16_t*               ptr,
                            int16_t                val,
                            SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int16_t
SCOREP_Atomic_ExchangeN_int16( int16_t*               ptr,
                               int16_t                val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE bool
SCOREP_Atomic_CompareExchangeN_int16( int16_t*               ptr,
                                      int16_t*               expected,
                                      int16_t                desired,
                                      bool                   weak,
                                      SCOREP_Atomic_Memorder successMemorder,
                                      SCOREP_Atomic_Memorder failureMemorder );

STATIC_INLINE int16_t
SCOREP_Atomic_AddFetch_int16( int16_t*               ptr,
                              int16_t                val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int16_t
SCOREP_Atomic_SubFetch_int16( int16_t*               ptr,
                              int16_t                val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int16_t
SCOREP_Atomic_AndFetch_int16( int16_t*               ptr,
                              int16_t                val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int16_t
SCOREP_Atomic_XorFetch_int16( int16_t*               ptr,
                              int16_t                val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int16_t
SCOREP_Atomic_OrFetch_int16( int16_t*               ptr,
                             int16_t                val,
                             SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int16_t
SCOREP_Atomic_NandFetch_int16( int16_t*               ptr,
                               int16_t                val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int16_t
SCOREP_Atomic_FetchAdd_int16( int16_t*               ptr,
                              int16_t                val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int16_t
SCOREP_Atomic_FetchSub_int16( int16_t*               ptr,
                              int16_t                val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int16_t
SCOREP_Atomic_FetchAnd_int16( int16_t*               ptr,
                              int16_t                val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int16_t
SCOREP_Atomic_FetchXor_int16( int16_t*               ptr,
                              int16_t                val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int16_t
SCOREP_Atomic_FetchOr_int16( int16_t*               ptr,
                             int16_t                val,
                             SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int16_t
SCOREP_Atomic_FetchNand_int16( int16_t*               ptr,
                               int16_t                val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int32_t
SCOREP_Atomic_LoadN_int32( int32_t*               ptr,
                           SCOREP_Atomic_Memorder memorder );

STATIC_INLINE void
SCOREP_Atomic_StoreN_int32( int32_t*               ptr,
                            int32_t                val,
                            SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int32_t
SCOREP_Atomic_ExchangeN_int32( int32_t*               ptr,
                               int32_t                val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE bool
SCOREP_Atomic_CompareExchangeN_int32( int32_t*               ptr,
                                      int32_t*               expected,
                                      int32_t                desired,
                                      bool                   weak,
                                      SCOREP_Atomic_Memorder successMemorder,
                                      SCOREP_Atomic_Memorder failureMemorder );

STATIC_INLINE int32_t
SCOREP_Atomic_AddFetch_int32( int32_t*               ptr,
                              int32_t                val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int32_t
SCOREP_Atomic_SubFetch_int32( int32_t*               ptr,
                              int32_t                val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int32_t
SCOREP_Atomic_AndFetch_int32( int32_t*               ptr,
                              int32_t                val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int32_t
SCOREP_Atomic_XorFetch_int32( int32_t*               ptr,
                              int32_t                val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int32_t
SCOREP_Atomic_OrFetch_int32( int32_t*               ptr,
                             int32_t                val,
                             SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int32_t
SCOREP_Atomic_NandFetch_int32( int32_t*               ptr,
                               int32_t                val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int32_t
SCOREP_Atomic_FetchAdd_int32( int32_t*               ptr,
                              int32_t                val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int32_t
SCOREP_Atomic_FetchSub_int32( int32_t*               ptr,
                              int32_t                val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int32_t
SCOREP_Atomic_FetchAnd_int32( int32_t*               ptr,
                              int32_t                val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int32_t
SCOREP_Atomic_FetchXor_int32( int32_t*               ptr,
                              int32_t                val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int32_t
SCOREP_Atomic_FetchOr_int32( int32_t*               ptr,
                             int32_t                val,
                             SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int32_t
SCOREP_Atomic_FetchNand_int32( int32_t*               ptr,
                               int32_t                val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int64_t
SCOREP_Atomic_LoadN_int64( int64_t*               ptr,
                           SCOREP_Atomic_Memorder memorder );

STATIC_INLINE void
SCOREP_Atomic_StoreN_int64( int64_t*               ptr,
                            int64_t                val,
                            SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int64_t
SCOREP_Atomic_ExchangeN_int64( int64_t*               ptr,
                               int64_t                val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE bool
SCOREP_Atomic_CompareExchangeN_int64( int64_t*               ptr,
                                      int64_t*               expected,
                                      int64_t                desired,
                                      bool                   weak,
                                      SCOREP_Atomic_Memorder successMemorder,
                                      SCOREP_Atomic_Memorder failureMemorder );

STATIC_INLINE int64_t
SCOREP_Atomic_AddFetch_int64( int64_t*               ptr,
                              int64_t                val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int64_t
SCOREP_Atomic_SubFetch_int64( int64_t*               ptr,
                              int64_t                val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int64_t
SCOREP_Atomic_AndFetch_int64( int64_t*               ptr,
                              int64_t                val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int64_t
SCOREP_Atomic_XorFetch_int64( int64_t*               ptr,
                              int64_t                val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int64_t
SCOREP_Atomic_OrFetch_int64( int64_t*               ptr,
                             int64_t                val,
                             SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int64_t
SCOREP_Atomic_NandFetch_int64( int64_t*               ptr,
                               int64_t                val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int64_t
SCOREP_Atomic_FetchAdd_int64( int64_t*               ptr,
                              int64_t                val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int64_t
SCOREP_Atomic_FetchSub_int64( int64_t*               ptr,
                              int64_t                val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int64_t
SCOREP_Atomic_FetchAnd_int64( int64_t*               ptr,
                              int64_t                val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int64_t
SCOREP_Atomic_FetchXor_int64( int64_t*               ptr,
                              int64_t                val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int64_t
SCOREP_Atomic_FetchOr_int64( int64_t*               ptr,
                             int64_t                val,
                             SCOREP_Atomic_Memorder memorder );

STATIC_INLINE int64_t
SCOREP_Atomic_FetchNand_int64( int64_t*               ptr,
                               int64_t                val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE intptr_t
SCOREP_Atomic_LoadN_intptr( intptr_t*              ptr,
                            SCOREP_Atomic_Memorder memorder );

STATIC_INLINE void
SCOREP_Atomic_StoreN_intptr( intptr_t*              ptr,
                             intptr_t               val,
                             SCOREP_Atomic_Memorder memorder );

STATIC_INLINE intptr_t
SCOREP_Atomic_ExchangeN_intptr( intptr_t*              ptr,
                                intptr_t               val,
                                SCOREP_Atomic_Memorder memorder );

STATIC_INLINE bool
SCOREP_Atomic_CompareExchangeN_intptr( intptr_t*              ptr,
                                       intptr_t*              expected,
                                       intptr_t               desired,
                                       bool                   weak,
                                       SCOREP_Atomic_Memorder successMemorder,
                                       SCOREP_Atomic_Memorder failureMemorder );

STATIC_INLINE intptr_t
SCOREP_Atomic_AddFetch_intptr( intptr_t*              ptr,
                               intptr_t               val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE intptr_t
SCOREP_Atomic_SubFetch_intptr( intptr_t*              ptr,
                               intptr_t               val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE intptr_t
SCOREP_Atomic_AndFetch_intptr( intptr_t*              ptr,
                               intptr_t               val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE intptr_t
SCOREP_Atomic_XorFetch_intptr( intptr_t*              ptr,
                               intptr_t               val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE intptr_t
SCOREP_Atomic_OrFetch_intptr( intptr_t*              ptr,
                              intptr_t               val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE intptr_t
SCOREP_Atomic_NandFetch_intptr( intptr_t*              ptr,
                                intptr_t               val,
                                SCOREP_Atomic_Memorder memorder );

STATIC_INLINE intptr_t
SCOREP_Atomic_FetchAdd_intptr( intptr_t*              ptr,
                               intptr_t               val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE intptr_t
SCOREP_Atomic_FetchSub_intptr( intptr_t*              ptr,
                               intptr_t               val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE intptr_t
SCOREP_Atomic_FetchAnd_intptr( intptr_t*              ptr,
                               intptr_t               val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE intptr_t
SCOREP_Atomic_FetchXor_intptr( intptr_t*              ptr,
                               intptr_t               val,
                               SCOREP_Atomic_Memorder memorder );

STATIC_INLINE intptr_t
SCOREP_Atomic_FetchOr_intptr( intptr_t*              ptr,
                              intptr_t               val,
                              SCOREP_Atomic_Memorder memorder );

STATIC_INLINE intptr_t
SCOREP_Atomic_FetchNand_intptr( intptr_t*              ptr,
                                intptr_t               val,
                                SCOREP_Atomic_Memorder memorder );

/* At configure time, the following block must not be
 * evaluated, but it is needed at make time. */
#if defined( HAVE_CONFIG_H )
#if HAVE( SCOREP_GCC_ATOMIC_BUILTINS )
#include "../atomic/SCOREP_Atomic.inc.c"
#endif /* SCOREP_GCC_ATOMIC_BUILTINS */
#undef STATIC_INLINE
#endif /* defined( HAVE_CONFIG_H ) */

#endif /* SCOREP_ATOMIC_H */
