#ifndef UTILS_ATOMIC_H
#define UTILS_ATOMIC_H

/*
 * This file is part of the Score-P software ecosystem (http://www.score-p.org)
 *
 * Copyright (c) 2019-2020, 2022-2023,
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


#ifndef __cplusplus
#include <stdbool.h>
#endif
#include <stdint.h>

/* This file is used during configure where it is included from
 * common/utils/src/atomic/UTILS_Atomic.inc.c which is compiled to determine
 * 'gcc atomic builtins' support. At configure time, the following block
 * must not be evaluated, but it is needed at make time. */
#if defined( HAVE_CONFIG_H )

#if HAVE( GCC_ATOMIC_BUILTINS )
#define STATIC_INLINE static inline
#else /* !GCC_ATOMIC_BUILTINS */
#define STATIC_INLINE
#endif /* !GCC_ATOMIC_BUILTINS */

/* define UTILS_CPU_RELAX to be a 'PAUSE' instruction to improve the performance of spin-wait loops, if available. */
#if defined( ASM_INLINE )
#if HAVE( CPU_INSTRUCTION_SET_X86_64 )
#define UTILS_CPU_RELAX ASM_INLINE volatile ( "pause" )
#elif HAVE( CPU_INSTRUCTION_SET_AARCH64 )
#define UTILS_CPU_RELAX ASM_INLINE volatile ( "yield" )
#endif
#endif /* defined( ASM_INLINE ) */

#if !defined( UTILS_CPU_RELAX )
#define UTILS_CPU_RELAX ( ( void )0 )
#endif /* !defined( UTILS_CPU_RELAX ) */

#endif /* defined( HAVE_CONFIG_H ) */

typedef enum UTILS_Atomic_Memorder
{
    UTILS_ATOMIC_RELAXED = 0,
    UTILS_ATOMIC_CONSUME,
    UTILS_ATOMIC_ACQUIRE,
    UTILS_ATOMIC_RELEASE,
    UTILS_ATOMIC_ACQUIRE_RELEASE,
    UTILS_ATOMIC_SEQUENTIAL_CONSISTENT
} UTILS_Atomic_Memorder;


