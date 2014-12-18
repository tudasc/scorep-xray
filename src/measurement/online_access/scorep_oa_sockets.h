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


#ifndef SCOREP_OA_SOCKETS_H
#define SCOREP_OA_SOCKETS_H


/**
 * @file
 *
 * @brief   Declaration of OA communication infrastructure functions and data types
 *
 */


#include <stdlib.h>
#include <stdint.h>
#include <scorep_oa_registry_protocol.h>

#define BUFSIZE 512

typedef struct
{
    int   id;
    char* app;          // the application name
    char* site;         // the name of the site
    char* mach;         // the name of the machine
    char* node;         // the name of the node
    int   port;         // the port number
    int   pid;          // the process id
    char* comp;         // the component
    char* tag;
}       r_info;

typedef struct
{
    char* hostname_;
    int   port_;
    int   sock_;
}       registry;

int
scorep_oa_sockets_server_startup_retry( uint64_t* init_port,
                                        int       retries,
                                        int       step );

/*
 * open connection to the registry server,
 * initialize data-structures
 */
registry*
scorep_oa_sockets_open_registry( const char* hostname,
                                 int         port );


/*
 * close the connection to the registry server
 * returns 1 on success, 0 on failure
 */
int
scorep_oa_sockets_close_registry( registry* reg );


/*
 * create an entry in the registry which doesn't contain data initially,
 * returns the entry's positive ID on success 0 on failure
 */
int
scorep_oa_sockets_registry_create_entry( registry*   reg,
                                         const char* app,
                                         const char* site,
                                         const char* mach,
                                         const char* node,
                                         int         port,
                                         int         pid,
                                         const char* comp,
                                         const char* tag );

int
scorep_oa_sockets_client_connect_retry( char* hostname,
                                        int   port,
                                        int   retries );

int
scorep_oa_sockets_socket_my_read( int   fd,
                                  char* ptr );

int
scorep_oa_sockets_blockread( int   sock,
                             char* ptr,
                             int   size );

int
scorep_oa_sockets_read_line( int   sock,
                             char* str,
                             int   maxlen );

void
scorep_oa_sockets_write_line( int         sock,
                              const char* str );

void
scorep_oa_sockets_write_data( int         sock,
                              const void* buf,
                              int         nbyte );

void
scorep_oa_sockets_register_with_registry( uint64_t port,
                                          uint64_t reg_port,
                                          char*    reg_host,
                                          char*    app_name );

int
scorep_oa_sockets_server_accept_client( int sock );

#endif /* SCOREP_OA_SOCKETS_H */
