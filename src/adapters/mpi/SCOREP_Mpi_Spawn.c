/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 *    RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
 *    Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 *    University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 *    Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 *    Technische Universitaet Muenchen, Germany
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
 * @file       SCOREP_Mpi_Spawn.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     alpha
 * @ingroup    MPI_Wrapper
 *
 * @brief C interface wrappers for process creation and management
 *        functions (spawning interface)
 */

#include <config.h>
#include "SCOREP_Mpi.h"
#include "scorep_mpi_communicator.h"
#include <SCOREP_Events.h>

/**
 * @name C wrappers
 * @{
 */

#if HAVE( DECL_PMPI_COMM_ACCEPT ) && !defined( SCOREP_MPI_NO_SPAWN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Comm_accept )
/**
 * Measurement wrapper for MPI_Comm_accept
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_CommMgnt.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 * It triggers an enter and an exit event. Additionally, between enter event and exit
 * event, @ref SCOREP_Definitions_NewInterimCommunicator is called.
 */
int
MPI_Comm_accept( SCOREP_MPI_CONST_DECL char* port_name, MPI_Info info, int root, MPI_Comm comm, MPI_Comm* newcomm )
{
    const int event_gen_active = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_SPAWN );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        SCOREP_EnterRegion( scorep_mpi_regid[ SCOREP__MPI_COMM_ACCEPT ] );
    }

    return_val = PMPI_Comm_accept( port_name, info, root, comm, newcomm );
    if ( *newcomm != MPI_COMM_NULL )
    {
        scorep_mpi_comm_create( *newcomm, comm );
    }

    if ( event_gen_active )
    {
        SCOREP_ExitRegion( scorep_mpi_regid[ SCOREP__MPI_COMM_ACCEPT ] );
        SCOREP_MPI_EVENT_GEN_ON();
    }

    return return_val;
}
#endif
#if HAVE( DECL_PMPI_COMM_CONNECT ) && !defined( SCOREP_MPI_NO_SPAWN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Comm_connect )
/**
 * Measurement wrapper for MPI_Comm_connect
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_CommMgnt.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 * It triggers an enter and an exit event. Additionally, between enter event and exit
 * event, @ref SCOREP_Definitions_NewInterimCommunicator is called.
 */
int
MPI_Comm_connect( SCOREP_MPI_CONST_DECL char* port_name, MPI_Info info, int root, MPI_Comm comm, MPI_Comm* newcomm )
{
    const int event_gen_active = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_SPAWN );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        SCOREP_EnterRegion( scorep_mpi_regid[ SCOREP__MPI_COMM_CONNECT ] );
    }

    return_val = PMPI_Comm_connect( port_name, info, root, comm, newcomm );
    if ( *newcomm != MPI_COMM_NULL )
    {
        scorep_mpi_comm_create( *newcomm, comm );
    }

    if ( event_gen_active )
    {
        SCOREP_ExitRegion( scorep_mpi_regid[ SCOREP__MPI_COMM_CONNECT ] );
        SCOREP_MPI_EVENT_GEN_ON();
    }

    return return_val;
}
#endif
#if HAVE( DECL_PMPI_COMM_JOIN ) && !defined( SCOREP_MPI_NO_SPAWN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Comm_join )
/**
 * Measurement wrapper for MPI_Comm_join
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_CommMgnt.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 * It triggers an enter and an exit event. Additionally, between enter event and exit
 * event, @ref SCOREP_Definitions_NewInterimCommunicator is called.
 */
int
MPI_Comm_join( int fd, MPI_Comm* newcomm )
{
    const int event_gen_active = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_SPAWN );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        SCOREP_EnterRegion( scorep_mpi_regid[ SCOREP__MPI_COMM_JOIN ] );
    }

    return_val = PMPI_Comm_join( fd, newcomm );
    if ( *newcomm != MPI_COMM_NULL )
    {
        scorep_mpi_comm_create( *newcomm, MPI_COMM_NULL );
    }

    if ( event_gen_active )
    {
        SCOREP_ExitRegion( scorep_mpi_regid[ SCOREP__MPI_COMM_JOIN ] );
        SCOREP_MPI_EVENT_GEN_ON();
    }

    return return_val;
}
#endif
#if HAVE( DECL_PMPI_COMM_SPAWN ) && !defined( SCOREP_MPI_NO_SPAWN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Comm_spawn )
/**
 * Measurement wrapper for MPI_Comm_spawn
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_CommMgnt.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 * It triggers an enter and an exit event. Additionally, between enter event and exit
 * event, @ref SCOREP_Definitions_NewInterimCommunicator is called.
 */
