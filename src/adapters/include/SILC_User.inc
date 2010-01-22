!
!  This file is part of the SILC project (http://www.silc.de)
!
!  Copyright (c) 2009-2011,
!     RWTH Aachen, Germany
!     Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
!     Technische Universitaet Dresden, Germany
!     University of Oregon, Eugene USA
!     Forschungszentrum Juelich GmbH, Germany
!     Technische Universitaet Muenchen, Germany
!
!  See the COPYING file in the package base directory for details.
!

!> @file       SILC_User.inc
!! @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
!! @status     BETA
!! @ingroup    SILC_User_Interface
!!
!! @brief User interface for manual instrumantation of Fortran files.
!! @author Daniel Lorenz
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
#define SILC_USER_METRIC_TYPE_DOUBLE 1

#define SILC_USER_METRIC_CONTEXT_GLOBAL   0
#define SILC_USER_METRIC_CONTEXT_CALLPATH 1

#define SILC_USER_METRIC_GROUP_DEFAULT -2

!************************************************************* Regions

#define SILC_USER_REGION_DEFINE( handle ) \
        SILC_REGION_HANDLE :: handle = SILC_INVALID_REGION

#define SILC_USER_REGION_BEGIN( handle, name, type ) call SILC_User_RegionBeginF(\
        handle, name, type, __FILE__, __LINE__)

#define SILC_USER_REGION_INIT( handle, name, type ) call SILC_User_RegionInitF(\
        handle, name, type, __FILE__, __LINE__)

#define SILC_USER_REGION_END( handle ) call SILC_User_RegionEndF(handle)

#define SILC_USER_REGION_ENTER( handle ) call SILC_User_RegionEnterF(handle)

#define SILC_GLOBAL_REGION_DEFINE( handle ) \
        blockdata handle##g;SILC_REGION_HANDLE::handle;common/handle##id/handle;data handle/SILC_INVALID_REGION/;end
      

#define SILC_GLOBAL_REGION_EXTERNAL( handle ) \
        SILC_REGION_HANDLE::handle; \
        common /handle##id/handle

#define SILC_USER_FUNC_DEFINE SILC_USER_REGION_DEFINE( silcufh )

#define SILC_USER_FUNC_BEGIN( name ) \
        SILC_USER_REGION_BEGIN(silcufh, name, \
        SILC_USER_REGION_TYPE_FUNCTION)

#define SILC_USER_FUNC_END SILC_USER_REGION_END(silcufh)

!************************************************************* User metric

#define SILC_USER_METRIC_GROUP_LOCAL( groupHandle ) \
        SILC_METRIC_GROUP :: groupHandle = SILC_INVALID_METRIC_GROUP

#define SILC_USER_METRIC_GROUP_GLOBAL( groupHandle ) \
        blockdata groupHandle##g;SILC_METRIC_GROUP::groupHandle;common/groupHandle##id/groupHandle;data groupHandle/SILC_INVALID_METRIC_GROUP/;end

#define SILC_USER_METRIC_GROUP_EXTERNAL( groupHandle ) \
        SILC_METRIC_GROUP::groupHandle; \
        common /groupHandle##id/ groupHandle

#define SILC_USER_METRIC_GROUP_INIT( groupHandle, name ) \
        call SILC_User_InitMetricGroupF( groupHandle, name )

#define SILC_USER_METRIC_LOCAL( metricHandle ) \
        SILC_METRIC_HANDLE :: metricHandle = SILC_INVALID_METRIC

#define SILC_USER_METRIC_GLOBAL( metricHandle ) \
        blockdata metricHandle##g;SILC_METRIC_HANDLE::metricHandle;common/metricHandle##id/metricHandle;data metricHandle/SILC_INVALID_METRIC/;end

#define SILC_USER_METRIC_EXTERNAL( metricHandle ) \
        SILC_METRIC_HANDLE::metricHandle;\
        common /metricHandle##id/ metricHandle

#define SILC_USER_METRIC_INIT( metricHandle, name, unit, type, context, groupHandle ) \
        call SILC_User_InitMetricF( metricHandle, name, unit, type, context, groupHandle )

#define SILC_USER_METRIC_INT64( metricHandle, value ) \
        call SILC_User_MetricInt64F( metricHandle, value )

#define SILC_USER_METRIC_DOUBLE( metricHandle, value ) \
        call SILC_User_MetricDoubleF( metricHandle, value )

!************************************************************* User control

#define SILC_RECORDING_ON call SILC_User_EnableRecordingF()

#define SILC_RECORDING_OFF call SILC_User_DisableRecordingF()

#define SILC_RECORDING_IS_ON( isOn ) call SILC_User_RecordingEnabledF( isOn )

!************************************************************* Empty defines

#else

#define SILC_USER_REGION_DEFINE( handle )
#define SILC_GLOBAL_REGION_DEFINE( handle )
#define SILC_USER_REGION_BEGIN( handle, name, type )
#define SILC_USER_REGION_INIT( handle, name, type )
#define SILC_USER_REGION_END( handle )
#define SILC_USER_REGION_ENTER( handle )
#define SILC_GLOBAL_REGION_EXTERNAL( handle )
#define SILC_USER_FUNC_DEFINE
#define SILC_USER_FUNC_BEGIN( name )
#define SILC_USER_FUNC_END
#define SILC_USER_METRIC_GROUP_LOCAL( groupHandle )
#define SILC_USER_METRIC_GROUP_GLOBAL( groupHandle )
#define SILC_USER_METRIC_GROUP_EXTERNAL( groupHandle )
#define SILC_USER_METRIC_GROUP_INIT( groupHandle, name )
#define SILC_USER_METRIC_GLOBAL( metricHandle )
#define SILC_USER_METRIC_EXTERNAL( metricHandle )
#define SILC_USER_METRIC_LOCAL( metricHandle )
#define SILC_USER_METRIC_INIT( metricHandle, name, unit, type, context, groupHandle )
#define SILC_USER_METRIC_INT64( metricHandle, value )
#define SILC_USER_METRIC_UINT64( metricHandle, value )
#define SILC_USER_METRIC_DOUBLE( metricHandle, value )
#define SILC_RECORDING_ON
#define SILC_RECORDING_OFF
#define SILC_RECORDING_IS_ON( isOn )

#endif