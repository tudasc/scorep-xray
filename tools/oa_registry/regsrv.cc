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
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 * @file regsrv.cc
 *
 * This file implements registry service class
 */

#include <config.h>

#include <cstdio>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <inttypes.h>


#include <UTILS_IO.h>
#include "regsrv.h"
#include "regsrv_client.h"
#include "regsrv_sockets.h"
#include "scorep_oa_registry_protocol.h"
#include "stringutil.h"


/* *INDENT-OFF* */
using namespace std;

#define MAX_REGION_NAME_LENGTH                          150
#define MAX_FILE_NAME_LENGTH                            150
#define MAX_COUNTER_NAME_LENGTH                         256
#define MAX_COUNTER_UNIT_LENGTH                         10

typedef struct SCOREP_OA_CallPathRegionDef_struct
{
    uint32_t region_id;
    char     name[ MAX_REGION_NAME_LENGTH ];
    char     file[ MAX_FILE_NAME_LENGTH ];
    uint32_t rfl;
    uint32_t rel;
    uint32_t adapter_type;
} SCOREP_OA_CallPathRegionDef;

typedef struct SCOREP_OA_FlatProfileMeasurement_struct
{
    uint32_t measurement_id;
    uint64_t rank;
    uint32_t thread;
    uint32_t region_id;
    uint64_t samples;
    uint32_t metric_id;
    uint64_t int_val;
} SCOREP_OA_FlatProfileMeasurement;

typedef struct SCOREP_OA_CallPathCounterDef_struct
{
    char     name[ MAX_COUNTER_NAME_LENGTH ];
    char     unit[ MAX_COUNTER_UNIT_LENGTH ];
    uint32_t status;
} SCOREP_OA_CallPathCounterDef;

int RegServ::open( int port )
{
    char hostname[ 100 ];
    if( UTILS_IO_GetHostname( hostname, 100 ) != 0 )
    {
        fprintf( stderr,"UTILS_IO_GetHostname() failed." );
        exit( 1 );
    }
    printf("Registry service: starting server on %s:%d...",hostname,port);

    server_socket_ = scorep_oa_sockets_server_startup_retry( &port, 1, 1 );
    if ( server_socket_ < 0 )
    {
        fprintf( stderr, "Selected registry port is busy" );
        exit( 160 );
    }
    port_ = port;
    printf( " OK\n" );
    return server_socket_;
}

void RegServ::close_connection()
{
    close( connection_socket_ );
    printf( "Registry service: connection closed!\n" );
}

void RegServ::close_server()
{
    close( server_socket_ );
    printf( "Registry service: server closed!\n" );
}

int RegServ::blocking_accept()
{
    printf( "Registry service: accepting connection..." );
    fflush( stdout );
    if ( server_socket_ < 0 )
    {
        fprintf( stderr, "Server is not started!" );
        exit( 1 );
    }


    connection_socket_ = scorep_oa_sockets_server_accept_client( server_socket_ );

    if( connection_socket_ < 0 )
    {
        fprintf( stderr, "Accepting connection failed!" );
        exit( 1 );
    }
    const int bufsize = 400;
    char welcome[ bufsize ];

    sprintf( welcome, MSG_WELCOME, get_hostname().c_str(),
             get_port(), "---", "---" );
    scorep_oa_sockets_write_line( connection_socket_, welcome );

    printf( " OK\n" );

    return connection_socket_;
}

