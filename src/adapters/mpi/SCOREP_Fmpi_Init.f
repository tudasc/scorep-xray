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

!>
!! @file       SCOREP_Fmpi_Init.f
!! @brief      Fortran functions, called during the initialization of the mpi adapter to
!!             obtain the value of MPI_STATUS_SIZE.
!!
!! @ingroup    MPI_Wrapper
!<

      SUBROUTINE scorep_fortran_get_mpi_status_size (status_size)
      INCLUDE 'mpif.h'
      INTEGER status_size
      status_size = MPI_STATUS_SIZE
      END

      SUBROUTINE scorep_fortran_get_mpi_bottom()
      INCLUDE  'mpif.h'
      CALL scorep_mpi_fortran_init_bottom(MPI_BOTTOM)
      END

      SUBROUTINE scorep_fortran_get_mpi_in_place()
      INCLUDE  'mpif.h'
      CALL scorep_mpi_fortran_init_in_place(MPI_IN_PLACE)
      END

      SUBROUTINE scorep_fortran_get_mpi_status_ignore()
      INCLUDE  'mpif.h'
      CALL scorep_mpi_fortran_init_status_ignore(MPI_STATUS_IGNORE)
      END

      SUBROUTINE scorep_fortran_get_mpi_statuses_ignore()
      INCLUDE  'mpif.h'
      CALL scorep_mpi_fortran_init_statuses_ignore(MPI_STATUSES_IGNORE)
      END

      SUBROUTINE scorep_fortran_get_mpi_unweighted()
      INCLUDE  'mpif.h'
      CALL scorep_mpi_fortran_init_unweighted(MPI_UNWEIGHTED)
      END
      
