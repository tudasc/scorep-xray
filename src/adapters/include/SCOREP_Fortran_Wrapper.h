/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen University, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

#ifndef SCOREP_FORTRAN_WRAPPER_H
#define SCOREP_FORTRAN_WRAPPER_H

#include <stdint.h>

/**
     @file SCOREP_Fortran_Wrapper.h
    @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
    @status     ALPHA

    This file provides macros to generate decoration for function names for C-functions
    called from Fortran. Furthermore, conversion types and constantsa are defined.
 */

/**
    @def SCOREP_FORTRAN_XSUFFIX
    Appends '_' to the name.
    @param name The name that gets decorated.
 */
#define SCOREP_FORTRAN_XSUFFIX( name ) name ## _

/**
     @def SCOREP_FORTRAN_XSUFFIX2
    Appends '__' to the name.
    @param name The name that gets decorated.
 */
#define SCOREP_FORTRAN_XSUFFIX2( name ) name ## __

/**
    @def SCOREP_FORTRAN_UPCASE
    Appends '_U' to the name. With generator tools definies can be generated to exchange
    the name_U by all upcase letters.
    @param name The name that gets decorated.
 */
#define SCOREP_FORTRAN_UPCASE( name ) name ## _U

/**
     @def SCOREP_FORTRAN_LOWCASE
    Appends '_L' to the name. With generator tools definies can be generated to exchange
    the name_L by all lowcase letters.
    @param name The name that gets decorated.
 */
#define SCOREP_FORTRAN_LOWCASE( name ) name ## _L

/**
    @def SCOREP_FORTRAN_SUFFIX
    Appends '_' to the name. For this SCOREP_FORTRAN_XSUFFIX is called to ensure the
    with nested calls, the inner name is substituted first.
    @param name The name that gets decorated.
 */
#define SCOREP_FORTRAN_SUFFIX( name ) SCOREP_FORTRAN_XSUFFIX( name )

/**
     @def SCOREP_FORTRAN_SUFFIX2
    Appends '__' to the name.  For this SCOREP_FORTRAN_XSUFFIX2 is called to ensure the
    with nested calls, the inner name is substituted first.
    @param name The name that gets decorated.
 */
#define SCOREP_FORTRAN_SUFFIX2( name ) SCOREP_FORTRAN_XSUFFIX2( name )

/**
    @def SCOREP_FORTRAN_SUB1(name)
    Defines the first possibility to decorate C-functions called from Fortran. It puts all
    letters in upcase.
    @param name The name that gets decorated.
 */
#define SCOREP_FORTRAN_SUB1( name )  SCOREP_FORTRAN_UPCASE( name )

/**
    @def SCOREP_FORTRAN_SUB2(name)
    Defines the first possibility to decorate C-functions called from Fortran. It puts all
    letters in lowcase.
    @param name The name that gets decorated.
 */
#define SCOREP_FORTRAN_SUB2( name )  SCOREP_FORTRAN_LOWCASE( name )

/**
    @def SCOREP_FORTRAN_SUB3(name)
    Defines the first possibility to decorate C-functions called from Fortran. It puts all
    letters in lowcase and appends '_'.
    @param name The name that gets decorated.
 */
#define SCOREP_FORTRAN_SUB3( name )  SCOREP_FORTRAN_SUFFIX( SCOREP_FORTRAN_LOWCASE( name ) )

/**
    @def SCOREP_FORTRAN_SUB4(name)
    Defines the first possibility to decorate C-functions called from Fortran. It puts all
    letters in lowcase and appends '__'.
    @param name The name that gets decorated.
 */
#define SCOREP_FORTRAN_SUB4( name )  SCOREP_FORTRAN_SUFFIX2( SCOREP_FORTRAN_LOWCASE( name ) )

/**
    Defines the type obtained from Fortran calls for region handles.
 */
typedef int64_t SCOREP_Fortran_RegionHandle;

/**
    Defines the type obtained from Fortran calls for metric handles.
 */
typedef int64_t SCOREP_Fortran_MetricHandle;

/**
    Defines the type obtained from Fortran calls for metric group handles.
 */
typedef int64_t SCOREP_Fortran_MetricGroup;

/**
    @def SCOREP_FORTRAN_INVALID_REGION
    Value of an invalid region handle from fortran calls.
 */
#define SCOREP_FORTRAN_INVALID_REGION -1

/**
    @def SCOREP_FORTRAN_INVALID_METRIC
    Value of an invalid metric handle from fortran calls.
 */
#define SCOREP_FORTRAN_INVALID_METRIC -1

/**
    @def SCOREP_FORTRAN_INVALID_GROUP
    Value of an invalid metric group handle from fortran calls.
 */
#define SCOREP_FORTRAN_INVALID_GROUP  -1

/**
    @def SCOREP_FORTRAN_INVALID_GROUP
    Value of the metric group handle for the default group from fortran calls.
 */
#define SCOREP_FORTRAN_DEFAULT_GROUP  -2

/**
    @def SCOREP_F2C_POINTER( handle ) Converts a 64 bit integer to a pointer.
         We need to store pointers (e.g. handles) in the Fortran routines. Therefor,
         the pointers need to be casted to an 64 bit integer.
    @param handle A 64 bit integer, that is casted to a pointer.
 */
/**
    @def SCOREP_C2F_POINTER( handle ) Converts a pointer to a 64 bit integer.
         We need to store pointers (e.g. handles) in the Fortran routines. Therefor,
         the pointers need to be casted to an 64 bit integer.
    @param handle A C pointer  that is casted to a 64 bit unsigned integer.
 */
# if SIZEOF_VOID_P == 8
#define SCOREP_F2C_POINTER( handle )  ( ( void* )( uint64_t )handle )
#define SCOREP_C2F_POINTER( handle )  ( ( uint64_t )handle )
#elif SIZEOF_VOID_P == 4
#define SCOREP_F2C_POINTER( handle )  ( ( void* )( uint32_t )handle )
#define SCOREP_C2F_POINTER( handle )  ( ( uint32_t )handle )
#else
#error Unsupported architecture. Only 32 bit and 64 bit architectures are supported.
#endif

/**
   @def SCOREP_F2C_REGION( handle )
   Converts a Fortran region handle to a C region handle.
   @param handle A Fortran region handle.
 */
#define SCOREP_F2C_REGION( handle ) ( handle )

/**
   @def SCOREP_C2F_REGION( handle )
   Converts a C region handle to a Fortran region handle.
   @param handle A C region handle.
 */
#define SCOREP_C2F_REGION( handle ) ( handle )

/**
   @def SCOREP_F2C_COUNTER( handle )
   Converts a Fortran counter handle to a C counter handle.
   @param handle A Fortran counter handle.
 */
#define SCOREP_F2C_COUNTER( handle ) ( handle )

/**
   @def SCOREP_C2F_COUNTER( handle )
   Converts a C counter handle to a Fortran counter handle.
   @param handle A C counter handle.
 */
#define SCOREP_C2F_COUNTER( handle ) ( handle )

/**
   @def SCOREP_F2C_COUNTER_GROUP( handle )
   Converts a Fortran counter group handle to a C counter group handle.
   @param handle A Fortran counter group handle.
 */
#define SCOREP_F2C_COUNTER_GROUP( handle ) ( handle )

/**
   @def SCOREP_C2F_COUNTER_GROUP( handle )
   Converts a C counter group handle to a Fortran counter group handle.
   @param handle A C counter group handle.
 */
#define SCOREP_C2F_COUNTER_GROUP( handle ) ( handle )



#endif /* SCOREP_FORTRAN_WRAPPER_H */
