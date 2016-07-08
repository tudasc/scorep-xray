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


#ifndef SCOREP_OA_SOCKETS_H
#define SCOREP_OA_SOCKETS_H


/**
 * @file        regsrv_sockets.h
 *
 * @brief   Declaration of OA communication infrastructure functions and data types
 *
 */

#ifdef __cplusplus
extern "C" {
#endif


int
scorep_oa_sockets_server_startup_retry( int* initPort,
                                        int  retries,
                                        int  step );

int
scorep_oa_sockets_client_connect_retry( const char* hostname,
                                        int         port,
                                        int         retries );

int
scorep_oa_sockets_blockread( int   sock,
                             char* ptr,
                             int   size );

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

int
scorep_oa_sockets_server_accept_client( int sock );

#ifdef __cplusplus
}
#endif

#endif /* SCOREP_OA_SOCKETS_H */
