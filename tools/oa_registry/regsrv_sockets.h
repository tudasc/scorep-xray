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
 * @file        regsrv_sockets.h
 *
 * @brief   Declaration of OA communication infrastructure functions and data types
 *
 */

#ifndef REGSRV_SOCKETS_H
#define REGSRV_SOCKETS_H

#ifdef __cplusplus
extern "C" {
#endif


#define BUFSIZE 512


int
scorep_oa_sockets_server_startup_retry
(
    int* init_port,
    int  retries,
    int  step
);


int
scorep_oa_sockets_client_connect_retry
(
    const char* hostname,
    int         port,
    int         retries
);

int
scorep_oa_sockets_socket_my_read
(
    int   fd,
    char* ptr
);

int
scorep_oa_sockets_blockread
(
    int   sock,
    char* ptr,
    int   size
);

int
scorep_oa_sockets_read_line
(
    int   sock,
    char* str,
    int   maxlen
);

void
scorep_oa_sockets_write_line
(
    int         sock,
    const char* str
);

void
scorep_oa_sockets_write_data
(
    int         sock,
    const void* buf,
    int         nbyte
);

int
scorep_oa_sockets_server_accept_client
(
    int sock
);

#ifdef __cplusplus
}
#endif

#endif /* SCOREP_OA_SOCKETS_H */
