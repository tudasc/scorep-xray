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

#ifndef SCOREP_USER_H
#define SCOREP_USER_H

/** @file       SCOREP_User.h
    @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
    @status     ALPHA
    @ingroup    SCOREP_User_Interface

    @brief contains the interface definitions for the manual user instrumentation.
 */

/* Guarded because it declares variables in every file where it is included. */
#ifdef SCOREP_USER_ENABLE
#include "SCOREP_User_Variables.h"
#include "SCOREP_User_Functions.h"
#include "SCOREP_OA_Functions.h"
#endif

/** @defgroup SCOREP_User SCOREP User Adapter

    The user adapter provides a set of macros for user manual instrumentation. The macros
    are inserted in the users source code and call functions of the SCOREP runtime system.
    The user should avoid calling the adapter functions directly.

    The user adapter is initialization functions struct is
    @ref SCOREP_User_Adapter. It is initialized from the measurement system. If a user
    instrumentation is called before the adapter is initialized, it will initialized the
    measurement system which call the initialization functions of the adapters.

    @defgroup SCOREP_User_Interface Macros for manual user instrumentation
    @ingroup SCOREP_User
    For every macro two definitions are provided: The first one inserts calls to the
    adapters, the second definitions resolve to nothing. Which implementation is used,
    depends on the definition of SCOREP_USER_ENABLE. If SCOREP_USER_ENABLE is defined, the
    macros insert calls to the adapter functions. If SCOREP_USER_ENABLE is undefined,
    the user instrumentation is removed by the preprocessor. This flag SCOREP_USER_ENABLE
    should be set through the instrumentation wrapper tool.

    Every source file which is instrumented must include a header file with the
    SCOREP User instrumentation header. For C/C++ programs this is the header file
    'SCOREP_User.h', for Fortran files, 'SCOREP_User.inc' must be included. Because the
    Fortran compilers can not expand the macros, the Fortran source code must be
    preprocessed by a C or C++ preprocessor, to include the headers and expand the
    macros. Which Fortran files are passed to the preprocessor depends on the suffix.
    Usually, suffixes .f and .f90 are not preprocessed, .F and .F90 files are
    preprocessed. However, this may depend on the used compiler.

   @{
 */

/* **************************************************************************************
 * Documentation for region enclosing macros
 * *************************************************************************************/

/** @def SCOREP_OA_REGION_DEFINE(handle)
    This macro defines a region. Every region has to be defined before it is first
    entered. The defines have to take place at the beginning of the enclosing block.
    @param handle  A variable name for a parameter must be provided. This variable is
                   declared in this macro. This handle is used in the
                   SCOREP_USER_REGION_BEGIN and SCOREP_USER_REGION_END statements to specify
                   which region is started, or ended.

    Example:
    @code
    void myfunc()
    {
      SCOREP_USER_REGION_DEFINE( my_region_handle )

      // do something

      SCOREP_USER_REGION_BEGIN( my_region_handle, "my_region",SCOREP_USER_REGION_TYPE_COMMON )

      // do something

      SCOREP_USER_REGION_END( my_region_handle )
    }
    @endcode
 */

/** @def SCOREP_USER_OA_PHASE_BEGIN(handle, name, type)
    This macro marks the start of a user defined Online Access phase region. The SCOREP_USER_OA_PHASE_BEGIN and
    SCOREP_USER_OA_PHASE_END must be correctly nested and be a potential global synchronization points, also it is
    recommended to mark the body of the application's main loop as a Online Access phase in order to utilize main loop
    iterations for iterative online analisys.
    @param handle  The handle of the associated user region, which will become a root of the call-tree.
                                   This handle must be declared using SCOREP_USER_REGION_DEFINE or SCOREP_GLOBAL_REGION_DEFINE before.
    @param name    A string containing the name of the new region. The name should be
                   unique.
    @param type    Specifies the type of the region. Possible values are
                   SCOREP_USER_REGION_TYPE_COMMON,
                   SCOREP_USER_REGION_TYPE_FUNCTION, SCOREP_USER_REGION_TYPE_LOOP,
                   SCOREP_USER_REGION_TYPE_DYNAMIC, SCOREP_USER_REGION_TYPE_PHASE, or a
                   combination of them.

    C/C++ example:
    @code
    void myfunc()
    {
      SCOREP_USER_REGION_DEFINE( my_region_handle )

      // application initialization

      for ( ) // main loop of the application
          {
                  SCOREP_USER_OA_PHASE_BEGIN( my_region_handle, "main loop",SCOREP_USER_REGION_TYPE_COMMON )

                  // do something

                  SCOREP_USER_OA_PHASE_END( my_region_handle )
          }

          // application finalization
    }
    @endcode

    Fortran example:
    @code
    program myProg
      SCOREP_USER_REGION_DEFINE( my_region_handle )
      ! more declarations

      SCOREP_USER_REGION_BEGIN( my_region_handle, "my_region",SCOREP_USER_REGION_TYPE_COMMON )
      ! do something
      SCOREP_USER_REGION_END( my_region_handle )

    end program myProg
    @endcode
 */

/** @def SCOREP_USER_REGION_BEGIN(handle, name, type)
    This macro marks the start of a user defined region. The SCOREP_USER_REGION_BEGIN and
    SCOREP_USER_REGION_END calls of all regions must be correctly nested.
    @param handle  The handle of the region to be started. This handle must be declared
                   using SCOREP_USER_REGION_DEFINE or SCOREP_GLOBAL_REGION_DEFINE before.
    @param name    A string containing the name of the new region. The name should be
                   unique.
    @param type    Specifies the type of the region. Possible values are
                   SCOREP_USER_REGION_TYPE_COMMON,
                   SCOREP_USER_REGION_TYPE_FUNCTION, SCOREP_USER_REGION_TYPE_LOOP,
                   SCOREP_USER_REGION_TYPE_DYNAMIC, SCOREP_USER_REGION_TYPE_PHASE, or a
                   combination of them.

    C/C++ example:
    @code
    void myfunc()
    {
      SCOREP_USER_REGION_DEFINE( my_region_handle )

      // do something

      SCOREP_USER_REGION_BEGIN( my_region_handle, "my_region",SCOREP_USER_REGION_TYPE_COMMON )

      // do something

      SCOREP_USER_REGION_END( my_region_handle )
    }
    @endcode

    Fortran example:
    @code
    program myProg
      SCOREP_USER_REGION_DEFINE( my_region_handle )
      ! more declarations

      SCOREP_USER_REGION_BEGIN( my_region_handle, "my_region",SCOREP_USER_REGION_TYPE_COMMON )
      ! do something
      SCOREP_USER_REGION_END( my_region_handle )

    end program myProg
    @endcode
 */

