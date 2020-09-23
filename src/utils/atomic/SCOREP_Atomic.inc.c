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
 * If CC does not support gcc atomic builtins, which is checked at configure
 * time by SCOREP_GCC_ATOMIC_BUILTINS, use a precompiled, instruction-set
 * specific implementation:
 * src/utils/atomic/SCOREP_Atomic.inc.@SCOREP_CPU_INSTRUCTION_SET@.s
 *
 * If we encounter an unknown instruction set during configure
 * (see SCOREP_CPU_INSTRUCTION_SETS),
 * (1) adapt SCOREP_CPU_INSTRUCTION_SETS to detect this new instruction set,
 * (2) provide SCOREP_Atomic.inc.<new_instruction_set>.s by compiling it using
 *     a gcc compiler that generates code for <new_instruction_set>, e.g.,
 *     gcc -Wall -march=<portable-arch> -mtune=generic -S -O3 SCOREP_Atomic.inc.c -o SCOREP_Atomic.inc.<new_instruction_set>.s
 * (3) Add SCOREP_Atomic.inc.<new_instruction_set>.s to EXTRA_DIST in
 *     src/utils/atomic/Makefile.inc.am
 *
 * If this file changes, the precompiled SCOREP_Atomic.inc.@SCOREP_CPU_INSTRUCTION_SET@.s
 * files need to be updated. As above, use this command with a gcc compiler that
 * generates code for <instruction_set>:
 * gcc -Wall -S -O3 SCOREP_Atomic.inc.c -o SCOREP_Atomic.inc.<instruction_set>.s
 *
 * In src/utils/atomic/Makefile.inc.am, we build libscorep_atomic.la in case
 * CC does not support gcc atomic builtins. Add this convenince library to
 * LIBADD where needed.
 *
 * This file is included by src/utils/include/SCOREP_Atomic.h if gcc
 * builtin atomics are supported. It is also used during configure to
 * check whether gcc atomic builtins are supported.
 */

/* At configure time, the following block must be evaluated,
 * but it is to be ignored at make time. */
#if !defined( HAVE_CONFIG_H )
#define STATIC_INLINE
#include "../include/SCOREP_Atomic.h"
#endif /* !define( HAVE_CONFIG_H ) */

#if HAVE_SCOREP_GCC_ATOMIC_BUILTINS_NEEDS_CASTS
#define PTR_PTR_TO_INT_PTR( ptr ) ( uintptr_t* )( ptr )
#define PTR_TO_INT( ptr ) ( uintptr_t )( ptr )
#define INT_TO_PTR( i ) ( void* )( i )
#else /* ! SCOREP_GCC_ATOMIC_BUILTINS_NEEDS_CASTS */
#define PTR_PTR_TO_INT_PTR( ptr ) ( ptr )
#define PTR_TO_INT( ptr ) ( ptr )
#define INT_TO_PTR( i ) ( i )
#endif /* ! SCOREP_GCC_ATOMIC_BUILTINS_NEEDS_CASTS */


static const int gcc_memorder[] = {
    __ATOMIC_RELAXED,
    __ATOMIC_CONSUME,
    __ATOMIC_ACQUIRE,
    __ATOMIC_RELEASE,
    __ATOMIC_ACQ_REL,
    __ATOMIC_SEQ_CST
};


STATIC_INLINE bool
SCOREP_Atomic_TestAndSet( void* ptr, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_test_and_set( ptr, gcc_memorder[ memorder ] );
}

STATIC_INLINE void
SCOREP_Atomic_clear( bool* ptr, SCOREP_Atomic_Memorder memorder )
{
    __atomic_clear( ptr, gcc_memorder[ memorder ] );
}

STATIC_INLINE void
SCOREP_Atomic_ThreadFence( SCOREP_Atomic_Memorder memorder )
{
    __atomic_thread_fence( gcc_memorder[ memorder ] );
}

STATIC_INLINE void*
SCOREP_Atomic_LoadN_void_ptr( void** ptr, SCOREP_Atomic_Memorder memorder )
{
    return INT_TO_PTR( __atomic_load_n( PTR_PTR_TO_INT_PTR( ptr ), gcc_memorder[ memorder ] ) );
}

STATIC_INLINE void
SCOREP_Atomic_StoreN_void_ptr( void** ptr, void* val, SCOREP_Atomic_Memorder memorder )
{
    __atomic_store_n( PTR_PTR_TO_INT_PTR( ptr ), PTR_TO_INT( val ), gcc_memorder[ memorder ] );
}

