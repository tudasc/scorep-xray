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


#ifndef SILC_INTERNAL_PLATFORM_H
#define SILC_INTERNAL_PLATFORM_H


/**
 * @file        silc_platform.h
 * @maintainer  Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @brief Platform dependent functionality.
 *
 */


#include <SILC_Error.h>


/**
 * Platform dependent initialization.
 *
 * Called from @ref SILC_InitMeasurement.
 *
 */
SILC_Error_Code
SILC_Platform_Init
(
    void
);


/**
 * The platform specific wall-clock timestamp function.
 *
 * @scope internal to the measurement system.
 * @exported as @ref SILC_GetWallClockTime
 */
SILC_Time
SILC_Platform_GetWallClockTime
(
    void
);


/**
 * The platform specific wall time resolution function.
 *
 */
SILC_Time
SILC_Platform_GetClockResolution
(
    void
);


/**
 * Gives the name of the running executable in a malloc'ed buffer.
 *
 */
char*
SILC_Platform_ExecName
(
    void
);


/**
 * Gives the name of the running architecture/platform
 * in a malloc'ed buffer.
 *
 */
char*
SILC_Platform_Name
(
    void
);


/**
 * Gives the directory of the global filesytem in a malloc'ed buffer.
 *
 */
char*
SILC_Platform_GDir
(
    void
);


/**
 * Gives the directory of the local filesytem in a malloc'ed buffer.
 *
 */
char*
SILC_Platform_LDir
(
    void
);


/**
 * Gives a unique id for the running machine (Ie. IP of the cluster node).
 *
 */
uint64_t
SILC_Platform_NodeId
(
    void
);


/**
 * Gives the name of the running machine (Ie. hostname) in a malloc'ed buffer.
 *
 */
char*
SILC_Platform_NodeName
(
    void
);


/**
 * Gives the number of cpus on the running machine.
 *
 * @question: is this the available/possible/allowed number of cpus?
 */
uint64_t
SILC_Platform_NumCpus
(
    void
);


/**
 * Gives the number of nodes/machines.
 *
 */
uint64_t
SILC_Platform_NumNodes
(
    void
);


#endif /* SILC_INTERNAL_PLATFORM_H */
