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

#include "SILC_Mpi.h"
#include "config.h"

/**
 * @file  SILC_Mpi_Spawn.c
 *
 * @brief C interface wrappers for process creation and management
 *        functions (spawning interface)
 */

/**
 * @name C wrappers
 * @{
 */

#if defined( HAVE_DECL_MPI_COMM_ACCEPT ) && !defined( SILC_MPI_NO_SPAWN ) && !defined( SILC_MPI_NO_EXTRA )
/**
 * Measurement wrapper for MPI_Comm_accept
 * @note Auto-generated by wrapgen from template: SILC_Mpi_CommMgnt.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 */
int
MPI_Comm_accept( char*     port_name,
                 MPI_Info  info,
                 int       root,
                 MPI_Comm  comm,
                 MPI_Comm* newcomm )
{
    const int event_gen_active = SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_SPAWN );
    int       return_val;

    if ( event_gen_active )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_COMM_ACCEPT ] );
    }

    return_val = PMPI_Comm_accept( port_name, info, root, comm, newcomm );
    if ( *newcomm != MPI_COMM_NULL )
    {
        silc_mpi_comm_create( *newcomm );
    }

    if ( event_gen_active )
    {
        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_COMM_ACCEPT ] );
        SILC_MPI_EVENT_GEN_ON();
    }

    return return_val;
}
#endif
#if defined( HAVE_DECL_MPI_COMM_CONNECT ) && !defined( SILC_MPI_NO_SPAWN ) && !defined( SILC_MPI_NO_EXTRA )
/**
 * Measurement wrapper for MPI_Comm_connect
 * @note Auto-generated by wrapgen from template: SILC_Mpi_CommMgnt.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 */
int
MPI_Comm_connect( char*     port_name,
                  MPI_Info  info,
                  int       root,
                  MPI_Comm  comm,
                  MPI_Comm* newcomm )
{
    const int event_gen_active = SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_SPAWN );
    int       return_val;

    if ( event_gen_active )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_COMM_CONNECT ] );
    }

    return_val = PMPI_Comm_connect( port_name, info, root, comm, newcomm );
    if ( *newcomm != MPI_COMM_NULL )
    {
        silc_mpi_comm_create( *newcomm );
    }

    if ( event_gen_active )
    {
        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_COMM_CONNECT ] );
        SILC_MPI_EVENT_GEN_ON();
    }

    return return_val;
}
#endif
#if defined( HAVE_DECL_MPI_COMM_JOIN ) && !defined( SILC_MPI_NO_SPAWN ) && !defined( SILC_MPI_NO_EXTRA )
/**
 * Measurement wrapper for MPI_Comm_join
 * @note Auto-generated by wrapgen from template: SILC_Mpi_CommMgnt.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 */
int
MPI_Comm_join( int       fd,
               MPI_Comm* intercomm )
{
    const int event_gen_active = SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_SPAWN );
    int       return_val;

    if ( event_gen_active )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_COMM_JOIN ] );
    }

    return_val = PMPI_Comm_join( fd, intercomm );
    if ( *newcomm != MPI_COMM_NULL )
    {
        silc_mpi_comm_create( *newcomm );
    }

    if ( event_gen_active )
    {
        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_COMM_JOIN ] );
        SILC_MPI_EVENT_GEN_ON();
    }

    return return_val;
}
#endif
#if defined( HAVE_DECL_MPI_COMM_SPAWN ) && !defined( SILC_MPI_NO_SPAWN ) && !defined( SILC_MPI_NO_EXTRA )
/**
 * Measurement wrapper for MPI_Comm_spawn
 * @note Auto-generated by wrapgen from template: SILC_Mpi_CommMgnt.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 */
int
MPI_Comm_spawn( char*     command,
                char*     argv[],
                int       maxprocs,
                MPI_Info  info,
                int       root,
                MPI_Comm  comm,
                MPI_Comm* intercomm,
                int       array_of_errcodes[] )
{
    const int event_gen_active = SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_SPAWN );
    int       return_val;

    if ( event_gen_active )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_COMM_SPAWN ] );
    }

    return_val = PMPI_Comm_spawn( command, argv, maxprocs, info, root, comm, intercomm, array_of_errcodes );
    if ( *newcomm != MPI_COMM_NULL )
    {
        silc_mpi_comm_create( *newcomm );
    }

    if ( event_gen_active )
    {
        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_COMM_SPAWN ] );
        SILC_MPI_EVENT_GEN_ON();
    }

    return return_val;
}
#endif
#if defined( HAVE_DECL_MPI_COMM_SPAWN_MULTIPLE ) && !defined( SILC_MPI_NO_SPAWN ) && !defined( SILC_MPI_NO_EXTRA )
/**
 * Measurement wrapper for MPI_Comm_spawn_multiple
 * @note Auto-generated by wrapgen from template: SILC_Mpi_CommMgnt.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 */
