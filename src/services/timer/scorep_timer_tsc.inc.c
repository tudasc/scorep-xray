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
 * Implement tsc timer according to cycle.h (see
 * http://www.fftw.org/cycle.h) based on HAVE_SCOREP_<TYPE>_TSC
 * defines. To be included into a function returning uint64_t.
 */


#if HAVE( SCOREP_SOLARIS_TSC )
    hrtime_t ticks = gethrtime();
    return ( uint64_t )ticks;
#endif

#if HAVE( SCOREP_POWERPC64_TSC )
    uint64_t ticks;
    asm volatile ( "mftb %0" : "=r" ( ticks ) );
    return ticks;
#endif

#if HAVE( SCOREP_POWERPC32_TSC )
    unsigned int tbl, tbu0, tbu1;
    do
    {
        __asm__ __volatile__ ( "mftbu %0" : "=r" ( tbu0 ) );
        __asm__ __volatile__ ( "mftb %0" : "=r" ( tbl ) );
        __asm__ __volatile__ ( "mftbu %0" : "=r" ( tbu1 ) );
    }
    while ( tbu0 != tbu1 );
    return ( ( ( uint64_t )tbu0 ) << 32 ) | ( uint64_t )tbl;
#endif

#if HAVE( SCOREP_X86_32_TSC )
    unsigned long long ticks;
    __asm__ __volatile__("rdtsc": "=A" (ticks));
    return ( uint64_t )ticks;
#endif

#if HAVE( SCOREP_X86_64_TSC )
    unsigned a, d;
    asm volatile ( "rdtsc" : "=a" ( a ), "=d" ( d ) );
    return ( ( uint64_t )a ) | ( ( ( uint64_t )d ) << 32 );
#endif

#if HAVE( SCOREP_X86_64_MSVC_TSC )
    return ( uint64_t )__rdtsc();
#endif

#if HAVE( SCOREP_IA64_INTEL_TSC )
    return ( uint64_t )__getReg( _IA64_REG_AR_ITC );
#endif

#if HAVE( SCOREP_IA64_GCC_TSC )
    unsigned long ticks;
    __asm__ __volatile__ ( "mov %0=ar.itc" : "=r" ( ticks ) );
    return ( uint64_t )ticks;
#endif

#if HAVE( SCOREP_IA64_HPUX_TSC )
    return ( uint64_t )_Asm_mov_from_ar( _AREG_ITC );
#endif

#if HAVE( SCOREP_IA64_MSVC_TSC )
    unsigned __int64
    __getReg( int whichReg );

    #pragma intrinsic(__getReg)
    return ( uint64_t )__getReg( 3116 );
#endif

#if HAVE( SCOREP_SPARC_TSC )
    unsigned long ticks;
    asm( "rd %tick, %l0" ); /* %10 is a 'general-purpose local register' */
    asm( "stx %%l0, %0"::"m"( ticks ) );
    return ( uint64_t )ticks;
#endif

#if HAVE( SCOREP_CRAY_TSC )
    return ( uint64_t )_rtc();
#endif

#if HAVE( SCOREP_ARMV8_TSC )
    uint64_t ticks;
    asm ( "mrs %[result], CNTVCT_EL0\n\t" :[ result ] "=r" ( ticks ) );
    return ticks;
#endif
