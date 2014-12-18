/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/** @file
 *
 *  This file contains the implementation of communication layer functions
 */


#include <config.h>

#include <UTILS_Debug.h>

#include "scorep_oa_connection.h"
#include "scorep_oa_sockets.h"
#include "scorep_status.h"
#include "scorep_ipc.h"

#include <stdio.h>


static int8_t scorep_oa_is_connected = 0;
static int    scorep_oa_socket       = 0;
int           scorep_oa_connection   = 0;



int
scorep_oa_connection_connect( void )
{
    UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "Entering %s", __func__ );
    if ( scorep_oa_is_connected )
    {
        return SCOREP_SUCCESS;
    }
    if ( SCOREP_Status_IsMpp() )
    {
        scorep_oa_port = scorep_oa_port + SCOREP_Status_GetRank();
    }
    scorep_oa_socket = scorep_oa_sockets_server_startup_retry( &scorep_oa_port, 10, 1 );
    if ( scorep_oa_socket == -1 )
    {
        _Exit( EXIT_FAILURE );
    }
    scorep_oa_is_connected = 1;
    scorep_oa_sockets_register_with_registry( scorep_oa_port, scorep_oa_registry_port, scorep_oa_registry_host, scorep_oa_app_name );
    scorep_oa_socket = scorep_oa_sockets_server_accept_client( scorep_oa_socket );
    //receive_and_process_requests(scorep_oa_socket);
    return scorep_oa_socket;
}

SCOREP_ErrorCode
scorep_oa_connection_disconnect( int connection )
{
    return SCOREP_SUCCESS;
}

SCOREP_ErrorCode
scorep_oa_connection_send_string( int         connection,
                                  const char* message_string )
{
    UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "%s sending: %s", __func__, message_string );
    scorep_oa_sockets_write_line( connection, message_string );
    return SCOREP_SUCCESS;
}

SCOREP_ErrorCode
scorep_oa_connection_send_data( int   connection,
                                void* message_data,
                                int   size,
                                int   type_size )
{
    scorep_oa_sockets_write_data( connection, &size, sizeof( int ) );
    scorep_oa_sockets_write_data( connection, message_data, size * type_size );
    return SCOREP_SUCCESS;
}


int
scorep_oa_connection_read_string( int   connection,
                                  char* message_string,
                                  int   maxlen )
{
    return scorep_oa_sockets_read_line( scorep_oa_socket, message_string, maxlen );
}
