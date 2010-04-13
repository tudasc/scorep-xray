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

#ifndef SILC_PROFILE_H
#define SILC_PROFILE_H

/**
 * @file        SILC_Profile.h
 * @maintainer  Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @brief Interface called by the measurement core.
 *
 */

#include <stddef.h>
#include <stdint.h>

#include <SILC_Thread_Types.h>
#include <SILC_Types.h>
#include <SILC_DefinitionHandles.h>

//typedef void* SILC_StringHandle;

/* ------------------------------------------------------------------ Type definitions */

/**
   Thread local data for the profiling system.
 */
typedef struct SILC_Profile_LocationData SILC_Profile_LocationData;


/* ----------------------------------------------------- Initialization / Finalization */

/**
   Initializes the Profiling system. Needed to be called before any other
   profile function is called.
   @param maxCallpathDepth Allows to limit the depth of the calltree. If the current
                           callpath becomes longer than specified by this parameter,
                           no further child nodes for this callpath are created.
                           This limit allows a reduction of the number of callpathes,
                           especially, if the application contains recursive function
                           calls.
   @param maxCallpathNum Allows to limit the number of nodes in the calltree. If the
                         number of nodes in the calltree reaches its limit, no further
                         callpathes are created. All new callpathes are collapsed into
                         a single node. This parameter allows to limit the memory
                         usage of the profile.
   @param numDenseMetrics The number of dense metrics which are recorded for each
                          callpath. It is assumed that a metric value is provided on
                          every enter and exit event. The difference of the metric value
                          between exit and enter is calculated and included in the
                          statistics.
   @param metrics An array of metric definitions. It is assmued, that on every enter
                  and exit event, the measured metrics are given in the same order as
                  the definitions in this array.
 */
void
SILC_Profile_Initialize( uint32_t            maxCallpathDepth,
                         uint32_t            maxCallpathNum,
                         int32_t             numDenseMetrics,
                         SILC_CounterHandle* metrics );

/**
   Deletes a existing profile and profile definition. Before other profile functions
   are called, the profiling system needs to be reinitialized by a call to
   @ref SILC_Profile_Initialize.
 */
void
SILC_Profile_Finalize();


/* -------------------------------------------------------------------- Callpath Events */

/**
   Called on enter events to update the profile accoringly.
   @param thread A pointer to the thread location data of the thread that executed
                 the enter event.
   @param region The handle of the entered region.
   @param type   The region type of the entered region.
   @param timestamp The timestamp, when the region was entered.
   @param metrics   An array with metric samples which were taken on the enter event.
                    The samples must be in the same order as the metric defintions
                    at the @ref SILC_Profile_Initialize call.
 */
void
SILC_Profile_Enter( SILC_Thread_LocationData* thread,
                    SILC_RegionHandle         region,
                    SILC_RegionType           type,
                    uint64_t                  timestamp,
                    uint64_t*                 metrics );

/**
   Called on exit events to update the profile accoringly.
   @param thread A pointer to the thread location data of the thread that executed
                 the exit event.
   @param region The handle of the left region.
   @param timestamp The timestamp, when the region was left.
   @param metrics   An array with metric samples which were taken on the exit event.
                    The samples must be in the same order as the metric defintions
                    at the @ref SILC_Profile_Initialize call.
 */
void
SILC_Profile_Exit( SILC_Thread_LocationData* thread,
                   SILC_RegionHandle         region,
                   uint64_t                  timestamp,
                   uint64_t*                 metrics );

/**
   Called when a user metric / atomic / context event for integer values was triggered.
   @param thread A pointer to the thread location data of the thread that executed
                 the event.
   @param metric Handle of the triggered metric.
   @param value  Sample for the metric.
 */
void
SILC_Profile_TriggerInteger( SILC_Thread_LocationData* thread,
                             SILC_CounterHandle        metric,
                             uint64_t                  value );