#ifdef __cplusplus
extern "C"
{
#endif

#define UTILS_Atomic_TestAndSet PACKAGE_MANGLE_NAME( UTILS_Atomic_TestAndSet )
STATIC_INLINE bool
UTILS_Atomic_TestAndSet( void*                 ptr,
                         UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_clear PACKAGE_MANGLE_NAME( UTILS_Atomic_clear )
STATIC_INLINE void
UTILS_Atomic_clear( bool*                 ptr,
                    UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_ThreadFence PACKAGE_MANGLE_NAME( UTILS_Atomic_ThreadFence )
STATIC_INLINE void
UTILS_Atomic_ThreadFence( UTILS_Atomic_Memorder memorder );

/* Cannot wrap __atomic_always_lock_free as argument size is required
 * to be a compile time constant. */
/* STATIC_INLINE bool
 * UTILS_Atomic_AlwaysLockFree( size_t size, void* ptr ); */

/* Cannot wrap __atomic_is_lock_free as it generates an undefined
 * reference error. */
/* STATIC_INLINE bool
 * UTILS_Atomic_IsLockFree( size_t size, void* ptr ); */

/** @note For all *_void_ptr() functions, the @a ptr and @a expected arguments
          are actually of type pointer-to-pointer, i.e., `void**`. Though that
          requires a cast to `(void**)` on the callsite. To avoid warnings they
          are just `void*` in the prototype. */

#define UTILS_Atomic_LoadN_void_ptr PACKAGE_MANGLE_NAME( UTILS_Atomic_LoadN_void_ptr )
STATIC_INLINE void*
UTILS_Atomic_LoadN_void_ptr( void*                 ptr,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_StoreN_void_ptr PACKAGE_MANGLE_NAME( UTILS_Atomic_StoreN_void_ptr )
STATIC_INLINE void
UTILS_Atomic_StoreN_void_ptr( void*                 ptr,
                              void*                 val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_ExchangeN_void_ptr PACKAGE_MANGLE_NAME( UTILS_Atomic_ExchangeN_void_ptr )
STATIC_INLINE void*
UTILS_Atomic_ExchangeN_void_ptr( void*                 ptr,
                                 void*                 val,
                                 UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_CompareExchangeN_void_ptr PACKAGE_MANGLE_NAME( UTILS_Atomic_CompareExchangeN_void_ptr )
STATIC_INLINE bool
UTILS_Atomic_CompareExchangeN_void_ptr( void*                 ptr,
                                        void*                 expected,
                                        void*                 desired,
                                        bool                  weak,
                                        UTILS_Atomic_Memorder successMemorder,
                                        UTILS_Atomic_Memorder failureMemorder );

#define UTILS_Atomic_AddFetch_void_ptr PACKAGE_MANGLE_NAME( UTILS_Atomic_AddFetch_void_ptr )
STATIC_INLINE void*
UTILS_Atomic_AddFetch_void_ptr( void*                 ptr,
                                void*                 val,
                                UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_SubFetch_void_ptr PACKAGE_MANGLE_NAME( UTILS_Atomic_SubFetch_void_ptr )
STATIC_INLINE void*
UTILS_Atomic_SubFetch_void_ptr( void*                 ptr,
                                void*                 val,
                                UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_AndFetch_void_ptr PACKAGE_MANGLE_NAME( UTILS_Atomic_AndFetch_void_ptr )
STATIC_INLINE void*
UTILS_Atomic_AndFetch_void_ptr( void*                 ptr,
                                void*                 val,
                                UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_XorFetch_void_ptr PACKAGE_MANGLE_NAME( UTILS_Atomic_XorFetch_void_ptr )
STATIC_INLINE void*
UTILS_Atomic_XorFetch_void_ptr( void*                 ptr,
                                void*                 val,
                                UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_OrFetch_void_ptr PACKAGE_MANGLE_NAME( UTILS_Atomic_OrFetch_void_ptr )
STATIC_INLINE void*
UTILS_Atomic_OrFetch_void_ptr( void*                 ptr,
                               void*                 val,
                               UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_NandFetch_void_ptr PACKAGE_MANGLE_NAME( UTILS_Atomic_NandFetch_void_ptr )
STATIC_INLINE void*
UTILS_Atomic_NandFetch_void_ptr( void*                 ptr,
                                 void*                 val,
                                 UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchAdd_void_ptr PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchAdd_void_ptr )
STATIC_INLINE void*
UTILS_Atomic_FetchAdd_void_ptr( void*                 ptr,
                                void*                 val,
                                UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchSub_void_ptr PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchSub_void_ptr )
STATIC_INLINE void*
UTILS_Atomic_FetchSub_void_ptr( void*                 ptr,
                                void*                 val,
                                UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchAnd_void_ptr PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchAnd_void_ptr )
STATIC_INLINE void*
UTILS_Atomic_FetchAnd_void_ptr( void*                 ptr,
                                void*                 val,
                                UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchXor_void_ptr PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchXor_void_ptr )
STATIC_INLINE void*
UTILS_Atomic_FetchXor_void_ptr( void*                 ptr,
                                void*                 val,
                                UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchOr_void_ptr PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchOr_void_ptr )
STATIC_INLINE void*
UTILS_Atomic_FetchOr_void_ptr( void*                 ptr,
                               void*                 val,
                               UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchNand_void_ptr PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchNand_void_ptr )
STATIC_INLINE void*
UTILS_Atomic_FetchNand_void_ptr( void*                 ptr,
                                 void*                 val,
                                 UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_LoadN_bool PACKAGE_MANGLE_NAME( UTILS_Atomic_LoadN_bool )
STATIC_INLINE bool
UTILS_Atomic_LoadN_bool( bool*                 ptr,
                         UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_StoreN_bool PACKAGE_MANGLE_NAME( UTILS_Atomic_StoreN_bool )
STATIC_INLINE void
UTILS_Atomic_StoreN_bool( bool*                 ptr,
                          bool                  val,
                          UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_ExchangeN_bool PACKAGE_MANGLE_NAME( UTILS_Atomic_ExchangeN_bool )
STATIC_INLINE bool
UTILS_Atomic_ExchangeN_bool( bool*                 ptr,
                             bool                  val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_CompareExchangeN_bool PACKAGE_MANGLE_NAME( UTILS_Atomic_CompareExchangeN_bool )
STATIC_INLINE bool
UTILS_Atomic_CompareExchangeN_bool( bool*                 ptr,
                                    bool*                 expected,
                                    bool                  desired,
                                    bool                  weak,
                                    UTILS_Atomic_Memorder successMemorder,
                                    UTILS_Atomic_Memorder failureMemorder );

/* Omit fetch* and *fetch operations for type bool as they cause
* compile errors with some gcc compilers (at least with 7.3). */

#define UTILS_Atomic_LoadN_char PACKAGE_MANGLE_NAME( UTILS_Atomic_LoadN_char )
STATIC_INLINE char
UTILS_Atomic_LoadN_char( char*                 ptr,
                         UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_StoreN_char PACKAGE_MANGLE_NAME( UTILS_Atomic_StoreN_char )
STATIC_INLINE void
UTILS_Atomic_StoreN_char( char*                 ptr,
                          char                  val,
                          UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_ExchangeN_char PACKAGE_MANGLE_NAME( UTILS_Atomic_ExchangeN_char )
STATIC_INLINE char
UTILS_Atomic_ExchangeN_char( char*                 ptr,
                             char                  val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_CompareExchangeN_char PACKAGE_MANGLE_NAME( UTILS_Atomic_CompareExchangeN_char )
STATIC_INLINE bool
UTILS_Atomic_CompareExchangeN_char( char*                 ptr,
                                    char*                 expected,
                                    char                  desired,
                                    bool                  weak,
                                    UTILS_Atomic_Memorder successMemorder,
                                    UTILS_Atomic_Memorder failureMemorder );

#define UTILS_Atomic_AddFetch_char PACKAGE_MANGLE_NAME( UTILS_Atomic_AddFetch_char )
STATIC_INLINE char
UTILS_Atomic_AddFetch_char( char*                 ptr,
                            char                  val,
                            UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_SubFetch_char PACKAGE_MANGLE_NAME( UTILS_Atomic_SubFetch_char )
STATIC_INLINE char
UTILS_Atomic_SubFetch_char( char*                 ptr,
                            char                  val,
                            UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_AndFetch_char PACKAGE_MANGLE_NAME( UTILS_Atomic_AndFetch_char )
STATIC_INLINE char
UTILS_Atomic_AndFetch_char( char*                 ptr,
                            char                  val,
                            UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_XorFetch_char PACKAGE_MANGLE_NAME( UTILS_Atomic_XorFetch_char )
STATIC_INLINE char
UTILS_Atomic_XorFetch_char( char*                 ptr,
                            char                  val,
                            UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_OrFetch_char PACKAGE_MANGLE_NAME( UTILS_Atomic_OrFetch_char )
STATIC_INLINE char
UTILS_Atomic_OrFetch_char( char*                 ptr,
                           char                  val,
                           UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_NandFetch_char PACKAGE_MANGLE_NAME( UTILS_Atomic_NandFetch_char )
STATIC_INLINE char
UTILS_Atomic_NandFetch_char( char*                 ptr,
                             char                  val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchAdd_char PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchAdd_char )
STATIC_INLINE char
UTILS_Atomic_FetchAdd_char( char*                 ptr,
                            char                  val,
                            UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchSub_char PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchSub_char )
STATIC_INLINE char
UTILS_Atomic_FetchSub_char( char*                 ptr,
                            char                  val,
                            UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchAnd_char PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchAnd_char )
STATIC_INLINE char
UTILS_Atomic_FetchAnd_char( char*                 ptr,
                            char                  val,
                            UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchXor_char PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchXor_char )
STATIC_INLINE char
UTILS_Atomic_FetchXor_char( char*                 ptr,
                            char                  val,
                            UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchOr_char PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchOr_char )
STATIC_INLINE char
UTILS_Atomic_FetchOr_char( char*                 ptr,
                           char                  val,
                           UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchNand_char PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchNand_char )
STATIC_INLINE char
UTILS_Atomic_FetchNand_char( char*                 ptr,
                             char                  val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_LoadN_uint8 PACKAGE_MANGLE_NAME( UTILS_Atomic_LoadN_uint8 )
STATIC_INLINE uint8_t
UTILS_Atomic_LoadN_uint8( uint8_t*              ptr,
                          UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_StoreN_uint8 PACKAGE_MANGLE_NAME( UTILS_Atomic_StoreN_uint8 )
STATIC_INLINE void
UTILS_Atomic_StoreN_uint8( uint8_t*              ptr,
                           uint8_t               val,
                           UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_ExchangeN_uint8 PACKAGE_MANGLE_NAME( UTILS_Atomic_ExchangeN_uint8 )
STATIC_INLINE uint8_t
UTILS_Atomic_ExchangeN_uint8( uint8_t*              ptr,
                              uint8_t               val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_CompareExchangeN_uint8 PACKAGE_MANGLE_NAME( UTILS_Atomic_CompareExchangeN_uint8 )
STATIC_INLINE bool
UTILS_Atomic_CompareExchangeN_uint8( uint8_t*              ptr,
                                     uint8_t*              expected,
                                     uint8_t               desired,
                                     bool                  weak,
                                     UTILS_Atomic_Memorder successMemorder,
                                     UTILS_Atomic_Memorder failureMemorder );

#define UTILS_Atomic_AddFetch_uint8 PACKAGE_MANGLE_NAME( UTILS_Atomic_AddFetch_uint8 )
STATIC_INLINE uint8_t
UTILS_Atomic_AddFetch_uint8( uint8_t*              ptr,
                             uint8_t               val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_SubFetch_uint8 PACKAGE_MANGLE_NAME( UTILS_Atomic_SubFetch_uint8 )
STATIC_INLINE uint8_t
UTILS_Atomic_SubFetch_uint8( uint8_t*              ptr,
                             uint8_t               val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_AndFetch_uint8 PACKAGE_MANGLE_NAME( UTILS_Atomic_AndFetch_uint8 )
STATIC_INLINE uint8_t
UTILS_Atomic_AndFetch_uint8( uint8_t*              ptr,
                             uint8_t               val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_XorFetch_uint8 PACKAGE_MANGLE_NAME( UTILS_Atomic_XorFetch_uint8 )
STATIC_INLINE uint8_t
UTILS_Atomic_XorFetch_uint8( uint8_t*              ptr,
                             uint8_t               val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_OrFetch_uint8 PACKAGE_MANGLE_NAME( UTILS_Atomic_OrFetch_uint8 )
STATIC_INLINE uint8_t
UTILS_Atomic_OrFetch_uint8( uint8_t*              ptr,
                            uint8_t               val,
                            UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_NandFetch_uint8 PACKAGE_MANGLE_NAME( UTILS_Atomic_NandFetch_uint8 )
STATIC_INLINE uint8_t
UTILS_Atomic_NandFetch_uint8( uint8_t*              ptr,
                              uint8_t               val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchAdd_uint8 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchAdd_uint8 )
STATIC_INLINE uint8_t
UTILS_Atomic_FetchAdd_uint8( uint8_t*              ptr,
                             uint8_t               val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchSub_uint8 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchSub_uint8 )
STATIC_INLINE uint8_t
UTILS_Atomic_FetchSub_uint8( uint8_t*              ptr,
                             uint8_t               val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchAnd_uint8 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchAnd_uint8 )
STATIC_INLINE uint8_t
UTILS_Atomic_FetchAnd_uint8( uint8_t*              ptr,
                             uint8_t               val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchXor_uint8 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchXor_uint8 )
STATIC_INLINE uint8_t
UTILS_Atomic_FetchXor_uint8( uint8_t*              ptr,
                             uint8_t               val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchOr_uint8 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchOr_uint8 )
STATIC_INLINE uint8_t
UTILS_Atomic_FetchOr_uint8( uint8_t*              ptr,
                            uint8_t               val,
                            UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchNand_uint8 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchNand_uint8 )
STATIC_INLINE uint8_t
UTILS_Atomic_FetchNand_uint8( uint8_t*              ptr,
                              uint8_t               val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_LoadN_uint16 PACKAGE_MANGLE_NAME( UTILS_Atomic_LoadN_uint16 )
STATIC_INLINE uint16_t
UTILS_Atomic_LoadN_uint16( uint16_t*             ptr,
                           UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_StoreN_uint16 PACKAGE_MANGLE_NAME( UTILS_Atomic_StoreN_uint16 )
STATIC_INLINE void
UTILS_Atomic_StoreN_uint16( uint16_t*             ptr,
                            uint16_t              val,
                            UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_ExchangeN_uint16 PACKAGE_MANGLE_NAME( UTILS_Atomic_ExchangeN_uint16 )
STATIC_INLINE uint16_t
UTILS_Atomic_ExchangeN_uint16( uint16_t*             ptr,
                               uint16_t              val,
                               UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_CompareExchangeN_uint16 PACKAGE_MANGLE_NAME( UTILS_Atomic_CompareExchangeN_uint16 )
STATIC_INLINE bool
UTILS_Atomic_CompareExchangeN_uint16( uint16_t*             ptr,
                                      uint16_t*             expected,
                                      uint16_t              desired,
                                      bool                  weak,
                                      UTILS_Atomic_Memorder successMemorder,
                                      UTILS_Atomic_Memorder failureMemorder );

#define UTILS_Atomic_AddFetch_uint16 PACKAGE_MANGLE_NAME( UTILS_Atomic_AddFetch_uint16 )
STATIC_INLINE uint16_t
UTILS_Atomic_AddFetch_uint16( uint16_t*             ptr,
                              uint16_t              val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_SubFetch_uint16 PACKAGE_MANGLE_NAME( UTILS_Atomic_SubFetch_uint16 )
STATIC_INLINE uint16_t
UTILS_Atomic_SubFetch_uint16( uint16_t*             ptr,
                              uint16_t              val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_AndFetch_uint16 PACKAGE_MANGLE_NAME( UTILS_Atomic_AndFetch_uint16 )
STATIC_INLINE uint16_t
UTILS_Atomic_AndFetch_uint16( uint16_t*             ptr,
                              uint16_t              val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_XorFetch_uint16 PACKAGE_MANGLE_NAME( UTILS_Atomic_XorFetch_uint16 )
STATIC_INLINE uint16_t
UTILS_Atomic_XorFetch_uint16( uint16_t*             ptr,
                              uint16_t              val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_OrFetch_uint16 PACKAGE_MANGLE_NAME( UTILS_Atomic_OrFetch_uint16 )
STATIC_INLINE uint16_t
UTILS_Atomic_OrFetch_uint16( uint16_t*             ptr,
                             uint16_t              val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_NandFetch_uint16 PACKAGE_MANGLE_NAME( UTILS_Atomic_NandFetch_uint16 )
STATIC_INLINE uint16_t
UTILS_Atomic_NandFetch_uint16( uint16_t*             ptr,
                               uint16_t              val,
                               UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchAdd_uint16 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchAdd_uint16 )
STATIC_INLINE uint16_t
UTILS_Atomic_FetchAdd_uint16( uint16_t*             ptr,
                              uint16_t              val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchSub_uint16 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchSub_uint16 )
STATIC_INLINE uint16_t
UTILS_Atomic_FetchSub_uint16( uint16_t*             ptr,
                              uint16_t              val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchAnd_uint16 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchAnd_uint16 )
STATIC_INLINE uint16_t
UTILS_Atomic_FetchAnd_uint16( uint16_t*             ptr,
                              uint16_t              val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchXor_uint16 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchXor_uint16 )
STATIC_INLINE uint16_t
UTILS_Atomic_FetchXor_uint16( uint16_t*             ptr,
                              uint16_t              val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchOr_uint16 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchOr_uint16 )
STATIC_INLINE uint16_t
UTILS_Atomic_FetchOr_uint16( uint16_t*             ptr,
                             uint16_t              val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchNand_uint16 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchNand_uint16 )
STATIC_INLINE uint16_t
UTILS_Atomic_FetchNand_uint16( uint16_t*             ptr,
                               uint16_t              val,
                               UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_LoadN_uint32 PACKAGE_MANGLE_NAME( UTILS_Atomic_LoadN_uint32 )
STATIC_INLINE uint32_t
UTILS_Atomic_LoadN_uint32( uint32_t*             ptr,
                           UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_StoreN_uint32 PACKAGE_MANGLE_NAME( UTILS_Atomic_StoreN_uint32 )
STATIC_INLINE void
UTILS_Atomic_StoreN_uint32( uint32_t*             ptr,
                            uint32_t              val,
                            UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_ExchangeN_uint32 PACKAGE_MANGLE_NAME( UTILS_Atomic_ExchangeN_uint32 )
STATIC_INLINE uint32_t
UTILS_Atomic_ExchangeN_uint32( uint32_t*             ptr,
                               uint32_t              val,
                               UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_CompareExchangeN_uint32 PACKAGE_MANGLE_NAME( UTILS_Atomic_CompareExchangeN_uint32 )
STATIC_INLINE bool
UTILS_Atomic_CompareExchangeN_uint32( uint32_t*             ptr,
                                      uint32_t*             expected,
                                      uint32_t              desired,
                                      bool                  weak,
                                      UTILS_Atomic_Memorder successMemorder,
                                      UTILS_Atomic_Memorder failureMemorder );

#define UTILS_Atomic_AddFetch_uint32 PACKAGE_MANGLE_NAME( UTILS_Atomic_AddFetch_uint32 )
STATIC_INLINE uint32_t
UTILS_Atomic_AddFetch_uint32( uint32_t*             ptr,
                              uint32_t              val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_SubFetch_uint32 PACKAGE_MANGLE_NAME( UTILS_Atomic_SubFetch_uint32 )
STATIC_INLINE uint32_t
UTILS_Atomic_SubFetch_uint32( uint32_t*             ptr,
                              uint32_t              val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_AndFetch_uint32 PACKAGE_MANGLE_NAME( UTILS_Atomic_AndFetch_uint32 )
STATIC_INLINE uint32_t
UTILS_Atomic_AndFetch_uint32( uint32_t*             ptr,
                              uint32_t              val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_XorFetch_uint32 PACKAGE_MANGLE_NAME( UTILS_Atomic_XorFetch_uint32 )
STATIC_INLINE uint32_t
UTILS_Atomic_XorFetch_uint32( uint32_t*             ptr,
                              uint32_t              val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_OrFetch_uint32 PACKAGE_MANGLE_NAME( UTILS_Atomic_OrFetch_uint32 )
STATIC_INLINE uint32_t
UTILS_Atomic_OrFetch_uint32( uint32_t*             ptr,
                             uint32_t              val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_NandFetch_uint32 PACKAGE_MANGLE_NAME( UTILS_Atomic_NandFetch_uint32 )
STATIC_INLINE uint32_t
UTILS_Atomic_NandFetch_uint32( uint32_t*             ptr,
                               uint32_t              val,
                               UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchAdd_uint32 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchAdd_uint32 )
STATIC_INLINE uint32_t
UTILS_Atomic_FetchAdd_uint32( uint32_t*             ptr,
                              uint32_t              val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchSub_uint32 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchSub_uint32 )
STATIC_INLINE uint32_t
UTILS_Atomic_FetchSub_uint32( uint32_t*             ptr,
                              uint32_t              val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchAnd_uint32 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchAnd_uint32 )
STATIC_INLINE uint32_t
UTILS_Atomic_FetchAnd_uint32( uint32_t*             ptr,
                              uint32_t              val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchXor_uint32 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchXor_uint32 )
STATIC_INLINE uint32_t
UTILS_Atomic_FetchXor_uint32( uint32_t*             ptr,
                              uint32_t              val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchOr_uint32 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchOr_uint32 )
STATIC_INLINE uint32_t
UTILS_Atomic_FetchOr_uint32( uint32_t*             ptr,
                             uint32_t              val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchNand_uint32 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchNand_uint32 )
STATIC_INLINE uint32_t
UTILS_Atomic_FetchNand_uint32( uint32_t*             ptr,
                               uint32_t              val,
                               UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_LoadN_uint64 PACKAGE_MANGLE_NAME( UTILS_Atomic_LoadN_uint64 )
STATIC_INLINE uint64_t
UTILS_Atomic_LoadN_uint64( uint64_t*             ptr,
                           UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_StoreN_uint64 PACKAGE_MANGLE_NAME( UTILS_Atomic_StoreN_uint64 )
STATIC_INLINE void
UTILS_Atomic_StoreN_uint64( uint64_t*             ptr,
                            uint64_t              val,
                            UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_ExchangeN_uint64 PACKAGE_MANGLE_NAME( UTILS_Atomic_ExchangeN_uint64 )
STATIC_INLINE uint64_t
UTILS_Atomic_ExchangeN_uint64( uint64_t*             ptr,
                               uint64_t              val,
                               UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_CompareExchangeN_uint64 PACKAGE_MANGLE_NAME( UTILS_Atomic_CompareExchangeN_uint64 )
STATIC_INLINE bool
UTILS_Atomic_CompareExchangeN_uint64( uint64_t*             ptr,
                                      uint64_t*             expected,
                                      uint64_t              desired,
                                      bool                  weak,
                                      UTILS_Atomic_Memorder successMemorder,
                                      UTILS_Atomic_Memorder failureMemorder );

#define UTILS_Atomic_AddFetch_uint64 PACKAGE_MANGLE_NAME( UTILS_Atomic_AddFetch_uint64 )
STATIC_INLINE uint64_t
UTILS_Atomic_AddFetch_uint64( uint64_t*             ptr,
                              uint64_t              val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_SubFetch_uint64 PACKAGE_MANGLE_NAME( UTILS_Atomic_SubFetch_uint64 )
STATIC_INLINE uint64_t
UTILS_Atomic_SubFetch_uint64( uint64_t*             ptr,
                              uint64_t              val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_AndFetch_uint64 PACKAGE_MANGLE_NAME( UTILS_Atomic_AndFetch_uint64 )
STATIC_INLINE uint64_t
UTILS_Atomic_AndFetch_uint64( uint64_t*             ptr,
                              uint64_t              val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_XorFetch_uint64 PACKAGE_MANGLE_NAME( UTILS_Atomic_XorFetch_uint64 )
STATIC_INLINE uint64_t
UTILS_Atomic_XorFetch_uint64( uint64_t*             ptr,
                              uint64_t              val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_OrFetch_uint64 PACKAGE_MANGLE_NAME( UTILS_Atomic_OrFetch_uint64 )
STATIC_INLINE uint64_t
UTILS_Atomic_OrFetch_uint64( uint64_t*             ptr,
                             uint64_t              val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_NandFetch_uint64 PACKAGE_MANGLE_NAME( UTILS_Atomic_NandFetch_uint64 )
STATIC_INLINE uint64_t
UTILS_Atomic_NandFetch_uint64( uint64_t*             ptr,
                               uint64_t              val,
                               UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchAdd_uint64 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchAdd_uint64 )
STATIC_INLINE uint64_t
UTILS_Atomic_FetchAdd_uint64( uint64_t*             ptr,
                              uint64_t              val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchSub_uint64 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchSub_uint64 )
STATIC_INLINE uint64_t
UTILS_Atomic_FetchSub_uint64( uint64_t*             ptr,
                              uint64_t              val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchAnd_uint64 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchAnd_uint64 )
STATIC_INLINE uint64_t
UTILS_Atomic_FetchAnd_uint64( uint64_t*             ptr,
                              uint64_t              val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchXor_uint64 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchXor_uint64 )
STATIC_INLINE uint64_t
UTILS_Atomic_FetchXor_uint64( uint64_t*             ptr,
                              uint64_t              val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchOr_uint64 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchOr_uint64 )
STATIC_INLINE uint64_t
UTILS_Atomic_FetchOr_uint64( uint64_t*             ptr,
                             uint64_t              val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchNand_uint64 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchNand_uint64 )
STATIC_INLINE uint64_t
UTILS_Atomic_FetchNand_uint64( uint64_t*             ptr,
                               uint64_t              val,
                               UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_LoadN_uintptr PACKAGE_MANGLE_NAME( UTILS_Atomic_LoadN_uintptr )
STATIC_INLINE uintptr_t
UTILS_Atomic_LoadN_uintptr( uintptr_t*            ptr,
                            UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_StoreN_uintptr PACKAGE_MANGLE_NAME( UTILS_Atomic_StoreN_uintptr )
STATIC_INLINE void
UTILS_Atomic_StoreN_uintptr( uintptr_t*            ptr,
                             uintptr_t             val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_ExchangeN_uintptr PACKAGE_MANGLE_NAME( UTILS_Atomic_ExchangeN_uintptr )
STATIC_INLINE uintptr_t
UTILS_Atomic_ExchangeN_uintptr( uintptr_t*            ptr,
                                uintptr_t             val,
                                UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_CompareExchangeN_uintptr PACKAGE_MANGLE_NAME( UTILS_Atomic_CompareExchangeN_uintptr )
STATIC_INLINE bool
UTILS_Atomic_CompareExchangeN_uintptr( uintptr_t*            ptr,
                                       uintptr_t*            expected,
                                       uintptr_t             desired,
                                       bool                  weak,
                                       UTILS_Atomic_Memorder successMemorder,
                                       UTILS_Atomic_Memorder failureMemorder );

#define UTILS_Atomic_AddFetch_uintptr PACKAGE_MANGLE_NAME( UTILS_Atomic_AddFetch_uintptr )
STATIC_INLINE uintptr_t
UTILS_Atomic_AddFetch_uintptr( uintptr_t*            ptr,
                               uintptr_t             val,
                               UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_SubFetch_uintptr PACKAGE_MANGLE_NAME( UTILS_Atomic_SubFetch_uintptr )
STATIC_INLINE uintptr_t
UTILS_Atomic_SubFetch_uintptr( uintptr_t*            ptr,
                               uintptr_t             val,
                               UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_AndFetch_uintptr PACKAGE_MANGLE_NAME( UTILS_Atomic_AndFetch_uintptr )
STATIC_INLINE uintptr_t
UTILS_Atomic_AndFetch_uintptr( uintptr_t*            ptr,
                               uintptr_t             val,
                               UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_XorFetch_uintptr PACKAGE_MANGLE_NAME( UTILS_Atomic_XorFetch_uintptr )
STATIC_INLINE uintptr_t
UTILS_Atomic_XorFetch_uintptr( uintptr_t*            ptr,
                               uintptr_t             val,
                               UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_OrFetch_uintptr PACKAGE_MANGLE_NAME( UTILS_Atomic_OrFetch_uintptr )
STATIC_INLINE uintptr_t
UTILS_Atomic_OrFetch_uintptr( uintptr_t*            ptr,
                              uintptr_t             val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_NandFetch_uintptr PACKAGE_MANGLE_NAME( UTILS_Atomic_NandFetch_uintptr )
STATIC_INLINE uintptr_t
UTILS_Atomic_NandFetch_uintptr( uintptr_t*            ptr,
                                uintptr_t             val,
                                UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchAdd_uintptr PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchAdd_uintptr )
STATIC_INLINE uintptr_t
UTILS_Atomic_FetchAdd_uintptr( uintptr_t*            ptr,
                               uintptr_t             val,
                               UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchSub_uintptr PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchSub_uintptr )
STATIC_INLINE uintptr_t
UTILS_Atomic_FetchSub_uintptr( uintptr_t*            ptr,
                               uintptr_t             val,
                               UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchAnd_uintptr PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchAnd_uintptr )
STATIC_INLINE uintptr_t
UTILS_Atomic_FetchAnd_uintptr( uintptr_t*            ptr,
                               uintptr_t             val,
                               UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchXor_uintptr PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchXor_uintptr )
STATIC_INLINE uintptr_t
UTILS_Atomic_FetchXor_uintptr( uintptr_t*            ptr,
                               uintptr_t             val,
                               UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchOr_uintptr PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchOr_uintptr )
STATIC_INLINE uintptr_t
UTILS_Atomic_FetchOr_uintptr( uintptr_t*            ptr,
                              uintptr_t             val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchNand_uintptr PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchNand_uintptr )
STATIC_INLINE uintptr_t
UTILS_Atomic_FetchNand_uintptr( uintptr_t*            ptr,
                                uintptr_t             val,
                                UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_LoadN_int8 PACKAGE_MANGLE_NAME( UTILS_Atomic_LoadN_int8 )
STATIC_INLINE int8_t
UTILS_Atomic_LoadN_int8( int8_t*               ptr,
                         UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_StoreN_int8 PACKAGE_MANGLE_NAME( UTILS_Atomic_StoreN_int8 )
STATIC_INLINE void
UTILS_Atomic_StoreN_int8( int8_t*               ptr,
                          int8_t                val,
                          UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_ExchangeN_int8 PACKAGE_MANGLE_NAME( UTILS_Atomic_ExchangeN_int8 )
STATIC_INLINE int8_t
UTILS_Atomic_ExchangeN_int8( int8_t*               ptr,
                             int8_t                val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_CompareExchangeN_int8 PACKAGE_MANGLE_NAME( UTILS_Atomic_CompareExchangeN_int8 )
STATIC_INLINE bool
UTILS_Atomic_CompareExchangeN_int8( int8_t*               ptr,
                                    int8_t*               expected,
                                    int8_t                desired,
                                    bool                  weak,
                                    UTILS_Atomic_Memorder successMemorder,
                                    UTILS_Atomic_Memorder failureMemorder );

#define UTILS_Atomic_AddFetch_int8 PACKAGE_MANGLE_NAME( UTILS_Atomic_AddFetch_int8 )
STATIC_INLINE int8_t
UTILS_Atomic_AddFetch_int8( int8_t*               ptr,
                            int8_t                val,
                            UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_SubFetch_int8 PACKAGE_MANGLE_NAME( UTILS_Atomic_SubFetch_int8 )
STATIC_INLINE int8_t
UTILS_Atomic_SubFetch_int8( int8_t*               ptr,
                            int8_t                val,
                            UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_AndFetch_int8 PACKAGE_MANGLE_NAME( UTILS_Atomic_AndFetch_int8 )
STATIC_INLINE int8_t
UTILS_Atomic_AndFetch_int8( int8_t*               ptr,
                            int8_t                val,
                            UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_XorFetch_int8 PACKAGE_MANGLE_NAME( UTILS_Atomic_XorFetch_int8 )
STATIC_INLINE int8_t
UTILS_Atomic_XorFetch_int8( int8_t*               ptr,
                            int8_t                val,
                            UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_OrFetch_int8 PACKAGE_MANGLE_NAME( UTILS_Atomic_OrFetch_int8 )
STATIC_INLINE int8_t
UTILS_Atomic_OrFetch_int8( int8_t*               ptr,
                           int8_t                val,
                           UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_NandFetch_int8 PACKAGE_MANGLE_NAME( UTILS_Atomic_NandFetch_int8 )
STATIC_INLINE int8_t
UTILS_Atomic_NandFetch_int8( int8_t*               ptr,
                             int8_t                val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchAdd_int8 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchAdd_int8 )
STATIC_INLINE int8_t
UTILS_Atomic_FetchAdd_int8( int8_t*               ptr,
                            int8_t                val,
                            UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchSub_int8 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchSub_int8 )
STATIC_INLINE int8_t
UTILS_Atomic_FetchSub_int8( int8_t*               ptr,
                            int8_t                val,
                            UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchAnd_int8 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchAnd_int8 )
STATIC_INLINE int8_t
UTILS_Atomic_FetchAnd_int8( int8_t*               ptr,
                            int8_t                val,
                            UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchXor_int8 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchXor_int8 )
STATIC_INLINE int8_t
UTILS_Atomic_FetchXor_int8( int8_t*               ptr,
                            int8_t                val,
                            UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchOr_int8 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchOr_int8 )
STATIC_INLINE int8_t
UTILS_Atomic_FetchOr_int8( int8_t*               ptr,
                           int8_t                val,
                           UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchNand_int8 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchNand_int8 )
STATIC_INLINE int8_t
UTILS_Atomic_FetchNand_int8( int8_t*               ptr,
                             int8_t                val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_LoadN_int16 PACKAGE_MANGLE_NAME( UTILS_Atomic_LoadN_int16 )
STATIC_INLINE int16_t
UTILS_Atomic_LoadN_int16( int16_t*              ptr,
                          UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_StoreN_int16 PACKAGE_MANGLE_NAME( UTILS_Atomic_StoreN_int16 )
STATIC_INLINE void
UTILS_Atomic_StoreN_int16( int16_t*              ptr,
                           int16_t               val,
                           UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_ExchangeN_int16 PACKAGE_MANGLE_NAME( UTILS_Atomic_ExchangeN_int16 )
STATIC_INLINE int16_t
UTILS_Atomic_ExchangeN_int16( int16_t*              ptr,
                              int16_t               val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_CompareExchangeN_int16 PACKAGE_MANGLE_NAME( UTILS_Atomic_CompareExchangeN_int16 )
STATIC_INLINE bool
UTILS_Atomic_CompareExchangeN_int16( int16_t*              ptr,
                                     int16_t*              expected,
                                     int16_t               desired,
                                     bool                  weak,
                                     UTILS_Atomic_Memorder successMemorder,
                                     UTILS_Atomic_Memorder failureMemorder );

#define UTILS_Atomic_AddFetch_int16 PACKAGE_MANGLE_NAME( UTILS_Atomic_AddFetch_int16 )
STATIC_INLINE int16_t
UTILS_Atomic_AddFetch_int16( int16_t*              ptr,
                             int16_t               val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_SubFetch_int16 PACKAGE_MANGLE_NAME( UTILS_Atomic_SubFetch_int16 )
STATIC_INLINE int16_t
UTILS_Atomic_SubFetch_int16( int16_t*              ptr,
                             int16_t               val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_AndFetch_int16 PACKAGE_MANGLE_NAME( UTILS_Atomic_AndFetch_int16 )
STATIC_INLINE int16_t
UTILS_Atomic_AndFetch_int16( int16_t*              ptr,
                             int16_t               val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_XorFetch_int16 PACKAGE_MANGLE_NAME( UTILS_Atomic_XorFetch_int16 )
STATIC_INLINE int16_t
UTILS_Atomic_XorFetch_int16( int16_t*              ptr,
                             int16_t               val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_OrFetch_int16 PACKAGE_MANGLE_NAME( UTILS_Atomic_OrFetch_int16 )
STATIC_INLINE int16_t
UTILS_Atomic_OrFetch_int16( int16_t*              ptr,
                            int16_t               val,
                            UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_NandFetch_int16 PACKAGE_MANGLE_NAME( UTILS_Atomic_NandFetch_int16 )
STATIC_INLINE int16_t
UTILS_Atomic_NandFetch_int16( int16_t*              ptr,
                              int16_t               val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchAdd_int16 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchAdd_int16 )
STATIC_INLINE int16_t
UTILS_Atomic_FetchAdd_int16( int16_t*              ptr,
                             int16_t               val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchSub_int16 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchSub_int16 )
STATIC_INLINE int16_t
UTILS_Atomic_FetchSub_int16( int16_t*              ptr,
                             int16_t               val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchAnd_int16 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchAnd_int16 )
STATIC_INLINE int16_t
UTILS_Atomic_FetchAnd_int16( int16_t*              ptr,
                             int16_t               val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchXor_int16 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchXor_int16 )
STATIC_INLINE int16_t
UTILS_Atomic_FetchXor_int16( int16_t*              ptr,
                             int16_t               val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchOr_int16 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchOr_int16 )
STATIC_INLINE int16_t
UTILS_Atomic_FetchOr_int16( int16_t*              ptr,
                            int16_t               val,
                            UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchNand_int16 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchNand_int16 )
STATIC_INLINE int16_t
UTILS_Atomic_FetchNand_int16( int16_t*              ptr,
                              int16_t               val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_LoadN_int32 PACKAGE_MANGLE_NAME( UTILS_Atomic_LoadN_int32 )
STATIC_INLINE int32_t
UTILS_Atomic_LoadN_int32( int32_t*              ptr,
                          UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_StoreN_int32 PACKAGE_MANGLE_NAME( UTILS_Atomic_StoreN_int32 )
STATIC_INLINE void
UTILS_Atomic_StoreN_int32( int32_t*              ptr,
                           int32_t               val,
                           UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_ExchangeN_int32 PACKAGE_MANGLE_NAME( UTILS_Atomic_ExchangeN_int32 )
STATIC_INLINE int32_t
UTILS_Atomic_ExchangeN_int32( int32_t*              ptr,
                              int32_t               val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_CompareExchangeN_int32 PACKAGE_MANGLE_NAME( UTILS_Atomic_CompareExchangeN_int32 )
STATIC_INLINE bool
UTILS_Atomic_CompareExchangeN_int32( int32_t*              ptr,
                                     int32_t*              expected,
                                     int32_t               desired,
                                     bool                  weak,
                                     UTILS_Atomic_Memorder successMemorder,
                                     UTILS_Atomic_Memorder failureMemorder );

#define UTILS_Atomic_AddFetch_int32 PACKAGE_MANGLE_NAME( UTILS_Atomic_AddFetch_int32 )
STATIC_INLINE int32_t
UTILS_Atomic_AddFetch_int32( int32_t*              ptr,
                             int32_t               val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_SubFetch_int32 PACKAGE_MANGLE_NAME( UTILS_Atomic_SubFetch_int32 )
STATIC_INLINE int32_t
UTILS_Atomic_SubFetch_int32( int32_t*              ptr,
                             int32_t               val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_AndFetch_int32 PACKAGE_MANGLE_NAME( UTILS_Atomic_AndFetch_int32 )
STATIC_INLINE int32_t
UTILS_Atomic_AndFetch_int32( int32_t*              ptr,
                             int32_t               val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_XorFetch_int32 PACKAGE_MANGLE_NAME( UTILS_Atomic_XorFetch_int32 )
STATIC_INLINE int32_t
UTILS_Atomic_XorFetch_int32( int32_t*              ptr,
                             int32_t               val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_OrFetch_int32 PACKAGE_MANGLE_NAME( UTILS_Atomic_OrFetch_int32 )
STATIC_INLINE int32_t
UTILS_Atomic_OrFetch_int32( int32_t*              ptr,
                            int32_t               val,
                            UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_NandFetch_int32 PACKAGE_MANGLE_NAME( UTILS_Atomic_NandFetch_int32 )
STATIC_INLINE int32_t
UTILS_Atomic_NandFetch_int32( int32_t*              ptr,
                              int32_t               val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchAdd_int32 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchAdd_int32 )
STATIC_INLINE int32_t
UTILS_Atomic_FetchAdd_int32( int32_t*              ptr,
                             int32_t               val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchSub_int32 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchSub_int32 )
STATIC_INLINE int32_t
UTILS_Atomic_FetchSub_int32( int32_t*              ptr,
                             int32_t               val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchAnd_int32 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchAnd_int32 )
STATIC_INLINE int32_t
UTILS_Atomic_FetchAnd_int32( int32_t*              ptr,
                             int32_t               val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchXor_int32 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchXor_int32 )
STATIC_INLINE int32_t
UTILS_Atomic_FetchXor_int32( int32_t*              ptr,
                             int32_t               val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchOr_int32 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchOr_int32 )
STATIC_INLINE int32_t
UTILS_Atomic_FetchOr_int32( int32_t*              ptr,
                            int32_t               val,
                            UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchNand_int32 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchNand_int32 )
STATIC_INLINE int32_t
UTILS_Atomic_FetchNand_int32( int32_t*              ptr,
                              int32_t               val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_LoadN_int64 PACKAGE_MANGLE_NAME( UTILS_Atomic_LoadN_int64 )
STATIC_INLINE int64_t
UTILS_Atomic_LoadN_int64( int64_t*              ptr,
                          UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_StoreN_int64 PACKAGE_MANGLE_NAME( UTILS_Atomic_StoreN_int64 )
STATIC_INLINE void
UTILS_Atomic_StoreN_int64( int64_t*              ptr,
                           int64_t               val,
                           UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_ExchangeN_int64 PACKAGE_MANGLE_NAME( UTILS_Atomic_ExchangeN_int64 )
STATIC_INLINE int64_t
UTILS_Atomic_ExchangeN_int64( int64_t*              ptr,
                              int64_t               val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_CompareExchangeN_int64 PACKAGE_MANGLE_NAME( UTILS_Atomic_CompareExchangeN_int64 )
STATIC_INLINE bool
UTILS_Atomic_CompareExchangeN_int64( int64_t*              ptr,
                                     int64_t*              expected,
                                     int64_t               desired,
                                     bool                  weak,
                                     UTILS_Atomic_Memorder successMemorder,
                                     UTILS_Atomic_Memorder failureMemorder );

#define UTILS_Atomic_AddFetch_int64 PACKAGE_MANGLE_NAME( UTILS_Atomic_AddFetch_int64 )
STATIC_INLINE int64_t
UTILS_Atomic_AddFetch_int64( int64_t*              ptr,
                             int64_t               val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_SubFetch_int64 PACKAGE_MANGLE_NAME( UTILS_Atomic_SubFetch_int64 )
STATIC_INLINE int64_t
UTILS_Atomic_SubFetch_int64( int64_t*              ptr,
                             int64_t               val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_AndFetch_int64 PACKAGE_MANGLE_NAME( UTILS_Atomic_AndFetch_int64 )
STATIC_INLINE int64_t
UTILS_Atomic_AndFetch_int64( int64_t*              ptr,
                             int64_t               val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_XorFetch_int64 PACKAGE_MANGLE_NAME( UTILS_Atomic_XorFetch_int64 )
STATIC_INLINE int64_t
UTILS_Atomic_XorFetch_int64( int64_t*              ptr,
                             int64_t               val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_OrFetch_int64 PACKAGE_MANGLE_NAME( UTILS_Atomic_OrFetch_int64 )
STATIC_INLINE int64_t
UTILS_Atomic_OrFetch_int64( int64_t*              ptr,
                            int64_t               val,
                            UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_NandFetch_int64 PACKAGE_MANGLE_NAME( UTILS_Atomic_NandFetch_int64 )
STATIC_INLINE int64_t
UTILS_Atomic_NandFetch_int64( int64_t*              ptr,
                              int64_t               val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchAdd_int64 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchAdd_int64 )
STATIC_INLINE int64_t
UTILS_Atomic_FetchAdd_int64( int64_t*              ptr,
                             int64_t               val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchSub_int64 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchSub_int64 )
STATIC_INLINE int64_t
UTILS_Atomic_FetchSub_int64( int64_t*              ptr,
                             int64_t               val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchAnd_int64 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchAnd_int64 )
STATIC_INLINE int64_t
UTILS_Atomic_FetchAnd_int64( int64_t*              ptr,
                             int64_t               val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchXor_int64 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchXor_int64 )
STATIC_INLINE int64_t
UTILS_Atomic_FetchXor_int64( int64_t*              ptr,
                             int64_t               val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchOr_int64 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchOr_int64 )
STATIC_INLINE int64_t
UTILS_Atomic_FetchOr_int64( int64_t*              ptr,
                            int64_t               val,
                            UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchNand_int64 PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchNand_int64 )
STATIC_INLINE int64_t
UTILS_Atomic_FetchNand_int64( int64_t*              ptr,
                              int64_t               val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_LoadN_intptr PACKAGE_MANGLE_NAME( UTILS_Atomic_LoadN_intptr )
STATIC_INLINE intptr_t
UTILS_Atomic_LoadN_intptr( intptr_t*             ptr,
                           UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_StoreN_intptr PACKAGE_MANGLE_NAME( UTILS_Atomic_StoreN_intptr )
STATIC_INLINE void
UTILS_Atomic_StoreN_intptr( intptr_t*             ptr,
                            intptr_t              val,
                            UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_ExchangeN_intptr PACKAGE_MANGLE_NAME( UTILS_Atomic_ExchangeN_intptr )
STATIC_INLINE intptr_t
UTILS_Atomic_ExchangeN_intptr( intptr_t*             ptr,
                               intptr_t              val,
                               UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_CompareExchangeN_intptr PACKAGE_MANGLE_NAME( UTILS_Atomic_CompareExchangeN_intptr )
STATIC_INLINE bool
UTILS_Atomic_CompareExchangeN_intptr( intptr_t*             ptr,
                                      intptr_t*             expected,
                                      intptr_t              desired,
                                      bool                  weak,
                                      UTILS_Atomic_Memorder successMemorder,
                                      UTILS_Atomic_Memorder failureMemorder );

#define UTILS_Atomic_AddFetch_intptr PACKAGE_MANGLE_NAME( UTILS_Atomic_AddFetch_intptr )
STATIC_INLINE intptr_t
UTILS_Atomic_AddFetch_intptr( intptr_t*             ptr,
                              intptr_t              val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_SubFetch_intptr PACKAGE_MANGLE_NAME( UTILS_Atomic_SubFetch_intptr )
STATIC_INLINE intptr_t
UTILS_Atomic_SubFetch_intptr( intptr_t*             ptr,
                              intptr_t              val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_AndFetch_intptr PACKAGE_MANGLE_NAME( UTILS_Atomic_AndFetch_intptr )
STATIC_INLINE intptr_t
UTILS_Atomic_AndFetch_intptr( intptr_t*             ptr,
                              intptr_t              val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_XorFetch_intptr PACKAGE_MANGLE_NAME( UTILS_Atomic_XorFetch_intptr )
STATIC_INLINE intptr_t
UTILS_Atomic_XorFetch_intptr( intptr_t*             ptr,
                              intptr_t              val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_OrFetch_intptr PACKAGE_MANGLE_NAME( UTILS_Atomic_OrFetch_intptr )
STATIC_INLINE intptr_t
UTILS_Atomic_OrFetch_intptr( intptr_t*             ptr,
                             intptr_t              val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_NandFetch_intptr PACKAGE_MANGLE_NAME( UTILS_Atomic_NandFetch_intptr )
STATIC_INLINE intptr_t
UTILS_Atomic_NandFetch_intptr( intptr_t*             ptr,
                               intptr_t              val,
                               UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchAdd_intptr PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchAdd_intptr )
STATIC_INLINE intptr_t
UTILS_Atomic_FetchAdd_intptr( intptr_t*             ptr,
                              intptr_t              val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchSub_intptr PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchSub_intptr )
STATIC_INLINE intptr_t
UTILS_Atomic_FetchSub_intptr( intptr_t*             ptr,
                              intptr_t              val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchAnd_intptr PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchAnd_intptr )
STATIC_INLINE intptr_t
UTILS_Atomic_FetchAnd_intptr( intptr_t*             ptr,
                              intptr_t              val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchXor_intptr PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchXor_intptr )
STATIC_INLINE intptr_t
UTILS_Atomic_FetchXor_intptr( intptr_t*             ptr,
                              intptr_t              val,
                              UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchOr_intptr PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchOr_intptr )
STATIC_INLINE intptr_t
UTILS_Atomic_FetchOr_intptr( intptr_t*             ptr,
                             intptr_t              val,
                             UTILS_Atomic_Memorder memorder );

#define UTILS_Atomic_FetchNand_intptr PACKAGE_MANGLE_NAME( UTILS_Atomic_FetchNand_intptr )
STATIC_INLINE intptr_t
UTILS_Atomic_FetchNand_intptr( intptr_t*             ptr,
                               intptr_t              val,
                               UTILS_Atomic_Memorder memorder );

#ifdef __cplusplus
}
#endif


/* At configure time, the following block must not be
 * evaluated, but it is needed at make time. */
#if defined( HAVE_CONFIG_H )
#if HAVE( GCC_ATOMIC_BUILTINS )
#include "../src/atomic/UTILS_Atomic.inc.c"
#endif /* GCC_ATOMIC_BUILTINS */
#undef STATIC_INLINE
#endif /* defined( HAVE_CONFIG_H ) */

#endif /* UTILS_ATOMIC_H */
