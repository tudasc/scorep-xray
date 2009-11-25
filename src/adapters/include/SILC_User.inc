! *
! * Fortran SILC interface for user manual instrumentation.
! *

!> @file  SILC_User.inc
!! @brief User interface for manual instrumantation of Fortran files.
!! @author Daniel Lorenz
!! @ingroup user_fortran
!<

!> @defgroup user_fortran Fortran interface for manual instrumentation
!! This module contains the macros used for user manual instrumentation in Fortran.
!! The instrumentation is only working if teh switch SILC_USER_ENABLE is defined. Else
!! All user instrumentation is removed automaically during preprocessing.
!< @{

!> @def SILC_USER_REGION_DEFINE( handle )
!!  This macro defines a region. Every region has to be defined before it is first
!!  entered. The defines have to take place declaration block.
!!  @param handle  A variable name for a parameter must be provided. This variable is
!!                 declared in this macro. This handle is used in the
!!                 SILC_USER_REGION_BEGIN and SILC_USER_REGION_END statements to specify
!!                 which region is started, or ended.
!!
!!  Example:
!!  @code
!!  program myProg
!!    SILC_USER_REGION_DEFINE( my_region_handle )
!!    ! more declarations
!!
!!    SILC_USER_REGION_BEGIN( my_region_handle, "my_region",SILC_USER_REGION_TYPE_COMMON )
!!    ! do something
!!    SILC_USER_REGION_END( my_region_handle )
!!
!!  end program myProg
!!  @endcode
!<

!>
!! @}
!<

#ifdef SILC_USER_ENABLE

!************************************************************* Data types

#define SILC_REGION_HANDLE integer
#define SILC_METRIC_GROUP  integer
#define SILC_METRIC_HANDLE integer

!************************************************************* Constants

#define SILC_INVALID_REGION        -1
#define SILC_INVALID_METRIC_GROUP  -1
#define SILC_INVALID_METRIC        -1

#define SILC_USER_REGION_TYPE_COMMON    0
#define SILC_USER_REGION_TYPE_FUNCTION  1
#define SILC_USER_REGION_TYPE_LOOP      2
#define SILC_USER_REGION_TYPE_DYNAMIC   4
#define SILC_USER_REGION_TYPE_PHASE     8

#define SILC_USER_METRIC_TYPE_INT64  0
#define SILC_USER_METRIC_TYPE_REAL64 1

#define SILC_USER_METRIC_CONTEXT_GLOBAL   0
#define SILC_USER_METRIC_CONTEXT_CALLPATH 1

#define SILC_USER_METRIC_GROUP_DEFAULT -2

!************************************************************* Regions

#define SILC_USER_REGION_DEFINE( handle ) \
        SILC_REGION_HANDLE :: handle = SILC_INVALID_REGION

#define SILC_USER_REGION_BEGIN( handle, name, type ) call SILC_User_RegionBeginF(\
        handle, name, type, __FILE__, __LINE__)

#define SILC_USER_REGION_END( handle ) call SILC_User_RegionEndF(handle)

#define SILC_GLOBAL_REGION_DEFINE( handle ) \
        module handle##mod; \
        SILC_REGION_HANDLE::handle=SILC_INVALID_REGION; \
        end module handle##mod

#define SILC_GLOBAL_REGION_EXTERNAL( handle ) \
        use handle##mod

#define SILC_USER_FUNCTION_DEFINE SILC_USER_REGION_DEFINE( silcufh )

#define SILC_USER_FUNCTION_BEGIN( name ) \
        SILC_USER_REGION_BEGIN(silcufh, name, \
        SILC_USER_REGION_TYPE_FUNCTION)

#define SILC_USER_FUNCTION_END SILC_USER_REGION_END(silcufh)

!************************************************************* User metric

#define SILC_USER_METRIC_GROUP_LOCAL( groupHandle ) \
        SILC_METRIC_GROUP :: groupHandle = SILC_INVALID_METRIC_GROUP

#define SILC_USER_METRIC_GROUP_GLOBAL( groupHandle ) \
        module groupHandle##mod; SILC_METRIC_GROUP::groupHandle=\
        SILC_INVALID_METRIC_GROUP; end module groupHandle##mod

#define SILC_USER_METRIC_GROUP_EXTERNAL( groupHandle ) use groupHandle##mod

#define SILC_USER_METRIC_GROUP_INIT( groupHandle, name ) \
        call SILC_User_InitMetricGroupF( groupHandle, name );

#define SILC_USER_METRIC_LOCAL( metricHandle ) \
        SILC_METRIC_HANDLE :: metricHandle = SILC_INVALID_METRIC

#define SILC_USER_METRIC_GLOBAL( metricHandle ) \
        module metricHandle##mod; SILC_METRIC_HANDLE::metricHandle=SILC_INVALID_METRIC;\
        end module metricHandle##mod

#define SILC_USER_METRIC_EXTERNAL( metricHandle ) \
        use metricHandle##mod

#define SILC_USER_METRIC_INIT( metricHandle, name, unit, type, context, groupHandle ) \
        call SILC_User_InitMetricF( metricHandle, name, unit, type, context, groupHandle )

#define SILC_USER_METRIC_INT64( metricHandle, value ) \
        call SILC_User_MetricInt64F( metricHandle, value )

#define SILC_USER_METRIC_REAL64( metricHandle, value ) \
        call SILC_User_MetricDoubleF( metricHandle, value )


!************************************************************* Empty defines

#else

#define SILC_USER_REGION_DEFINE( handle )
#define SILC_GLOBAL_REGION_DEFINE( handle )
#define SILC_USER_REGION_BEGIN( handle )
#define SILC_USER_REGION_END( handle )
#define SILC_GLOBAL_REGION_EXTERNAL( handle )
#define SILC_USER_FUNCTION_DEFINE
#define SILC_USER_FUNCTION_BEGIN( name )
#define SILC_USER_FUNCTION_END

#endif