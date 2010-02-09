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


/**
 * @file       SILC_DefinitionLockingSerial.c
 *             Implementation of SILC_DefinitionLocking.h in the serial and
 *             MPI case.
 * @todo       When using threaded MPI this implementation may not be sufficient.
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status ALPHA
 *
 *
 */


#include <SILC_DefinitionLocking.h>


void
SILC_LockSourceFileDefinition()
{
    /* Nothing to do when running serial. */
};

void
SILC_UnlockSourceFileDefinition()
{
    /* Nothing to do when running serial. */
};


void
SILC_LockRegionDefinition()
{
    /* Nothing to do when running serial. */
};

void
SILC_UnlockRegionDefinition()
{
    /* Nothing to do when running serial. */
};


void
SILC_LockMPICommunicatorDefinition()
{
    /* Nothing to do when running serial. */
};

void
SILC_UnlockMPICommunicatorDefinition()
{
    /* Nothing to do when running serial. */
};


void
SILC_LockMPIWindowDefinition()
{
    /* Nothing to do when running serial. */
};

void
SILC_UnlockMPIWindowDefinition()
{
    /* Nothing to do when running serial. */
};


void
SILC_LockMPICartesianTopologyDefinition()
{
    /* Nothing to do when running serial. */
};

void
SILC_UnlockMPICartesianTopologyDefinition()
{
    /* Nothing to do when running serial. */
};


void
SILC_LockMPICartesianCoordsDefinition()
{
    /* Nothing to do when running serial. */
};

void
SILC_UnlockMPICartesianCoordsDefinition()
{
    /* Nothing to do when running serial. */
};


void
SILC_LockCounterGroupDefinition()
{
    /* Nothing to do when running serial. */
};

void
SILC_UnlockCounterGroupDefinition()
{
    /* Nothing to do when running serial. */
};


void
SILC_LockCounterDefinition()
{
    /* Nothing to do when running serial. */
};

void
SILC_UnlockCounterDefinition()
{
    /* Nothing to do when running serial. */
};


void
SILC_LockIOFileGroupDefinition()
{
    /* Nothing to do when running serial. */
};

void
SILC_UnlockIOFileGroupDefinition()
{
    /* Nothing to do when running serial. */
};


void
SILC_LockIOFileDefinition()
{
    /* Nothing to do when running serial. */
};

void
SILC_UnlockIOFileDefinition()
{
    /* Nothing to do when running serial. */
};


void
SILC_LockMarkerGroupDefinition()
{
    /* Nothing to do when running serial. */
};

void
SILC_UnlockMarkerGroupDefinition()
{
    /* Nothing to do when running serial. */
};


void
SILC_LockMarkerDefinition()
{
    /* Nothing to do when running serial. */
};

void
SILC_UnlockMarkerDefinition()
{
    /* Nothing to do when running serial. */
};


void
SILC_LockParameterDefinition()
{
    /* Nothing to do when running serial. */
};

void
SILC_UnlockParameterDefinition()
{
    /* Nothing to do when running serial. */
};
