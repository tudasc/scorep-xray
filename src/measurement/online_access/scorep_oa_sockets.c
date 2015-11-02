/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2014,
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
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * This file contains implementation of sockets communication between SCOREP OA and EA
 */


#include <config.h>

#include <UTILS_Debug.h>
#include <UTILS_Error.h>
#include <UTILS_CStr.h>
#include <UTILS_IO.h>
#include "scorep_oa_sockets.h"


#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#ifdef WITH_MPI
#include <mpi.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

static int   read_cnt;
static char* read_ptr;
static char  read_buf[ 1000 ];


int
scorep_oa_sockets_server_startup_retry( uint64_t* init_port,
                                        int       retries,
                                        int       step )
{
    int                sock;
    int                yes  = 1;
    int                stat = -1;
    int                port;
    struct sockaddr_in my_addr;                 /* my address information */
    UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "Entering %s", __func__ );

    /**
     * create a new socket socket() returns positive interger on success
     */

    for ( port = ( int )*init_port; port <= *init_port + retries * step && stat == -1; port = port + step )
    {
        stat = 0;

        if ( ( sock = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
        {
            if ( port + step > *init_port + retries * step )
            {
                UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "socket_server_startup::socket()" );
            }
            stat = -1;
        }
        else
        {
            if ( setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof( int ) ) < 0 )
            {
                if ( port + step > *init_port + retries * step )
                {
                    UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "socket_server_startup::setsockopt()" );
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
                    if ( port + step > *init_port + retries * step )
                    {
                        UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "socket_server_startup::bind()" );
                    }
                    stat = -1;
                }
                else
                {
                    if ( listen( sock, 1 ) < 0 )
                    {
                        if ( port + step > *init_port + retries * step )
                        {
                            UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "socket_server_startup::listen()" );
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
        UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "Exiting %s with successs, port = %d", __func__, port );
        *init_port = ( uint64_t )port;
        return sock;
    }
}

registry*
scorep_oa_sockets_open_registry( const char* hostname,
                                 int         port )
{
    registry* reg = ( registry* )malloc( sizeof( registry ) );
    char      buf[ BUFSIZE ];


    reg->hostname_ = UTILS_CStr_dup( hostname );
    reg->port_     = port;

    reg->sock_ = scorep_oa_sockets_client_connect_retry( reg->hostname_, reg->port_, 10 );

    if ( reg->sock_ < 0 )
    {
        if ( reg->hostname_ )
        {
            free( reg->hostname_ );
        }
        free( reg );
        return 0;
    }

    scorep_oa_sockets_read_line( reg->sock_, buf, BUFSIZE );

    if ( strncmp( buf, PREFIX_SUCCESS, strlen( PREFIX_SUCCESS ) ) )
    {
        /** something went wrong */
        close( reg->sock_ );
        if ( reg->hostname_ )
        {
            free( reg->hostname_ );
        }
        free( reg );
        return 0;
    }

    return reg;
}


/*
 * close the connection to the registry server
 */
int
scorep_oa_sockets_close_registry( registry* reg )
{
    int  ret;
    char buf[ BUFSIZE ];

    sprintf( buf, "%s\n", CMD_QUIT );
    scorep_oa_sockets_write_line( reg->sock_, buf );

    scorep_oa_sockets_read_line( reg->sock_, buf, BUFSIZE );
    if ( strncmp( buf, PREFIX_SUCCESS, strlen( PREFIX_SUCCESS ) ) )
    {
        ret = 0;
    }
    else
    {
        ret = 1;
    }

    close( reg->sock_ );
    if ( reg->hostname_ )
    {
        free( reg->hostname_ );
    }
    free( reg );

    return ret;
}


int
scorep_oa_sockets_registry_create_entry( registry*   reg,
                                         const char* app,
                                         const char* site,
                                         const char* mach,
                                         const char* node,
                                         int         port,
                                         int         pid,
                                         const char* comp,
                                         const char* tag )
{
    int  id = 0;
    char buf[ BUFSIZE ];
    int  n = 0;

    sprintf( buf, "%s "
             "app=\"%s\" "
             "site=\"%s\" "
             "mach=\"%s\" "
             "node=\"%s\" "
             "port=%d "
             "pid=%d "
             "comp=\"%s\" "
             "tag=\"%s\"\n"
             , CMD_CREATE, app, site, mach, node, port, pid, comp, tag );
    scorep_oa_sockets_write_line( reg->sock_, buf );
    scorep_oa_sockets_read_line( reg->sock_, buf, BUFSIZE );
    UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "received from registry: %s", buf );
    n = sscanf( buf, MSG_CREATE_SUCCESS, &id );
    if ( n < 1 )
    {
        return 0;
    }

    return id;
}