int
MPI_Comm_spawn_multiple( int       count,
                         char*     array_of_commands[],
                         char**    array_of_argv[],
                         int       array_of_maxprocs[],
                         MPI_Info  array_of_info[],
                         int       root,
                         MPI_Comm  comm,
                         MPI_Comm* intercomm,
                         int       array_of_errcodes[] )
{
    const int event_gen_active = SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_SPAWN );
    int       return_val;

    if ( event_gen_active )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_COMM_SPAWN_MULTIPLE ] );
    }

    return_val = PMPI_Comm_spawn_multiple( count, array_of_commands, array_of_argv, array_of_maxprocs, array_of_info, root, comm, intercomm, array_of_errcodes );
    if ( *newcomm != MPI_COMM_NULL )
    {
        silc_mpi_comm_create( *newcomm );
    }

    if ( event_gen_active )
    {
        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_COMM_SPAWN_MULTIPLE ] );
        SILC_MPI_EVENT_GEN_ON();
    }

    return return_val;
}
#endif

#if defined( HAS_MPI2_SPAWN ) && defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Comm_disconnect
 * @note Auto-generated by wrapgen from template: comm_mgnt.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 */
int
MPI_Comm_disconnect( MPI_Comm* comm )
{
    const int event_gen_active = SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_SPAWN );
    int       return_val;

    if ( event_gen_active )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_COMM_DISCONNECT ] );
    }

    silc_mpi_comm_free( *comm );
    return_val = PMPI_Comm_disconnect( comm );

    if ( event_gen_active )
    {
        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_COMM_DISCONNECT ] );
        SILC_MPI_EVENT_GEN_ON();
    }

    return return_val;
}
#endif

#if defined( HAVE_DECL_MPI_COMM_GET_PARENT ) && !defined( SILC_MPI_NO_SPAWN ) && !defined( SILC_MPI_NO_EXTRA )
/**
 * Measurement wrapper for MPI_Comm_get_parent
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Comm_get_parent( MPI_Comm* parent )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_SPAWN ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_COMM_GET_PARENT ] );

        return_val = PMPI_Comm_get_parent( parent );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_COMM_GET_PARENT ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Comm_get_parent( parent );
    }

    return return_val;
}
#endif


#if defined( HAVE_DECL_MPI_LOOKUP_NAME ) && !defined( SILC_MPI_NO_SPAWN ) && !defined( SILC_MPI_NO_EXTRA )
/**
 * Measurement wrapper for MPI_Lookup_name
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Lookup_name( char*    service_name,
                 MPI_Info info,
                 char*    port_name )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_SPAWN ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_LOOKUP_NAME ] );

        return_val = PMPI_Lookup_name( service_name, info, port_name );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_LOOKUP_NAME ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Lookup_name( service_name, info, port_name );
    }

    return return_val;
}
#endif

#if defined( HAVE_DECL_MPI_PUBLISH_NAME ) && !defined( SILC_MPI_NO_SPAWN ) && !defined( SILC_MPI_NO_EXTRA )
/**
 * Measurement wrapper for MPI_Publish_name
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Publish_name( char*    service_name,
                  MPI_Info info,
                  char*    port_name )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_SPAWN ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_PUBLISH_NAME ] );

        return_val = PMPI_Publish_name( service_name, info, port_name );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_PUBLISH_NAME ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Publish_name( service_name, info, port_name );
    }

    return return_val;
}
#endif

#if defined( HAVE_DECL_MPI_UNPUBLISH_NAME ) && !defined( SILC_MPI_NO_SPAWN ) && !defined( SILC_MPI_NO_EXTRA )
/**
 * Measurement wrapper for MPI_Unpublish_name
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Unpublish_name( char*    service_name,
                    MPI_Info info,
                    char*    port_name )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_SPAWN ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_UNPUBLISH_NAME ] );

        return_val = PMPI_Unpublish_name( service_name, info, port_name );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_UNPUBLISH_NAME ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Unpublish_name( service_name, info, port_name );
    }

    return return_val;
}
#endif


#if defined( HAVE_DECL_MPI_CLOSE_PORT ) && !defined( SILC_MPI_NO_SPAWN ) && !defined( SILC_MPI_NO_EXTRA )
/**
 * Measurement wrapper for MPI_Close_port
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Close_port( char* port_name )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_SPAWN ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_CLOSE_PORT ] );

        return_val = PMPI_Close_port( port_name );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_CLOSE_PORT ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Close_port( port_name );
    }

    return return_val;
}
#endif

#if defined( HAVE_DECL_MPI_OPEN_PORT ) && !defined( SILC_MPI_NO_SPAWN ) && !defined( SILC_MPI_NO_EXTRA )
/**
 * Measurement wrapper for MPI_Open_port
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Open_port( MPI_Info info,
               char*    port_name )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_SPAWN ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_OPEN_PORT ] );

        return_val = PMPI_Open_port( info, port_name );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_OPEN_PORT ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Open_port( info, port_name );
    }

    return return_val;
}
#endif


/**
 * @}
 */
