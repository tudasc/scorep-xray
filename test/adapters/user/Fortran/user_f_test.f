!
! This file is part of the SILC project (http:
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
      blockdata glo1g;integer::glo1;common/glo1id/glo1;data glo1/-1/;end
      blockdata glo2g;integer::glo2;common/glo2id/glo2;data glo2/-1/;end
      blockdata glo3g;integer::glo3;common/glo3id/glo3;data glo3/-1/;end

      program SilcTest

      integer * 8 :: i1 = 19
      double precision :: r1 = 24.5
      logical :: l
      integer :: region1 = -1
      integer :: silcufh = -1
      integer :: local1 = -1
      integer :: local2 = -1
      integer :: local3 = -1

      write (*,*) "real type:", r8
      call SILC_User_RegionBeginF( silcufh, "SilcTest", 1, "test.f", 19)
      call SILC_User_InitMetricGroupF( local1, "local1" )

      call SILC_User_RegionBeginF( region1, "Region1", 0, "test.f", 23)


      call SILC_User_InitMetricF( local2, "local2", "s", 0, 0, -2 )

      call SILC_User_InitMetricF( local3, "local3", "s", 1, 0, local1 )


      call SILC_User_MetricInt64F( local2, i1 )
      call SILC_User_MetricDoubleF( local3, r1 )

      call SILC_User_RegionEndF(region1)
      call SILC_User_RegionEndF(silcufh)

      call SILC_User_DisableRecordingF()
      call SILC_User_RecordingEnabledF( l )
      if (.not. l) then
        call SILC_User_EnableRecordingF()
      endif
      end program SilcTest
