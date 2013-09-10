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


#ifndef SCOREP_OA_CONNECTION_H
#define SCOREP_OA_CONNECTION_H


/**
 * @file
 *
 * @brief   Declaration of OA communication infrastructure functions and data types
 *
 */

#include <SCOREP_ErrorCodes.h>

extern int      connection;
extern uint64_t silc_oa_port;
extern uint64_t silc_oa_registry_port;
extern char*    silc_oa_registry_host;
extern char*    silc_oa_app_name;

int
scorep_oa_connection_connect
(
);

SCOREP_ErrorCode
scorep_oa_connection_disconnect
(
    int connection
);

SCOREP_ErrorCode
scorep_oa_connection_send_string
(
    int         connection,
    const char* message_string
);

SCOREP_ErrorCode
scorep_oa_connection_send_data
(
    int   connection,
    void* message_data,
    int   size,
    int   type_size
);

int
scorep_oa_connection_read_string
(
    int   connection,
    char* message_string,
    int   maxlen
);

#endif /* SCOREP_OA_CONNECTION_H */
