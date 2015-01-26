!
! This file is part of the SCOREP project (http:
!
! Copyright (c) 2009-2011,
! RWTH Aachen, Germany
! Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
! Technische Universitaet Dresden, Germany
! University of Oregon, Eugene USA
! Forschungszentrum Juelich GmbH, Germany
! Technische Universitaet Muenchen, Germany
!
! See the COPYING file in the package base directory for details.
!

#include <scorep/SCOREP_User.inc>

      subroutine instrument_begin(name)
      character(len=*) :: name

      integer type
      type = SCOREP_USER_REGION_TYPE_COMMON
      SCOREP_USER_REGION_BY_NAME_BEGIN(name,type)

      end subroutine instrument_begin

      subroutine instrument_end(name)

      character(len=*) :: name
      SCOREP_USER_REGION_BY_NAME_END(name)

      end subroutine instrument_end

      program ScorepTest

      integer * 8 i1
      double precision r1
      integer * 8 iparam_value
      integer * 8 uparam_value
      integer l
      integer type
      integer m2_type
      integer m3_type
      integer m_context

      SCOREP_USER_REGION_DEFINE(region1)
      SCOREP_USER_REGION_DEFINE(region2)
      SCOREP_USER_FUNC_DEFINE()
      SCOREP_USER_METRIC_LOCAL(local2)
      SCOREP_USER_METRIC_LOCAL(local3)
      SCOREP_USER_PARAMETER_DEFINE(iparam)
      SCOREP_USER_PARAMETER_DEFINE(uparam)
      SCOREP_USER_PARAMETER_DEFINE(sparam)

      i1 = 19
      r1 = 24.5
      iparam_value = -1
      uparam_value = 1
      type = SCOREP_USER_REGION_TYPE_FUNCTION
      m2_type = SCOREP_USER_METRIC_TYPE_INT64
      m3_type = SCOREP_USER_METRIC_TYPE_DOUBLE
      m_context = SCOREP_USER_METRIC_CONTEXT_GLOBAL

      SCOREP_USER_FUNC_BEGIN("ScorepTest")
      SCOREP_USER_REGION_INIT(region2,"Region2",type)
      SCOREP_USER_REGION_BEGIN(region1,"Region1",type)
      SCOREP_USER_REGION_ENTER(region2)
      SCOREP_USER_PARAMETER_INT64(iparam,"iparam",iparam_value)
      SCOREP_USER_PARAMETER_UINT64(uparam,"uparam",uparam_value)
      SCOREP_USER_PARAMETER_STRING(sparam,"sparam","hello")

      SCOREP_USER_METRIC_INIT(local2,"local2","s",m2_type,m_context)
      SCOREP_USER_METRIC_INIT(local3,"local3","s",m3_type,m_context)

      SCOREP_USER_METRIC_INT64(local2,i1)
      SCOREP_USER_METRIC_DOUBLE(local3,r1)

      SCOREP_USER_REGION_END(region2)
      SCOREP_USER_REGION_END(region1)
      SCOREP_USER_FUNC_END()

      SCOREP_RECORDING_OFF()
      SCOREP_RECORDING_IS_ON(l)
      if (l .eq. 0) then
         SCOREP_RECORDING_ON()
      endif
      SCOREP_USER_REGION_BEGIN(region1,"Region1",type)
      SCOREP_USER_REGION_END(region1)

      call instrument_begin("byname_region")
      call instrument_end("byname_region")

      end program ScorepTest
