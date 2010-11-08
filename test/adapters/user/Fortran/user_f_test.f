!
! This file is part of the SCOREP project (http:
!
! Copyright (c) 2009-2011,
! RWTH Aachen University, Germany
! Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
! Technische Universitaet Dresden, Germany
! University of Oregon, Eugene, USA
! Forschungszentrum Juelich GmbH, Germany
! German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
! Technische Universitaet Muenchen, Germany
!
! See the COPYING file in the package base directory for details.
!

#include <SCOREP_User.inc>

      SCOREP_GLOBAL_REGION_DEFINE(glo1)
      SCOREP_USER_METRIC_GROUP_GLOBAL(glo2)
      SCOREP_USER_METRIC_GLOBAL(glo3)

      program ScorepTest

      integer * 8 :: i1 = 19
      double precision :: r1 = 24.5
      logical :: l
      SCOREP_USER_REGION_DEFINE(region1)
      SCOREP_USER_FUNC_DEFINE
      SCOREP_USER_METRIC_GROUP_LOCAL(local1)
      SCOREP_USER_METRIC_LOCAL(local2)
      SCOREP_USER_METRIC_LOCAL(local3)

      SCOREP_USER_FUNC_BEGIN("ScorepTest")
      SCOREP_USER_METRIC_GROUP_INIT(local1,"local1")
      SCOREP_USER_REGION_BEGIN(region1,"Region1",SCOREP_USER_REGION_TYPE_FUNCTION)

      SCOREP_USER_METRIC_INIT(local2,"local2","s",SCOREP_USER_METRIC_TYPE_INT64,SCOREP_USER_METRIC_CONTEXT_GLOBAL,SCOREP_USER_METRIC_GROUP_DEFAULT)
      SCOREP_USER_METRIC_INIT(local3,"local3","s",SCOREP_USER_METRIC_TYPE_DOUBLE,SCOREP_USER_METRIC_CONTEXT_GLOBAL,local1)

      SCOREP_USER_METRIC_INT64(local2, i1)
      SCOREP_USER_METRIC_DOUBLE(local3, r1)

      SCOREP_USER_REGION_END(region1)
      SCOREP_USER_FUNC_END

      SCOREP_RECORDING_OFF
      SCOREP_RECORDING_IS_ON( l )
      if (.not. l) then
         SCOREP_RECORDING_ON  
      endif
      end program ScorepTest
