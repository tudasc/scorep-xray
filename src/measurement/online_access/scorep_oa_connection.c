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
 * Copyright (c) 2009-2013, 2015-2016,
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


static int socket               = 0;
int        scorep_oa_connection = 0;


inline int
scorep_oa_connection_connect( void )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Entering %s", __func__ );
    static int8_t oa_is_connected = 0;
    if ( oa_is_connected )
    {
        return SCOREP_SUCCESS;
    }
    if ( SCOREP_Status_IsMpp() )
    {
        scorep_oa_port = scorep_oa_port + SCOREP_Status_GetRank();
    }
    socket = scorep_oa_sockets_server_startup_retry( &scorep_oa_port, 10, 1 );
    if ( socket == -1 )
    {
        _Exit( EXIT_FAILURE );
    }
    oa_is_connected = 1;
    scorep_oa_sockets_register_with_registry( scorep_oa_port, scorep_oa_registry_port, scorep_oa_registry_host, scorep_oa_app_name );
    socket = scorep_oa_sockets_server_accept_client( socket );
    return socket;
}

inline SCOREP_ErrorCode
scorep_oa_connection_disconnect( void )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Entering %s", __func__ );
    if ( scorep_oa_sockets_server_disconnect( socket ) == -1 )
    {
        return SCOREP_ERROR_OA_SERVER_DISCONNECT;
    }
    return SCOREP_SUCCESS;
}

inline SCOREP_ErrorCode
scorep_oa_connection_send_string( int         connection,
                                  const char* messageString )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "%s sending: %s", __func__, messageString );
    scorep_oa_sockets_write_line( connection, messageString );
    return SCOREP_SUCCESS;
}

inline SCOREP_ErrorCode
scorep_oa_connection_send_data( int   connection,
                                void* messageData,
                                int   size,
                                int   typeSize )
{
    scorep_oa_sockets_write_data( connection, &size, sizeof( int ) );
    scorep_oa_sockets_write_data( connection, messageData, size * typeSize );
    return SCOREP_SUCCESS;
}

inline int
scorep_oa_connection_read_string( int   connection,
                                  char* messageString,
                                  int   maxLen )
{
    return scorep_oa_sockets_read_line( socket, messageString, maxLen );
}
