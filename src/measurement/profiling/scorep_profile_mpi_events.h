/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
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

#ifndef SCOREP_PROFILE_MPI_EVENTS_H
#define SCOREP_PROFILE_MPI_EVENTS_H

/**
 * @file        scorep_profile_mpi_events.h
 *
 */

#include <SCOREP_DefinitionHandles.h>

SCOREP_MetricHandle
scorep_profile_get_bytes_recv_metric_handle( void );

SCOREP_MetricHandle
scorep_profile_get_bytes_send_metric_handle( void );

#endif // SCOREP_PROFILE_MPI_EVENTS_H
