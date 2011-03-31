!
!  This file is part of the Score-P software (http://www.score-p.org)
!
!  Copyright (c) 2009-2011,
!     RWTH Aachen University, Germany
!     Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
!     Technische Universitaet Dresden, Germany
!     University of Oregon, Eugene, USA
!     Forschungszentrum Juelich GmbH, Germany
!     German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
!     Technische Universitaet Muenchen, Germany
!
!  See the COPYING file in the package base directory for details.
!

!> @file       SCOREP_User.inc
!! @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
!! @status     BETA
!! @ingroup    SCOREP_User_Interface
!!
!! @brief User interface for manual instrumantation of Fortran files.
!! @author Daniel Lorenz
!<

#ifdef SCOREP_USER_ENABLE

!************************************************************* Data types

#define SCOREP_REGION_HANDLE    integer*8
#define SCOREP_METRIC_GROUP     integer*8
#define SCOREP_METRIC_HANDLE    integer*8
#define SCOREP_PARAMETER_HANDLE integer*8

!************************************************************* Constants

#define SCOREP_INVALID_REGION        -1
#define SCOREP_INVALID_METRIC_GROUP  -1
#define SCOREP_INVALID_METRIC        -1
#define SCOREP_INAVLID_PARAMETER     -1

#define SCOREP_USER_REGION_TYPE_COMMON    0
#define SCOREP_USER_REGION_TYPE_FUNCTION  1
#define SCOREP_USER_REGION_TYPE_LOOP      2
#define SCOREP_USER_REGION_TYPE_DYNAMIC   4
#define SCOREP_USER_REGION_TYPE_PHASE     8

#define SCOREP_USER_METRIC_TYPE_INT64  0
#define SCOREP_USER_METRIC_TYPE_DOUBLE 1

#define SCOREP_USER_METRIC_CONTEXT_GLOBAL   0
#define SCOREP_USER_METRIC_CONTEXT_CALLPATH 1

#define SCOREP_USER_METRIC_GROUP_DEFAULT -2

!************************************************************* Regions

#define SCOREP_USER_REGION_DEFINE( handle ) \
        SCOREP_REGION_HANDLE :: handle = SCOREP_INVALID_REGION

#define SCOREP_USER_REGION_BEGIN( handle, name, type )\
        call SCOREP_F_Begin(handle,name,type,"",__LINE__)

#define SCOREP_USER_REGION_INIT( handle, name, type )\
        call SCOREP_F_Init(handle,name,type,"",__LINE__)

#define SCOREP_USER_REGION_END( handle ) call SCOREP_F_RegionEnd( handle )

#define SCOREP_USER_REGION_ENTER( handle ) call SCOREP_F_RegionEnter( handle )

#define SCOREP_GLOBAL_REGION_DEFINE( handle ) \
        blockdata##handle##g;SCOREP_REGION_HANDLE::handle;common/handle##id/handle;data##handle/SCOREP_INVALID_REGION/;end

#define SCOREP_GLOBAL_REGION_EXTERNAL( handle ) \
        SCOREP_REGION_HANDLE::handle; \
        common /handle##id/handle

#define SCOREP_USER_FUNC_DEFINE SCOREP_USER_REGION_DEFINE( scrpfh )

#define SCOREP_USER_FUNC_BEGIN( name ) \
        SCOREP_USER_REGION_BEGIN(scrpfh, name, \
        SCOREP_USER_REGION_TYPE_FUNCTION)

#define SCOREP_USER_FUNC_END SCOREP_USER_REGION_END(scrpfh)

!************************************************************* User metric

#define SCOREP_USER_METRIC_GROUP_LOCAL( groupHandle ) \
        SCOREP_METRIC_GROUP :: groupHandle = SCOREP_INVALID_METRIC_GROUP

#define SCOREP_USER_METRIC_GROUP_GLOBAL( groupHandle ) \
        blockdata##groupHandle##g;SCOREP_METRIC_GROUP::groupHandle;common/groupHandle##id/groupHandle;data##groupHandle/SCOREP_INVALID_METRIC_GROUP/;end

#define SCOREP_USER_METRIC_GROUP_EXTERNAL( groupHandle ) \
        SCOREP_METRIC_GROUP::groupHandle; \
        common /groupHandle##id/ groupHandle

#define SCOREP_USER_METRIC_GROUP_INIT( groupHandle, name ) \
        call SCOREP_F_InitMetricGroup(groupHandle,name)

#define SCOREP_USER_METRIC_LOCAL( metricHandle ) \
        SCOREP_METRIC_HANDLE :: metricHandle = SCOREP_INVALID_METRIC

#define SCOREP_USER_METRIC_GLOBAL( metricHandle ) \
        blockdata##metricHandle##g;SCOREP_METRIC_HANDLE::metricHandle;common/metricHandle##id/metricHandle;data##metricHandle/SCOREP_INVALID_METRIC/;end

#define SCOREP_USER_METRIC_EXTERNAL( metricHandle ) \
        SCOREP_METRIC_HANDLE::metricHandle;\
        common /metricHandle##id/ metricHandle

#define SCOREP_USER_METRIC_INIT( metricHandle, name, unit, type, context, groupHandle ) \
        call SCOREP_F_InitMetric(metricHandle,name,unit,type,context,groupHandle)

#define SCOREP_USER_METRIC_INT64( metricHandle, value ) \
        call SCOREP_F_MetricInt64( metricHandle, value )

#define SCOREP_USER_METRIC_DOUBLE( metricHandle, value ) \
        call SCOREP_F_MetricDouble( metricHandle, value )

!************************************************************* User control

#define SCOREP_RECORDING_ON call SCOREP_F_EnableRecording()

#define SCOREP_RECORDING_OFF call SCOREP_F_DisableRecording()

#define SCOREP_RECORDING_IS_ON( isOn ) call SCOREP_F_RecordingEnabled( isOn )

!************************************************************* Parameter

#define SCOREP_PARAMETER_DEFINE( handle ) \
        SCOREP_PARAMETER_HANDLE :: handle = SCOREP_INAVLID_PARAMETER

#define SCOREP_PARAMETER_INT64( handle, name, value ) \
        call SCOREP_F_ParameterInt64( handle, name, value )

#define SCOREP_PARAMETER_STRING( handle, name, value )\
        call SCOREP_F_ParameterInt64( handle, name, value )

!************************************************************* Empty defines

#else

#define SCOREP_USER_REGION_DEFINE( handle )
#define SCOREP_GLOBAL_REGION_DEFINE( handle )
#define SCOREP_USER_REGION_BEGIN( handle, name, type )
#define SCOREP_USER_REGION_INIT( handle, name, type )
#define SCOREP_USER_REGION_END( handle )
#define SCOREP_USER_REGION_ENTER( handle )
#define SCOREP_GLOBAL_REGION_EXTERNAL( handle )
#define SCOREP_USER_FUNC_DEFINE
#define SCOREP_USER_FUNC_BEGIN( name )
#define SCOREP_USER_FUNC_END
#define SCOREP_USER_METRIC_GROUP_LOCAL( groupHandle )
#define SCOREP_USER_METRIC_GROUP_GLOBAL( groupHandle )
#define SCOREP_USER_METRIC_GROUP_EXTERNAL( groupHandle )
#define SCOREP_USER_METRIC_GROUP_INIT( groupHandle, name )
#define SCOREP_USER_METRIC_GLOBAL( metricHandle )
#define SCOREP_USER_METRIC_EXTERNAL( metricHandle )
#define SCOREP_USER_METRIC_LOCAL( metricHandle )
#define SCOREP_USER_METRIC_INIT( metricHandle, name, unit, type, context, groupHandle )
#define SCOREP_USER_METRIC_INT64( metricHandle, value )
#define SCOREP_USER_METRIC_UINT64( metricHandle, value )
#define SCOREP_USER_METRIC_DOUBLE( metricHandle, value )
#define SCOREP_RECORDING_ON
#define SCOREP_RECORDING_OFF
#define SCOREP_RECORDING_IS_ON( isOn )

#endif
