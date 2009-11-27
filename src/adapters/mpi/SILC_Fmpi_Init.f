!
!  This file is part of the SILC project (http://www.silc.de)
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
!! @file       SILC_Fmpi_Init.f
!! @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
!! @status     BETA
!! @ingroup    MPI_Wrapper
!!
!! @brief Fortran functions, called during the initialization of the mpi adapter to
!!        obtain the value of MPI_STATUS_SIZE.
!<

      SUBROUTINE silc_mpi_get_status_size___ (status_size)
      INTEGER status_size
      CALL silc_mpi_get_status_size (status_size)
      END 

      SUBROUTINE silc_mpi_get_status_size__ (status_size)
      INTEGER status_size
      CALL silc_mpi_get_status_size (status_size)
      END

      SUBROUTINE silc_mpi_get_status_size_ (status_size)
      INTEGER status_size
      CALL silc_mpi_get_status_size (status_size)
      END

      SUBROUTINE silc_mpi_get_status_size (status_size)
      INCLUDE 'mpif.h'
      INTEGER status_size
      status_size = MPI_STATUS_SIZE
      END
