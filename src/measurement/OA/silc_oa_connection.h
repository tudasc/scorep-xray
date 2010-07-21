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


#ifndef SILC_OA_CONNECTION_H
#define SILC_OA_CONNECTION_H


/**
 * @file        silc_oa_connection.h
 * @maintainer  Yury Oleynik <oleynik@in.tum.de>
 *
 * @brief   Declaration of OA communication infrastructure functions and data types
 *
 * @status ALPHA
 */


#include "SILC_Types.h"
#include "SILC_Error.h"

extern int connection;

int
///@TODO switch to appropriate connection object which contains call backs to the chosen communication layer
silc_oa_connection_connect
(
);

SILC_Error_Code
silc_oa_connection_disconnect
(
    int connection                              ///@TODO switch to appropriate connection object which contains call backs to the chosen communication layer
);

SILC_Error_Code
silc_oa_connection_send_string
(
    int         connection,                     ///@TODO switch to appropriate connection object which contains call backs to the chosen communication layer
    const char* message_string
);

SILC_Error_Code
silc_oa_connection_send_data
(
    int   connection,                                   ///@TODO switch to appropriate connection object which contains call backs to the chosen communication layer
    void* message_data,
    int   size,
    int   type_size
);

int
silc_oa_connection_read_string
(
    int   connection,                   ///@TODO switch to appropriate connection object which contains call backs to the chosen communication layer
    char* message_string,
    int   maxlen
);

#endif /* SILC_OA_CONNECTION_H */