int RegServ::execute_test( const char* scenario_file )
{

    printf( "Periscope Emulator: starting test...\n" );
    std::map< int , RegEntry* >::iterator it;
    printf( "Periscope Emulator: connecting to monitoring processes..." );
    for( it = reg_data_.begin(); it != reg_data_.end(); it++ )
    {

        int sock = scorep_oa_sockets_client_connect_retry( it->second->node.c_str(), it->second->port, 1 );
        if ( sock < 0 )
        {
            fprintf( stderr, "\nPeriscope Emulator: could not connect to the monitoring process at %s:%d\n",
                     it->second->node.c_str(),
                     it->second->port);
            exit( 1 );
        }

        it->second->test_comm_sock = sock;
    }
    printf( "OK\n" );

    //open scenario file
    ifstream f ( scenario_file );
    if ( !f.is_open() )
    {
        fprintf( stderr, "Periscope Emulator: Opening test scenario file failed!\n" );
        exit( 1 );
    }

    string  request;
    int     pos;
    string  delim   = "|"; //delimiter for the request lines in scenario file
    string  none    = "-"; //no confirmation from OA
    string  data    = "data"; //receive data
    string  wait    = "<wait>"; //wait for input from OA
    string  buffer, response, recv_data;

    while ( 1 ) //loop over lines in the scenario file
    {
        // read one line from the scenario file into buffer
        getline( f, request );
        if ( f.eof() )
            break;

        pos = get_token( request, 0, delim, buffer );
        pos = get_token( request, pos, delim, response );
        pos = get_token( request, pos, delim, recv_data );

        printf( "Read: %s%s%s%s%s\n", buffer.c_str(), delim.c_str(), response.c_str(), delim.c_str(), recv_data.c_str() );
        for( it = reg_data_.begin(); it != reg_data_.end(); it++ )
            /// loop over registered monitoring processes
        {
            if ( buffer == wait )
                printf( "Periscope Emulator: waiting for message...\n" );
            else
            {
                printf( "Periscope Emulator: sending command to process %i: %s\n", it->second->pid, buffer.c_str() );
                scorep_oa_sockets_write_line( it->second->test_comm_sock, ( buffer + "\n" ).c_str() );
            }

            if ( response != none )
            {
                int maxlen = MAX_MESSAGE_SIZE;
                char buf[ MAX_MESSAGE_SIZE ];
                buf[ 0 ] = 0;
                memset( buf, '\0', maxlen );
                while ( ( scorep_oa_sockets_read_line( it->second->test_comm_sock, buf, maxlen ) ) == 0 )
                {
                }
                printf( "Periscope Emulator: received from process %i: %s\n", it->second->pid, buf );
                if ( strcmp( response.c_str(), buf ) != 0 )
                {
                    fprintf( stderr, "Periscope Emulator: Wrong answer. Expected: %s. Received: %s\n", response.c_str(), buf );
                    exit( 1 );
                }
            }

            if ( recv_data == data )
            {
                int number;
                int nr = scorep_oa_sockets_blockread( it->second->test_comm_sock,
                                                      ( char* )( &number ),
                                                      sizeof( int ) );
                if ( nr != sizeof( int ) )
                {
                    fprintf( stderr, "Periscope Emulator: Couldn't receive number of records\n" );
                    exit( 1 );
                }

                printf( "Periscope Emulator: Expecting Merged Region Definitions from process %i, size: 0x%x entries of size %" PRIu64 "\n",
                        it->second->pid,
                        number,
                        ( uint64_t ) sizeof( SCOREP_OA_CallPathRegionDef ) );

                if ( number > 0 )
                {
                    SCOREP_OA_CallPathRegionDef* recv_buffer = (SCOREP_OA_CallPathRegionDef *)
                            calloc( number, sizeof( SCOREP_OA_CallPathRegionDef ) );
                    if ( !recv_buffer )
                    {
                        fprintf( stderr, "Periscope Emulator: Error allocating recv_buffer\n" );
                        exit( 1 );
                    }
                    nr = scorep_oa_sockets_blockread( it->second->test_comm_sock,
                                                      ( char* )( recv_buffer ),
                                                      number * sizeof( SCOREP_OA_CallPathRegionDef ) );

                    if( nr != number * sizeof( SCOREP_OA_CallPathRegionDef ) )
                    {
                        fprintf( stderr, "Periscope Emulator: Couldn't receive measurements buffer\n" );
                        exit( 1 );
                    }
                    printf( "Periscope Emulator: Got Regions Definitions from process %i:\n", it->second->pid );
                    for ( int i = 0; i < number; i++ )
                    {
                        printf( "record %d: \t|region_id=%" PRIu32 " \t| name=%s \t| file=%s \t| rfl=%" PRIu32 " \t| rel=%" PRIu32 " \t| adapter_type=%" PRIu32 "\t|\n",
                                i,
                                recv_buffer[ i ].region_id,
                                recv_buffer[ i ].name,
                                recv_buffer[ i ].file,
                                recv_buffer[ i ].rfl,
                                recv_buffer[ i ].rel,
                                recv_buffer[ i ].adapter_type
                        );
                    }
                    free( recv_buffer );

                }

                int maxlen = MAX_MESSAGE_SIZE;
                char buf[ MAX_MESSAGE_SIZE ];
                buf[ 0 ] = 0;
                memset( buf, '\0', maxlen );
                while ( ( scorep_oa_sockets_read_line( it->second->test_comm_sock, buf, maxlen ) ) == 0 )
                {
                }
                printf( "Periscope Emulator: received from process %i: %s\n", it->second->pid, buf );

                number = 0;
                nr     = scorep_oa_sockets_blockread( it->second->test_comm_sock,
                                                      ( char* )( &number ),
                                                      sizeof( int ));
                if( nr != sizeof( int ) )
                {
                    fprintf( stderr, "Periscope Emulator: Couldn't receive number of records\n" );
                    exit( 1 );
                }

                printf( "Periscope Emulator: Expecting Flat Profile from process %i, size: %" PRId32 " entries of size %" PRIu64 "\n",
                        it->second->pid,
                        number,
                        ( uint64_t ) sizeof( SCOREP_OA_FlatProfileMeasurement ) );

                if ( number > 0 )
                {

                    SCOREP_OA_FlatProfileMeasurement* recv_buffer = ( SCOREP_OA_FlatProfileMeasurement * )
                            calloc( number, sizeof( SCOREP_OA_FlatProfileMeasurement ) );

                    if ( !recv_buffer )
                    {
                        fprintf( stderr, "Periscope Emulator: Error allocating recv_buffer\n" );
                        exit( 1 );
                    }
                    nr = scorep_oa_sockets_blockread( it->second->test_comm_sock,
                                                      ( char* )( recv_buffer ),
                                                      number * sizeof( SCOREP_OA_FlatProfileMeasurement ) );

                    if( nr != number * sizeof( SCOREP_OA_FlatProfileMeasurement ) )
                    {
                        fprintf( stderr, "Periscope Emulator: Couldn't receive measurements buffer\n" );
                        exit( 1 );
                    }

                    printf( "Periscope Emulator: Got Flat Profile from process %i:\n", it->second->pid );
                    for ( int i = 0; i < number; i++ )
                    {
                        printf( "record %d: \t|meas_id=%" PRIu32 " \t| rank=%" PRIu64 " \t| thread=%" PRIu32 " \t| region_id=%" PRIu32 " \t| samples=%" PRIu64 " \t| counter=%" PRIu32 "\t| int_val=%" PRIu64 " \t|\n",
                                i,
                                recv_buffer[ i ].measurement_id,
                                recv_buffer[ i ].rank,
                                recv_buffer[ i ].thread,
                                recv_buffer[ i ].region_id,
                                recv_buffer[ i ].samples,
                                recv_buffer[ i ].metric_id,
                                recv_buffer[ i ].int_val
                        );
                    }

                    free( recv_buffer );
                }


                buf[ 0 ] = 0;
                memset( buf, '\0', maxlen );
                while ( ( scorep_oa_sockets_read_line( it->second->test_comm_sock, buf, maxlen ) ) == 0 )
                {
                }
                printf( "Periscope Emulator: received from process %i: %s\n", it->second->pid, buf );

                number = 0;
                nr     = scorep_oa_sockets_blockread( it->second->test_comm_sock,
                                                      ( char* )( &number ),
                                                      sizeof( int ) );
                if ( nr != sizeof( int ) )
                {
                    fprintf( stderr, "Periscope Emulator: Couldn't receive number of records\n" );
                    exit( 1 );
                }

                printf( "Periscope Emulator: Expecting Metric definitions from process %i, size: %" PRId32 " entries of size %" PRIu64 "\n",
                        it->second->pid,
                        number,
                        ( uint64_t )sizeof( SCOREP_OA_CallPathCounterDef ) );

                if ( number > 0 )
                {

                    SCOREP_OA_CallPathCounterDef* recv_buffer = ( SCOREP_OA_CallPathCounterDef* )
                            calloc( number, sizeof( SCOREP_OA_CallPathCounterDef ) );

                    if ( !recv_buffer )
                    {
                        fprintf( stderr, "Periscope Emulator: Error allocating recv_buffer\n" );
                        exit( 1 );
                    }
                    nr = scorep_oa_sockets_blockread( it->second->test_comm_sock,
                                                      ( char* )( recv_buffer ),
                                                      number * sizeof( SCOREP_OA_CallPathCounterDef ) );

                    if ( nr != number * sizeof( SCOREP_OA_CallPathCounterDef ) )
                    {
                        fprintf( stderr, "Periscope Emulator: Couldn't receive measurements buffer\n" );
                        exit( 1 );
                    }

                    printf( "Periscope Emulator: Got metric definitions from process %i:\n", it->second->pid );
                    for ( int i = 0; i < number; i++ )
                    {
                        printf( "record %d: \t| counter name=%s \t| unit=%s \t| status=%" PRIu32 "\n",
                                i,
                                recv_buffer[ i ].name,
                                recv_buffer[ i ].unit,
                                recv_buffer[ i ].status
                        );
                    }

                    free( recv_buffer );
                }
            }
        }
    }

    f.close();

    for( it = reg_data_.begin(); it != reg_data_.end(); it++ )
    {
        close( it->second->test_comm_sock );
        delete it->second;
    }
    printf( "Periscope Emulator: test successfully completed!\n" );
    return 0;
}


