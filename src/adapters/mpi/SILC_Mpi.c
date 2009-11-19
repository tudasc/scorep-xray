/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
**  KOJAK       http://www.fz-juelich.de/jsc/kojak/                        **
*****************************************************************************
**  Copyright (c) 1998-2009                                                **
**  Forschungszentrum Juelich, Juelich Supercomputing Centre               **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

/**
   @file  SILC_Mpi.c
   @brief Main file for C interface measurement wrappers
   @ingroup MPI_Wrapper
 */

#include "SILC_Mpi.h"

MPI_Status* silc_my_status_array = 0;

int32_t     silc_my_status_array_size = 0;

MPI_Status*
silc_get_status_array( int32_t size )
{
    if ( silc_my_status_array_size == 0 )
    {
        /* -- never used: initialize -- */
        silc_my_status_array = malloc( size * sizeof( MPI_Status ) );
        if ( silc_my_status_array == NULL )
        {
            SILC_ERROR_POSIX();
        }
        silc_my_status_array_size = size;
    }
    else if ( size > silc_my_status_array_size )
    {
        /* -- not enough room: expand -- */
        silc_my_status_array = realloc( silc_my_status_array, size * sizeof( MPI_Status ) );
        if ( silc_my_status_array == NULL )
        {
            SILC_ERROR_POSIX();
        }
        silc_my_status_array_size = size;
    }
    return silc_my_status_array;
}

/** Flag to indicate whether event generation is turned on or off. If it is set to 0,
    events are generated. If it is set to non-zero, no events are generated.
 */
int32_t silc_mpi_nogen = 0;