int
MPI_Comm_spawn( SCOREP_MPI_CONST_DECL char* command, char* argv[], int maxprocs, MPI_Info info, int root, MPI_Comm comm, MPI_Comm* newcomm, int array_of_errcodes[] )
{
    const int event_gen_active = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_SPAWN );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        SCOREP_EnterRegion( scorep_mpi_regid[ SCOREP__MPI_COMM_SPAWN ] );
    }

    return_val = PMPI_Comm_spawn( command, argv, maxprocs, info, root, comm, newcomm, array_of_errcodes );
    if ( *newcomm != MPI_COMM_NULL )
    {
        scorep_mpi_comm_create( *newcomm, comm );
    }

    if ( event_gen_active )
    {
        SCOREP_ExitRegion( scorep_mpi_regid[ SCOREP__MPI_COMM_SPAWN ] );
        SCOREP_MPI_EVENT_GEN_ON();
    }

    return return_val;
}
#endif
#if HAVE( DECL_PMPI_COMM_SPAWN_MULTIPLE ) && !defined( SCOREP_MPI_NO_SPAWN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Comm_spawn_multiple )
/**
 * Measurement wrapper for MPI_Comm_spawn_multiple
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_CommMgnt.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 * It triggers an enter and an exit event. Additionally, between enter event and exit
 * event, @ref SCOREP_Definitions_NewInterimCommunicator is called.
 */
int
MPI_Comm_spawn_multiple( int count, char* array_of_commands[], char** array_of_argv[], SCOREP_MPI_CONST_DECL int array_of_maxprocs[], SCOREP_MPI_CONST_DECL MPI_Info array_of_info[], int root, MPI_Comm comm, MPI_Comm* newcomm, int array_of_errcodes[] )
{
    const int event_gen_active = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_SPAWN );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        SCOREP_EnterRegion( scorep_mpi_regid[ SCOREP__MPI_COMM_SPAWN_MULTIPLE ] );
    }

    return_val = PMPI_Comm_spawn_multiple( count, array_of_commands, array_of_argv, array_of_maxprocs, array_of_info, root, comm, newcomm, array_of_errcodes );
    if ( *newcomm != MPI_COMM_NULL )
    {
        scorep_mpi_comm_create( *newcomm, comm );
    }

    if ( event_gen_active )
    {
        SCOREP_ExitRegion( scorep_mpi_regid[ SCOREP__MPI_COMM_SPAWN_MULTIPLE ] );
        SCOREP_MPI_EVENT_GEN_ON();
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_COMM_DISCONNECT ) && !defined( SCOREP_MPI_NO_SPAWN )
/**
 * Measurement wrapper for MPI_Comm_disconnect
 * @note Auto-generated by wrapgen from template: comm_mgnt.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 * Triggers an enter and exit event.
 */
int
MPI_Comm_disconnect( MPI_Comm* comm )
{
    const int event_gen_active = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_SPAWN );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        SCOREP_EnterRegion( scorep_mpi_regid[ SCOREP__MPI_COMM_DISCONNECT ] );
    }

    scorep_mpi_comm_free( *comm );
    return_val = PMPI_Comm_disconnect( comm );

    if ( event_gen_active )
    {
        SCOREP_ExitRegion( scorep_mpi_regid[ SCOREP__MPI_COMM_DISCONNECT ] );
        SCOREP_MPI_EVENT_GEN_ON();
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_COMM_GET_PARENT ) && !defined( SCOREP_MPI_NO_SPAWN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Comm_get_parent )
/**
 * Measurement wrapper for MPI_Comm_get_parent
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 * Triggers an enter and exit event.
 * It wraps the MPI_Comm_get_parent call with enter and exit events.
 */
int
MPI_Comm_get_parent( MPI_Comm* parent )
{
    int return_val;

    if ( SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_SPAWN ) )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        SCOREP_EnterRegion( scorep_mpi_regid[ SCOREP__MPI_COMM_GET_PARENT ] );

        return_val = PMPI_Comm_get_parent( parent );

        SCOREP_ExitRegion( scorep_mpi_regid[ SCOREP__MPI_COMM_GET_PARENT ] );
        SCOREP_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Comm_get_parent( parent );
    }

    return return_val;
}
#endif


