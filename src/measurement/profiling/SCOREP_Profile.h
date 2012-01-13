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

#ifndef SCOREP_PROFILE_H
#define SCOREP_PROFILE_H

/**
 * @file        SCOREP_Profile.h
 * @maintainer  Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @brief Interface called by the measurement core to construct the profile.
 *
 * The profile is created with the following structure:
 * For each location a node of type @ref scorep_profile_node_thread_root is created which
 * are linked together as siblings. The children of the @ref scorep_profile_node_thread_root
 * nodes are of type @ref scorep_profile_node_thread_start and represent each an activation
 * (start of a parallel region in OpenMP). In the initial thread one
 * @ref scorep_profile_node_thread_start node is created, too. They contain informatation
 * from which callpath they are created. Their children are the region or parameter
 * nodes called from within the parallel region.
 *
 * Whenever a region is entered, a child of the node representing the current callpath
 * is created, if it does not already exist. If a parameter is triggered a new node
 * for this parameter value is created if it does not already exists.
 *
 * During post-processing the structure of the profile altered. First the nodes of
 * type @scorep_profile_node_thread_start are expanded, so that for each thread the full
 * calltree appears below its root node.
 */

#include <stddef.h>
#include <stdint.h>

#include <SCOREP_Thread_Types.h>
#include <SCOREP_Types.h>
#include <SCOREP_DefinitionHandles.h>

/* ------------------------------------------------------------------ Type definitions */

/**
   Thread local data for the profiling system.
 */
typedef struct SCOREP_Profile_LocationData SCOREP_Profile_LocationData;

/**
   The enumeration type for the post processing flag. The postprecessing flag
   is a bitvector which allows to specify which steps the post-processing
   in function @ref SCOREP_Profile_Process are perfomed.
   Each bit corresponds to one possible step. Nevertheless, some steps have others
   as prerequiste, and enable them if they are chosen.
   Thus, combine steps with the bitwise or opeator '|' and not with '+'.
 */
typedef enum
{
    SCOREP_Profile_ProcessNone     = 0,
    SCOREP_Profile_ProcessThreads  = 1,
    SCOREP_Profile_ProcessCallpath = 3, /* Threadprocessing is a prerequisiste */
    SCOREP_Profile_ParamToRegion   = 4,
    SCOREP_Profile_Phase           = 8,

    SCOREP_Profile_ProcessDefault  = 15
} SCOREP_Profile_ProcessingFlag;

/**
   The type for specifing the output format of the profile.
 */
typedef enum
{
    SCOREP_Profile_OutputNone        = 0,
    SCOREP_Profile_OutputTauSnapshot = 1,
    SCOREP_Profile_OutputCube4       = 2,
} SCOREP_Profile_OutputFormat;

//extern const int SCOREP_Profile_OutputDefault;

/* ----------------------------------------------------- Initialization / Finalization */

/**
   Registers configuration variables to the SCOREP measurement system. Enables the
   configuration and program start from SCOREP configuration mechanisms.
 */
void
SCOREP_Profile_Register();

/**
   Initializes the Profiling system. Needed to be called before any other
   profile function is called.
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
SCOREP_Profile_Initialize( uint8_t              numDenseMetrics,
                           SCOREP_MetricHandle* metrics );

/**
   Deletes a existing profile and profile definition. Before other profile functions
   are called, the profiling system needs to be reinitialized by a call to
   @ref SCOREP_Profile_Initialize.
 */
void
SCOREP_Profile_Finalize();


/**
   Post processes a profile. This function performs transfomations on the calltree
   like expansion of thread start nodes and assignment of callpath handles to every node.
   Which steps are included can be specified via parameters. It must be called before
   unification in order to register callpathe sto be unified.
 */
void
SCOREP_Profile_Process( SCOREP_Profile_ProcessingFlag processFlags );

/**
   Writes the Profile. The output format can be set via environment variable
   SCOREP_ProfileFormat. Possible values are None, TauSnapshot, Cube4, Default.
   Should be called after unification.
 */
void
SCOREP_Profile_Write();

/**
   Configures the callpath size for the next profile. The configuration is stored and
   applied on the next (re)initialization of the profile. It has no effect on the current
   profile.
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
 */
void
SCOREP_Profile_SetCalltreeConfiguration( uint32_t maxCallpathDepth,
                                         uint32_t maxCallpathNum );

/* -------------------------------------------------------------------- Callpath Events */

/**
   Called on enter events to update the profile accoringly.
   @param thread A pointer to the thread location data of the thread that executed
                 the enter event.
   @param region The handle of the entered region.
   @param type   The region type of the entered region.
   @param timestamp The timestamp, when the region was entered.
   @param metrics   An array with metric samples which were taken on the enter event.
                    The samples must be in the same order as the metric definitions
                    at the @ref SCOREP_Profile_Initialize call.
 */