/** @def SCOREP_USER_REGION_INIT(handle, name, type)
    This macro initializes a user defined region. If the region handle is already
    inititialized, no operation is executed.
    @param handle  The handle of the region to be started. This handle must be declared
                   using SCOREP_USER_REGION_DEFINE or SCOREP_GLOBAL_REGION_DEFINE before.
    @param name    A string containing the name of the new region. The name should be
                   unique.
    @param type    Specifies the type of the region. Possible values are
                   SCOREP_USER_REGION_TYPE_COMMON,
                   SCOREP_USER_REGION_TYPE_FUNCTION, SCOREP_USER_REGION_TYPE_LOOP,
                   SCOREP_USER_REGION_TYPE_DYNAMIC, SCOREP_USER_REGION_TYPE_PHASE, or a
                   combination of them.

    C/C++ example:
    @code
    void myfunc()
    {
      SCOREP_USER_REGION_DEFINE( my_region_handle )

      // do something

      SCOREP_USER_REGION_INIT( my_region_handle, "my_region",SCOREP_USER_REGION_TYPE_COMMON )
      SCOREP_USER_REGION_ENTER( my_region_handle )

      // do something

      SCOREP_USER_REGION_END( my_region_handle )
    }
    @endcode

    Fortran example:
    @code
    program myProg
      SCOREP_USER_REGION_DEFINE( my_region_handle )
      ! more declarations

      SCOREP_USER_REGION_INIT( my_region_handle, "my_region",SCOREP_USER_REGION_TYPE_COMMON )
      SCOREP_USER_REGION_ENTER( my_region_handle )
      ! do something
      SCOREP_USER_REGION_END( my_region_handle )

    end program myProg
    @endcode
 */

/** @def SCOREP_USER_REGION_END(handle)
    This macro marks the end of a user defined region.  The SCOREP_USER_REGION_BEGIN and
    SCOREP_USER_REGION_END calls of all regions must be correctly nested.
    @param name  The handle of the region which ended here.
                 It must be the same handle which is used as the start of the region.

    C/C++ example:
    @code
    void myfunc()
    {
      SCOREP_USER_REGION_DEFINE( my_region_handle )

      // do something

      SCOREP_USER_REGION_BEGIN( my_region_handle, "my_region",SCOREP_USER_REGION_TYPE_COMMON )

      // do something

      SCOREP_USER_REGION_END( my_region_handle )
    }
    @endcode

    Fortran example:
    @code
    program myProg
      SCOREP_USER_REGION_DEFINE( my_region_handle )
      ! more declarations

      SCOREP_USER_REGION_BEGIN( my_region_handle, "my_region",SCOREP_USER_REGION_TYPE_COMMON )
      ! do something
      SCOREP_USER_REGION_END( my_region_handle )

    end program myProg
    @endcode
 */

/** @def SCOREP_USER_REGION_ENTER(handle)
    This macro marks the beginning of a user defined and already initialized region.
    The SCOREP_USER_REGION_BEGIN/SCOREP_USER_REGION_ENTER and
    SCOREP_USER_REGION_END calls of all regions must be correctly nested. To initialize
    the region handle, @ref SCOREP_USER_REGION_INIT or @ref SCOREP_USER_REGION_BEGIN must
    be called before.
    @param name  The handle of the region which ended here.
                 It must be the same handle which is used as the start of the region.

    C/C++ example:
    @code
    void myfunc()
    {
      SCOREP_USER_REGION_DEFINE( my_region_handle )

      // do something

      SCOREP_USER_REGION_INIT( my_region_handle, "my_region",SCOREP_USER_REGION_TYPE_COMMON )
      SCOREP_USER_REGION_ENTER( my_region_handle )

      // do something

      SCOREP_USER_REGION_END( my_region_handle )
    }
    @endcode

    Fortran example:
    @code
    program myProg
      SCOREP_USER_REGION_DEFINE( my_region_handle )
      ! more declarations

      SCOREP_USER_REGION_INIT( my_region_handle, "my_region",SCOREP_USER_REGION_TYPE_COMMON )
      SCOREP_USER_REGION_ENTER( my_region_handle )
      ! do something
      SCOREP_USER_REGION_END( my_region_handle )

    end program myProg
    @endcode
 */

/** @def SCOREP_USER_FUNC_DEFINE
    This macro is for Fortran only. It declares the handle for a function. Every function
    handle must be declared in the declaration part of the subroutine or function if
    the SCOREP_USER_FUNC_BEGIN and SCOREP_USER_FUNC_END macros are used.

   Example:
    @code
    subroutine myfunc
      SCOREP_USER_FUNC_DEFINE
      ! more declarations

      SCOREP_USER_FUNC_BEGIN( "myfunc" )
      ! do something
      SCOREP_USER_FUNC_END

    end subroutine myfunc
    @endcode
    Note that in Fortran the function need to be declared using SCOREP_USER_FUNC_DEFINE
    before.
 */

/** @def SCOREP_USER_FUNC_BEGIN
    This macro marks the start of a function. It should be inserted at the beginning
    of the instrumented function. It will generate a region, with the function
    name.

    The C/C++ version of this command takes no arguments. In Fortran one argument is
    required for the name of the function. Furthermore, the handle must be declared
    explicitly in Fortran.

    @param name Fortan only: A string containing the name of the function.

    C/C++ example:
    @code
    void myfunc()
    {
      // declarations

      SCOREP_USER_FUNC_BEGIN

      // do something

      SCOREP_USER_FUNC_END
    }
    @endcode

    Fortran example:
    @code
    subroutine myfunc
      SCOREP_USER_FUNC_DEFINE
      ! more declarations

      SCOREP_USER_FUNC_BEGIN( "myfunc" )
      ! do something
      SCOREP_USER_FUNC_END

    end subroutine myfunc
    @endcode
    Note that in Fortran the function need to be declared using SCOREP_USER_FUNC_DEFINE
    before.
 */

/** @def SCOREP_USER_FUNC_END
    This macro marks the end of a function. It should be inserted at
    every return point of the instrumented function.

    C/C++ example:
    @code
    void myfunc()
    {
      // declarations

      SCOREP_USER_FUNC_BEGIN

      // do something
      if ( some_expression )
      {
        SCOREP_USER_FUNC_END
        return;
      }

      SCOREP_USER_FUNC_END
    }
    @endcode

    Fortran example:
    @code
    subroutine myfunc
      SCOREP_USER_FUNC_DEFINE
      ! more declarations

      SCOREP_USER_FUNC_BEGIN( "myfunc" )
      ! do something
      SCOREP_USER_FUNC_END

    end subroutine myfunc
    @endcode
    Note that in Fortran the function need to be declared using SCOREP_USER_FUNC_DEFINE
    before.
 */

