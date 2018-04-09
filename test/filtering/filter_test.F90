!
! This file is part of the Score-P software (http://www.score-p.org)
!
! Copyright (c) 2009-2011,
! RWTH Aachen University, Germany
!
! Copyright (c) 2009-2011,
! Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
!
! Copyright (c) 2009-2011, 2014-2015,
! Technische Universitaet Dresden, Germany
!
! Copyright (c) 2009-2011,
! University of Oregon, Eugene, USA
!
! Copyright (c) 2009-2011,
! Forschungszentrum Juelich GmbH, Germany
!
! Copyright (c) 2009-2011,
! German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
!
! Copyright (c) 2009-2011,
! Technische Universitaet Muenchen, Germany
!
! This software may be modified and distributed under the terms of
! a BSD-style license.  See the COPYING file in the package base
! directory for details.
!

#include <scorep/SCOREP_User.inc>

      subroutine Foo
      implicit none
      SCOREP_USER_FUNC_DEFINE()
      SCOREP_USER_FUNC_BEGIN( "foo" )
      write (*,*) "In Foo"
      SCOREP_USER_FUNC_END()
      end subroutine Foo

      subroutine BAR
      implicit none
      SCOREP_USER_FUNC_DEFINE()
      SCOREP_USER_FUNC_BEGIN( "BAR" )
      write (*,*) "In BAR"
      SCOREP_USER_FUNC_END()
      end subroutine BAR

      subroutine baz
      implicit none
      SCOREP_USER_FUNC_DEFINE()
      SCOREP_USER_FUNC_BEGIN( "baz" )
      write (*,*) "In baz"
      SCOREP_USER_FUNC_END()
      end subroutine baz

      program FilterTest
      implicit none
      SCOREP_USER_FUNC_DEFINE()
      SCOREP_USER_FUNC_BEGIN( "MAIN" )

      write (*,*) "In main"
      call Foo
      call BAR
      call baz

      SCOREP_USER_FUNC_END()
      end program FilterTest
