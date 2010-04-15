/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

#ifndef SILC_FORTRAN_WRAPPER_H
#define SILC_FORTRAN_WRAPPER_H

#include <stdint.h>
#include <config.h>

/**
     @file SILC_Fortran_Wrapper.h
    @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
    @status     ALPHA

    This file provides macros to generate decoration for function names for C-functions
    called from Fortran. Furthermore, conversion types and constantsa are defined.
 */

/**
    @def SILC_FORTRAN_XSUFFIX
    Appends '_' to the name.
    @param name The name that gets decorated.
 */
#define SILC_FORTRAN_XSUFFIX( name ) name ## _

/**
     @def SILC_FORTRAN_XSUFFIX2
    Appends '__' to the name.
    @param name The name that gets decorated.
 */
#define SILC_FORTRAN_XSUFFIX2( name ) name ## __

/**
    @def SILC_FORTRAN_UPCASE
    Appends '_U' to the name. With generator tools definies can be generated to exchange
    the name_U by all upcase letters.
    @param name The name that gets decorated.
 */
#define SILC_FORTRAN_UPCASE( name ) name ## _U

/**
     @def SILC_FORTRAN_LOWCASE
    Appends '_L' to the name. With generator tools definies can be generated to exchange
    the name_L by all lowcase letters.
    @param name The name that gets decorated.
 */
#define SILC_FORTRAN_LOWCASE( name ) name ## _L

/**
    @def SILC_FORTRAN_SUFFIX
    Appends '_' to the name. For this SILC_FORTRAN_XSUFFIX is called to ensure the
    with nested calls, the inner name is substituted first.
    @param name The name that gets decorated.
 */
#define SILC_FORTRAN_SUFFIX( name ) SILC_FORTRAN_XSUFFIX( name )

/**
     @def SILC_FORTRAN_SUFFIX2
    Appends '__' to the name.  For this SILC_FORTRAN_XSUFFIX2 is called to ensure the
    with nested calls, the inner name is substituted first.
    @param name The name that gets decorated.
 */
#define SILC_FORTRAN_SUFFIX2( name ) SILC_FORTRAN_XSUFFIX2( name )

/**
    @def SILC_FORTRAN_SUB1(name)
    Defines the first possibility to decorate C-functions called from Fortran. It puts all
    letters in upcase.
    @param name The name that gets decorated.
 */
#define SILC_FORTRAN_SUB1( name )  SILC_FORTRAN_UPCASE( name )

/**
    @def SILC_FORTRAN_SUB2(name)
    Defines the first possibility to decorate C-functions called from Fortran. It puts all
    letters in lowcase.
    @param name The name that gets decorated.
 */
#define SILC_FORTRAN_SUB2( name )  SILC_FORTRAN_LOWCASE( name )

/**
    @def SILC_FORTRAN_SUB3(name)
    Defines the first possibility to decorate C-functions called from Fortran. It puts all
    letters in lowcase and appends '_'.
    @param name The name that gets decorated.
 */
#define SILC_FORTRAN_SUB3( name )  SILC_FORTRAN_SUFFIX( SILC_FORTRAN_LOWCASE( name ) )

/**
    @def SILC_FORTRAN_SUB4(name)
    Defines the first possibility to decorate C-functions called from Fortran. It puts all
    letters in lowcase and appends '__'.
    @param name The name that gets decorated.
 */
#define SILC_FORTRAN_SUB4( name )  SILC_FORTRAN_SUFFIX2( SILC_FORTRAN_LOWCASE( name ) )

/**
    Defines the type obtained from Fortran calls for region handles.
 */
typedef int SILC_Fortran_RegionHandle;

/**
    Defines the type obtained from Fortran calls for metric handles.
 */
typedef int SILC_Fortran_MetricHandle;

/**
    Defines the type obtained from Fortran calls for metric group handles.
 */
typedef int SILC_Fortran_MetricGroup;

/**
    @def SILC_FORTRAN_INVALID_REGION
    Value of an invalid region handle from fortran calls.
 */
#define SILC_FORTRAN_INVALID_REGION -1

/**
    @def SILC_FORTRAN_INVALID_METRIC
    Value of an invalid metric handle from fortran calls.
 */
#define SILC_FORTRAN_INVALID_METRIC -1

/**
    @def SILC_FORTRAN_INVALID_GROUP
    Value of an invalid metric group handle from fortran calls.
 */
#define SILC_FORTRAN_INVALID_GROUP  -1

/**
    @def SILC_FORTRAN_INVALID_GROUP
    Value of the metric group handle for the default group from fortran calls.
 */
#define SILC_FORTRAN_DEFAULT_GROUP  -2

/**
    @def SILC_F2C_POINTER( handle ) Converts a 64 bit integer to a pointer.
         We need to store pointers (e.g. handles) in the Fortran routines. Therefor,
         the pointers need to be casted to an 64 bit integer.
    @param handle A 64 bit integer, that is casted to a pointer.
 */
/**
    @def SILC_C2F_POINTER( handle ) Converts a pointer to a 64 bit integer.
         We need to store pointers (e.g. handles) in the Fortran routines. Therefor,
         the pointers need to be casted to an 64 bit integer.
    @param handle A C pointer  that is casted to a 64 bit unsigned integer.
 */
# if SIZEOF_VOID_P == 8
#define SILC_F2C_POINTER( handle )  ( ( void* )( uint64_t )handle )
#define SILC_C2F_POINTER( handle )  ( ( uint64_t )handle )
#elif SIZEOF_VOID_P == 4
#define SILC_F2C_POINTER( handle )  ( ( void* )( uint32_t )handle )
#define SILC_C2F_POINTER( handle )  ( ( uint32_t )handle )
#else
#error Unsupported architecture. Only 32 bit and 64 bit architectures are supported.
#endif

/**
   @def SILC_F2C_REGION( handle )
   Converts a Fortran region handle to a C region handle.
   @param handle A Fortran region handle.
 */
#define SILC_F2C_REGION( handle ) ( ( SILC_RegionHandle )SILC_F2C_POINTER( handle ) )

/**
   @def SILC_C2F_REGION( handle )
   Converts a C region handle to a Fortran region handle.
   @param handle A C region handle.
 */
#define SILC_C2F_REGION( handle ) SILC_C2F_POINTER( handle )

/**
   @def SILC_F2C_COUNTER( handle )
   Converts a Fortran counter handle to a C counter handle.
   @param handle A Fortran counter handle.
 */
#define SILC_F2C_COUNTER( handle ) ( ( SILC_CounterHandle )SILC_F2C_POINTER( handle ) )

/**
   @def SILC_C2F_COUNTER( handle )
   Converts a C counter handle to a Fortran counter handle.
   @param handle A C counter handle.
 */
#define SILC_C2F_COUNTER( handle ) SILC_C2F_POINTER( handle )

/**
   @def SILC_F2C_COUNTER_GROUP( handle )
   Converts a Fortran counter group handle to a C counter group handle.
   @param handle A Fortran counter group handle.
 */
#define SILC_F2C_COUNTER_GROUP( handle ) ( ( SILC_CounterGroupHandle )SILC_F2C_POINTER( handle ) )

/**
   @def SILC_C2F_COUNTER_GROUP( handle )
   Converts a C counter group handle to a Fortran counter group handle.
   @param handle A C counter group handle.
 */
#define SILC_C2F_COUNTER_GROUP( handle ) SILC_C2F_POINTER( handle )



#endif /* SILC_FORTRAN_WRAPPER_H */
