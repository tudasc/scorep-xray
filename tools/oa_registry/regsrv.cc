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

/**
 * @file regsrv.cc
 * @maintainer Yury Oleynik <oleynik@in.tum.de>
 * @status alpha
 *
 * This file implements registry service class
 */
#include <config.h>

 
#include <stdio.h>
#include <stdlib.h>


#include "regsrv.h"
#include "regsrv_client.h"
#include "regsrv_sockets.h"
#include "scorep_oa_registry_protocol.h"

int RegServ::open( int port )
{
	printf("Registry service: starting server...");

    server_socket_ = scorep_oa_sockets_server_startup_retry( &port, 1, 1 );
    if ( server_socket_ < 0 )
    {
    	fprintf(stderr,"Selected regisrty port is busy");
    	return -1;
    }
    port_=port;
    printf(" OK\n");
    return server_socket_;
}

void RegServ::close_connection()
{
	close( connection_socket_ );
	printf("Registry service: connection closed!\n");
}

void RegServ::close_server()
{
	close( server_socket_ );
	printf("Registry service: server closed!\n");
}

int RegServ::blocking_accept( )
{
	printf("Registry service: accepting connection...");
    fflush(stdout);
    if (server_socket_<0){
    	fprintf(stderr,"Server is not started!");
    	return -1;
    }
    	
    
    connection_socket_ = scorep_oa_sockets_server_accept_client( server_socket_ );
    
    if( connection_socket_ < 0)
    {
    	fprintf(stderr,"Accepting connection failed!");
    	return -1;
    }
    const int bufsize=400;
    char welcome[bufsize];
  
    sprintf( welcome, MSG_WELCOME, get_hostname().c_str(),
	   get_port(), "---", "---" );
    scorep_oa_sockets_write_line(connection_socket_, welcome);
    
    printf(" OK\n");

    return connection_socket_;
}

int RegServ::execute_test()
{
	printf("Registry service: starting test...\n");
	std::map< int , RegEntry* >::iterator it;
	printf("Registry service: connecting to monitoring processes...");	
	for( it=reg_data_.begin(); it!=reg_data_.end(); it++ )
	{
	
		int sock=scorep_oa_sockets_client_connect_retry(it->second->node.c_str(),it->second->port,1);
		if (sock<0)
		{
			printf("\nRegistry service: test failed!!! could not connect to the monitoring process at %s:%d\n",it->second->node.c_str(),it->second->port);
			return -1;
		}
	
		it->second->test_comm_sock=sock;
	}
	printf("OK\n");	
	
	///TODO read from test scenario file and send requests line by line to the all the registered monitor processes using example:
	
	/*
	 	if something goes wrong print the error message to the stdout and return -1;
	 */
	/*
	
	while (EOF) 
	///loop over lines in the scenario file
	{	
		// read one line from the scenario file into buffer
		
		for( it=reg_data_.begin(); it!=reg_data_.end(); it++ )
		/// loop over registered monitoring processes
		{
					
			scorep_oa_sockets_write_line( it->second->test_comm_sock, buffer );
			
			int maxlen = MAX_MESSAGE_SIZE;
			char buf[ maxlen ];
			buf[ 0 ] = 0;
			bzero( buf, maxlen );
		
			int length;
			while ( ( length = scorep_oa_sockets_read_line( it->second->test_comm_sock, buf, maxlen ) ) == 0 )
			{
			}
			printf("%s",buf);
			
		}
	 }
	 */
	for( it=reg_data_.begin(); it!=reg_data_.end(); it++ )
	{
		close(it->second->test_comm_sock);
	}
	printf("Registry service: test successfully complete!\n");
	return 0;
}


void usage( int argc, char* argv[] )
{
  fprintf(stderr, "Usage: %s <portnumber>\n", argv[0]);
}

int main( int argc, char* argv[] ){
	
	printf("Registry service: started\n");
	
	int port=31337;
	
	RegServ regsrv;
	
	regsrv.open(50001);
	
	while (1)
	{
		regsrv.blocking_accept();
		RegServClient *client = new RegServClient(&regsrv);
		int mode=REGSRV_MODE_LISTEN;
		while (mode==REGSRV_MODE_LISTEN)
			mode=client->receive_and_handle_request();	
		regsrv.close_connection();
		delete client;
		if(mode==REGSRV_MODE_TEST)
			regsrv.execute_test();
	}
	regsrv.close_server();
	printf("Registry service: finished");
  return 0;
}