#if HAVE( DECL_PMPI_LOOKUP_NAME ) && !defined( SCOREP_MPI_NO_SPAWN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Lookup_name )
/**
 * Measurement wrapper for MPI_Lookup_name
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 * Triggers an enter and exit event.
 * It wraps the MPI_Lookup_name call with enter and exit events.
 */
int
MPI_Lookup_name( SCOREP_MPI_CONST_DECL char* service_name, MPI_Info info, char* port_name )
{
    int return_val;

    if ( SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_SPAWN ) )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        SCOREP_EnterRegion( scorep_mpi_regid[ SCOREP__MPI_LOOKUP_NAME ] );

        return_val = PMPI_Lookup_name( service_name, info, port_name );

        SCOREP_ExitRegion( scorep_mpi_regid[ SCOREP__MPI_LOOKUP_NAME ] );
        SCOREP_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Lookup_name( service_name, info, port_name );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_PUBLISH_NAME ) && !defined( SCOREP_MPI_NO_SPAWN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Publish_name )
/**
 * Measurement wrapper for MPI_Publish_name
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 * Triggers an enter and exit event.
 * It wraps the MPI_Publish_name call with enter and exit events.
 */
int
MPI_Publish_name( SCOREP_MPI_CONST_DECL char* service_name, MPI_Info info, SCOREP_MPI_CONST_DECL char* port_name )
{
    int return_val;

    if ( SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_SPAWN ) )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        SCOREP_EnterRegion( scorep_mpi_regid[ SCOREP__MPI_PUBLISH_NAME ] );

        return_val = PMPI_Publish_name( service_name, info, port_name );

        SCOREP_ExitRegion( scorep_mpi_regid[ SCOREP__MPI_PUBLISH_NAME ] );
        SCOREP_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Publish_name( service_name, info, port_name );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_UNPUBLISH_NAME ) && !defined( SCOREP_MPI_NO_SPAWN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Unpublish_name )
/**
 * Measurement wrapper for MPI_Unpublish_name
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 * Triggers an enter and exit event.
 * It wraps the MPI_Unpublish_name call with enter and exit events.
 */
int
MPI_Unpublish_name( SCOREP_MPI_CONST_DECL char* service_name, MPI_Info info, SCOREP_MPI_CONST_DECL char* port_name )
{
    int return_val;

    if ( SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_SPAWN ) )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        SCOREP_EnterRegion( scorep_mpi_regid[ SCOREP__MPI_UNPUBLISH_NAME ] );

        return_val = PMPI_Unpublish_name( service_name, info, port_name );

        SCOREP_ExitRegion( scorep_mpi_regid[ SCOREP__MPI_UNPUBLISH_NAME ] );
        SCOREP_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Unpublish_name( service_name, info, port_name );
    }

    return return_val;
}
#endif


#if HAVE( DECL_PMPI_CLOSE_PORT ) && !defined( SCOREP_MPI_NO_SPAWN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Close_port )
/**
 * Measurement wrapper for MPI_Close_port
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 * Triggers an enter and exit event.
 * It wraps the MPI_Close_port call with enter and exit events.
 */
int
MPI_Close_port( SCOREP_MPI_CONST_DECL char* port_name )
{
    int return_val;

    if ( SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_SPAWN ) )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        SCOREP_EnterRegion( scorep_mpi_regid[ SCOREP__MPI_CLOSE_PORT ] );

        return_val = PMPI_Close_port( port_name );

        SCOREP_ExitRegion( scorep_mpi_regid[ SCOREP__MPI_CLOSE_PORT ] );
        SCOREP_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Close_port( port_name );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_OPEN_PORT ) && !defined( SCOREP_MPI_NO_SPAWN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Open_port )
/**
 * Measurement wrapper for MPI_Open_port
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 * Triggers an enter and exit event.
 * It wraps the MPI_Open_port call with enter and exit events.
 */
int
MPI_Open_port( MPI_Info info, char* port_name )
{
    int return_val;

    if ( SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_SPAWN ) )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        SCOREP_EnterRegion( scorep_mpi_regid[ SCOREP__MPI_OPEN_PORT ] );

        return_val = PMPI_Open_port( info, port_name );

        SCOREP_ExitRegion( scorep_mpi_regid[ SCOREP__MPI_OPEN_PORT ] );
        SCOREP_MPI_EVENT_GEN_ON();
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
