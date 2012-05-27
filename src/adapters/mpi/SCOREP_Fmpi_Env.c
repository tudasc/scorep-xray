/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 *    RWTH Aachen University, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
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
 * @file  SCOREP_Fmpi_Env.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     alpha
 * @ingroup    MPI_Wrapper
 *
 * @brief Fortran interface wrappers for environmental management
 */

#include <config.h>
#include "SCOREP_Fmpi.h"

#include <stdlib.h>
#include <scorep_utility/SCOREP_Error.h>

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
 * @name Fortran utility functions
 * @{
 */

/** lowercase define for initialization call for fortran MPI_BOTTOM */
#define scorep_mpi_fortran_init_bottom_L scorep_mpi_fortran_init_bottom
/** uppercase define for initialization call for fortran MPI_BOTTOM */
#define scorep_mpi_fortran_init_bottom_U SCOREP_MPI_FORTRAN_INIT_BOTTOM
/** lowercase define for initialization call for fortran MPI_IN_PLACE */
#define scorep_mpi_fortran_init_in_place_L scorep_mpi_fortran_init_in_place
/** uppercase define for initialization call for fortran MPI_IN_PLACE */
#define scorep_mpi_fortran_init_in_place_U SCOREP_MPI_FORTRAN_INIT_IN_PLACE
/** lowercase define for initialization call for fortran MPI_STATUS_IGNORE */
#define scorep_mpi_fortran_init_status_ignore_L scorep_mpi_fortran_init_status_ignore
/** uppercase define for initialization call for fortran MPI_STATUS_IGNORE */
#define scorep_mpi_fortran_init_status_ignore_U SCOREP_MPI_FORTRAN_INIT_STATUS_IGNORE
/** lowercase define for initialization call for fortran MPI_STATUSES_IGNORE */
#define scorep_mpi_fortran_init_statuses_ignore_L scorep_mpi_fortran_init_statuses_ignore
/** uppercase define for initialization call for fortran MPI_STATUSES_IGNORE */
#define scorep_mpi_fortran_init_statuses_ignore_U SCOREP_MPI_FORTRAN_INIT_STATUSES_IGNORE
/** lowercase define for initialization call for fortran MPI_UNWEIGHTED */
#define scorep_mpi_fortran_init_unweighted_L scorep_mpi_fortran_init_unweighted
/** uppercase define for initialization call for fortran MPI_UNWEIGHTED */
#define scorep_mpi_fortran_init_unweighted_U SCOREP_MPI_FORTRAN_INIT_UNWEIGHTED
/** address of fortran variable used as reference for MPI_UNWEIGHTED */

/** initialization function for fortran MPI_BOTTOM
 * @param bottom address of variable acting as reference for MPI_BOTTOM
 */
void
FSUB( scorep_mpi_fortran_init_bottom )( MPI_Fint* bottom )
{
#if HAVE( MPI_BOTTOM )
    scorep_mpi_fortran_bottom = bottom;
#endif
}

/** initialization function for fortran MPI_IN_PLACE
 * @param in_place address of variable acting as reference for MPI_IN_PLACE
 */
void
FSUB( scorep_mpi_fortran_init_in_place )( MPI_Fint* in_place )
{
#if HAVE( MPI_IN_PLACE )
    scorep_mpi_fortran_in_place = in_place;
#endif
}

/** initialization function for fortran MPI_STATUS_IGNORE
 * @param status_ignore address of variable acting as reference for MPI_STATUS_IGNORE
 */
void
FSUB( scorep_mpi_fortran_init_status_ignore )( MPI_Fint* status_ignore )
{
#if HAVE( MPI_STATUS_IGNORE )
    scorep_mpi_fortran_status_ignore = status_ignore;
#endif
}

/** initialization function for fortran MPI_STATUSES_IGNORE
 * @param statuses_ignore address of variable acting as reference for MPI_STATUSES_IGNORE
 */
void
FSUB( scorep_mpi_fortran_init_statuses_ignore )( MPI_Fint* statuses_ignore )
{
#if HAVE( MPI_STATUSES_IGNORE )
    scorep_mpi_fortran_statuses_ignore = statuses_ignore;
#endif
}

/** initialization function for fortran MPI_UNWEIGHTED
 * @param statuses_ignore address of variable acting as reference for MPI_UNWEIGHTED
 */
void
FSUB( scorep_mpi_fortran_init_unweighted )( MPI_Fint* unweighted )
{
#if HAVE( MPI_UNWEIGHTED )
    scorep_mpi_fortran_unweighted = unweighted;
#endif
}

/**
 * @}
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
    *ierr = MPI_Initialized( flag );
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
    *ierr = MPI_Init_thread( 0, ( char*** )0, *required, provided );
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
    *ierr = MPI_Init( 0, ( char*** )0 );
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
    *ierr = MPI_Finalize();
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
    *ierr = MPI_Finalized( flag );
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
    *ierr = MPI_Is_thread_main( flag );
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
    *ierr = MPI_Query_thread( provided );
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
    if ( sizeof( MPI_Fint ) != sizeof( int ) )
    {
        SCOREP_ERROR( SCOREP_ERROR_F2C_INT_SIZE_MISMATCH, "" );
    }
    *ierr = MPI_Init_thread( 0, ( char*** )0, *required, provided );
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
    if ( sizeof( MPI_Fint ) != sizeof( int ) )
    {
        SCOREP_ERROR( SCOREP_ERROR_F2C_INT_SIZE_MISMATCH, "" );
    }
    *ierr = MPI_Init( 0, ( char*** )0 );
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
    *ierr = MPI_Finalize();
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
    *ierr = MPI_Finalized( flag );
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
    *ierr = MPI_Is_thread_main( flag );
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
    *ierr = MPI_Query_thread( provided );
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
    *ierr = MPI_Initialized( flag );
}
#endif

#endif

/**
 * @}
 */
