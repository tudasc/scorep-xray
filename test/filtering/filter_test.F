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

      subroutine Foo
      write (*,*) "In Foo"
      end subroutine Foo

      subroutine BAR
      write (*,*) "In BAR"
      end subroutine BAR

      subroutine baz
      write (*,*) "In baz"
      end subroutine baz

      program FilterTest

      write (*,*) "In main"
      call Foo
      call BAR
      call baz

      end program FilterTest
