/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2018,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * Copyright (c) 2016,
 * Technische Universitaet Darmstadt, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2011                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2010-2011                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


/**
 * @file
 * @ingroup    MPI_Wrapper
 *
 * @brief Fortran interface wrappers for environmental management
 */

#include <config.h>
#include "SCOREP_Fmpi.h"

#include <stdlib.h>
#include <UTILS_Error.h>

/* uppercase defines */
/** @def MPI_Finalize_U
    Exchange MPI_Finalize_U by MPI_FINALIZE.
    It is used for the Fortran wrappers of MPI_Finalize.
 */
#define MPI_Finalize_U MPI_FINALIZE

/** @def MPI_Finalized_U
    Exchange MPI_Finalized_U by MPI_FINALIZED.
    It is used for the Fortran wrappers of MPI_Finalized.
 */
#define MPI_Finalized_U MPI_FINALIZED

/** @def MPI_Get_library_version_U
    Exchange MPI_Get_library_version_U by MPI_GET_LIBRARY_VERSION.
    It is used for the Fortran wrappers of MPI_Get_library_version.
 */
#define MPI_Get_library_version_U MPI_GET_LIBRARY_VERSION

/** @def MPI_Init_U
    Exchange MPI_Init_U by MPI_INIT.
    It is used for the Fortran wrappers of MPI_Init.
 */
#define MPI_Init_U MPI_INIT

/** @def MPI_Init_thread_U
    Exchange MPI_Init_thread_U by MPI_INIT_THREAD.
    It is used for the Fortran wrappers of MPI_Init_thread.
 */
#define MPI_Init_thread_U MPI_INIT_THREAD

/** @def MPI_Initialized_U
    Exchange MPI_Initialized_U by MPI_INITIALIZED.
    It is used for the Fortran wrappers of MPI_Initialized.
 */
#define MPI_Initialized_U MPI_INITIALIZED

/** @def MPI_Is_thread_main_U
    Exchange MPI_Is_thread_main_U by MPI_IS_THREAD_MAIN.
    It is used for the Fortran wrappers of MPI_Is_thread_main.
 */
#define MPI_Is_thread_main_U MPI_IS_THREAD_MAIN

/** @def MPI_Query_thread_U
    Exchange MPI_Query_thread_U by MPI_QUERY_THREAD.
    It is used for the Fortran wrappers of MPI_Query_thread.
 */
#define MPI_Query_thread_U MPI_QUERY_THREAD


/* lowercase defines */
/** @def MPI_Finalize_L
    Exchanges MPI_Finalize_L by mpi_finalize.
    It is used for the Fortran wrappers of MPI_Finalize.
 */
#define MPI_Finalize_L mpi_finalize

/** @def MPI_Finalized_L
    Exchanges MPI_Finalized_L by mpi_finalized.
    It is used for the Fortran wrappers of MPI_Finalized.
 */
#define MPI_Finalized_L mpi_finalized

/** @def MPI_Get_library_version_L
    Exchanges MPI_Get_library_version_L by mpi_get_library_version.
    It is used for the Fortran wrappers of MPI_Get_library_version.
 */
#define MPI_Get_library_version_L mpi_get_library_version

/** @def MPI_Init_L
    Exchanges MPI_Init_L by mpi_init.
    It is used for the Fortran wrappers of MPI_Init.
 */
#define MPI_Init_L mpi_init

/** @def MPI_Init_thread_L
    Exchanges MPI_Init_thread_L by mpi_init_thread.
    It is used for the Fortran wrappers of MPI_Init_thread.
 */
#define MPI_Init_thread_L mpi_init_thread

/** @def MPI_Initialized_L
    Exchanges MPI_Initialized_L by mpi_initialized.
    It is used for the Fortran wrappers of MPI_Initialized.
 */
#define MPI_Initialized_L mpi_initialized

/** @def MPI_Is_thread_main_L
    Exchanges MPI_Is_thread_main_L by mpi_is_thread_main.
    It is used for the Fortran wrappers of MPI_Is_thread_main.
 */
#define MPI_Is_thread_main_L mpi_is_thread_main

/** @def MPI_Query_thread_L
    Exchanges MPI_Query_thread_L by mpi_query_thread.
    It is used for the Fortran wrappers of MPI_Query_thread.
 */
#define MPI_Query_thread_L mpi_query_thread


/**
 * @name Fortran wrappers
 * @{
 */

#ifndef NEED_F2C_CONV

#if HAVE( DECL_PMPI_INITIALIZED ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ENV ) && !defined( MPI_Initialized )
/**
 * Measurement wrapper for MPI_Initialized
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup env
 */