/**
   Called when a user metric / atomic / context event for double values was triggered.
   @param thread A pointer to the thread location data of the thread that executed
                 the event.
   @param metric Handle of the triggered metric.
   @param value  Sample for the metric.
 */
void
SILC_Profile_TriggerDouble( SILC_Thread_LocationData* thread,
                            SILC_CounterHandle        metric,
                            double                    value );

/**
   Called when a string parameter was triggered
   @param thread A pointer to the thread location data of the thread that executed
                 the event.
   @param param  Handle of the triggered parameter.
   @param string Handle of the parameter string value.
 */
void
SILC_Profile_ParameterString( SILC_Thread_LocationData* thread,
                              SILC_ParameterHandle      param,
                              SILC_StringHandle         string );

/**
   Called when a integer parameter was triggered
   @param thread A pointer to the thread location data of the thread that executed
                 the event.
   @param param  Handle of the triggered parameter.
   @param value  The parameter integer value.
 */
void
SILC_Profile_ParameterInteger( SILC_Thread_LocationData* thread,
                               SILC_ParameterHandle      param,
                               int64_t                   value );

/* --------------------------------------------------------------------- Thread Events */

/**
 * Allocate and initialize a valid SILC_Profile_LocationData object.
 *
 */
SILC_Profile_LocationData*
SILC_Profile_CreateLocationData();


/**
 * Clean up @a profileLocationData at the end of a phase or at the end of the
 * measurement.
 *
 * @param profileLocationData The object to be deleted
 */
void
SILC_Profile_DeleteLocationData( SILC_Profile_LocationData* profileLocationData );

/**
   Called if one or more threads are created by this region. It is used to Mark the
   creation point in the profile tree. This allows the reconstruction of the full
   callpathes per thread later.
   @param threadData  A pointer to the thread location data of the thread that executed
                      the event.
   @param  maxChildThreads Not used. Uppe bound of the number of created threads.
 */
void
SILC_Profile_OnFork( SILC_Thread_LocationData* threadData,
                     size_t                    maxChildThreads );

/**
 * Triggered on thread creation, i.e. when a thread is encountered the first
 * time. Note that several thread can share the same location data.
 *
 * @param locationData Location data of the current thread.
 * @param parentLocationData Location data of the parent thread, may equal @a
 * locationData.
 */
void
SILC_Profile_OnThreadCreation( SILC_Thread_LocationData* locationData,
                               SILC_Thread_LocationData* parentLocationData );

/**
 * Triggered at the start of every thread/parallel region. Always triggered,
 * even after thread creation. In contrast to creation this function may be
 * triggered multiple times, e.g. if we reenter a parallel region again or if
 * we reuse the location/thread in a different parallel region.
 *
 * @param locationData Location data of the current thread inside the paralell
 * region.
 * @param parentLocationData Location data of the parent thread, may equal @a
 * locationData.
 */
void
SILC_Profile_OnThreadActivation( SILC_Thread_LocationData* locationData,
                                 SILC_Thread_LocationData* parentLocationData );


/**
 * Triggered after the end of every thread/parallel region, i.e. in the join
 * event.
 *
 * @param locationData Location data of the deactivated thread inside the
 * parallel region.
 * @param parentLocationData Location data of the parent thread, may equal @a
 * locationData.
 */
void
SILC_Profile_OnThreadDectivation( SILC_Thread_LocationData* locationData,
                                  SILC_Thread_LocationData* parentLocationData );


/**
 * Triggered on location creation, i.e. when a location is encountered the first
 * time. Note that several threads can share the same location data.
 *
 * @param locationData Location data of the current thread.
 * @param parentLocationData Location data of the parent thread, may equal @a
 * locationData.
 */
void
SILC_Profile_OnLocationCreation( SILC_Thread_LocationData* locationData,
                                 SILC_Thread_LocationData* parentLocationData );


#endif // SILC_PROFILE_H
