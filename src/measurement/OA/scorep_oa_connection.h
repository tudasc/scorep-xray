/*
 * This file is part of the SCOREP project (http://www.scorep.de)
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


#ifndef SCOREP_OA_CONNECTION_H
#define SCOREP_OA_CONNECTION_H


/**
 * @file        scorep_oa_connection.h
 * @maintainer  Yury Oleynik <oleynik@in.tum.de>
 *
 * @brief   Declaration of OA communication infrastructure functions and data types
 *
 * @status alpha
 */

#include "scorep_utility/SCOREP_Utils.h"

extern int connection;

int
///@TODO switch to appropriate connection object which contains call backs to the chosen communication layer
scorep_oa_connection_connect
(
);

SCOREP_Error_Code
scorep_oa_connection_disconnect
(
    int connection                              ///@TODO switch to appropriate connection object which contains call backs to the chosen communication layer
);

SCOREP_Error_Code
scorep_oa_connection_send_string
(
    int         connection,                     ///@TODO switch to appropriate connection object which contains call backs to the chosen communication layer
    const char* message_string
);

SCOREP_Error_Code
scorep_oa_connection_send_data
(
    int   connection,                                   ///@TODO switch to appropriate connection object which contains call backs to the chosen communication layer
    void* message_data,
    int   size,
    int   type_size
);

int
scorep_oa_connection_read_string
(
    int   connection,                   ///@TODO switch to appropriate connection object which contains call backs to the chosen communication layer
    char* message_string,
    int   maxlen
);

#endif /* SCOREP_OA_CONNECTION_H */
