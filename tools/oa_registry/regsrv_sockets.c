/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011, 2015-2016,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file regsrv_sockets.c
 *
 * This file contains implementation of sockets communication functions
 */
#include <config.h>

#include <UTILS_Debug.h>
#include <UTILS_Error.h>
#include <UTILS_CStr.h>
#include <UTILS_IO.h>
#include "regsrv_sockets.h"
#include "scorep_oa_registry_protocol.h"

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

static int   read_cnt;
static char* read_ptr;
static char  read_buf[ 1000 ];


int
scorep_oa_sockets_server_startup_retry( int* initPort,
                                        int  retries,
                                        int  step )
{
    int                sock;
    int                stat = -1;
    int                port;
    struct sockaddr_in my_addr;                 /* my address information */
    //UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Entering %s", __func__ );

    /**
     * create a new socket socket() returns positive integer on success
     */

    for ( port = ( int )*initPort; port <= *initPort + retries * step && stat == -1; port = port + step )
    {
        stat = 0;

        if ( ( sock = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
        {
            if ( port + step > *initPort + retries * step )
            {
                //UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "socket_server_startup::socket()" );
            }
            stat = -1;
        }
        else
        {
            int yes = 1;
            if ( setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof( int ) ) < 0 )
            {
                if ( port + step > *initPort + retries * step )
                {
                    //UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "socket_server_startup::setsockopt()" );
                }
                stat = -1;
            }
            else
            {
                my_addr.sin_family      = AF_INET;                        /** host byte order */
                my_addr.sin_port        = htons( port );                  /** short, network byte order */
                my_addr.sin_addr.s_addr = INADDR_ANY;                     /** automatically fill with my IP */
                memset( &( my_addr.sin_zero ), '\0', 8 );                 /** zero the rest of the struct */

                if ( bind( sock, ( struct sockaddr* )&my_addr, sizeof( struct sockaddr ) ) < 0 )
                {
                    if ( port + step > *initPort + retries * step )
                    {
                        //UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "socket_server_startup::bind()" );
                    }
                    stat = -1;
                }
                else
                {
                    if ( listen( sock, 1 ) < 0 )
                    {
                        if ( port + step > *initPort + retries * step )
                        {
                            //UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "socket_server_startup::listen()" );
                        }
                        stat = -1;
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
    }

    if ( stat == -1 )
    {
        return -1;
    }
    else
    {
        //UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Exiting %s with success, port = %d", __func__, port );
        *initPort = port;
        return sock;
    }
}

int
scorep_oa_sockets_client_connect_retry( const char* hostname,
                                        int         port,
                                        int         retries )
{
    struct addrinfo  hints;
    struct addrinfo* result;
    int              s, sock;
    int              success, i;

    if ( port >= 999999 )
    {
        UTILS_WARNING( "Port number %d is too big\n", port );
        return -1;
    }

    char* port_s = ( char* )malloc( 6 * sizeof( char ) );
    UTILS_ASSERT( port_s );
    sprintf( port_s, "%d", port );

    success = -1;
    for ( i = 0; i < retries && success == -1; i++ )
    {
        sleep( 4 );
        success = 0;

        memset( &hints, 0, sizeof( struct addrinfo ) );
        hints.ai_family   = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags    = 0;
        hints.ai_protocol = 0;              /* Any protocol */

        s = getaddrinfo( hostname, port_s, &hints, &result );
        if ( s != 0 )
        {
            if ( i == retries - 1 )
            {
                UTILS_WARNING( "Could not get address info for %s:%d\n", hostname, port );
            }
            success = -1;
            freeaddrinfo( result );
            continue;
        }

        sock = socket( result->ai_family, result->ai_socktype, result->ai_protocol );
        if ( sock == -1 )
        {
            if ( i == retries - 1 )
            {
                UTILS_WARNING( "Could not create socket %s:%d\n", hostname, port );
            }
            success = -1;
            freeaddrinfo( result );
            continue;
        }

        if ( connect( sock, result->ai_addr, result->ai_addrlen ) == -1 )
        {
            if ( i == retries - 1 )
            {
                UTILS_WARNING( "Could not connect to %s:%d\n", hostname, port );
            }
            success = -1;
            freeaddrinfo( result );
            continue;
        }
        freeaddrinfo( result );
    }

    free( port_s );

    if ( success == -1 )
    {
        sock = -1;
    }
    return sock;
}


inline void
scorep_oa_sockets_write_line( int         sock,
                              const char* str )
{
    if ( write( sock, str, strlen( str ) ) == -1 )
    {
        UTILS_WARNING( "Could not write to socket!\n" );
    }
}

inline void
scorep_oa_sockets_write_data( int         sock,
                              const void* buf,
                              int         nbyte )
{
    if ( write( sock, buf, nbyte ) == -1 )
    {
        UTILS_WARNING( "Could not write to socket!\n" );
    }
}

static inline int
sockets_socket_my_read( int   fd,
                        char* ptr )
{
    if ( read_cnt <= 0 )
    {
again:
        if ( ( read_cnt = read( fd, read_buf, sizeof( read_buf ) ) ) < 0 )
        {
            if ( errno == EINTR )
            {
                goto again;
            }
            return -1;
        }
        else
        if ( read_cnt == 0 )
        {
            return 0;
        }
        read_ptr = read_buf;
    }

    read_cnt--;
    *ptr = *read_ptr++;
    return 1;
}


int
scorep_oa_sockets_blockread(  int   sock,
                              char* ptr,
                              int   size )
{
    int  n, rc;
    char c;

    for ( n = 1; n <= size; n++ )
    {
        rc = sockets_socket_my_read( sock, &c );
        if ( rc == 1 )
        {
            *ptr++ = c;
        }
        else
        if ( rc == 0 )
        {
            sleep( 1 );
        }
        else
        {
            return -1;
        }
    }
    return n - 1;
}



int
scorep_oa_sockets_read_line( int   sock,
                             char* str,
                             int   maxLen )
{
    int  n, rc;
    char c, * ptr;

    ptr = str;
    for ( n = 1; n < maxLen; n++ )
    {
        if ( ( rc = sockets_socket_my_read( sock, &c ) ) == 1 )
        {
            if ( c  == '\n' )
            {
                break;                          /* newline is stored, like fgets() */
            }
            *ptr++ = c;
        }
        else
        if ( rc == 0 )
        {
            *ptr = 0;
            return n - 1;                           /* EOF, n - 1 bytes were read */
        }
        else
        {
            return -1;                              /* error, errno set by read() */
        }
    }

    *ptr = 0;          /* null terminate like fgets() */

    return n;
}

int
scorep_oa_sockets_server_accept_client( int sock )
{
    int newsock;

    struct sockaddr_in client_addr;       /* client's address information */

    unsigned int sin_size;

    sin_size = sizeof( struct sockaddr_in );
    //UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Waiting for client to connect..." );
    if ( ( newsock = accept( sock, ( struct sockaddr* )&client_addr, &sin_size ) ) < 0 )
    {
        //UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "socket_server_accept_client::accept() error" );
        return -1;
    }

    return newsock;
}

inline int
scorep_oa_sockets_server_disconnect( int sock )
{
    return close( sock );
}