/** @def SCOREP_GLOBAL_REGION_DEFINE( handle )
    This macro declares a region handle in a global scope for usage in more than one code
    block in more than one source file. Every global region must only declared once using
    this macro. All other files in which this region handle is accessed must declare the
    region handle with SCOREP_GLOBAL_REGION_EXTERNAL( handle ). A global region has no
    special associated source region with it. Enter and exit events for global regions
    are created with SCOREP_USER_REGION_BEGIN and SCOREP_USER_REGION_END, respectively.
    Its name and type is determined at the first enter event and is not changed on later
    events, even if other code blocks conatain a different name or type in their
    SCOREP_USER_REGION_BEGIN statement.

    This macro is not available in Fortran.

    @param handle  A name for a parameter must be provided. This parameter is declared in
                   the macro. This handle is used in the SCOREP_USER_REGION_BEGIN and
                   SCOREP_USER_REGION_END statements to specify which region is started,
                   or ended.
                   If you are using a Fortran version which has a limited length of code
                   lines, the length of the @a handle parameter must be at most 4
                   characters, else the declaration line exceeds the allowed length.

    C/C++ example:
    @code
    // In File1:
    SCOREP_GLOBAL_REGION_DEFINE( my_global_handle )

    void myfunc()
    {
      SCOREP_USER_REGION_BEGIN( my_global_handle, "my_global", SCOREP_USER_REGION_PHASE )

      // do something

      SCOREP_USER_REGION_END( my_global_handle )
    }
    @endcode
    @code
    // In File2:
    SCOREP_GLOBAL_EXTERNAL( my_global_handle )

    void foo()
    {
      SCOREP_USER_REGION_BEGIN( my_global_handle, "my_global", SCOREP_USER_REGION_PHASE )

      // do something

      SCOREP_USER_REGION_END( my_global_handle )
    }
    @endcode

   /** @def SCOREP_GLOBAL_REGION_EXTERNAL( handle )
    This macro declares an axternally defined global region.  Every global region must
    only declared once usingSCOREP_GLOBAL_REGION_DEFINE( handle ). All other files in
    which this region handle is accessed must declare the region handle with
    SCOREP_GLOBAL_REGION_EXTERNAL( handle ).  A global region has no
    special associated source region with it. Enter and exit events for global regions
    are created with SCOREP_USER_REGION_BEGIN and SCOREP_USER_REGION_END, respectively.
    Its name and type is determined at the first enter event and is not changed on later
    events, even if other code blocks conatain a different name or type in their
    SCOREP_USER_REGION_BEGIN statement.

    This macro is not available in Fortran

    @param handle  A name for a variable must be provided. This variable name must be
                   the same like for the corresponding SCOREP_GLOBAL_REGION_DEFINE
                   statement. The handle is used in the SCOREP_USER_REGION_BEGIN and
                   SCOREP_USER_REGION_END statements to specify which region is started,
                   or ended.

    C/C++ example:
    @code
    // In File 1
    SCOREP_GLOBAL_REGION_DEFINE( my_global_handle )

    void myfunc()
    {
      SCOREP_USER_REGION_BEGIN( my_global_handle, "my_global", SCOREP_USER_REGION_PHASE )

      // do something

      SCOREP_USER_REGION_END( my_global_handle )
    }
    @endcode
    @code
    // In File 2
    SCOREP_GLOBAL_EXTERNAL( my_global_handle )

    void foo()
    {
      SCOREP_USER_REGION_BEGIN( my_global_handle, "my_global", SCOREP_USER_REGION_PHASE )

      // do something

      SCOREP_USER_REGION_END( my_global_handle )
    }
    @endcode
 */

/* **************************************************************************************
 * Region enclosing macros
 * *************************************************************************************/
/* Empty define for SCOREP_USER_FUNC_DEFINE to allow documentation of the macro and
   let it disappear in C/C++ codes */
#define SCOREP_USER_FUNC_DEFINE

#ifdef SCOREP_USER_ENABLE

#define SCOREP_USER_OA_PHASE_BEGIN( handle ) SCOREP_OA_PhaseBegin( &handle );

#define SCOREP_USER_OA_PHASE_END( handle ) SCOREP_OA_PhaseEnd( &handle );

#define SCOREP_USER_REGION_DEFINE( handle ) \
    static SCOREP_User_RegionHandle handle = SCOREP_INVALID_USER_REGION;

#define SCOREP_USER_REGION_ENTER( handle ) SCOREP_User_RegionEnter( &handle );

#define SCOREP_USER_REGION_BEGIN( handle, name, type ) SCOREP_User_RegionBegin( \
        &handle, &SCOREP_User_LastFileName, &SCOREP_User_LastFileHandle, name, \
        type, __FILE__, __LINE__ );

#define SCOREP_USER_REGION_INIT( handle, name, type ) SCOREP_User_RegionInit( \
        &handle, &SCOREP_User_LastFileName, &SCOREP_User_LastFileHandle, name, \
        type, __FILE__, __LINE__ );

#define SCOREP_USER_REGION_END( handle ) SCOREP_User_RegionEnd( handle );

#define SCOREP_USER_FUNC_BEGIN static SCOREP_User_RegionHandle \
    scorep_user_func_handle =  SCOREP_INVALID_USER_REGION; \
    SCOREP_User_RegionBegin( &scorep_user_func_handle, &SCOREP_User_LastFileName, \
                             &SCOREP_User_LastFileHandle, __func__,      \
                             SCOREP_USER_REGION_TYPE_FUNCTION, __FILE__, __LINE__ );

#define SCOREP_USER_FUNC_END SCOREP_User_RegionEnd( scorep_user_func_handle );

#define SCOREP_GLOBAL_REGION_DEFINE( handle ) \
    SCOREP_User_RegionHandle handle = SCOREP_INVALID_USER_REGION;

#define SCOREP_GLOBAL_REGION_EXTERNAL( handle ) \
    extern SCOREP_User_RegionHandle handle;

#endif // SCOREP_USER_ENABLE

/* **************************************************************************************
 * Documentation for Parameter macros
 * *************************************************************************************/
/** @def SCOREP_USER_PARAMETER_INT64(name,value)
    This statement adds a 64 bit signed integer type parameter for parameter-based
    profiling to the current region.
    The calltree for the region is split according to the different values of the
    parameters with the same name.
    It is possible to add an arbitrary number of parameters to a region. Each parameter
    must have a unique name. However, it is not recommended to use more than 1 parameter
    per region.
    During one visit it is not allowed to use the same name twice
    for two different parameters.
    @param name   A string containing the name of the parameter.
    @param value  The value of the parameter. It must be possible for implicit casts to
                  cast it to a 64 bit integer.

    C/C++ example:
    @code
    void myfunc(int64 myint)
    {
      SCOREP_USER_REGION_DEFINE( my_region_handle )
      SCOREP_USER_REGION_BEGIN( my_region_handle, "my_region",SCOREP_USER_REGION_TYPE_COMMON )
      SCOREP_USER_PARAMETER_INT64("A nice int",myint)

      // do something

      SCOREP_USER_REGION_END( my_region_handle )
    }
    @endcode
 */

