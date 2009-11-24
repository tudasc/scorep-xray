! *
! * Fortran SILC interface for user manual instrumentation.
! *

#ifdef SILC_USER_ENABLE

!************************************************************* Constants
#define SILC_INVALID_REGION  -1

#define SILC_USER_REGION_TYPE_COMMON    0
#define SILC_USER_REGION_TYPE_FUNCTION  1
#define SILC_USER_REGION_TYPE_LOOP      2
#define SILC_USER_REGION_TYPE_DYNAMIC   4
#define SILC_USER_REGION_TYPE_PHASE     8

!************************************************************* Regions
#define SILC_USER_REGION_DEFINE( handle ) \
   integer :: handle = SILC_INVALID_REGION

#define SILC_USER_REGION_BEGIN( handle, name, type ) call SILC_User_RegionBeginF(\
        handle, name, type, __FILE__, __LINE__)

#define SILC_USER_REGION_END( handle ) call SILC_User_RegionEndF(handle)

#define SILC_GLOBAL_REGION_DEFINE( handle ) \
        module handle##mod; \
        integer::handle=SILC_INVALID_REGION; \
        end module handle##mod

#define SILC_GLOBAL_REGION_EXTERNAL( handle ) \
        use handle##mod

#define SILC_USER_FUNCTION_DEFINE SILC_USER_REGION_DEFINE( silcufh )

#define SILC_USER_FUNCTION_BEGIN( name ) \
        SILC_USER_REGION_BEGIN(silcufh, name, \
        SILC_USER_REGION_TYPE_FUNCTION)

#define SILC_USER_FUNCTION_END SILC_USER_REGION_END(silcufh)

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