int
scorep_oa_sockets_registry_delete_entry( registry* reg,
                                         int       entid )
{
    char buf[ BUFSIZE ];
    int  id;
    int  n = 0;

    sprintf( buf, "%s %d\n", CMD_DELETE, entid );
    scorep_oa_sockets_write_line( reg->sock_, buf );

    scorep_oa_sockets_read_line( reg->sock_, buf, BUFSIZE );
    n = sscanf( buf, MSG_DELETE_SUCCESS, &id );

    if ( n < 1 )
    {
        return 0;
    }
    return id;
}

int
scorep_oa_sockets_client_connect_retry( char* hostname,
                                        int   port,
                                        int   retries )
{
    struct addrinfo  hints;
    struct addrinfo* result;
    int              s, sock;
    int              success, i;

    if ( port >= 999999 )
    {
        UTILS_WARNING( "Port number %d is too big", port );
        return -1;
    }

    char* port_s = ( char* )malloc( 6 * sizeof( char ) );
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
                UTILS_WARNING( "Could not get address info for %s:%d", hostname, port );
            }
            success = -1;
            continue;
        }

        sock = socket( result->ai_family, result->ai_socktype, result->ai_protocol );
        if ( sock == -1 )
        {
            if ( i == retries - 1 )
            {
                UTILS_WARNING( "Could not create socket %s:%d", hostname, port );
            }
            success = -1;
            continue;
        }

        if ( connect( sock, result->ai_addr, result->ai_addrlen ) == -1 )
        {
            if ( i == retries - 1 )
            {
                UTILS_WARNING( "Could not connect to %s:%d", hostname, port );
            }
            success = -1;
            continue;
        }
    }

    free( port_s );

    if ( success == -1 )
    {
        sock = -1;
    }
    return sock;
}



void
scorep_oa_sockets_write_line( int         sock,
                              const char* str )
{
    int result = write( sock, str, strlen( str ) );
}

void
scorep_oa_sockets_write_data( int         sock,
                              const void* buf,
                              int         nbyte )
{
    int result = write( sock, buf, nbyte );
}

