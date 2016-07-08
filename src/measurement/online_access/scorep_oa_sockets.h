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
 * Copyright (c) 2009-2011, 2015-2016,
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
    char* hostname_;
    int   port_;
    int   sock_;
} registry;

int
scorep_oa_sockets_server_startup_retry( uint64_t* initPort,
                                        int       retries,
                                        int       step );

int
scorep_oa_sockets_read_line( int   sock,
                             char* str,
                             int   maxLen );

void
scorep_oa_sockets_write_line( int         sock,
                              const char* str );

void
scorep_oa_sockets_write_data( int         sock,
                              const void* buf,
                              int         nbyte );

void
scorep_oa_sockets_register_with_registry( uint64_t port,
                                          uint64_t regPort,
                                          char*    regHost,
                                          char*    appName );

int
scorep_oa_sockets_server_accept_client( int sock );

int
scorep_oa_sockets_server_disconnect( int sock );

#endif /* SCOREP_OA_SOCKETS_H */