/** @def SCOREP_USER_PARAMETER_STRING(name,value)
    This statement adds a string type parameter for parameter-based
    profiling to the current region.
    The calltree for the region is split according to the different values of the
    parameters with the same name.
    It is possible to add an arbitrary number of parameters to a region. Each parameter
    must have a unique name. However, it is not recommended to use more than 1 parameter
    per region.
    During one visit it is not allowed to use the same name twice
    for two different parameters.
    @param name   A string containing the name of the parameter.
    @param value  The value of the parameter. It must be a pointer to a C-string (a
                  NULL-terminated string).

    Example:
    @code
    void myfunc(char *mystring)
    {
      SCOREP_USER_REGION_DEFINE( my_region_handle )
      SCOREP_USER_REGION_BEGIN( my_region_handle, "my_region",SCOREP_USER_REGION_TYPE_COMMON )
      SCOREP_USER_PARAMETER_STRING("A nice string",mystring)

      // do something

      SCOREP_USER_REGION_END( my_region_handle )
    }
    @endcode
 */

/* **************************************************************************************
 * Parameter macros
 * *************************************************************************************/
#ifdef SCOREP_USER_ENABLE

#define SCOREP_USER_PARAMETER_INT64( name, value ) { \
        static SCOREP_User_ParameterHandle scorep_param = SCOREP_USER_INVALID_PARAMETER; \
        SCOREP_User_ParameterInt64( &scorep_param, name, value ); }

#define SCOREP_USER_PARAMETER_STRING( name, value ) { \
        static SCOREP_User_ParameterHandle scorep_param = SCOREP_USER_INVALID_PARAMETER; \
        SCOREP_User_ParameterString( &scorep_param, name, value ); }

#endif // SCOREP_USER_ENABLE

/* **************************************************************************************
 * Documentation for User Counter macros
 * *************************************************************************************/

/** @def  SCOREP_USER_METRIC_GROUP_LOCAL(groupHandle)
    Declares a metric group.
    It defines a variable which must be in scope at all places where
    the metric group is used. If it is used in more than one place it need to be a global
    definition. Its scope can never be beyond one source file. If a larger scope is
    needed use SCOREP_USER_METRIC_GROUP_GLOBAL.
    @param groupHandle The name of the variable which contains the groupHandle.

    C/C++ example:
    @code
    SCOREP_USER_METRIC_GROUP_LOCAL( my_local_group )
    SCOREP_USER_METRIC_LOCAL( my_local_metric )

    int main()
    {
      SCOREP_USER_METRIC_GROUP_INIT( my_local_group )
      SCOREP_USER_METRIC_INIT( my_local_metric, "My Metric", "seconds", \
                             SCOREP_USER_METRIC_TYPE_UINT64, \
                             SCOREP_USER_METRIC_CONTEXT_GLOBAL, my_local_group )
      // do something
    }

    void foo()
    {
      uint64 my_int = get_some_int_value();
      SCOREP_USER_METRIC_UINT64( my_local_metric, my_int )
    }
    @endcode

    Fortran example:
    @code
    program myProg
      SCOREP_USER_METRIC_GROUP_LOCAL( my_local_group )
      SCOREP_USER_METRIC_LOCAL( my_local_metric )
      integer (kind=selected_int_kind(8)):: my_int = 19
    ! more declarations


      SCOREP_USER_METRIC_GROUP_INIT( my_local_group )
      SCOREP_USER_METRIC_INIT( my_local_metric, "My Metric", "seconds", &
                             SCOREP_USER_METRIC_TYPE_INT64, &
                             SCOREP_USER_METRIC_CONTEXT_GLOBAL, my_local_group )

    ! do something

      SCOREP_USER_METRIC_INT64( my_local_metric, my_int )
    end program myProg
    @endcode
 */

/** @def SCOREP_USER_METRIC_GROUP_GLOBAL( groupHandle)
    Declares a group handle for user metrics for usage in more than one file. Every global
    group handle must only be declared in one file using SCOREP_USER_METRIC_GROUP_GLOBAL.
    All other files in which the same group handle is  accessed, must declare the handle
    using SCOREP_USER_METRIC_GROUP_EXTERNAL.

    This macro is not available in Fortran.

    @param groupHandle The variable name for the variable declared in this statement.
                       If you are using a Fortran version which has a limited length of
                       code lines, the length of the @a handle parameter must be at most
                       4 characters, else the declaration line exceeds the allowed length.

    C/C++ example:
    @code
    // In File 1
    SCOREP_USER_METRIC_GROUP_GLOBAL( my_global_group )
    SCOREP_USER_METRIC_GLOBAL( my_global_metric )

    int main()
    {
      SCOREP_USER_METRIC_GROUP_INIT( my_global_group )
      SCOREP_USER:METRIC_INIT( my_global_metric, "My Global Metric", "seconds", \
                             SCOREP_USER_METRIC_TYPE_UINT64, \
                             SCOREP_USER_METRIC_CONTEXT_GLOBAL, my_global_group )
      // do something
    }

    void foo()
    {
      uint64 my_int = get_some_int_value();
      SCOREP_USER_METRIC_UINT64( my_global_metric, my_int )
    }
    @endcode
    @code
    // In File 2
    SCOREP_USER_METRIC_GROUP_EXTERNAL( my_global_group )
    SCOREP_USER_METRIC_EXTERNAL( my_global_metric )

    void bar()
    {
      double my_double = get_some_double();
      uint64 my_int = get_some_int();
      SCOREP_USER_METRIC_LOCAL( my_local_metric )
      SCOREP_USER_METRIC_INIT( my_local_metric, "My Metric", "m", \
                             SCOREP_USER_METRIC_TYPE_DOUBLE, \
                             SCOREP_USER_METRIC_CONTEXT_GLOBAL, my_global_group )
      SCOREP_USER_METRIC_DOUBLE( my_local_metric, my_double )
      SCOREP_USER_METRIC_UINT64( my_global_metric, my_int )
    }
    @endcode
 */

