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
 * @file  SILC_Mpi_Err.c
 *
 * @brief C interface wrappers for error handling
 */

/**
 * @name C wrappers
 * @{
 */

#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Add_error_class
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup err
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Add_error_class
(
    int* errorclass
)
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_ERR ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_ADD_ERROR_CLASS ] );

        return_val = PMPI_Add_error_class( errorclass );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_ADD_ERROR_CLASS ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Add_error_class( errorclass );
    }

    return return_val;
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Add_error_code
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup err
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Add_error_code
(
    int  errorclass,
    int* errorcode
)
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_ERR ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_ADD_ERROR_CODE ] );

        return_val = PMPI_Add_error_code( errorclass, errorcode );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_ADD_ERROR_CODE ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Add_error_code( errorclass, errorcode );
    }

    return return_val;
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Add_error_string
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup err
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Add_error_string
(
    int   errorcode,
    char* string
)
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_ERR ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_ADD_ERROR_STRING ] );

        return_val = PMPI_Add_error_string( errorcode, string );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_ADD_ERROR_STRING ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Add_error_string( errorcode, string );
    }

    return return_val;
}
#endif
#if defined( HAS_MPI_EXTRA ) && defined( HAS_MPI_ERR )
/**
 * Measurement wrapper for MPI_Errhandler_create
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup err
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Errhandler_create
(
    MPI_Handler_function* function,
    MPI_Errhandler*       errhandler
)
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_ERR ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_ERRHANDLER_CREATE ] );

        return_val = PMPI_Errhandler_create( function, errhandler );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_ERRHANDLER_CREATE ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Errhandler_create( function, errhandler );
    }

    return return_val;
}
#endif
#if defined( HAS_MPI_EXTRA ) && defined( HAS_MPI_ERR )
/**
 * Measurement wrapper for MPI_Errhandler_free
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup err
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Errhandler_free
(
    MPI_Errhandler* errhandler
)
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_ERR ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_ERRHANDLER_FREE ] );

        return_val = PMPI_Errhandler_free( errhandler );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_ERRHANDLER_FREE ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Errhandler_free( errhandler );
    }

    return return_val;
}
#endif
#if defined( HAS_MPI_EXTRA ) && defined( HAS_MPI_ERR )
/**
 * Measurement wrapper for MPI_Errhandler_get
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup err
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Errhandler_get
(
    MPI_Comm        comm,
    MPI_Errhandler* errhandler
)
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_ERR ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_ERRHANDLER_GET ] );

        return_val = PMPI_Errhandler_get( comm, errhandler );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_ERRHANDLER_GET ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Errhandler_get( comm, errhandler );
    }

    return return_val;
}
#endif
#if defined( HAS_MPI_EXTRA ) && defined( HAS_MPI_ERR )
/**
 * Measurement wrapper for MPI_Errhandler_set
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup err
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Errhandler_set
(
    MPI_Comm       comm,
    MPI_Errhandler errhandler
)
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_ERR ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_ERRHANDLER_SET ] );

        return_val = PMPI_Errhandler_set( comm, errhandler );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_ERRHANDLER_SET ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Errhandler_set( comm, errhandler );
    }

    return return_val;
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Error_class
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup err
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Error_class
(
    int  errorcode,
    int* errorclass
)
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_ERR ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_ERROR_CLASS ] );

        return_val = PMPI_Error_class( errorcode, errorclass );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_ERROR_CLASS ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Error_class( errorcode, errorclass );
    }

    return return_val;
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Error_string
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup err
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Error_string
(
    int   errorcode,
    char* string,
    int*  resultlen
)
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_ERR ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_ERROR_STRING ] );

        return_val = PMPI_Error_string( errorcode, string, resultlen );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_ERROR_STRING ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Error_string( errorcode, string, resultlen );
    }

    return return_val;
}
#endif

/**
 * @}
 */
