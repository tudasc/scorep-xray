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

#ifndef SILC_PROFILE_TREE_H
#define SILC_PROFILE_TREE_H

/**
 * @file        silc_profile_tree.h
 * @maintainer  Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @brief Interface called by the measurement core, to build the profile call tree.
 *
 */

#include "silc_profile_thread_interaction.h"
#include <stddef.h>
#include <stdint.h>
#include <SILC_Thread_Types.h>
#include <SILC_Types.h>

/* ----------------------------------------------------- Initialization / Finalization */

/** Initializes the Profiling system */
void
SILC_Profile_Initialize( int32_t             numDenseMetrics,
                         SILC_CounterHandle* metrics );

/** Clean up */
void
SILC_Profile_Finalize();


/* ---------------------------------------------------------------------------- Events */

/** Enter a region */
void
SILC_Profile_Enter( SILC_Thread_LocationData* thread,
                    SILC_RegionHandle         region,
                    SILC_RegionType           type,
                    uint64_t                  timestamp,
                    uint64_t*                 metrics );

/** Exit a region */
void
SILC_Profile_Exit( SILC_Thread_LocationData* thread,
                   SILC_RegionHandle         region,
                   uint64_t                  timestamp,
                   uint64_t*                 metrics );

/** Triggers a user metric / atomic / context event */
void
SILC_Profile_TriggerInteger( SILC_Thread_LocationData* thread,
                             SILC_CounterHandle        metric,
                             uint64_t                  value );

/** Triggers a user metric / atomic / context event */
void
SILC_Profile_TriggerDouble( SILC_Thread_LocationData* thread,
                            SILC_CounterHandle        metric,
                            double                    value );

/** String parameter */
void
SILC_Profile_ParameterString( SILC_Thread_LocationData* thread,
                              SILC_ParameterHandle      param,
                              SILC_StringHandle         string );

/** Integer parameter */
void
SILC_Profile_ParameterInteger( SILC_Thread_LocationData* thread,
                               SILC_ParameterHandle      param,
                               int64_t                   value );

/** Called if one or more threads are created by this region. */
void
SILC_Profile_OnFork( SILC_Thread_LocationData* threadData,
                     size_t                    maxChildThreads );

#endif // SILC_PROFILE_TREE_H
