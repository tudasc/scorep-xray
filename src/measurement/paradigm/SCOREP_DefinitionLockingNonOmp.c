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


/**
 * @file       SCOREP_DefinitionLockingNonOmp.c
 *             Implementation of SCOREP_DefinitionLocking.h in the non OpenMP
 *             case.
 * @todo       When using threaded MPI this implementation may not be sufficient.
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 *
 */


#include <config.h>

#include <SCOREP_DefinitionLocking.h>
#include <scorep_definition_locking.h>


void
SCOREP_DefinitionLocks_Initialize()
{
    /* Nothing to do when running serial. */
}

void
SCOREP_DefinitionLocks_Finalize()
{
    /* Nothing to do when running serial. */
}

void
SCOREP_LockSourceFileDefinition()
{
    /* Nothing to do when running serial. */
}

void
SCOREP_UnlockSourceFileDefinition()
{
    /* Nothing to do when running serial. */
}


void
SCOREP_LockRegionDefinition()
{
    /* Nothing to do when running serial. */
}

void
SCOREP_UnlockRegionDefinition()
{
    /* Nothing to do when running serial. */
}


void
SCOREP_LockMPICommunicatorDefinition()
{
    /* Nothing to do when running serial. */
}

void
SCOREP_UnlockMPICommunicatorDefinition()
{
    /* Nothing to do when running serial. */
}


void
SCOREP_LockMPIWindowDefinition()
{
    /* Nothing to do when running serial. */
}

void
SCOREP_UnlockMPIWindowDefinition()
{
    /* Nothing to do when running serial. */
}


void
SCOREP_LockMPICartesianTopologyDefinition()
{
    /* Nothing to do when running serial. */
}

void
SCOREP_UnlockMPICartesianTopologyDefinition()
{
    /* Nothing to do when running serial. */
}


void
SCOREP_LockMPICartesianCoordsDefinition()
{
    /* Nothing to do when running serial. */
}

void
SCOREP_UnlockMPICartesianCoordsDefinition()
{
    /* Nothing to do when running serial. */
}


void
SCOREP_LockCounterGroupDefinition()
{
    /* Nothing to do when running serial. */
}

void
SCOREP_UnlockCounterGroupDefinition()
{
    /* Nothing to do when running serial. */
}


void
SCOREP_LockCounterDefinition()
{
    /* Nothing to do when running serial. */
}

void
SCOREP_UnlockCounterDefinition()
{
    /* Nothing to do when running serial. */
}


void
SCOREP_LockIOFileGroupDefinition()
{
    /* Nothing to do when running serial. */
}

void
SCOREP_UnlockIOFileGroupDefinition()
{
    /* Nothing to do when running serial. */
}


void
SCOREP_LockIOFileDefinition()
{
    /* Nothing to do when running serial. */
}

void
SCOREP_UnlockIOFileDefinition()
{
    /* Nothing to do when running serial. */
}


void
SCOREP_LockMarkerGroupDefinition()
{
    /* Nothing to do when running serial. */
}

void
SCOREP_UnlockMarkerGroupDefinition()
{
    /* Nothing to do when running serial. */
}


void
SCOREP_LockMarkerDefinition()
{
    /* Nothing to do when running serial. */
}

void
SCOREP_UnlockMarkerDefinition()
{
    /* Nothing to do when running serial. */
}


void
SCOREP_LockParameterDefinition()
{
    /* Nothing to do when running serial. */
}

void
SCOREP_UnlockParameterDefinition()
{
    /* Nothing to do when running serial. */
}


void
SCOREP_LockLocationDefinition()
{
    /* Nothing to do when running serial. */
}

void
SCOREP_UnlockLocationDefinition()
{
    /* Nothing to do when running serial. */
}