/** @def SCOREP_USER_METRIC_GROUP_EXTERNAL( groupHandle )
    Declares an external group handle. Every global
    group handle must only be declared in one file using SCOREP_USER_METRIC_GROUP_GLOBAL.
    All other files in which the same group handle is  accessed, must declare the handle
    using SCOREP_USER_METRIC_GROUP_EXTERNAL.

    This macro is not available in Fortran

    @param groupHandle The variable name for the variable declared in this statement.

    C/C++ example:
    @code
    // In File 1
    SCOREP_USER_METRIC_GROUP_GLOBAL( my_global_group )
    SCOREP_USER_METRIC_GLOBAL( my_global_metric )

    int main()
    {
      SCOREP_USER_METRIC_GROUP_INIT( my_global_group )
      SCOREP_USER:METRIC_INIT( my_global_metric, "My Global Metric", "seconds", \
                             SCOREP_USER_METRIC_TYPE_UINT64, \
                             SCOREP_USER_METRIC_CONTEXT_GLOBAL, my_global_group )
      // do something
    }

    void foo()
    {
      uint64 my_int = get_some_int_value();
      SCOREP_USER_METRIC_UINT64( my_global_metric, my_int )
    }
    @endcode
    @code
    // In File 2
    SCOREP_USER_METRIC_GROUP_EXTERNAL( my_global_group )
    SCOREP_USER_METRIC_EXTERNAL( my_global_metric )

    void bar()
    {
      double my_double = get_some_double();
      uint64 my_int = get_some_int();
      SCOREP_USER_METRIC_LOCAL( my_local_metric )
      SCOREP_USER_METRIC_INIT( my_local_metric, "My Metric", "m", \
                             SCOREP_USER_METRIC_TYPE_DOUBLE, \
                             SCOREP_USER_METRIC_CONTEXT_GLOBAL, my_global_group )
      SCOREP_USER_METRIC_DOUBLE( my_local_metric, my_double )
      SCOREP_USER_METRIC_UINT64( my_global_metric, my_int )
    }
    @endcode
 */

/** @def SCOREP_USER_METRIC_GROUP_INIT( groupHandle, name )
    Initializes a metric group. Each group need to be initialized before it is used for
    the first time.
    @param groupHandle The handle for the initilaized group. It must be declared using
                       SCOREP_USER_METRIC_GROUP_DEF.
    @param name        A string containing a unique name for that metric group.

    C/C++ example:
    @code
    SCOREP_USER_METRIC_GROUP_LOCAL( my_local_group )
    SCOREP_USER_METRIC_LOCAL( my_local_metric )

    int main()
    {
      SCOREP_USER_METRIC_GROUP_INIT( my_local_group )
      SCOREP_USER_METRIC_INIT( my_local_metric, "My Metric", "seconds", \
                             SCOREP_USER_METRIC_TYPE_UINT64, \
                             SCOREP_USER_METRIC_CONTEXT_GLOBAL, my_local_group )
      // do something
    }

    void foo()
    {
      uint64 my_int = get_some_int_value();
      SCOREP_USER_METRIC_UINT64( my_local_metric, my_int )
    }
    @endcode

    Fortran example:
    @code
    program myProg
      SCOREP_USER_METRIC_GROUP_LOCAL( my_local_group )
      SCOREP_USER_METRIC_LOCAL( my_local_metric )
      integer (kind=selected_int_kind(8)):: my_int = 19
    ! more declarations


      SCOREP_USER_METRIC_GROUP_INIT( my_local_group )
      SCOREP_USER_METRIC_INIT( my_local_metric, "My Metric", "s", &
                             SCOREP_USER_METRIC_TYPE_INT64, &
                             SCOREP_USER_METRIC_CONTEXT_GLOBAL, my_local_group )

    ! do something

      SCOREP_USER_METRIC_INT64( my_local_metric, my_int )
    end program myProg
    @endcode
 */

/** @def SCOREP_USER_METRIC_LOCAL(metricHandle)
    Declares a handle for a user metric.
    It defines a variable which must be in scope at all places where
    the metric is used. If it is used in more than one place it need to be a global
    definition.
    @param metricHandle The name of the variable which will be declared for storing
                        the meric handle.

    C/C++ example:
    @code
    SCOREP_USER_METRIC_LOCAL( my_local_metric )

    int main()
    {
      SCOREP_USER_METRIC_INIT( my_local_metric, "My Metric", "seconds", \
                             SCOREP_USER_METRIC_TYPE_UINT64, \
                             SCOREP_USER_METRIC_CONTEXT_GLOBAL, \
                             SCOREP_USER_METRIC_GROUP_DEFAULT )
      // do something
    }

    void foo()
    {
      uint64 my_int = get_some_int_value();
      SCOREP_USER_METRIC_UINT64( my_local_metric, my_int )
    }
    @endcode

    Fortran example:
    @code
    program myProg
      SCOREP_USER_METRIC_LOCAL( my_local_metric )
      integer (kind=selected_int_kind(8)):: my_int = 19
    ! more declarations


      SCOREP_USER_METRIC_INIT( my_local_metric, "My Metric", "seconds", &
                             SCOREP_USER_METRIC_TYPE_INT64, &
                             SCOREP_USER_METRIC_CONTEXT_GLOBAL, &
                             SCOREP_USER_METRIC_GROUP_DEFAULT )

    ! do something

      SCOREP_USER_METRIC_INT64( my_local_metric, my_int )
    end program myProg
    @endcode
 */

/** @def SCOREP_USER_METRIC_GLOBAL( metricHandle )
    Declares a handle for a user metric as a global variable. It must be used
    if a metric handle is accessed in more than one file. Every
    global metric must be declared only in one file using SCOREP_USER_METRIC_GLOBAL. All
    other files in which this handle is accessed must declare it with
    SCOREP_USER_METRIC_EXTERNAL.

    This macro is not available in Fortran.

    @param metricHandle The variable name for the handle.
                        If you are using a Fortran version which has a limited length of
                        code lines, the
                        length of the @a handle parameter must be at most 4 characters,
                        else the declaration line exceeds the allowed length.


    C/C++ example:
    @code
    // In File 1
    SCOREP_USER_METRIC_GLOBAL( my_global_metric )

    int main()
    {
      SCOREP_USER:METRIC_INIT( my_global_metric, "My Global Metric", "seconds", \
                             SCOREP_USER_METRIC_TYPE_UINT64, \
                             SCOREP_USER_METRIC_CONTEXT_GLOBAL, \
                             SCOREP_USER_METRIC_GROUP_DEFAULT )
      // do something
    }

    void foo()
    {
      uint64 my_int = get_some_int_value();
      SCOREP_USER_METRIC_UINT64( my_global_metric, my_int )
    }
    @endcode
    @code
    // In File 2
    SCOREP_USER_METRIC_EXTERNAL( my_global_metric )

    void bar()
    {
      uint64 my_int = get_some_int_value();
      SCOREP_USER_METRIC_UINT64( my_global_metric, my_int )
    }
    @endcode
 */

