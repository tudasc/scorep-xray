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

#include "scorep_utility/SCOREP_Utils.h"
#include "scorep_oa_connection.h"
#include "scorep_oa_sockets.h"

#include <stdio.h>
#include <mpi.h>

static int8_t scorep_oa_is_connected = 0;
static int    scorep_oa_socket       = 0;
static int    sil_oa_port            = 50010;
int           connection             = 0;


int
///@todo switch to appropriate connection object which contains call backs to the chosen communication layer
scorep_oa_connection_connect
(
)
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Entering %s\n", __FUNCTION__ );
    if ( scorep_oa_is_connected )
    {
        return SCOREP_SUCCESS;
    }
    sil_oa_port      = 50001;
    scorep_oa_socket = scorep_oa_sockets_server_startup_retry( &sil_oa_port, 10, 1 );
    if ( scorep_oa_socket == -1 )
    {
        //_Exit( EXIT_FAILURE );
        //return SCOREP_ERROR_MEMORY_OUT_OF_PAGES;				///@todo introduce my own error codes
    }
    scorep_oa_is_connected = 1;
    scorep_oa_sockets_register_with_registry( sil_oa_port );
    scorep_oa_socket = scorep_oa_sockets_server_accept_client( scorep_oa_socket );
    //receive_and_process_requests(scorep_oa_socket);
    return scorep_oa_socket;
}

SCOREP_Error_Code
scorep_oa_connection_disconnect
(
    int connection                              ///@todo switch to appropriate connection object which contains call backs to the chosen communication layer
)
{
    return SCOREP_SUCCESS;
}

SCOREP_Error_Code
scorep_oa_connection_send_string
(
    int         connection,                     ///@todo switch to appropriate connection object which contains call backs to the chosen communication layer
    const char* message_string
)
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA, "%s sending: %s\n", __FUNCTION__, message_string );
    scorep_oa_sockets_write_line( connection, message_string );
    return SCOREP_SUCCESS;
}

SCOREP_Error_Code
scorep_oa_connection_send_data
(
    int   connection,                           ///@todo switch to appropriate connection object which contains call backs to the chosen communication layer
    void* message_data,
    int   size,
    int   type_size
)
{
    scorep_oa_sockets_write_data( connection, &size, sizeof( int ) );
    scorep_oa_sockets_write_data( connection, message_data, size * type_size );   ///@todo switch to appropriate return measurement type
    return SCOREP_SUCCESS;
}


int
scorep_oa_connection_read_string
(
    int   connection,                   ///@todo switch to appropriate connection object which contains call backs to the chosen communication layer
    char* message_string,
    int   maxlen
)
{
    return scorep_oa_sockets_read_line( scorep_oa_socket, message_string, maxlen );                 ///@todo when connection object is deployed use it connection instead of socket
}
