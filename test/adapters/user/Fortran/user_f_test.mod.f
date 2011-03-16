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
      blockdataglo1g;integer*8::glo1;common/glo1id/glo1;dataglo1/-1/;end
      blockdataglo2g;integer*8::glo2;common/glo2id/glo2;dataglo2/-1/;end
      blockdataglo3g;integer*8::glo3;common/glo3id/glo3;dataglo3/-1/;end
      program ScorepTest
      integer * 8 i1
      double precision r1
      integer l
      integer*8 region1
      integer*8 scrpfh
      integer*8 local1
      integer*8 local2
      integer*8 local3
      call SCOREP_F_Begin(scrpfh,"ScorepTest",1,"user_test.f",32)
      call SCOREP_F_InitMetricGroup(local1,"local1")
      call SCOREP_F_Begin(region1,"Region1",1,"user_test.f",34)
      call SCOREP_F_InitMetric(local2,"local2","s",0,0,-2)
      call SCOREP_F_InitMetric(local3,"local3","s",1,0,local1)
      call SCOREP_F_MetricInt64( local2, i1 )
      call SCOREP_F_MetricDouble( local3, r1 )
      call SCOREP_F_RegionEnd( region1 )
      call SCOREP_F_RegionEnd( scrpfh )
      call SCOREP_F_DisableRecording()
      call SCOREP_F_RecordingEnabled( l )
      if (l .eq. 0) then
         call SCOREP_F_EnableRecording()
      endif
      end program ScorepTest