STATIC_INLINE void*
SCOREP_Atomic_ExchangeN_void_ptr( void** ptr, void* val, SCOREP_Atomic_Memorder memorder )
{
    return INT_TO_PTR( __atomic_exchange_n( PTR_PTR_TO_INT_PTR( ptr ), PTR_TO_INT( val ), gcc_memorder[ memorder ] ) );
}

STATIC_INLINE bool
SCOREP_Atomic_CompareExchangeN_void_ptr( void** ptr, void** expected, void* desired, bool weak, SCOREP_Atomic_Memorder successMemorder, SCOREP_Atomic_Memorder failureMemorder )
{
    return __atomic_compare_exchange_n( PTR_PTR_TO_INT_PTR( ptr ), PTR_PTR_TO_INT_PTR( expected ), PTR_TO_INT( desired ), weak, gcc_memorder[ successMemorder ], gcc_memorder[ failureMemorder ] );
}

STATIC_INLINE void*
SCOREP_Atomic_AddFetch_void_ptr( void** ptr, void* val, SCOREP_Atomic_Memorder memorder )
{
    return INT_TO_PTR( __atomic_add_fetch( PTR_PTR_TO_INT_PTR( ptr ), PTR_TO_INT( val ), gcc_memorder[ memorder ] ) );
}

STATIC_INLINE void*
SCOREP_Atomic_SubFetch_void_ptr( void** ptr, void* val, SCOREP_Atomic_Memorder memorder )
{
    return INT_TO_PTR( __atomic_sub_fetch( PTR_PTR_TO_INT_PTR( ptr ), PTR_TO_INT( val ), gcc_memorder[ memorder ] ) );
}

STATIC_INLINE void*
SCOREP_Atomic_AndFetch_void_ptr( void** ptr, void* val, SCOREP_Atomic_Memorder memorder )
{
    return INT_TO_PTR( __atomic_and_fetch( PTR_PTR_TO_INT_PTR( ptr ), PTR_TO_INT( val ), gcc_memorder[ memorder ] ) );
}

STATIC_INLINE void*
SCOREP_Atomic_XorFetch_void_ptr( void** ptr, void* val, SCOREP_Atomic_Memorder memorder )
{
    return INT_TO_PTR( __atomic_xor_fetch( PTR_PTR_TO_INT_PTR( ptr ), PTR_TO_INT( val ), gcc_memorder[ memorder ] ) );
}

STATIC_INLINE void*
SCOREP_Atomic_OrFetch_void_ptr( void** ptr, void* val, SCOREP_Atomic_Memorder memorder )
{
    return INT_TO_PTR( __atomic_or_fetch( PTR_PTR_TO_INT_PTR( ptr ), PTR_TO_INT( val ), gcc_memorder[ memorder ] ) );
}

STATIC_INLINE void*
SCOREP_Atomic_NandFetch_void_ptr( void** ptr, void* val, SCOREP_Atomic_Memorder memorder )
{
    return INT_TO_PTR( __atomic_nand_fetch( PTR_PTR_TO_INT_PTR( ptr ), PTR_TO_INT( val ), gcc_memorder[ memorder ] ) );
}

STATIC_INLINE void*
SCOREP_Atomic_FetchAdd_void_ptr( void** ptr, void* val, SCOREP_Atomic_Memorder memorder )
{
    return INT_TO_PTR( __atomic_fetch_add( PTR_PTR_TO_INT_PTR( ptr ), PTR_TO_INT( val ), gcc_memorder[ memorder ] ) );
}

STATIC_INLINE void*
SCOREP_Atomic_FetchSub_void_ptr( void** ptr, void* val, SCOREP_Atomic_Memorder memorder )
{
    return INT_TO_PTR( __atomic_fetch_sub( PTR_PTR_TO_INT_PTR( ptr ), PTR_TO_INT( val ), gcc_memorder[ memorder ] ) );
}

STATIC_INLINE void*
SCOREP_Atomic_FetchAnd_void_ptr( void** ptr, void* val, SCOREP_Atomic_Memorder memorder )
{
    return INT_TO_PTR( __atomic_fetch_and( PTR_PTR_TO_INT_PTR( ptr ), PTR_TO_INT( val ), gcc_memorder[ memorder ] ) );
}

STATIC_INLINE void*
SCOREP_Atomic_FetchXor_void_ptr( void** ptr, void* val, SCOREP_Atomic_Memorder memorder )
{
    return INT_TO_PTR( __atomic_fetch_xor( PTR_PTR_TO_INT_PTR( ptr ), PTR_TO_INT( val ), gcc_memorder[ memorder ] ) );
}

