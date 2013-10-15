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
#ifndef SCOREP_COMPILER_PGI

      program ScorepTest

      integer * 8 :: i1 = 19
      double precision :: r1 = 24.5
      integer :: l
      SCOREP_USER_REGION_DEFINE(region1)
      SCOREP_USER_REGION_DEFINE(region2)
      SCOREP_USER_FUNC_DEFINE()
      SCOREP_USER_METRIC_LOCAL(local2)
      SCOREP_USER_METRIC_LOCAL(local3)
      SCOREP_USER_PARAMETER_DEFINE(iparam)
      SCOREP_USER_PARAMETER_DEFINE(uparam)
      SCOREP_USER_PARAMETER_DEFINE(sparam)
      integer :: type = SCOREP_USER_REGION_TYPE_FUNCTION

      SCOREP_USER_FUNC_BEGIN("ScorepTest")
      SCOREP_USER_REGION_INIT(region2,"Region2",type)
      SCOREP_USER_REGION_BEGIN(region1,"Region1",type)
      SCOREP_USER_REGION_ENTER(region2)
      SCOREP_USER_PARAMETER_INT64(iparam,"iparam",-1)
      SCOREP_USER_PARAMETER_UINT64(uparam,"uparam",-1)
      SCOREP_USER_PARAMETER_STRING(sparam,"sparam","hello")

      SCOREP_USER_METRIC_INIT(local2,"local2","s",0,0)
      SCOREP_USER_METRIC_INIT(local3,"local3","s",2,0)
      SCOREP_USER_METRIC_INT64(local2, i1)
      SCOREP_USER_METRIC_DOUBLE(local3, r1)

      SCOREP_USER_REGION_END(region2)
      SCOREP_USER_REGION_END(region1)
      SCOREP_USER_FUNC_END()

      SCOREP_RECORDING_OFF()
      SCOREP_RECORDING_IS_ON( l )
      if (l .eq. 0) then
         SCOREP_RECORDING_ON()  
      endif
      SCOREP_USER_REGION_BEGIN(region1,"Region1",type)
      SCOREP_USER_REGION_END(region1)
      end
#else

      program ScorepTest

      integer * 8 i1 
      double precision r1
      integer l
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

      SCOREP_USER_FUNC_BEGIN("ScorepTest")
      SCOREP_USER_REGION_INIT(region2,"Region2",0) 
      SCOREP_USER_REGION_BEGIN(region1,"Region1",0)
      SCOREP_USER_REGION_ENTER(region2)
      SCOREP_USER_PARAMETER_INT64(iparam,"iparam",-1)
      SCOREP_USER_PARAMETER_UINT64(uparam,"uparam",-1)
      SCOREP_USER_PARAMETER_STRING(sparam,"sparam","hello")

      SCOREP_USER_METRIC_INIT(local2,"local2","s",SCOREP_USER_METRIC_TYPE_INT64,SCOREP_USER_METRIC_CONTEXT_GLOBAL)
      SCOREP_USER_METRIC_INIT(local3,"local3","s",SCOREP_USER_METRIC_TYPE_DOUBLE,SCOREP_USER_METRIC_CONTEXT_GLOBAL)

      SCOREP_USER_METRIC_INT64(local2, i1)
      SCOREP_USER_METRIC_DOUBLE(local3, r1)

      SCOREP_USER_REGION_END(region2)
      SCOREP_USER_REGION_END(region1)
      SCOREP_USER_FUNC_END()

      SCOREP_RECORDING_OFF()
      SCOREP_RECORDING_IS_ON( l )
      if (l .eq. 0) then
         SCOREP_RECORDING_ON()  
      endif
      SCOREP_USER_REGION_BEGIN(region1,"Region1",SCOREP_USER_REGION_TYPE_FUNCTION)
      SCOREP_USER_REGION_END(region1)
      end
#endif