void usage( int argc, char* argv[] )
{
    fprintf( stderr, "Usage: %s <portnumber> [test=<filename>]\n", argv[ 0 ] );
}

int main( int argc, char* argv[] )
{
    int     do_test = 0;
    int     port    = 31337;
    int     pos;
    string  delim   = "=";
    string  test_arg, scenario_file;

    if ( argc > 1 ) port = atoi( argv[ 1 ] );
    if ( argc > 2 )
    {
        pos = get_token(argv[ 2 ], 0, delim, test_arg);
        pos = get_token(argv[ 2 ], pos, delim, scenario_file);

        if ( strcasecmp( test_arg.c_str(), "test" ) )
        {
            fprintf( stderr, "Wrong argument %s\n", argv[2] );
            usage( argc, argv );
            exit( 1 );
        }

        if ( scenario_file.size() == 0 )
        {
            fprintf( stderr, "No scenario file provided\n" );
            usage( argc, argv );
            exit( 1 );
        }
        do_test=1;
    }

    RegServ regsrv;

    printf( "Registry service: started\n" );
    if ( do_test )
        printf( "Registry service: test requested, using scenario file: %s\n", scenario_file.c_str() );

    if ( regsrv.open( port ) < 0 )
    {
        fprintf( stderr, "Error starting server on %d\n", port );
        usage( argc, argv );
        exit( 1 );
    }


    while ( 1 )
    {
        regsrv.blocking_accept();
        RegServClient *client = new RegServClient( &regsrv );
        int mode = REGSRV_MODE_LISTEN;
        while ( mode == REGSRV_MODE_LISTEN )
            mode = client->receive_and_handle_request();
        regsrv.close_connection();
        delete client;
        if ( mode == REGSRV_MODE_TEST || do_test == 1 )
        {
            regsrv.execute_test( scenario_file.c_str() );
            break;
        }
    }
    regsrv.close_connection();
    regsrv.close_server();
    printf( "Registry service: finished\n" );
    return 0;
}