STATIC_INLINE void*
SCOREP_Atomic_FetchOr_void_ptr( void** ptr, void* val, SCOREP_Atomic_Memorder memorder )
{
    return INT_TO_PTR( __atomic_fetch_or( PTR_PTR_TO_INT_PTR( ptr ), PTR_TO_INT( val ), gcc_memorder[ memorder ] ) );
}

STATIC_INLINE void*
SCOREP_Atomic_FetchNand_void_ptr( void** ptr, void* val, SCOREP_Atomic_Memorder memorder )
{
    return INT_TO_PTR( __atomic_fetch_nand( PTR_PTR_TO_INT_PTR( ptr ), PTR_TO_INT( val ), gcc_memorder[ memorder ] ) );
}

STATIC_INLINE bool
SCOREP_Atomic_LoadN_bool( bool* ptr, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_load_n( ptr, gcc_memorder[ memorder ] );
}

STATIC_INLINE void
SCOREP_Atomic_StoreN_bool( bool* ptr, bool val, SCOREP_Atomic_Memorder memorder )
{
    __atomic_store_n( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE bool
SCOREP_Atomic_ExchangeN_bool( bool* ptr, bool val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_exchange_n( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE bool
SCOREP_Atomic_CompareExchangeN_bool( bool* ptr, bool* expected, bool desired, bool weak, SCOREP_Atomic_Memorder successMemorder, SCOREP_Atomic_Memorder failureMemorder )
{
    return __atomic_compare_exchange_n( ptr, expected, desired, weak, gcc_memorder[ successMemorder ], gcc_memorder[ failureMemorder ] );
}

STATIC_INLINE char
SCOREP_Atomic_LoadN_char( char* ptr, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_load_n( ptr, gcc_memorder[ memorder ] );
}

STATIC_INLINE void
SCOREP_Atomic_StoreN_char( char* ptr, char val, SCOREP_Atomic_Memorder memorder )
{
    __atomic_store_n( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE char
SCOREP_Atomic_ExchangeN_char( char* ptr, char val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_exchange_n( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE bool
SCOREP_Atomic_CompareExchangeN_char( char* ptr, char* expected, char desired, bool weak, SCOREP_Atomic_Memorder successMemorder, SCOREP_Atomic_Memorder failureMemorder )
{
    return __atomic_compare_exchange_n( ptr, expected, desired, weak, gcc_memorder[ successMemorder ], gcc_memorder[ failureMemorder ] );
}

STATIC_INLINE char
SCOREP_Atomic_AddFetch_char( char* ptr, char val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_add_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE char
SCOREP_Atomic_SubFetch_char( char* ptr, char val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_sub_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE char
SCOREP_Atomic_AndFetch_char( char* ptr, char val, SCOREP_Atomic_Memorder memorder )
{
    return ( char )__atomic_and_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE char
SCOREP_Atomic_XorFetch_char( char* ptr, char val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_xor_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE char
SCOREP_Atomic_OrFetch_char( char* ptr, char val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_or_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE char
SCOREP_Atomic_NandFetch_char( char* ptr, char val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_nand_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE char
SCOREP_Atomic_FetchAdd_char( char* ptr, char val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_add( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE char
SCOREP_Atomic_FetchSub_char( char* ptr, char val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_sub( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE char
SCOREP_Atomic_FetchAnd_char( char* ptr, char val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_and( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE char
SCOREP_Atomic_FetchXor_char( char* ptr, char val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_xor( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE char
SCOREP_Atomic_FetchOr_char( char* ptr, char val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_or( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE char
SCOREP_Atomic_FetchNand_char( char* ptr, char val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_nand( ptr, val, gcc_memorder[ memorder ] );
}



STATIC_INLINE uint8_t
SCOREP_Atomic_LoadN_uint8( uint8_t* ptr, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_load_n( ptr, gcc_memorder[ memorder ] );
}

STATIC_INLINE void
SCOREP_Atomic_StoreN_uint8( uint8_t* ptr, uint8_t val, SCOREP_Atomic_Memorder memorder )
{
    __atomic_store_n( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint8_t
SCOREP_Atomic_ExchangeN_uint8( uint8_t* ptr, uint8_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_exchange_n( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE bool
SCOREP_Atomic_CompareExchangeN_uint8( uint8_t* ptr, uint8_t* expected, uint8_t desired, bool weak, SCOREP_Atomic_Memorder successMemorder, SCOREP_Atomic_Memorder failureMemorder )
{
    return __atomic_compare_exchange_n( ptr, expected, desired, weak, gcc_memorder[ successMemorder ], gcc_memorder[ failureMemorder ] );
}

STATIC_INLINE uint8_t
SCOREP_Atomic_AddFetch_uint8( uint8_t* ptr, uint8_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_add_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint8_t
SCOREP_Atomic_SubFetch_uint8( uint8_t* ptr, uint8_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_sub_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint8_t
SCOREP_Atomic_AndFetch_uint8( uint8_t* ptr, uint8_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_and_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint8_t
SCOREP_Atomic_XorFetch_uint8( uint8_t* ptr, uint8_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_xor_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint8_t
SCOREP_Atomic_OrFetch_uint8( uint8_t* ptr, uint8_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_or_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint8_t
SCOREP_Atomic_NandFetch_uint8( uint8_t* ptr, uint8_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_nand_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint8_t
SCOREP_Atomic_FetchAdd_uint8( uint8_t* ptr, uint8_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_add( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint8_t
SCOREP_Atomic_FetchSub_uint8( uint8_t* ptr, uint8_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_sub( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint8_t
SCOREP_Atomic_FetchAnd_uint8( uint8_t* ptr, uint8_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_and( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint8_t
SCOREP_Atomic_FetchXor_uint8( uint8_t* ptr, uint8_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_xor( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint8_t
SCOREP_Atomic_FetchOr_uint8( uint8_t* ptr, uint8_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_or( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint8_t
SCOREP_Atomic_FetchNand_uint8( uint8_t* ptr, uint8_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_nand( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint16_t
SCOREP_Atomic_LoadN_uint16( uint16_t* ptr, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_load_n( ptr, gcc_memorder[ memorder ] );
}

STATIC_INLINE void
SCOREP_Atomic_StoreN_uint16( uint16_t* ptr, uint16_t val, SCOREP_Atomic_Memorder memorder )
{
    __atomic_store_n( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint16_t
SCOREP_Atomic_ExchangeN_uint16( uint16_t* ptr, uint16_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_exchange_n( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE bool
SCOREP_Atomic_CompareExchangeN_uint16( uint16_t* ptr, uint16_t* expected, uint16_t desired, bool weak, SCOREP_Atomic_Memorder successMemorder, SCOREP_Atomic_Memorder failureMemorder )
{
    return __atomic_compare_exchange_n( ptr, expected, desired, weak, gcc_memorder[ successMemorder ], gcc_memorder[ failureMemorder ] );
}

STATIC_INLINE uint16_t
SCOREP_Atomic_AddFetch_uint16( uint16_t* ptr, uint16_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_add_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint16_t
SCOREP_Atomic_SubFetch_uint16( uint16_t* ptr, uint16_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_sub_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint16_t
SCOREP_Atomic_AndFetch_uint16( uint16_t* ptr, uint16_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_and_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint16_t
SCOREP_Atomic_XorFetch_uint16( uint16_t* ptr, uint16_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_xor_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint16_t
SCOREP_Atomic_OrFetch_uint16( uint16_t* ptr, uint16_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_or_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint16_t
SCOREP_Atomic_NandFetch_uint16( uint16_t* ptr, uint16_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_nand_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint16_t
SCOREP_Atomic_FetchAdd_uint16( uint16_t* ptr, uint16_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_add( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint16_t
SCOREP_Atomic_FetchSub_uint16( uint16_t* ptr, uint16_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_sub( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint16_t
SCOREP_Atomic_FetchAnd_uint16( uint16_t* ptr, uint16_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_and( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint16_t
SCOREP_Atomic_FetchXor_uint16( uint16_t* ptr, uint16_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_xor( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint16_t
SCOREP_Atomic_FetchOr_uint16( uint16_t* ptr, uint16_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_or( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint16_t
SCOREP_Atomic_FetchNand_uint16( uint16_t* ptr, uint16_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_nand( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint32_t
SCOREP_Atomic_LoadN_uint32( uint32_t* ptr, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_load_n( ptr, gcc_memorder[ memorder ] );
}

STATIC_INLINE void
SCOREP_Atomic_StoreN_uint32( uint32_t* ptr, uint32_t val, SCOREP_Atomic_Memorder memorder )
{
    __atomic_store_n( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint32_t
SCOREP_Atomic_ExchangeN_uint32( uint32_t* ptr, uint32_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_exchange_n( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE bool
SCOREP_Atomic_CompareExchangeN_uint32( uint32_t* ptr, uint32_t* expected, uint32_t desired, bool weak, SCOREP_Atomic_Memorder successMemorder, SCOREP_Atomic_Memorder failureMemorder )
{
    return __atomic_compare_exchange_n( ptr, expected, desired, weak, gcc_memorder[ successMemorder ], gcc_memorder[ failureMemorder ] );
}

STATIC_INLINE uint32_t
SCOREP_Atomic_AddFetch_uint32( uint32_t* ptr, uint32_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_add_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint32_t
SCOREP_Atomic_SubFetch_uint32( uint32_t* ptr, uint32_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_sub_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint32_t
SCOREP_Atomic_AndFetch_uint32( uint32_t* ptr, uint32_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_and_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint32_t
SCOREP_Atomic_XorFetch_uint32( uint32_t* ptr, uint32_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_xor_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint32_t
SCOREP_Atomic_OrFetch_uint32( uint32_t* ptr, uint32_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_or_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint32_t
SCOREP_Atomic_NandFetch_uint32( uint32_t* ptr, uint32_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_nand_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint32_t
SCOREP_Atomic_FetchAdd_uint32( uint32_t* ptr, uint32_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_add( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint32_t
SCOREP_Atomic_FetchSub_uint32( uint32_t* ptr, uint32_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_sub( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint32_t
SCOREP_Atomic_FetchAnd_uint32( uint32_t* ptr, uint32_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_and( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint32_t
SCOREP_Atomic_FetchXor_uint32( uint32_t* ptr, uint32_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_xor( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint32_t
SCOREP_Atomic_FetchOr_uint32( uint32_t* ptr, uint32_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_or( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint32_t
SCOREP_Atomic_FetchNand_uint32( uint32_t* ptr, uint32_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_nand( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint64_t
SCOREP_Atomic_LoadN_uint64( uint64_t* ptr, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_load_n( ptr, gcc_memorder[ memorder ] );
}

STATIC_INLINE void
SCOREP_Atomic_StoreN_uint64( uint64_t* ptr, uint64_t val, SCOREP_Atomic_Memorder memorder )
{
    __atomic_store_n( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint64_t
SCOREP_Atomic_ExchangeN_uint64( uint64_t* ptr, uint64_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_exchange_n( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE bool
SCOREP_Atomic_CompareExchangeN_uint64( uint64_t* ptr, uint64_t* expected, uint64_t desired, bool weak, SCOREP_Atomic_Memorder successMemorder, SCOREP_Atomic_Memorder failureMemorder )
{
    return __atomic_compare_exchange_n( ptr, expected, desired, weak, gcc_memorder[ successMemorder ], gcc_memorder[ failureMemorder ] );
}

STATIC_INLINE uint64_t
SCOREP_Atomic_AddFetch_uint64( uint64_t* ptr, uint64_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_add_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint64_t
SCOREP_Atomic_SubFetch_uint64( uint64_t* ptr, uint64_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_sub_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint64_t
SCOREP_Atomic_AndFetch_uint64( uint64_t* ptr, uint64_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_and_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint64_t
SCOREP_Atomic_XorFetch_uint64( uint64_t* ptr, uint64_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_xor_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint64_t
SCOREP_Atomic_OrFetch_uint64( uint64_t* ptr, uint64_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_or_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint64_t
SCOREP_Atomic_NandFetch_uint64( uint64_t* ptr, uint64_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_nand_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint64_t
SCOREP_Atomic_FetchAdd_uint64( uint64_t* ptr, uint64_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_add( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint64_t
SCOREP_Atomic_FetchSub_uint64( uint64_t* ptr, uint64_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_sub( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint64_t
SCOREP_Atomic_FetchAnd_uint64( uint64_t* ptr, uint64_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_and( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint64_t
SCOREP_Atomic_FetchXor_uint64( uint64_t* ptr, uint64_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_xor( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint64_t
SCOREP_Atomic_FetchOr_uint64( uint64_t* ptr, uint64_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_or( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uint64_t
SCOREP_Atomic_FetchNand_uint64( uint64_t* ptr, uint64_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_nand( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uintptr_t
SCOREP_Atomic_LoadN_uintptr( uintptr_t* ptr, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_load_n( ptr, gcc_memorder[ memorder ] );
}

STATIC_INLINE void
SCOREP_Atomic_StoreN_uintptr( uintptr_t* ptr, uintptr_t val, SCOREP_Atomic_Memorder memorder )
{
    __atomic_store_n( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uintptr_t
SCOREP_Atomic_ExchangeN_uintptr( uintptr_t* ptr, uintptr_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_exchange_n( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE bool
SCOREP_Atomic_CompareExchangeN_uintptr( uintptr_t* ptr, uintptr_t* expected, uintptr_t desired, bool weak, SCOREP_Atomic_Memorder successMemorder, SCOREP_Atomic_Memorder failureMemorder )
{
    return __atomic_compare_exchange_n( ptr, expected, desired, weak, gcc_memorder[ successMemorder ], gcc_memorder[ failureMemorder ] );
}

STATIC_INLINE uintptr_t
SCOREP_Atomic_AddFetch_uintptr( uintptr_t* ptr, uintptr_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_add_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uintptr_t
SCOREP_Atomic_SubFetch_uintptr( uintptr_t* ptr, uintptr_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_sub_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uintptr_t
SCOREP_Atomic_AndFetch_uintptr( uintptr_t* ptr, uintptr_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_and_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uintptr_t
SCOREP_Atomic_XorFetch_uintptr( uintptr_t* ptr, uintptr_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_xor_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uintptr_t
SCOREP_Atomic_OrFetch_uintptr( uintptr_t* ptr, uintptr_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_or_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uintptr_t
SCOREP_Atomic_NandFetch_uintptr( uintptr_t* ptr, uintptr_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_nand_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uintptr_t
SCOREP_Atomic_FetchAdd_uintptr( uintptr_t* ptr, uintptr_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_add( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uintptr_t
SCOREP_Atomic_FetchSub_uintptr( uintptr_t* ptr, uintptr_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_sub( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uintptr_t
SCOREP_Atomic_FetchAnd_uintptr( uintptr_t* ptr, uintptr_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_and( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uintptr_t
SCOREP_Atomic_FetchXor_uintptr( uintptr_t* ptr, uintptr_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_xor( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uintptr_t
SCOREP_Atomic_FetchOr_uintptr( uintptr_t* ptr, uintptr_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_or( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE uintptr_t
SCOREP_Atomic_FetchNand_uintptr( uintptr_t* ptr, uintptr_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_nand( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int8_t
SCOREP_Atomic_LoadN_int8( int8_t* ptr, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_load_n( ptr, gcc_memorder[ memorder ] );
}

STATIC_INLINE void
SCOREP_Atomic_StoreN_int8( int8_t* ptr, int8_t val, SCOREP_Atomic_Memorder memorder )
{
    __atomic_store_n( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int8_t
SCOREP_Atomic_ExchangeN_int8( int8_t* ptr, int8_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_exchange_n( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE bool
SCOREP_Atomic_CompareExchangeN_int8( int8_t* ptr, int8_t* expected, int8_t desired, bool weak, SCOREP_Atomic_Memorder successMemorder, SCOREP_Atomic_Memorder failureMemorder )
{
    return __atomic_compare_exchange_n( ptr, expected, desired, weak, gcc_memorder[ successMemorder ], gcc_memorder[ failureMemorder ] );
}

STATIC_INLINE int8_t
SCOREP_Atomic_AddFetch_int8( int8_t* ptr, int8_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_add_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int8_t
SCOREP_Atomic_SubFetch_int8( int8_t* ptr, int8_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_sub_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int8_t
SCOREP_Atomic_AndFetch_int8( int8_t* ptr, int8_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_and_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int8_t
SCOREP_Atomic_XorFetch_int8( int8_t* ptr, int8_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_xor_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int8_t
SCOREP_Atomic_OrFetch_int8( int8_t* ptr, int8_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_or_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int8_t
SCOREP_Atomic_NandFetch_int8( int8_t* ptr, int8_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_nand_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int8_t
SCOREP_Atomic_FetchAdd_int8( int8_t* ptr, int8_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_add( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int8_t
SCOREP_Atomic_FetchSub_int8( int8_t* ptr, int8_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_sub( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int8_t
SCOREP_Atomic_FetchAnd_int8( int8_t* ptr, int8_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_and( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int8_t
SCOREP_Atomic_FetchXor_int8( int8_t* ptr, int8_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_xor( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int8_t
SCOREP_Atomic_FetchOr_int8( int8_t* ptr, int8_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_or( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int8_t
SCOREP_Atomic_FetchNand_int8( int8_t* ptr, int8_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_nand( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int16_t
SCOREP_Atomic_LoadN_int16( int16_t* ptr, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_load_n( ptr, gcc_memorder[ memorder ] );
}

STATIC_INLINE void
SCOREP_Atomic_StoreN_int16( int16_t* ptr, int16_t val, SCOREP_Atomic_Memorder memorder )
{
    __atomic_store_n( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int16_t
SCOREP_Atomic_ExchangeN_int16( int16_t* ptr, int16_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_exchange_n( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE bool
SCOREP_Atomic_CompareExchangeN_int16( int16_t* ptr, int16_t* expected, int16_t desired, bool weak, SCOREP_Atomic_Memorder successMemorder, SCOREP_Atomic_Memorder failureMemorder )
{
    return __atomic_compare_exchange_n( ptr, expected, desired, weak, gcc_memorder[ successMemorder ], gcc_memorder[ failureMemorder ] );
}

STATIC_INLINE int16_t
SCOREP_Atomic_AddFetch_int16( int16_t* ptr, int16_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_add_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int16_t
SCOREP_Atomic_SubFetch_int16( int16_t* ptr, int16_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_sub_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int16_t
SCOREP_Atomic_AndFetch_int16( int16_t* ptr, int16_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_and_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int16_t
SCOREP_Atomic_XorFetch_int16( int16_t* ptr, int16_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_xor_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int16_t
SCOREP_Atomic_OrFetch_int16( int16_t* ptr, int16_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_or_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int16_t
SCOREP_Atomic_NandFetch_int16( int16_t* ptr, int16_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_nand_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int16_t
SCOREP_Atomic_FetchAdd_int16( int16_t* ptr, int16_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_add( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int16_t
SCOREP_Atomic_FetchSub_int16( int16_t* ptr, int16_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_sub( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int16_t
SCOREP_Atomic_FetchAnd_int16( int16_t* ptr, int16_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_and( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int16_t
SCOREP_Atomic_FetchXor_int16( int16_t* ptr, int16_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_xor( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int16_t
SCOREP_Atomic_FetchOr_int16( int16_t* ptr, int16_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_or( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int16_t
SCOREP_Atomic_FetchNand_int16( int16_t* ptr, int16_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_nand( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int32_t
SCOREP_Atomic_LoadN_int32( int32_t* ptr, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_load_n( ptr, gcc_memorder[ memorder ] );
}

STATIC_INLINE void
SCOREP_Atomic_StoreN_int32( int32_t* ptr, int32_t val, SCOREP_Atomic_Memorder memorder )
{
    __atomic_store_n( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int32_t
SCOREP_Atomic_ExchangeN_int32( int32_t* ptr, int32_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_exchange_n( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE bool
SCOREP_Atomic_CompareExchangeN_int32( int32_t* ptr, int32_t* expected, int32_t desired, bool weak, SCOREP_Atomic_Memorder successMemorder, SCOREP_Atomic_Memorder failureMemorder )
{
    return __atomic_compare_exchange_n( ptr, expected, desired, weak, gcc_memorder[ successMemorder ], gcc_memorder[ failureMemorder ] );
}

STATIC_INLINE int32_t
SCOREP_Atomic_AddFetch_int32( int32_t* ptr, int32_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_add_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int32_t
SCOREP_Atomic_SubFetch_int32( int32_t* ptr, int32_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_sub_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int32_t
SCOREP_Atomic_AndFetch_int32( int32_t* ptr, int32_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_and_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int32_t
SCOREP_Atomic_XorFetch_int32( int32_t* ptr, int32_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_xor_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int32_t
SCOREP_Atomic_OrFetch_int32( int32_t* ptr, int32_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_or_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int32_t
SCOREP_Atomic_NandFetch_int32( int32_t* ptr, int32_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_nand_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int32_t
SCOREP_Atomic_FetchAdd_int32( int32_t* ptr, int32_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_add( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int32_t
SCOREP_Atomic_FetchSub_int32( int32_t* ptr, int32_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_sub( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int32_t
SCOREP_Atomic_FetchAnd_int32( int32_t* ptr, int32_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_and( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int32_t
SCOREP_Atomic_FetchXor_int32( int32_t* ptr, int32_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_xor( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int32_t
SCOREP_Atomic_FetchOr_int32( int32_t* ptr, int32_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_or( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int32_t
SCOREP_Atomic_FetchNand_int32( int32_t* ptr, int32_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_nand( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int64_t
SCOREP_Atomic_LoadN_int64( int64_t* ptr, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_load_n( ptr, gcc_memorder[ memorder ] );
}

STATIC_INLINE void
SCOREP_Atomic_StoreN_int64( int64_t* ptr, int64_t val, SCOREP_Atomic_Memorder memorder )
{
    __atomic_store_n( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int64_t
SCOREP_Atomic_ExchangeN_int64( int64_t* ptr, int64_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_exchange_n( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE bool
SCOREP_Atomic_CompareExchangeN_int64( int64_t* ptr, int64_t* expected, int64_t desired, bool weak, SCOREP_Atomic_Memorder successMemorder, SCOREP_Atomic_Memorder failureMemorder )
{
    return __atomic_compare_exchange_n( ptr, expected, desired, weak, gcc_memorder[ successMemorder ], gcc_memorder[ failureMemorder ] );
}

STATIC_INLINE int64_t
SCOREP_Atomic_AddFetch_int64( int64_t* ptr, int64_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_add_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int64_t
SCOREP_Atomic_SubFetch_int64( int64_t* ptr, int64_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_sub_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int64_t
SCOREP_Atomic_AndFetch_int64( int64_t* ptr, int64_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_and_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int64_t
SCOREP_Atomic_XorFetch_int64( int64_t* ptr, int64_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_xor_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int64_t
SCOREP_Atomic_OrFetch_int64( int64_t* ptr, int64_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_or_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int64_t
SCOREP_Atomic_NandFetch_int64( int64_t* ptr, int64_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_nand_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int64_t
SCOREP_Atomic_FetchAdd_int64( int64_t* ptr, int64_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_add( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int64_t
SCOREP_Atomic_FetchSub_int64( int64_t* ptr, int64_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_sub( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int64_t
SCOREP_Atomic_FetchAnd_int64( int64_t* ptr, int64_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_and( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int64_t
SCOREP_Atomic_FetchXor_int64( int64_t* ptr, int64_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_xor( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int64_t
SCOREP_Atomic_FetchOr_int64( int64_t* ptr, int64_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_or( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE int64_t
SCOREP_Atomic_FetchNand_int64( int64_t* ptr, int64_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_nand( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE intptr_t
SCOREP_Atomic_LoadN_intptr( intptr_t* ptr, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_load_n( ptr, gcc_memorder[ memorder ] );
}

STATIC_INLINE void
SCOREP_Atomic_StoreN_intptr( intptr_t* ptr, intptr_t val, SCOREP_Atomic_Memorder memorder )
{
    __atomic_store_n( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE intptr_t
SCOREP_Atomic_ExchangeN_intptr( intptr_t* ptr, intptr_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_exchange_n( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE bool
SCOREP_Atomic_CompareExchangeN_intptr( intptr_t* ptr, intptr_t* expected, intptr_t desired, bool weak, SCOREP_Atomic_Memorder successMemorder, SCOREP_Atomic_Memorder failureMemorder )
{
    return __atomic_compare_exchange_n( ptr, expected, desired, weak, gcc_memorder[ successMemorder ], gcc_memorder[ failureMemorder ] );
}

STATIC_INLINE intptr_t
SCOREP_Atomic_AddFetch_intptr( intptr_t* ptr, intptr_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_add_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE intptr_t
SCOREP_Atomic_SubFetch_intptr( intptr_t* ptr, intptr_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_sub_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE intptr_t
SCOREP_Atomic_AndFetch_intptr( intptr_t* ptr, intptr_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_and_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE intptr_t
SCOREP_Atomic_XorFetch_intptr( intptr_t* ptr, intptr_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_xor_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE intptr_t
SCOREP_Atomic_OrFetch_intptr( intptr_t* ptr, intptr_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_or_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE intptr_t
SCOREP_Atomic_NandFetch_intptr( intptr_t* ptr, intptr_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_nand_fetch( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE intptr_t
SCOREP_Atomic_FetchAdd_intptr( intptr_t* ptr, intptr_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_add( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE intptr_t
SCOREP_Atomic_FetchSub_intptr( intptr_t* ptr, intptr_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_sub( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE intptr_t
SCOREP_Atomic_FetchAnd_intptr( intptr_t* ptr, intptr_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_and( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE intptr_t
SCOREP_Atomic_FetchXor_intptr( intptr_t* ptr, intptr_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_xor( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE intptr_t
SCOREP_Atomic_FetchOr_intptr( intptr_t* ptr, intptr_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_or( ptr, val, gcc_memorder[ memorder ] );
}

STATIC_INLINE intptr_t
SCOREP_Atomic_FetchNand_intptr( intptr_t* ptr, intptr_t val, SCOREP_Atomic_Memorder memorder )
{
    return __atomic_fetch_nand( ptr, val, gcc_memorder[ memorder ] );
}

#undef PTR_PTR_TO_INT_PTR
#undef PTR_TO_INT
#undef INT_TO_PTR

#if !defined( HAVE_CONFIG_H )
#undef STATIC_INLINE
#endif /* !defined( HAVE_CONFIG_H ) */
