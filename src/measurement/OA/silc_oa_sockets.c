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

/** @file silc_oa_sockets.c
    @maintainer Yury Oleynik <oleynik@in.tum.de>
    @status     ALPHA

    This file contains implementation of sockets communication between SILC OA and EA
 */


#include <config.h>

#include "SILC_Error.h"
#include "silc_oa_sockets.h"
#include "SILC_Utils.h"

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


static int   read_cnt;
static char* read_ptr;
static char  read_buf[ 1000 ];


int
silc_oa_sockets_server_startup_retry
(
    int* init_port,
    int  retries,
    int  step
)
{
    int                sock;
    int                yes  = 1;
    int                stat = -1;
    int                port;
    struct sockaddr_in my_addr;                 /* my address information */
    printf( "Entering %s\n", __FUNCTION__ );

    /**
     * create a new socket socket() returns positive interger on success
     */

    for ( port = *init_port; port <= *init_port + retries * step && stat == -1; port = port + step )
    {
        stat = 0;

        if ( ( sock = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
        {
            if ( port + step > *init_port + retries * step )
            {
                printf( "socket_server_startup::socket()\n" );
            }
            stat = -1;
        }
        else
        {
            if ( setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof( int ) ) < 0 )
            {
                if ( port + step > *init_port + retries * step )
                {
                    printf( "socket_server_startup::setsockopt()\n" );
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
                        printf( "socket_server_startup::bind()\n" );
                    }
                    stat = -1;
                }
                else
                {
                    if ( listen( sock, 1 ) < 0 )
                    {
                        if ( port + step > *init_port + retries * step )
                        {
                            printf( "socket_server_startup::listen()\n" );
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
        printf( "Exiting %s with successs, port = %d\n", __FUNCTION__, port );
        *init_port = port;
        return sock;
    }
}

registry*
silc_oa_sockets_open_registry
(
    const char* hostname,
    int         port
)
{
    registry* reg = ( registry* )malloc( sizeof( registry ) );                                          ///@TODO: switch to silc memory allocator
    char      buf[ BUFSIZE ];


    reg->hostname_ = SILC_CStr_dup( hostname );
    reg->port_     = port;

    reg->sock_ = silc_oa_sockets_client_connect_retry( reg->hostname_, reg->port_, 10 );

    if ( reg->sock_ < 0 )
    {
        if ( reg->hostname_ )
        {
            free( reg->hostname_ );
        }
        free( reg );
        return 0;
    }

    silc_oa_sockets_read_line( reg->sock_, buf, BUFSIZE );

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
silc_oa_sockets_close_registry
(
    registry* reg
)
{
    int  ret;
    char buf[ BUFSIZE ];

    sprintf( buf, "%s\n", CMD_QUIT );
    silc_oa_sockets_write_line( reg->sock_, buf );

    silc_oa_sockets_read_line( reg->sock_, buf, BUFSIZE );
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
silc_oa_sockets_registry_create_entry
(
    registry*   reg,
    const char* app,
    const char* site,
    const char* mach,
    const char* node,
    int         port,
    int         pid,
    const char* comp,
    const char* tag
)
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
    silc_oa_sockets_write_line( reg->sock_, buf );
    silc_oa_sockets_read_line( reg->sock_, buf, BUFSIZE );
    n = sscanf( buf, MSG_CREATE_SUCCESS, &id );
    if ( n < 1 )
    {
        return 0;
    }

    return id;
}

int
silc_oa_sockets_client_connect_retry
(
    char* hostname,
    int   port,
    int   retries
)
{
    int                sock;
    struct sockaddr_in pin;
    struct hostent*    hp;
    int                success, i;

    success = -1;
    for ( i = 0; i < retries && success == -1; i++ )
    {
        sleep( 4 );
        success = 0;
        if ( ( hp = gethostbyname( hostname ) ) == 0 )
        {
            if ( i == retries - 1 )
            {
                perror( "socket_client_connect::gethostbyname()" );
            }
            success = -1;
        }
        else
        {
            /* fill in the socket structure with host information */
            memset( &pin, 0, sizeof( pin ) );
            pin.sin_family      = AF_INET;
            pin.sin_addr.s_addr = ( ( struct in_addr* )( hp->h_addr ) )->s_addr;
            pin.sin_port        = htons( port );

            /* grab an Internet domain socket */
            if ( ( sock = socket( AF_INET, SOCK_STREAM, 0 ) ) == -1 )
            {
                if ( i == retries - 1 )
                {
                    perror( "socket_client_connect::socket()" );
                }
                success = -1;
            }
            else
            {
                /* connect to PORT on HOST */
                if ( connect( sock, ( struct sockaddr* )&pin, sizeof( pin ) ) == -1 )
                {
                    if ( i == retries - 1 )
                    {
                        perror( "socket_client_connect::connect()" );
                    }
                    success = -1;
                }
            }
        }
    }
    if ( success == -1 )
    {
        sock = -1;
    }
    return sock;
}




void
silc_oa_sockets_write_line
(
    int         sock,
    const char* str
)
{
    write( sock, str, strlen( str ) );
}

void
silc_oa_sockets_write_data
(
    int         sock,
    const void* buf,
    int         nbyte
)
{
    write( sock, buf, nbyte );
}

int
silc_oa_sockets_socket_my_read
(
    int   fd,
    char* ptr
)
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
silc_oa_sockets_blockread
(
    int   sock,
    char* ptr,
    int   size
)
{
    int  n, rc;
    char c;

    for ( n = 1; n <= size; n++ )
    {
        rc = silc_oa_sockets_socket_my_read( sock, &c );
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
silc_oa_sockets_read_line
(
    int   sock,
    char* str,
    int   maxlen
)
{
    int  n, rc;
    char c, * ptr;

    ptr = str;
    for ( n = 1; n < maxlen; n++ )
    {
        if ( ( rc = silc_oa_sockets_socket_my_read( sock, &c ) ) == 1 )
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
silc_oa_sockets_register_with_registry
(
    int port
)
{
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
    P_info myinfo, * allinfo;
    int    entry_id;
    char   appl_name[ 2000 ];
    char   psc_reghost[ 200 ];
    int    psc_regport;

    sprintf( appl_name, "appl" );       /// need to get appl name from config variable
    sprintf( psc_reghost, "hlrb2" );    /// need to get appl name from config variable
    psc_regport = 50001;


    PMPI_Initialized( &initialized );
    if ( !initialized )
    {
        printf( "ERROR: MPI not initialized\n" );
        exit( 1 );
    }
    PMPI_Comm_size( MPI_COMM_WORLD, &nprocs );
    PMPI_Comm_rank( MPI_COMM_WORLD, &rank );

    allinfo = ( P_info* )calloc( nprocs, sizeof( P_info ) );            ///@TODO: switch to silc memory allocator

    gethostname( myinfo.hostname, 100 );
    myinfo.cpu  = 1;                                                                                    /// get rid of later
    myinfo.port = port;
    myinfo.rank = rank;

    if ( rank == 0 )
    {
        //Master collects info via MPI and adds entries to registry

        reg = silc_oa_sockets_open_registry( psc_reghost, psc_regport );
        if ( !reg )
        {
            fprintf( stderr, "Cannot open registry at %s:%d\n", psc_reghost, psc_regport );
            exit( 1 );
        }

        PMPI_Gather( &myinfo, sizeof( P_info ), MPI_BYTE, allinfo, sizeof( P_info ), MPI_BYTE, 0, MPI_COMM_WORLD );

        int* ids = ( int* )calloc( nprocs, sizeof( int ) );                             ///@TODO: switch to silc memory allocator
        for ( i = 0; i < nprocs; i++ )
        {
            printf( "Registering process %d with port %d and host %s\n", allinfo[ i ].rank, allinfo[ i ].port, allinfo[ i ].hostname );
            entry_id = silc_oa_sockets_registry_create_entry( reg, appl_name, "LRZ",
                                                              "ALTIX4700", allinfo[ i ].hostname, allinfo[ i ].port, allinfo[ i ].rank + 1, "MRIMONITOR", "none" );
            ids[ allinfo[ i ].rank ] = entry_id;
            if ( entry_id == 0 )
            {
                fprintf( stderr, "Failed to create registry entry\n" );
            }
            else
            {
                printf( "Entry_id= %d\n", entry_id );
            }

//			sprintf(str,"PERISCOPE processrank [%i:%i:%i]", nprocs, allinfo[i].rank, allinfo[i].cpu);
//			registry_store_string(reg, entry_id, str);
        }

        if ( !silc_oa_sockets_close_registry( reg ) )
        {
            fprintf( stderr, "Something went wrong when closing registry\n" );
        }
        PMPI_Scatter( ids, 1, MPI_INTEGER, &entry_id, 1, MPI_INTEGER, 0, MPI_COMM_WORLD );
    }
    else
    {
        //client determines local info and provides it to master
        //hostname, port, rank, cpu
        //MPI_GATHER(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm)
        int help_int;
        PMPI_Gather( &myinfo, sizeof( P_info ), MPI_BYTE, allinfo, sizeof( P_info ), MPI_BYTE, 0, MPI_COMM_WORLD );
        PMPI_Scatter( &help_int, 1, MPI_INTEGER, &entry_id, 1, MPI_INTEGER, 0, MPI_COMM_WORLD );
    }
}

int
silc_oa_sockets_server_accept_client
(
    int sock
)
{
    int                newsock;

    struct sockaddr_in client_addr;       /* client's address information */

//size_t sin_size;				///@TODO find out which size should be this variable
    unsigned int sin_size;

    sin_size = sizeof( struct sockaddr_in );
    printf( "Waiting for client to connect...\n" );
    if ( ( newsock = accept( sock, ( struct sockaddr* )&client_addr, &sin_size ) ) < 0 )
    {
        printf( "socket_server_accept_client::accept() error" );
        return -1;
    }

    return newsock;
}