int
scorep_oa_sockets_socket_my_read( int   fd,
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
scorep_oa_sockets_blockread( int   sock,
                             char* ptr,
                             int   size )
{
    int  n, rc;
    char c;

    for ( n = 1; n <= size; n++ )
    {
        rc = scorep_oa_sockets_socket_my_read( sock, &c );
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
                             int   maxlen )
{
    int  n, rc;
    char c, * ptr;

    ptr = str;
    for ( n = 1; n < maxlen; n++ )
    {
        if ( ( rc = scorep_oa_sockets_socket_my_read( sock, &c ) ) == 1 )
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

void
scorep_oa_sockets_register_with_registry( uint64_t port,
                                          uint64_t reg_port,
                                          char*    reg_host,
                                          char*    app_name )
{
    UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "Entering %s", __func__ );
    registry* reg;
    int       i;
    int       nprocs, rank, initialized;
    struct p_info
    {
        char hostname[ 100 ];
        int  port;
        int  cpu;
        int  rank;
    };

    typedef struct p_info P_info;
    P_info myinfo;
    int    entry_id;
    char   appl_name[ 2000 ];
    char   psc_reghost[ 200 ];
    int    psc_regport;
    char   site_name[ 50 ];
    char   machine_name[ 50 ];
    char   library_name[ 50 ];

    sprintf( appl_name, "%s", app_name );
    sprintf( psc_reghost, "%s", reg_host );
    //UTILS_IO_GetHostname( psc_reghost, 100 );
    psc_regport = ( int )reg_port;
    sprintf( site_name, "none" );
    sprintf( machine_name, "none" );
    sprintf( library_name, "SCOREP" );

#ifdef WITH_MPI

    P_info* allinfo;

    PMPI_Initialized( &initialized );
    if ( !initialized )
    {
        UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "ERROR: MPI not initialized" );
        exit( 1 );
    }
    PMPI_Comm_size( MPI_COMM_WORLD, &nprocs );
    PMPI_Comm_rank( MPI_COMM_WORLD, &rank );

    allinfo = ( P_info* )calloc( nprocs, sizeof( P_info ) );

    UTILS_IO_GetHostname( myinfo.hostname, 100 );
    myinfo.cpu  = 1;
    myinfo.port = ( int )port;
    myinfo.rank = rank;

    if ( rank == 0 )
    {
        //Master collects info via MPI and adds entries to registry

        reg = scorep_oa_sockets_open_registry( psc_reghost, psc_regport );
        if ( !reg )
        {
            fprintf( stderr, "Cannot open registry at %s:%d\n", psc_reghost, psc_regport );
            exit( 1 );
        }

        PMPI_Gather( &myinfo, sizeof( P_info ), MPI_BYTE, allinfo, sizeof( P_info ), MPI_BYTE, 0, MPI_COMM_WORLD );

        int* ids = ( int* )calloc( nprocs, sizeof( int ) );
        for ( i = 0; i < nprocs; i++ )
        {
            UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "Registering process %d with port %d and host %s", allinfo[ i ].rank, allinfo[ i ].port, allinfo[ i ].hostname );
            entry_id = scorep_oa_sockets_registry_create_entry( reg,
                                                                appl_name,
                                                                site_name,
                                                                machine_name,
                                                                allinfo[ i ].hostname,
                                                                allinfo[ i ].port,
                                                                allinfo[ i ].rank + 1,
                                                                library_name,
                                                                "none" );
            ids[ allinfo[ i ].rank ] = entry_id;
            if ( entry_id == 0 )
            {
                fprintf( stderr, "Failed to create registry entry\n" );
            }
            else
            {
                UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "Entry_id= %d", entry_id );
            }

//            sprintf( str, "PERISCOPE processrank [%i:%i:%i]", nprocs, allinfo[i].rank, allinfo[i].cpu );
//            registry_store_string( reg, entry_id, str );
        }

        if ( !scorep_oa_sockets_close_registry( reg ) )
        {
            UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "Something went wrong when closing registry" );
        }

        PMPI_Scatter( ids, 1, MPI_INTEGER, &entry_id, 1, MPI_INTEGER, 0, MPI_COMM_WORLD );
    }
    else
    {
        //client determines local info and provides it to master
        //hostname, port, rank, cpu
        //MPI_GATHER( sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm )

        int help_int;
        PMPI_Gather( &myinfo, sizeof( P_info ), MPI_BYTE, allinfo, sizeof( P_info ), MPI_BYTE, 0, MPI_COMM_WORLD );

        PMPI_Scatter( &help_int, 1, MPI_INTEGER, &entry_id, 1, MPI_INTEGER, 0, MPI_COMM_WORLD );
    }

    free( allinfo );
#else

    myinfo.cpu  = 1;                                                                                    /// get rid of later
    myinfo.port = ( int )port;
    myinfo.rank = 1;
    UTILS_IO_GetHostname( myinfo.hostname, 100 );

    reg = scorep_oa_sockets_open_registry( psc_reghost, psc_regport );
    if ( !reg )
    {
        fprintf( stderr, "Cannot open registry at %s:%d\n", psc_reghost, psc_regport );
        exit( 1 );
    }
    entry_id = scorep_oa_sockets_registry_create_entry( reg,
                                                        appl_name,
                                                        site_name,
                                                        machine_name,
                                                        myinfo.hostname,
                                                        myinfo.port,
                                                        myinfo.rank,
                                                        library_name,
                                                        "none" );
    if ( entry_id == 0 )
    {
        fprintf( stderr, "Failed to create registry entry\n" );
    }
    else
    {
        UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "Entry_id= %d", entry_id );
    }

    if ( !scorep_oa_sockets_close_registry( reg ) )
    {
        UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "Something went wrong when closing registry" );
    }
#endif
}

int
scorep_oa_sockets_server_accept_client( int sock )
{
    int newsock;

    struct sockaddr_in client_addr;       /* client's address information */

//size_t sin_size;
    unsigned int sin_size;

    sin_size = sizeof( struct sockaddr_in );
    UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "Waiting for client to connect..." );
    if ( ( newsock = accept( sock, ( struct sockaddr* )&client_addr, &sin_size ) ) < 0 )
    {
        UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "socket_server_accept_client::accept() error" );
        return -1;
    }

    return newsock;
}