/** @def SCOREP_USER_METRIC_EXTERNAL( metricHandle )
    Declares an externally defined handle for a user metric. Every
    global metric must be declared only in one file using SCOREP_USER_METRIC_GLOBAL. All
    other files in which this handle is accessed must declare it with
    SCOREP_USER_METRIC_EXTERNAL.

    This macro is not available in Fortran.

    @param metricHandle The variable name of the handle. it must be the same name as used
                        in the corresponding SCOREP_USER_METRIC_GLOBAL statement.

    C/C++ example:
    @code
    // In File 1
    SCOREP_USER_METRIC_GLOBAL( my_global_metric )

    int main()
    {
      SCOREP_USER_METRIC_INIT( my_global_metric, "My Global Metric", "seconds", \
                             SCOREP_USER_METRIC_TYPE_UINT64, \
                             SCOREP_USER_METRIC_CONTEXT_GLOBAL, \
                             SCOREP_USER_METRIC_GROUP_DEFAULT )
      // do something
    }

    void foo()
    {
      uint64 my_int = get_some_int_value();
      SCOREP_USER_METRIC_UINT64( my_global_metric, my_int )
    }
    @endcode
    @code
    // In File 2
    SCOREP_USER_METRIC_EXTERNAL( my_global_metric )

    void bar()
    {
      uint64 my_int = get_some_int_value();
      SCOREP_USER_METRIC_UINT64( my_global_metric, my_int )
    }
    @endcode
 */

/** @def SCOREP_USER_METRIC_INIT(metricHandle,name, unit, type, context,groupHandle)
    Initializes a new user counter. Each counter must be initialized before it is
    triggered the first time. The handle must be declared using SCOREP_USER_METRIC_LOCAL,
    SCOREP_USER_METRIC_GLOBAL, or SCOREP_USER_METRIC_EXTERNAL.
    @param metricHandle Provides a variable name of the variable to store the metric
                handle. The variable is declared by the macro.
    @param name A string containing a unique name for the counter.
    @param unit A string containing a the unit of the data.
    @param type Specifies the data type of the counter. It must be one of the following:
                SCOREP_USER_METRIC_TYPE_INT64, SCOREP_USER_METRIC_TYPE_UINT64,
                SCOREP_USER_METRIC_TYPE_DOUBLE. In Fortran is SCOREP_USER_METRIC_TYPE_UINT64
                not available.
    @param context Specifies the context for which the counter is measured. IT must be
                one of the following: SCOREP_USER_METRIC_CONTEXT_GLOBAL, or
                SCOREP_USER_METRIC_CONTEXT_CALLPATH.
    @param groupHandle A handle of the group to which this counter belongs.
                If this group does not exist, it will be created. If the default group
                should be used, specify SCOREP_USER_METRIC_GROUP_DEFAULT as group handle.

    C/C++ example:
    @code
    SCOREP_USER_METRIC_LOCAL( my_local_metric )

    int main()
    {
      SCOREP_USER_METRIC_INIT( my_local_metric, "My Metric", "seconds", \
                             SCOREP_USER_METRIC_TYPE_UINT64, \
                             SCOREP_USER_METRIC_CONTEXT_GLOBAL, \
                             SCOREP_USER_METRIC_GROUP_DEFAULT )
      // do something
    }

    void foo()
    {
      uint64 my_int = get_some_int_value();
      SCOREP_USER_METRIC_UINT64( my_local_metric, my_int )
    }
    @endcode

    Fortran example:
    @code
    program myProg
      SCOREP_USER_METRIC_LOCAL( my_local_metric )
      integer (kind=selected_int_kind(8)):: my_int = 19
    ! more declarations


      SCOREP_USER_METRIC_INIT( my_local_metric, "My Metric", "seconds", &
                             SCOREP_USER_METRIC_TYPE_INT64, &
                             SCOREP_USER_METRIC_CONTEXT_GLOBAL, &
                             SCOREP_USER_METRIC_GROUP_DEFAULT )

    ! do something

      SCOREP_USER_METRIC_INT64( my_local_metric, my_int )
    end program myProg
    @endcode
 */

/** @def SCOREP_USER_METRIC_INT64(name,value)
    Triggers a new event for a user counter of a 64 bit integer data type.
    Each user metric must be declared with SCOREP_USER_COUNTER_LOCAL,
    SCOREP_USER_COUNTER_GLOBAL, or SCOREP_USER_COUNTER_EXTERNAL and initialized with
    SCOREP_USER_COUNTER_INIT before it is triggered for the first time.
    @param metricHandle The handle of the metric for which avalue is given in this
                 statement.
    @param value The value of the counter. It must be possible for implicit casts to
                  cast it to a 64 bit integer.

    C/C++ example:
    @code
    SCOREP_USER_METRIC_LOCAL( my_local_metric )

    int main()
    {
      SCOREP_USER_METRIC_INIT( my_local_metric, "My Metric", "seconds", \
                             SCOREP_USER_METRIC_TYPE_INT64, \
                             SCOREP_USER_METRIC_CONTEXT_GLOBAL, \
                             SCOREP_USER_METRIC_GROUP_DEFAULT )
      // do something
    }

    void foo()
    {
      int64 my_int = get_some_int_value();
      SCOREP_USER_METRIC_INT64( my_local_metric, my_int )
    }
    @endcode

    Fortran example:
    @code
    program myProg
      SCOREP_USER_METRIC_LOCAL( my_local_metric )
      integer (kind=selected_int_kind(8)):: my_int = 19
    ! more declarations


      SCOREP_USER_METRIC_INIT( my_local_metric, "My Metric", "seconds", &
                             SCOREP_USER_METRIC_TYPE_INT64, &
                             SCOREP_USER_METRIC_CONTEXT_GLOBAL, &
                             SCOREP_USER_METRIC_GROUP_DEFAULT )

    ! do something

      SCOREP_USER_METRIC_INT64( my_local_metric, my_int )
    end program myProg
    @endcode
 */

/** @def SCOREP_USER_METRIC_UINT64(metricHandle,value)
    Triggers a new event for a user counter of a 64 bit unsigned  integer data type.
    Each user metric must be declared with SCOREP_USER_COUNTER_LOCAL,
    SCOREP_USER_COUNTER_GLOBAL, or SCOREP_USER_COUNTER_EXTERNAL and initialized with
    SCOREP_USER_COUNTER_INIT before it is triggered for the first time.

    In Fortran is the unsigned integer type metric not available.

    @param metricHandle The handle of the metric for which avalue is given in this
                 statement.
    @param value The value of the counter. It must be possible for implicit casts to
                  cast it to a 64 bit unsigned integer.

    Example:
    @code
    SCOREP_USER_METRIC_LOCAL( my_local_metric )

    int main()
    {
      SCOREP_USER_METRIC_INIT( my_local_metric, "My Metric", "seconds", \
                             SCOREP_USER_METRIC_TYPE_UINT64, \
                             SCOREP_USER_METRIC_CONTEXT_GLOBAL, \
                             SCOREP_USER_METRIC_GROUP_DEFAULT )
      // do something
    }

    void foo()
    {
      uint64 my_int = get_some_int_value();
      SCOREP_USER_METRIC_UINT64( my_local_metric, my_int )
    }
    @endcode
 */

