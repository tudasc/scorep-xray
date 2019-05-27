/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015-2016,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2019,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 * This file is based on cycle.h (see http://www.fftw.org/cycle.h)
 * which comes with following license:
 *
 *
 * Copyright (c) 2003, 2007-8 Matteo Frigo
 * Copyright (c) 2003, 2007-8 Massachusetts Institute of Technology
 *
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */


/**
 * @file
 * Extract the #ifdef tsc detection from cycle.h (see
 * http://www.fftw.org/cycle.h) and define a HAVE_SCOREP_<TYPE>_TSC;
 * include necessary header files based on detected tsc.
 */

#if HAVE_SYS_TIME_H
#include <sys/time.h>
#endif /* HAVE_SYS_TIME_H */
#include <time.h>
#include <stdint.h>

#define COMPILE_TIME_ASSERT( pred ) switch ( 0 ) { case 0: \
                                                   case pred: \
                                                       ; }


/* Solaris */
#if defined( HAVE_GETHRTIME ) && defined( HAVE_HRTIME_T ) && !defined( HAVE_TICK_COUNTER )
static void
_compile_time_assert( void )
{
    COMPILE_TIME_ASSERT( sizeof( uint64_t ) >= sizeof( hrtime_t ) );
}
#define HAVE_TICK_COUNTER
#define HAVE_SCOREP_SOLARIS_TSC 1
#endif

/* 64 bit PowerPC ``cycle'' counter using the time base register. */
#if ( defined( __powerpc64__ ) || defined( __ppc64__ ) || defined( __PPC64__ ) ) && !defined( HAVE_TICK_COUNTER )
#define HAVE_TICK_COUNTER
#define HAVE_SCOREP_POWERPC64_TSC 1
#endif

/* 32bit PowerPC ``cycle'' counter using the time base register. */
#if ( ( ( ( defined( __GNUC__ ) && ( defined( __powerpc__ ) || defined( __ppc__ ) ) ) || ( defined( __MWERKS__ ) && defined( macintosh ) ) ) ) || ( defined( __IBM_GCC_ASM ) && ( defined( __powerpc__ ) || defined( __ppc__ ) ) ) )  && !defined( HAVE_TICK_COUNTER )
#define HAVE_TICK_COUNTER
#define HAVE_SCOREP_POWERPC32_TSC 1
#endif

/* Pentium cycle counter */
/* cycle.h claims it to be unreliable, but why? We check for constant_tsc and nonstop_tsc elsewhere. */
#if ( defined( __GNUC__ ) || defined( __ICC ) ) && defined( __i386__ )  && !defined( HAVE_TICK_COUNTER )
#define HAVE_TICK_COUNTER
#define HAVE_SCOREP_X86_32_TSC 1
#endif

/* X86-64 cycle counter */
#if ( defined( __GNUC__ ) || defined( __ICC ) || defined( __SUNPRO_C ) || defined( __PGI ) ) && defined( __x86_64__ )  && !defined( HAVE_TICK_COUNTER )
#define HAVE_TICK_COUNTER
#define HAVE_SCOREP_X86_64_TSC 1
#endif

/* X86-64 cycle counter, Visual C++, courtesy of Dirk Michaelis */
#if _MSC_VER >= 1400 && ( defined( _M_AMD64 ) || defined( _M_X64 ) ) && !defined( HAVE_TICK_COUNTER )
static void
_compile_time_assert( void )
{
    COMPILE_TIME_ASSERT( sizeof( uint64_t ) >= sizeof( unsigned __int64 ) );
}
#define HAVE_TICK_COUNTER
#define HAVE_SCOREP_X86_64_MSVC_TSC 1
#endif

/* IA64 cycle counter, Intel */
#if ( defined( __EDG_VERSION ) || defined( __ECC ) ) && defined( __ia64__ ) && !defined( HAVE_TICK_COUNTER )
static void
_compile_time_assert( void )
{
    COMPILE_TIME_ASSERT( sizeof( uint64_t ) >= sizeof( unsigned long ) );
}
#define HAVE_TICK_COUNTER
#define HAVE_SCOREP_IA64_INTEL_TSC 1
#endif

