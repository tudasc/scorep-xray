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


#ifndef SILC_OA_SOCKETS_H
#define SILC_OA_SOCKETS_H


/**
 * @file        silc_oa_sockets.h
 * @maintainer  Yury Oleynik <oleynik@in.tum.de>
 *
 * @brief   Declaration of OA communication infrastructure functions and data types
 *
 * @status alpha
 */


#include "silc_utility/SILC_Utils.h"

#include <silc_oa_registry_protocol.h>

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
silc_oa_sockets_server_startup_retry
(
    int* init_port,
    int  retries,
    int  step
);

/*
 * open connection to the registry server,
 * initialize data-structures
 */
registry*
silc_oa_sockets_open_registry
(
    const char* hostname,
    int         port
);


/*
 * close the connection to the registry server
 * returns 1 on success, 0 on failure
 */
int
silc_oa_sockets_close_registry
(
    registry* reg
);


/*
 * create an entry in the registry which doesn't contain data initially,
 * returns the entry's positive ID on success 0 on failure
 */
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
);

int
silc_oa_sockets_client_connect_retry
(
    char* hostname,
    int   port,
    int   retries
);

int
silc_oa_sockets_socket_my_read
(
    int   fd,
    char* ptr
);

int
silc_oa_sockets_blockread
(
    int   sock,
    char* ptr,
    int   size
);

int
silc_oa_sockets_read_line
(
    int   sock,
    char* str,
    int   maxlen
);

void
silc_oa_sockets_write_line
(
    int         sock,
    const char* str
);

void
silc_oa_sockets_write_data
(
    int         sock,
    const void* buf,
    int         nbyte
);

void
silc_oa_sockets_register_with_registry
(
    int port
);

int
silc_oa_sockets_server_accept_client
(
    int sock
);

#endif /* SILC_OA_SOCKETS_H */