/** @def SCOREP_USER_METRIC_DOUBLE(metricHandle,value)
    Triggers a new event for a user counter of a double precision floating point data
    type. Each user metric must be declared with SCOREP_USER_COUNTER_LOCAL,
    SCOREP_USER_COUNTER_GLOBAL, or SCOREP_USER_COUNTER_EXTERNAL and initialized with
    SCOREP_USER_COUNTER_INIT before it is triggered for the first time.
    @param metricHandle The handle of the metric for which avalue is given in this
                 statement.
    @param value The value of the counter. It must be possible for implicit casts to
                  cast it to a double.

    Example:
    @code
    SCOREP_USER_METRIC_LOCAL( my_local_metric )

    int main()
    {
      SCOREP_USER_METRIC_INIT( my_local_metric, "My Metric", "seconds", \
                             SCOREP_USER_METRIC_TYPE_DOUBLE, \
                             SCOREP_USER_METRIC_CONTEXT_GLOBAL, \
                             SCOREP_USER_METRIC_GROUP_DEFAULT )
      // do something
    }

    void foo()
    {
      double my_double = get_some_double_value();
      SCOREP_USER_METRIC_DOUBLE( my_local_metric, my_double )
    }
    @endcode

    Fortran example:
    @code
    program myProg
      SCOREP_USER_METRIC_LOCAL( my_local_metric )
      real (kind=selected_int_kind(14,200)):: my_real = 24.5
    ! more declarations


      SCOREP_USER_METRIC_INIT( my_local_metric, "My Metric", "seconds", &
                             SCOREP_USER_METRIC_TYPE_DOUBLE, &
                             SCOREP_USER_METRIC_CONTEXT_GLOBAL, &
                             SCOREP_USER_METRIC_GROUP_DEFAULT )

    ! do something

      SCOREP_USER_METRIC_DOUBLE( my_local_metric, my_real )
    end program myProg
    @endcode
 */

/* **************************************************************************************
 * User Counter macros
 * *************************************************************************************/
#ifdef SCOREP_USER_ENABLE

#define SCOREP_USER_METRIC_GROUP_LOCAL( groupHandle ) static \
    SCOREP_CounterGroupHandle groupHandle  \
        = SCOREP_INVALID_COUNTER_GROUP;

#define SCOREP_USER_METRIC_GROUP_GLOBAL( groupHandle )  SCOREP_CounterGroupHandle \
    groupHandle = SCOREP_INVALID_COUNTER_GROUP;

#define SCOREP_USER_METRIC_GROUP_EXTERNAL( groupHandle ) \
    extern SCOREP_CounterGroupHandle groupHandle;

#define SCOREP_USER_METRIC_GROUP_INIT( groupHandle, \
                                       name ) SCOREP_User_InitMetricGroup( \
        &groupHandle, name );

#define SCOREP_USER_METRIC_LOCAL( metricHandle ) static SCOREP_CounterHandle \
    metricHandle                                                                \
        = SCOREP_INVALID_COUNTER;

#define SCOREP_USER_METRIC_GLOBAL( metricHandle ) SCOREP_CounterHandle metricHandle \
        = SCOREP_INVALID_COUNTER;

#define SCOREP_USER_METRIC_EXTERNAL( metricHandle ) \
    extern SCOREP_CounterHandle metricHandle;

#define SCOREP_USER_METRIC_INIT( metricHandle, name, unit, type, context, \
                                 groupHandle ) \
    SCOREP_User_InitMetric( &metricHandle, name, unit, type, context, groupHandle );

#define SCOREP_USER_METRIC_INT64( metricHandle, \
                                  value )  SCOREP_User_TriggerMetricInt64( \
        metricHandle, value );

#define SCOREP_USER_METRIC_UINT64( metricHandle, \
                                   value )  SCOREP_User_TriggerMetricInt64( \
        metricHandle, value );

#define SCOREP_USER_METRIC_DOUBLE( metricHandle, \
                                   value ) SCOREP_User_TriggerMetricDouble( \
        metricHandle, value );

#endif // SCOREP_USER_ENABLE

/* **************************************************************************************
 * Documentation for C++ specific macros
 * *************************************************************************************/

/** @def SCOREP_USER_REGION(name,type)
    Instruments a codeblock as a region with the given name. It inserts a local variable
    of the type class SCOREP_User_Region. Its constructor generates the enter event and
    its destructor generates the exit event. Thus, only one statement is necessary to
    instrument the code block. This statement is only in C++ available.
    @param name    A string containing the name of the new region. The name should be
                   unique.
    @param type    Specifies the type of the region. Possible values are
                   SCOREP_USER_REGION_TYPE_COMMON,
                   SCOREP_USER_REGION_TYPE_FUNCTION, SCOREP_USER_REGION_TYPE_LOOP,
                   SCOREP_USER_REGION_TYPE_DYNAMIC, SCOREP_USER_REGION_TYPE_PHASE, or a
                   combination of them.

    Example:
    @code
    void myfunc()
    {
      SCOREP_USER_REGION_( "myfunc", SCOREP_USER_REGION_TYPE_FUNCTION )

      // do something
    }
    @endcode
 */
/* **************************************************************************************
 * C++ specific macros
 * *************************************************************************************/
#ifdef SCOREP_USER_ENABLE

#ifdef __cplusplus

#define SCOREP_USER_REGION( name, type ) static SCOREP_User_RegionClass \
    scorep_user_region_inst( name, type, __FILE__, __LINE__ );

#else // __cplusplus

#define SCOREP_USER_REGION( name, type )

#endif // __cplusplus

#endif // SCOREP_USER_ENABLE

/* **************************************************************************************
 * Documentation for Control macros
 * *************************************************************************************/

/** @def SCOREP_RECORDING_ON
    Enables recording of events. If already enabled, this command has no effect.
    The control is not restricted to events from the user adapter, but enables the
    recording of all events.

    C/C++ example:
    @code
    void foo()
    {
      SCOREP_RECORDING_OFF

      // do something

     SCOREP_RECORDING_ON
    }
    @endcode

    Fortran example:
    @code
    subroutine foo

      SCOREP_RECORDING_OFF
    ! do something
      SCOREP_RECORDING_ON

    end subroutine foo
    @endcode
 */

/** @def SCOREP_RECORDING_OFF
    Disables recording of events. If already disabled, this command has no effect.
    The control is not restricted to events from the user adapter, but disables the
    recording of all events.

    C/C++ example:
    @code
    void foo()
    {
      SCOREP_RECORDING_OFF

      // do something

     SCOREP_RECORDING_ON
    }
    @endcode

    Fortran example:
    @code
    subroutine foo

      SCOREP_RECORDING_OFF
    ! do something
      SCOREP_RECORDING_ON

    end subroutine foo
    @endcode
 */