/* IA64 cycle counter, GCC */
#if defined( __GNUC__ ) && defined( __ia64__ ) && !defined( HAVE_TICK_COUNTER )
static void
_compile_time_assert( void )
{
    COMPILE_TIME_ASSERT( sizeof( uint64_t ) >= sizeof( unsigned long ) );
}
#define HAVE_TICK_COUNTER
#define HAVE_SCOREP_IA64_GCC_TSC 1
#endif

/* IA64 cycle counter, HP/UX IA64 compiler, courtesy Teresa L. Johnson */
#if defined( __hpux ) && defined( __ia64 ) && !defined( HAVE_TICK_COUNTER )
static void
_compile_time_assert( void )
{
    COMPILE_TIME_ASSERT( sizeof( uint64_t ) >= sizeof( unsigned long ) );
}
#define HAVE_TICK_COUNTER
#define HAVE_SCOREP_IA64_HPUX_TSC 1
#endif

/* IA64 cycle counter, Microsoft Visual C++ */
#if defined( _MSC_VER ) && defined( _M_IA64 ) && !defined( HAVE_TICK_COUNTER )
static void
_compile_time_assert( void )
{
    COMPILE_TIME_ASSERT( sizeof( uint64_t ) >= sizeof( unsigned __int64 ) );
}
#define HAVE_TICK_COUNTER
#define HAVE_SCOREP_IA64_MSVC_TSC 1
#endif

/* SPARC cycle counter for Fujitsu systems. */
#if defined( __GNUC__ ) && defined( __sparc_v9__ ) && !defined( HAVE_TICK_COUNTER )
static void
_compile_time_assert( void )
{
    COMPILE_TIME_ASSERT( sizeof( uint64_t ) >= sizeof( unsigned long ) );
}
#define HAVE_TICK_COUNTER
#define HAVE_SCOREP_SPARC_TSC 1
#endif

/* Cray UNICOS _rtc() intrinsic function, available for Cray compiler only. */
#if defined( HAVE__RTC ) && !defined( HAVE_TICK_COUNTER )
static void
_compile_time_assert( void )
{
    COMPILE_TIME_ASSERT( sizeof( uint64_t ) >= sizeof( long long ) );
}
#define HAVE_TICK_COUNTER
#define HAVE_SCOREP_CRAY_TSC 1
#endif

#if defined( __aarch64__ ) && !defined( HAVE_TICK_COUNTER )
#define HAVE_TICK_COUNTER
#define HAVE_SCOREP_ARMV8_TSC 1
#endif


#undef COMPILE_TIME_ASSERT


#ifndef HAVE_TICK_COUNTER
#error No TSC counter detected.
#endif


/* Is a timer for frequency interpolation available? */
#if !HAVE( BACKEND_SCOREP_TIMER_CLOCK_GETTIME ) && !HAVE( BACKEND_SCOREP_TIMER_GETTIMEOFDAY )
#error No timer for frequency interpolation available.
#endif /* !HAVE( BACKEND_SCOREP_TIMER_CLOCK_GETTIME ) && !HAVE( BACKEND_SCOREP_TIMER_GETTIMEOFDAY ) */


/* Include additional headers necessary for particular tsc, if any. */
#if HAVE( SCOREP_SOLARIS_TSC )
#endif

#if HAVE( SCOREP_POWERPC64_TSC )
#endif

#if HAVE( SCOREP_POWERPC32_TSC )
#endif

#if HAVE( SCOREP_X86_32_TSC )
#endif

#if HAVE( SCOREP_X86_64_TSC )
#endif

#if HAVE( SCOREP_X86_64_MSVC_TSC )
# include <intrin.h>
#endif

#if HAVE( SCOREP_IA64_INTEL_TSC )
# include <ia64intrin.h>
#endif

#if HAVE( SCOREP_IA64_GCC_TSC )
#endif

#if HAVE( SCOREP_IA64_HPUX_TSC )
# include <machine/sys/inline.h>
#endif

#if HAVE( SCOREP_IA64_MSVC_TSC )
#endif

#if HAVE( SCOREP_SPARC_TSC )
#endif

#if HAVE( SCOREP_CRAY_TSC )
# ifdef HAVE_INTRINSICS_H
#  include <intrinsics.h>
# endif
#endif

#if HAVE( SCOREP_ARMV8_TSC )
#endif