void
SCOREP_Profile_Enter( SCOREP_Thread_LocationData* thread,
                      SCOREP_RegionHandle         region,
                      SCOREP_RegionType           type,
                      uint64_t                    timestamp,
                      uint64_t*                   metrics );

/**
   Called on exit events to update the profile accoringly.
   @param thread A pointer to the thread location data of the thread that executed
                 the exit event.
   @param region The handle of the left region.
   @param timestamp The timestamp, when the region was left.
   @param metrics   An array with metric samples which were taken on the exit event.
                    The samples must be in the same order as the metric definitions
                    at the @ref SCOREP_Profile_Initialize call.
 */
void
SCOREP_Profile_Exit( SCOREP_Thread_LocationData* thread,
                     SCOREP_RegionHandle         region,
                     uint64_t                    timestamp,
                     uint64_t*                   metrics );

/**
   Called when a user metric / atomic / context event for integer values was triggered.
   @param thread A pointer to the thread location data of the thread that executed
                 the event.
   @param metric Handle of the triggered metric.
   @param value  Sample for the metric.
 */
void
SCOREP_Profile_TriggerInteger( SCOREP_Thread_LocationData* thread,
                               SCOREP_MetricHandle         metric,
                               uint64_t                    value );

/**
   Called when a user metric / atomic / context event for double values was triggered.
   @param thread A pointer to the thread location data of the thread that executed
                 the event.
   @param metric Handle of the triggered metric.
   @param value  Sample for the metric.
 */
void
SCOREP_Profile_TriggerDouble( SCOREP_Thread_LocationData* thread,
                              SCOREP_MetricHandle         metric,
                              double                      value );

/**
   Called when a string parameter was triggered
   @param thread A pointer to the thread location data of the thread that executed
                 the event.
   @param param  Handle of the triggered parameter.
   @param string Handle of the parameter string value.
 */
void
SCOREP_Profile_ParameterString( SCOREP_Thread_LocationData* thread,
                                SCOREP_ParameterHandle      param,
                                SCOREP_StringHandle         string );

/**
   Called when a integer parameter was triggered
   @param thread A pointer to the thread location data of the thread that executed
                 the event.
   @param param  Handle of the triggered parameter.
   @param value  The parameter integer value.
 */
void
SCOREP_Profile_ParameterInteger( SCOREP_Thread_LocationData* thread,
                                 SCOREP_ParameterHandle      param,
                                 int64_t                     value );

/* --------------------------------------------------------------------- Thread Events */

/**
 * Allocate and initialize a valid SCOREP_Profile_LocationData object.
 *
 */
SCOREP_Profile_LocationData*
SCOREP_Profile_CreateLocationData();


/**
 * Clean up @a profileLocationData at the end of a phase or at the end of the
 * measurement.
 *
 * @param profileLocationData The object to be deleted
 */
void
SCOREP_Profile_DeleteLocationData( SCOREP_Profile_LocationData* profileLocationData );

/**
   Called if one or more threads are created by this region. It is used to Mark the
   creation point in the profile tree. This allows the reconstruction of the full
   callpathes per thread later.
   @param threadData  A pointer to the thread location data of the thread that executed
                      the event.
   @param  maxChildThreads Not used. Uppe bound of the number of created threads.
 */
void
SCOREP_Profile_OnFork( SCOREP_Thread_LocationData* threadData,
                       size_t                      maxChildThreads );

/**
 * Triggered on thread creation, i.e. when a thread is encountered the first
 * time. Note that several thread can share the same location data.
 *
 * @param locationData Location data of the current thread.
 * @param parentLocationData Location data of the parent thread, may equal @a
 * locationData.
 */
void
SCOREP_Profile_OnThreadCreation( SCOREP_Thread_LocationData* locationData,
                                 SCOREP_Thread_LocationData* parentLocationData );

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
SCOREP_Profile_OnThreadActivation( SCOREP_Thread_LocationData* locationData,
                                   SCOREP_Thread_LocationData* parentLocationData );


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
SCOREP_Profile_OnThreadDectivation( SCOREP_Thread_LocationData* locationData,
                                    SCOREP_Thread_LocationData* parentLocationData );


/**
 * Triggered on location creation, i.e. when a location is encountered the first
 * time. Note that several threads can share the same location data.
 *
 * @param locationData Location data of the current thread.
 * @param parentLocationData Location data of the parent thread, may equal @a
 * locationData.
 */
void
SCOREP_Profile_OnLocationCreation( SCOREP_Thread_LocationData* locationData,
                                   SCOREP_Thread_LocationData* parentLocationData );


#endif // SCOREP_PROFILE_H