/** @def SCOREP_RECORDING_IS_ON
    In C/C++ it behaves like a function call which returns wether recording is
    enabled or not. It returns false if the recording of events is disabled, else
    it returns true.

    C/C++ example:
    @code
    void foo()
    {
      if ( SCOREP_RECORDING_IS_ON )
      {
        // do something
      }
    }
    @endcode

    In Fortran, this macro has a different syntax. An integer variable must be
    specified as parameter, which is set to non-zero if recording is enabled, else the
    value is set to zero.

    Fortran example:
    @code
    subroutine foo
      integer :: l

      SCOREP_RECORDING_IS_ON( l )
      if (l .eq. 0) then
        ! do something
      end if

    end subroutine foo
    @endcode
 */

/* **************************************************************************************
 * Control macros
 * *************************************************************************************/

#ifdef SCOREP_USER_ENABLE

#define SCOREP_RECORDING_ON SCOREP_User_EnableRecording();

#define SCOREP_RECORDING_OFF SCOREP_User_DisableRecording();

#define SCOREP_RECORDING_IS_ON SCOREP_User_RecordingEnabled()

#endif // SCOREP_USER_ENABLE

/* **************************************************************************************
 * Documentation for Virtual Topologies
 * *************************************************************************************/

/** @def SCOREP_DEFINE_CARTESIAN_2D
    Defines a two-dimentional cartesian topology.
    @param name     A string containing the name of the topology.
    @param numX     Number of processes in the x-dimension.
    @param numY     Number of processes in the y-dimension.
    @param periodX  A truth value, if the x-dimension is periodical. A value of zero means
                    false, other values mean true.
    @param periodY  A truth value, if the y-dimension is periodical. A value of zero means
                    false, other values mean true.
    @return A handle to the newly created topology.
 */

/** @def SCOREP_DEFINE_CARTESIAN_3D
    Defines a three-dimentional cartesian topology.
    @param name     A string containing the name of the topology.
    @param numX     Number of processes in the x-dimension.
    @param numY     Number of processes in the y-dimension.
    @param numZ     Number of processes in the z-dimension.
    @param periodX  A truth value, if the x-dimension is periodical. A value of zero means
                    false, other values mean true.
    @param periodY  A truth value, if the y-dimension is periodical. A value of zero means
                    false, other values mean true.
    @param periodZ  A truth value, if the z-dimension is periodical. A value of zero means
                    false, other values mean true.
    @return A handle to the newly created topology.
 */

/** @def SCOREP_DEFINE_COORDINATE_2D
    Defines the a coordinate in a two-dimensional cartesian topology.
    @param topId    Handle of a previously defined two-dimensional cartesian topology.
    @param coordX   X-coordinate
    @param coordY   Y-coordinate
 */

/** @def SCOREP_DEFINE_COORDINATE_3D
    Defines the a coordinate in a three-dimensional cartesian topology.
    @param topId    Handle of a previously defined two-dimensional cartesian topology.
    @param coordX   X-coordinate
    @param coordY   Y-coordinate
    @param coordZ   Z-coordinate
 */

/* **************************************************************************************
 * Virtual Topologies
 * *************************************************************************************/
#ifdef SCOREP_USER_ENABLE

#define SCOREP_DEFINE_CARTESIAN_2D( name, numX, numY, periodX, periodY ) \
    SCOREP_User_DefineTopology2D( name, numX, numY, periodX, periodY )

#define SCOREP_DEFINE_CARTESIAN_3D( name, numX, numY, numZ, periodX, periodY, \
                                    periodZ ) \
    SCOREP_User_DefineTopology3D( name, numX, numY, numZ, periodX, periodY, \
                                  periodZ )

#define SCOREP_DEFINE_COORDINATE_2D( topId, coordX, coordY ) \
    SCOREP_User_DefineCoordinates2D( topId, coordX, coordY );

#define SCOREP_DEFINE_COORDINATE_3D( topId, coordX, coordY, coordZ ) \
    SCOREP_User_DefineCoordinates3D( topId, coordX, coordY, coordZ );

/* **************************************************************************************
 * Empty macros, if user instrumentation is disabled
 * *************************************************************************************/
#else // SCOREP_USER_ENABLE

#define SCOREP_USER_REGION( name, type )
#define SCOREP_USER_REGION_DEFINE( handle )
#define SCOREP_USER_REGION_BEGIN( handle, name, type )
#define SCOREP_USER_REGION_INIT( handle, name, type )
#define SCOREP_USER_REGION_END( handle )
#define SCOREP_USER_REGION_ENTER( handle )
#define SCOREP_USER_FUNC_BEGIN
#define SCOREP_USER_FUNC_END
#define SCOREP_GLOBAL_REGION_DEFINE( handle )
#define SCOREP_GLOBAL_REGION_EXTERNAL ( handle )
#define SCOREP_USER_PARAMETER_INT64( name, value )
#define SCOREP_USER_PARAMETER_STRING( name, value )
#define SCOREP_USER_METRIC_GROUP_LOCAL( groupHandle )
#define SCOREP_USER_METRIC_GROUP_GLOBAL( groupHandle )
#define SCOREP_USER_METRIC_GROUP_EXTERNAL( groupHandle )
#define SCOREP_USER_METRIC_GROUP_INIT( groupHandle, name )
#define SCOREP_USER_METRIC_GLOBAL( metricHandle )
#define SCOREP_USER_METRIC_EXTERNAL( metricHandle )
#define SCOREP_USER_METRIC_LOCAL( metricHandle )
#define SCOREP_USER_METRIC_INIT( metricHandle, name, unit, type, context, \
                                 groupHandle )
#define SCOREP_USER_METRIC_INT64( metricHandle, value )
#define SCOREP_USER_METRIC_UINT64( metricHandle, value )
#define SCOREP_USER_METRIC_DOUBLE( metricHandle, value )
#define SCOREP_RECORDING_ON
#define SCOREP_RECORDING_OFF
#define SCOREP_RECORDING_IS_ON 0
#define SCOREP_DEFINE_CARTESIAN_2D( name, numX, numY, periodX, periodY )
#define SCOREP_DEFINE_CARTESIAN_3D( name, numX, numY, numZ, periodX, periodY, \
                                    periodZ )
#define SCOREP_DEFINE_COORDINATE_2D( topId, coordX, coordY )
#define SCOREP_DEFINE_COORDINATE_3D( topId, coordX, coordY, coordZ )

#endif // SCOREP_USER_ENABLE

/** @} */

#endif // SCOREP_USER_H
