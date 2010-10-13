!
!  This file is part of the SCOREP project (http://www.scorep.de)
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

!>
!! @file       SCOREP_Fmpi_Init.f
!! @brief      Fortran functions, called during the initialization of the mpi adapter to
!!             obtain the value of MPI_STATUS_SIZE.
!!
!! @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
!! @status     alpha
!! @ingroup    MPI_Wrapper
!<

      SUBROUTINE scorep_mpi_get_status_size___ (status_size)
      INTEGER status_size
      CALL scorep_mpi_get_status_size (status_size)
      END 

      SUBROUTINE scorep_mpi_get_status_size__ (status_size)
      INTEGER status_size
      CALL scorep_mpi_get_status_size (status_size)
      END

      SUBROUTINE scorep_mpi_get_status_size_ (status_size)
      INTEGER status_size
      CALL scorep_mpi_get_status_size (status_size)
      END

      SUBROUTINE scorep_mpi_get_status_size (status_size)
      INCLUDE 'mpif.h'
      INTEGER status_size
      status_size = MPI_STATUS_SIZE
      END

      SUBROUTINE scorep_mpi_fortran_init_cb___()
      call scorep_mpi_fortran_init_cb()
      END

      SUBROUTINE scorep_mpi_fortran_init_cb__()
      call scorep_mpi_fortran_init_cb()
      END

      SUBROUTINE scorep_mpi_fortran_init_cb_()
      call scorep_mpi_fortran_init_cb()
      END

      SUBROUTINE scorep_mpi_fortran_init_cb()
      INCLUDE  'mpif.h'
      CALL scorep_mpi_fortran_init_bottom(MPI_BOTTOM)
      CALL scorep_mpi_fortran_init_in_place(MPI_IN_PLACE)
      CALL scorep_mpi_fortran_init_status_ignore(MPI_STATUS_IGNORE)
      CALL scorep_mpi_fortran_init_statuses_ignore(MPI_STATUSES_IGNORE)
      END
      