void
FSUB( MPI_Initialized )( int* flag, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Initialized( flag );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

/* If MPI_Comm_f2c is HAVE as a simple macro (which typically does nothing)
 * as it would require a "real" function if it is really needed
 * => we can save the f2c and c2s conversions */

#if HAVE( DECL_PMPI_INIT_THREAD )
/**
 * Measurement wrapper for MPI_Init_tread
 * @note Manually adapted wrapper
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup env
 */
void
FSUB( MPI_Init_thread )( int* required,
                         int* provided,
                         int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    *ierr = MPI_Init_thread( 0, ( char*** )0, *required, provided );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

/**
 * Measurement wrapper for MPI_Init
 * @note Manually adapted wrapper
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup env
 */
void
FSUB( MPI_Init )( int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    *ierr = MPI_Init( 0, ( char*** )0 );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

#if HAVE( DECL_PMPI_FINALIZE )
/**
 * Measurement wrapper for MPI_Finalize
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup env
 */
void
FSUB( MPI_Finalize )( int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Finalize();

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FINALIZED ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ENV ) && !defined( MPI_Finalized )
/**
 * Measurement wrapper for MPI_Finalized
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup env
 */
void
FSUB( MPI_Finalized )( int* flag, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Finalized( flag );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_GET_LIBRARY_VERSION ) && !defined( SCOREP_MPI_NO_ENV ) && !defined( MPI_Get_library_version )
/**
 * Measurement wrapper for MPI_Get_library_version
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-3
 * @ingroup env
 */
void
FSUB( MPI_Get_library_version )( char* version, int* resultlen, int* ierr, scorep_fortran_charlen_t version_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    char*  c_version     = NULL;
    size_t c_version_len = 0;
    c_version = ( char* )malloc( ( version_len + 1 ) * sizeof( char ) );
    if ( !c_version )
    {
        exit( EXIT_FAILURE );
    }



    *ierr = MPI_Get_library_version( c_version, resultlen );


    c_version_len = strlen( c_version );
    strncpy( version, c_version, c_version_len );
    memset( version + c_version_len, ' ', version_len - c_version_len );
    free( c_version );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_IS_THREAD_MAIN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ENV ) && !defined( MPI_Is_thread_main )
/**
 * Measurement wrapper for MPI_Is_thread_main
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup env
 */
void
FSUB( MPI_Is_thread_main )( int* flag, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Is_thread_main( flag );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_QUERY_THREAD ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ENV ) && !defined( MPI_Query_thread )
/**
 * Measurement wrapper for MPI_Query_thread
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup env
 */
void
FSUB( MPI_Query_thread )( int* provided, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Query_thread( provided );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#else /* !NEED_F2C_CONV */

#if HAVE( DECL_PMPI_INIT_THREAD )
/**
 * Measurement wrapper for MPI_Init_tread
 * @note Manually adapted wrapper
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup env
 */
void
FSUB( MPI_Init_thread )( MPI_Fint* required,
                         MPI_Fint* provided,
                         MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( sizeof( MPI_Fint ) != sizeof( int ) )
    {
        UTILS_ERROR( SCOREP_ERROR_F2C_INT_SIZE_MISMATCH,
                     "Shouldn't this error abort!" );
    }
    *ierr = MPI_Init_thread( 0, ( char*** )0, *required, provided );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

/**
 * Measurement wrapper for MPI_Init
 * @note Manually adapted wrapper
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup env
 */
void
FSUB( MPI_Init )( MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( sizeof( MPI_Fint ) != sizeof( int ) )
    {
        UTILS_ERROR( SCOREP_ERROR_F2C_INT_SIZE_MISMATCH,
                     "Shouldn't this error abort!" );
    }
    *ierr = MPI_Init( 0, ( char*** )0 );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

#if HAVE( DECL_PMPI_FINALIZE )
/**
 * Measurement wrapper for MPI_Finalize
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup env
 * For the order of events see @ref MPI_Finalize
 */
void
FSUB( MPI_Finalize )( MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Finalize();

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_FINALIZED ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ENV ) && !defined( MPI_Finalized )
/**
 * Measurement wrapper for MPI_Finalized
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup env
 * For the order of events see @ref MPI_Finalized
 */
void
FSUB( MPI_Finalized )( MPI_Fint* flag, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Finalized( flag );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_GET_LIBRARY_VERSION ) && !defined( SCOREP_MPI_NO_ENV ) && !defined( MPI_Get_library_version )
/**
 * Measurement wrapper for MPI_Get_library_version
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-3
 * @ingroup env
 * For the order of events see @ref MPI_Get_library_version
 */
void
FSUB( MPI_Get_library_version )( char* version, MPI_Fint* resultlen, MPI_Fint* ierr, scorep_fortran_charlen_t version_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    char*  c_version     = NULL;
    size_t c_version_len = 0;
    c_version = ( char* )malloc( ( version_len + 1 ) * sizeof( char ) );
    if ( !c_version )
    {
        exit( EXIT_FAILURE );
    }



    *ierr = MPI_Get_library_version( c_version, resultlen );


    c_version_len = strlen( c_version );
    strncpy( version, c_version, c_version_len );
    memset( version + c_version_len, ' ', version_len - c_version_len );
    free( c_version );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_IS_THREAD_MAIN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ENV ) && !defined( MPI_Is_thread_main )
/**
 * Measurement wrapper for MPI_Is_thread_main
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup env
 * For the order of events see @ref MPI_Is_thread_main
 */
void
FSUB( MPI_Is_thread_main )( MPI_Fint* flag, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Is_thread_main( flag );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_QUERY_THREAD ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ENV ) && !defined( MPI_Query_thread )
/**
 * Measurement wrapper for MPI_Query_thread
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup env
 * For the order of events see @ref MPI_Query_thread
 */
void
FSUB( MPI_Query_thread )( MPI_Fint* provided, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Query_thread( provided );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#if HAVE( DECL_PMPI_INITIALIZED ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ENV ) && !defined( MPI_Initialized )
/**
 * Measurement wrapper for MPI_Initialized
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup env
 */
void
FSUB( MPI_Initialized )( int* flag, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Initialized( flag );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#endif

/**
 * @}
 */
