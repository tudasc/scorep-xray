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


#ifndef SCOREP_INTERNAL_PLATFORM_H
#define SCOREP_INTERNAL_PLATFORM_H


/**
 * @file        scorep_platform.h
 * @maintainer  Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @brief Platform dependent functionality.
 *
 */


#include <UTILS_Error.h>


/**
 * Platform dependent initialization.
 *
 * Called from @ref SCOREP_InitMeasurement.
 *
 */
SCOREP_ErrorCode
SCOREP_Platform_Init
(
    void
);


/**
 * The platform specific wall-clock timestamp function.
 *
 * @scope internal to the measurement system.
 * @exported as @ref SCOREP_GetWallClockTime
 */
SCOREP_Time
SCOREP_Platform_GetWallClockTime
(
    void
);


/**
 * The platform specific wall time resolution function.
 *
 */
SCOREP_Time
SCOREP_Platform_GetClockResolution
(
    void
);


/**
 * Gives the name of the running executable in a malloc'ed buffer.
 *
 */
char*
SCOREP_Platform_ExecName
(
    void
);


/**
 * Gives the name of the running architecture/platform
 * in a malloc'ed buffer.
 *
 */
char*
SCOREP_Platform_Name
(
    void
);


/**
 * Gives the directory of the global filesytem in a malloc'ed buffer.
 *
 */
char*
SCOREP_Platform_GDir
(
    void
);


/**
 * Gives the directory of the local filesytem in a malloc'ed buffer.
 *
 */
char*
SCOREP_Platform_LDir
(
    void
);


/**
 * Gives a unique id for the running machine (Ie. IP of the cluster node).
 *
 */
uint64_t
SCOREP_Platform_NodeId
(
    void
);


/**
 * Gives the name of the running machine (Ie. hostname) in a malloc'ed buffer.
 *
 */
char*
SCOREP_Platform_NodeName
(
    void
);


/**
 * Gives the number of cpus on the running machine.
 *
 * @question: is this the available/possible/allowed number of cpus?
 */
uint64_t
SCOREP_Platform_NumCpus
(
    void
);


/**
 * Gives the number of nodes/machines.
 *
 */
uint64_t
SCOREP_Platform_NumNodes
(
    void
);


#endif /* SCOREP_INTERNAL_PLATFORM_H */
