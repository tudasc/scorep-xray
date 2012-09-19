/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
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

/** @file scorep_oa_connection.c
 *  @maintainer Yury Oleynik <oleynik@in.tum.de>
 *  @status alpha
 *
 *  This file contains the implementation of communication layer functions
 */


#include <config.h>

#include "scorep_oa_connection.h"
#include "scorep_oa_sockets.h"
#include "scorep_mpi.h"

#include <stdio.h>


static int8_t scorep_oa_is_connected = 0;
static int    scorep_oa_socket       = 0;
int           connection             = 0;



int
scorep_oa_connection_connect
(
)
{
    UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "Entering %s", __func__ );
    if ( scorep_oa_is_connected )
    {
        return SCOREP_SUCCESS;
    }
    if ( SCOREP_Mpi_HasMpi() )
    {
        silc_oa_port = silc_oa_port + SCOREP_Mpi_GetRank();
    }
    scorep_oa_socket = scorep_oa_sockets_server_startup_retry( &silc_oa_port, 10, 1 );
    if ( scorep_oa_socket == -1 )
    {
        _Exit( EXIT_FAILURE );
    }
    scorep_oa_is_connected = 1;
    scorep_oa_sockets_register_with_registry( silc_oa_port, silc_oa_registry_port, silc_oa_registry_host, silc_oa_app_name );
    scorep_oa_socket = scorep_oa_sockets_server_accept_client( scorep_oa_socket );
    //receive_and_process_requests(scorep_oa_socket);
    return scorep_oa_socket;
}

SCOREP_ErrorCode
scorep_oa_connection_disconnect
(
    int connection
)
{
    return SCOREP_SUCCESS;
}

SCOREP_ErrorCode
scorep_oa_connection_send_string
(
    int         connection,
    const char* message_string
)
{
    UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "%s sending: %s", __func__, message_string );
    scorep_oa_sockets_write_line( connection, message_string );
    return SCOREP_SUCCESS;
}

SCOREP_ErrorCode
scorep_oa_connection_send_data
(
    int   connection,
    void* message_data,
    int   size,
    int   type_size
)
{
    scorep_oa_sockets_write_data( connection, &size, sizeof( int ) );
    scorep_oa_sockets_write_data( connection, message_data, size * type_size );
    return SCOREP_SUCCESS;
}


int
scorep_oa_connection_read_string
(
    int   connection,
    char* message_string,
    int   maxlen
)
{
    return scorep_oa_sockets_read_line( scorep_oa_socket, message_string, maxlen );
}
