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

/** @file silc_oa_connection.c
    @maintainer Yury Oleynik <oleynik@in.tum.de>
    @status     ALPHA

    This file contains the implementation of communication layer functions
 */


#include <config.h>

#include "SILC_Error.h"
#include "silc_oa_connection.h"
#include "silc_oa_sockets.h"

#include <stdio.h>
#include <mpi.h>

static int8_t silc_oa_is_connected = 0;
static int    silc_oa_socket       = 0;
static int    sil_oa_port          = 50010;
int           connection           = 0;


int
///@todo switch to appropriate connection object which contains call backs to the chosen communication layer
silc_oa_connection_connect
(
)
{
    printf( "Entering %s\n", __FUNCTION__ );
    if ( silc_oa_is_connected )
    {
        return SILC_SUCCESS;
    }
    sil_oa_port    = 50001;
    silc_oa_socket = silc_oa_sockets_server_startup_retry( &sil_oa_port, 10, 1 );
    if ( silc_oa_socket == -1 )
    {
        //_Exit( EXIT_FAILURE );
        //return SILC_ERROR_MEMORY_OUT_OF_PAGES;				///@todo introduce my own error codes
    }
    silc_oa_is_connected = 1;
    silc_oa_sockets_register_with_registry( sil_oa_port );
    silc_oa_socket = silc_oa_sockets_server_accept_client( silc_oa_socket );
    //receive_and_process_requests(silc_oa_socket);
    return silc_oa_socket;
}

SILC_Error_Code
silc_oa_connection_disconnect
(
    int connection                              ///@todo switch to appropriate connection object which contains call backs to the chosen communication layer
)
{
    return SILC_SUCCESS;
}

SILC_Error_Code
silc_oa_connection_send_string
(
    int         connection,                     ///@todo switch to appropriate connection object which contains call backs to the chosen communication layer
    const char* message_string
)
{
    printf( "%s sending: %s\n", __FUNCTION__, message_string );
    silc_oa_sockets_write_line( connection, message_string );
    return SILC_SUCCESS;
}

SILC_Error_Code
silc_oa_connection_send_data
(
    int   connection,                           ///@todo switch to appropriate connection object which contains call backs to the chosen communication layer
    void* message_data,
    int   size,
    int   type_size
)
{
    silc_oa_sockets_write_data( connection, &size, sizeof( int ) );
    silc_oa_sockets_write_data( connection, message_data, size * type_size );   ///@todo switch to appropriate return measurement type
    return SILC_SUCCESS;
}


int
silc_oa_connection_read_string
(
    int   connection,                   ///@todo switch to appropriate connection object which contains call backs to the chosen communication layer
    char* message_string,
    int   maxlen
)
{
    return silc_oa_sockets_read_line( silc_oa_socket, message_string, maxlen );                 ///@todo when connection object is deployed use it connection instead of socket
}
