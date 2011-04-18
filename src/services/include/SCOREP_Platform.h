#ifndef SCOREP_PLATFORM_H
#define SCOREP_PLATFORM_H

/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


/**
 * @file       SCOREP_Platform.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */

#include <stdbool.h>
#include <stdint.h>

/*

   (0) Platform specific initialization:

   Called once on every node to allow platform specific initializations

   - elg_pform_init()

   (1) Event notification:

   The module is notified upon the occurrence of the following events
   by calling one of the event-notification functions:

   - elg_pform_mpi_init():     MPI has just been initialized
   - elg_pform_mpi_finalize(): MPI will be finalized now

   (2) Clock

   If the platform supports a global clock then elg_pform_is_gclock()
   returns a value != 0, otherwise it returns 0. Depending on the
   result elg_pform_wtime() returns either the local or the global
   wall-clock time in seconds.

   (3) File system

   It is assumed that the platform provides a global file system and
   that the current working directory is part of it.  elg_pform_ldir()
   returns the value of EPK_LDIR which may be used to specify the name
   of a local directory that can be used to store temporary files.

   (3) Platform and SMP node information

   Configuration information of the total system and the SMP nodes
   where the function is called

   elg_pform_name():          Unique string identifying the platform
   elg_pform_num_nodes():     Total number of SM nodes in the system
   elg_pform_node_id():       Unique number identifying the SMP node
   elg_pform_node_name():     Unique string identifying the SMP node
   elg_pform_node_num_cpus(): Number of CPUs in SMP node
   elg_pform_hw_topol():      HW topology

 */

/*  Called once on every node to allow platform specific initializations */
void
SCOREP_Platform_Initialize();

/*  Called once on every node to allow platform specific finalizations */
void
SCOREP_Platform_Finalize();


/* is a global clock provided ? */
bool
SCOREP_Platform_ClockIsGlobal();

/* platform name */
const char*
SCOREP_Platform_GetName();

/* total number of nodes (if available, otherwise 1) */
uint32_t
SCOREP_Platform_GetNumberOfNodes();

/* unique numeric SMP-node identifier */
uint64_t
SCOREP_Platform_GetNodeId();

/* unique string SMP-node identifier */
const char*
SCOREP_Platform_GetNodeName();

/* number of CPUs */
uint32_t
SCOREP_Platform_GetNumberOfCPUs();

/* hardware topology */
typedef struct
{
    uint32_t xsize, ysize, zsize;       /* size of dimension         */
    uint32_t xcoord, ycoord, zcoord;    /* coordinates of called CPU */
    uint32_t xperiod, yperiod, zperiod; /* periodicity of dimension  */
} SCOREP_Platform_HardwareTopology;

SCOREP_Platform_HardwareTopology*
SCOREP_Platform_GetHardwareTopology();


#endif /* SCOREP_PLATFORM_H */
