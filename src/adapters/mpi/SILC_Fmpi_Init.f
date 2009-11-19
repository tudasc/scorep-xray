c     MPI_STATUS_SIZE

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
