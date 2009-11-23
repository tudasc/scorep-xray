/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


#include "SILC_Mpi.h"
#include "config.h"

/**
 * @file  SILC_Mpi_Misc.c
 *
 * @brief C interface wrappers for miscelaneous and handler conversion
 *        functions
 */

/**
 * @name C wrappers
 * @{
 */

#if HAVE( DECL_MPI_ADDRESS ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MISC )
/**
 * Measurement wrapper for MPI_Address
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup misc
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Address( void*     location,
             MPI_Aint* address )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_MISC ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_ADDRESS ] );

        return_val = PMPI_Address( location, address );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_ADDRESS ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Address( location, address );
    }

    return return_val;
}
#endif

#if HAVE( DECL_MPI_ALLOC_MEM ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MISC )
/**
 * Measurement wrapper for MPI_Alloc_mem
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup misc
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Alloc_mem( MPI_Aint size,
               MPI_Info info,
               void*    baseptr )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_MISC ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_ALLOC_MEM ] );

        return_val = PMPI_Alloc_mem( size, info, baseptr );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_ALLOC_MEM ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Alloc_mem( size, info, baseptr );
    }

    return return_val;
}
#endif

#if HAVE( DECL_MPI_FREE_MEM ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MISC )
/**
 * Measurement wrapper for MPI_Free_mem
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup misc
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Free_mem( void* base )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_MISC ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_FREE_MEM ] );

        return_val = PMPI_Free_mem( base );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_FREE_MEM ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Free_mem( base );
    }

    return return_val;
}
#endif

#if HAVE( DECL_MPI_GET_ADDRESS ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MISC )
/**
 * Measurement wrapper for MPI_Get_address
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup misc
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Get_address( void*     location,
                 MPI_Aint* address )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_MISC ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_GET_ADDRESS ] );

        return_val = PMPI_Get_address( location, address );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_GET_ADDRESS ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Get_address( location, address );
    }

    return return_val;
}
#endif

#if HAVE( DECL_MPI_GET_VERSION ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MISC )
/**
 * Measurement wrapper for MPI_Get_version
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup misc
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Get_version( int* version,
                 int* subversion )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_MISC ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_GET_VERSION ] );

        return_val = PMPI_Get_version( version, subversion );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_GET_VERSION ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Get_version( version, subversion );
    }

    return return_val;
}
#endif

#if HAVE( DECL_MPI_INFO_CREATE ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MISC )
/**
 * Measurement wrapper for MPI_Info_create
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup misc
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Info_create( MPI_Info* info )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_MISC ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_INFO_CREATE ] );

        return_val = PMPI_Info_create( info );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_INFO_CREATE ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Info_create( info );
    }

    return return_val;
}
#endif

#if HAVE( DECL_MPI_INFO_DELETE ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MISC )
/**
 * Measurement wrapper for MPI_Info_delete
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup misc
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Info_delete( MPI_Info info,
                 char*    key )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_MISC ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_INFO_DELETE ] );

        return_val = PMPI_Info_delete( info, key );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_INFO_DELETE ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Info_delete( info, key );
    }

    return return_val;
}
#endif

#if HAVE( DECL_MPI_INFO_DUP ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MISC )
/**
 * Measurement wrapper for MPI_Info_dup
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup misc
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Info_dup( MPI_Info  info,
              MPI_Info* newinfo )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_MISC ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_INFO_DUP ] );

        return_val = PMPI_Info_dup( info, newinfo );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_INFO_DUP ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Info_dup( info, newinfo );
    }

    return return_val;
}
#endif

#if HAVE( DECL_MPI_INFO_FREE ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MISC )
/**
 * Measurement wrapper for MPI_Info_free
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup misc
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Info_free( MPI_Info* info )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_MISC ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_INFO_FREE ] );

        return_val = PMPI_Info_free( info );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_INFO_FREE ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Info_free( info );
    }

    return return_val;
}
#endif

#if HAVE( DECL_MPI_INFO_GET ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MISC )
/**
 * Measurement wrapper for MPI_Info_get
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup misc
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Info_get( MPI_Info info,
              char*    key,
              int      valuelen,
              char*    value,
              int*     flag )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_MISC ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_INFO_GET ] );

        return_val = PMPI_Info_get( info, key, valuelen, value, flag );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_INFO_GET ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Info_get( info, key, valuelen, value, flag );
    }

    return return_val;
}
#endif

#if HAVE( DECL_MPI_INFO_GET_NKEYS ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MISC )
/**
 * Measurement wrapper for MPI_Info_get_nkeys
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup misc
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Info_get_nkeys( MPI_Info info,
                    int*     nkeys )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_MISC ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_INFO_GET_NKEYS ] );

        return_val = PMPI_Info_get_nkeys( info, nkeys );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_INFO_GET_NKEYS ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Info_get_nkeys( info, nkeys );
    }

    return return_val;
}
#endif

#if HAVE( DECL_MPI_INFO_GET_NTHKEY ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MISC )
/**
 * Measurement wrapper for MPI_Info_get_nthkey
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup misc
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Info_get_nthkey( MPI_Info info,
                     int      n,
                     char*    key )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_MISC ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_INFO_GET_NTHKEY ] );

        return_val = PMPI_Info_get_nthkey( info, n, key );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_INFO_GET_NTHKEY ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Info_get_nthkey( info, n, key );
    }

    return return_val;
}
#endif

#if HAVE( DECL_MPI_INFO_GET_VALUELEN ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MISC )
/**
 * Measurement wrapper for MPI_Info_get_valuelen
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup misc
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Info_get_valuelen( MPI_Info info,
                       char*    key,
                       int*     valuelen,
                       int*     flag )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_MISC ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_INFO_GET_VALUELEN ] );

        return_val = PMPI_Info_get_valuelen( info, key, valuelen, flag );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_INFO_GET_VALUELEN ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Info_get_valuelen( info, key, valuelen, flag );
    }

    return return_val;
}
#endif

#if HAVE( DECL_MPI_INFO_SET ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MISC )
/**
 * Measurement wrapper for MPI_Info_set
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup misc
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Info_set( MPI_Info info,
              char*    key,
              char*    value )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_MISC ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_INFO_SET ] );

        return_val = PMPI_Info_set( info, key, value );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_INFO_SET ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Info_set( info, key, value );
    }

    return return_val;
}
#endif

#if HAVE( DECL_MPI_OP_CREATE ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MISC )
/**
 * Measurement wrapper for MPI_Op_create
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup misc
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Op_create( MPI_User_function* function,
               int                commute,
               MPI_Op*            op )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_MISC ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_OP_CREATE ] );

        return_val = PMPI_Op_create( function, commute, op );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_OP_CREATE ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Op_create( function, commute, op );
    }

    return return_val;
}
#endif

#if HAVE( DECL_MPI_OP_FREE ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MISC )
/**
 * Measurement wrapper for MPI_Op_free
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup misc
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Op_free( MPI_Op* op )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_MISC ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_OP_FREE ] );

        return_val = PMPI_Op_free( op );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_OP_FREE ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Op_free( op );
    }

    return return_val;
}
#endif

#if HAVE( DECL_MPI_REQUEST_GET_STATUS ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MISC )
/**
 * Measurement wrapper for MPI_Request_get_status
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup misc
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Request_get_status( MPI_Request request,
                        int*        flag,
                        MPI_Status* status )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_MISC ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_REQUEST_GET_STATUS ] );

        return_val = PMPI_Request_get_status( request, flag, status );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_REQUEST_GET_STATUS ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Request_get_status( request, flag, status );
    }

    return return_val;
}
#endif


/**
 * @}
 */
