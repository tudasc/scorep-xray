#ifndef SCOREP_TIMING_INTERNAL_H
#define SCOREP_TIMING_INTERNAL_H

/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 *    RWTH Aachen, Germany
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


/**
 * @file       src/measurement/scorep_runtime_management_timings.h
 *
 * Provide timing macros, variables, and utilities for timing the Score-P
 * initialization and finalization functions. You need to have
 * HAVE_SCOREP_RUNTIME_MANAGEMENT_TIMINGS defined to non-zero to activate the
 * macros. @see SCOREP_RuntimeManagement.c, scorep_mpi_enabled, and
 * SCOREP_Mpi_Init.c.
 *
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#if HAVE( SCOREP_RUNTIME_MANAGEMENT_TIMINGS )

#include <stdint.h>
#include "scorep_mpi.h"

#define SCOREP_TIMING_FUNCTIONS                                         \
    SCOREP_TIMING_TRANSFORM_OP( MeasurementDuration  )                  \
    SCOREP_TIMING_TRANSFORM_OP( SCOREP_InitMeasurement )                \
    SCOREP_TIMING_TRANSFORM_OP( SCOREP_InitMeasurementMPI )             \
    SCOREP_TIMING_TRANSFORM_OP( scorep_finalize )                       \
    SCOREP_TIMING_TRANSFORM_OP( SCOREP_ConfigInit )                     \
    SCOREP_TIMING_TRANSFORM_OP( SCOREP_Env_RegisterCoreEnvironmentVariables ) \
    SCOREP_TIMING_TRANSFORM_OP( SCOREP_Filter_Register )                \
    SCOREP_TIMING_TRANSFORM_OP( scorep_subsystems_register )            \
    SCOREP_TIMING_TRANSFORM_OP( SCOREP_Profile_Register )               \
    SCOREP_TIMING_TRANSFORM_OP( SCOREP_Tracing_Register )               \
    SCOREP_TIMING_TRANSFORM_OP( SCOREP_OA_Register )                    \
    SCOREP_TIMING_TRANSFORM_OP( SCOREP_ConfigApplyEnv )                 \
    SCOREP_TIMING_TRANSFORM_OP( SCOREP_Status_Initialize )              \
    SCOREP_TIMING_TRANSFORM_OP( SCOREP_Timer_Initialize )               \
    SCOREP_TIMING_TRANSFORM_OP( SCOREP_BeginEpoch )                     \
    SCOREP_TIMING_TRANSFORM_OP( SCOREP_CreateExperimentDir )            \
    SCOREP_TIMING_TRANSFORM_OP( SCOREP_Memory_Initialize )              \
    SCOREP_TIMING_TRANSFORM_OP( scorep_otf2_initialize )                \
    SCOREP_TIMING_TRANSFORM_OP( SCOREP_Definitions_Initialize )         \
    SCOREP_TIMING_TRANSFORM_OP( SCOREP_Thread_Initialize )              \
    SCOREP_TIMING_TRANSFORM_OP( SCOREP_Filter_Initialize )              \
    SCOREP_TIMING_TRANSFORM_OP( scorep_subsystems_initialize )          \
    SCOREP_TIMING_TRANSFORM_OP( scorep_subsystems_initialize_location ) \
    SCOREP_TIMING_TRANSFORM_OP( scorep_profile_initialize )             \
    SCOREP_TIMING_TRANSFORM_OP( scorep_trigger_exit_callbacks )         \
    SCOREP_TIMING_TRANSFORM_OP( SCOREP_SynchronizeClocks )              \
    SCOREP_TIMING_TRANSFORM_OP( SCOREP_EndEpoch )                       \
    SCOREP_TIMING_TRANSFORM_OP( SCOREP_Filter_Finalize )                \
    SCOREP_TIMING_TRANSFORM_OP( SCOREP_Tracing_FinalizeEventWriters )   \
    SCOREP_TIMING_TRANSFORM_OP( SCOREP_Profile_Process )                \
    SCOREP_TIMING_TRANSFORM_OP( SCOREP_DefineSystemTree )               \
    SCOREP_TIMING_TRANSFORM_OP( SCOREP_Unify )                          \
    SCOREP_TIMING_TRANSFORM_OP( scorep_profile_finalize )               \
    SCOREP_TIMING_TRANSFORM_OP( SCOREP_Definitions_Write )              \
    SCOREP_TIMING_TRANSFORM_OP( SCOREP_Definitions_Finalize )           \
    SCOREP_TIMING_TRANSFORM_OP( scorep_otf2_finalize )                  \
    SCOREP_TIMING_TRANSFORM_OP( SCOREP_Location_Finalize )              \
    SCOREP_TIMING_TRANSFORM_OP( scorep_subsystems_finalize )            \
    SCOREP_TIMING_TRANSFORM_OP( scorep_dump_config )                    \
    SCOREP_TIMING_TRANSFORM_OP( SCOREP_ConfigFini )                     \
    SCOREP_TIMING_TRANSFORM_OP( SCOREP_RenameExperimentDir )            \
    SCOREP_TIMING_TRANSFORM_OP( SCOREP_Status_Finalize )                \
    SCOREP_TIMING_TRANSFORM_OP( scorep_subsystems_deregister )          \
    SCOREP_TIMING_TRANSFORM_OP( SCOREP_Thread_Finalize )                \
    SCOREP_TIMING_TRANSFORM_OP( SCOREP_Memory_Finalize )

enum scorep_timing_functions
{
    #define SCOREP_TIMING_TRANSFORM_OP( function ) function ## _,
    SCOREP_TIMING_FUNCTIONS
    #undef SCOREP_TIMING_TRANSFORM_OP
    scorep_timing_num_entries
};

extern uint64_t timing_start_MeasurementDuration;    // set in SCOREP_InitMeasurement(),
                                                     // used in scorep_finalize()
extern const char* scorep_timing_function_names[ scorep_timing_num_entries ];
extern double      scorep_timing_sendbuf_durations[ scorep_timing_num_entries ];
extern double      scorep_timing_recvbuf_durations_max[ scorep_timing_num_entries ];
extern double      scorep_timing_recvbuf_durations_min[ scorep_timing_num_entries ];

#define SCOREP_TIME( function )                                         \
    {                                                                   \
        uint64_t timing_start = SCOREP_GetClockTicks();                 \
        function();                                                     \
        if ( function ## _ >= 0 && function ## _ < scorep_timing_num_entries ) \
        {                                                               \
            scorep_timing_sendbuf_durations[ function ## _ ] = SCOREP_GetClockTicks() - timing_start; \
        }                                                               \
    }

#define SCOREP_TIME_WITH_ARGS( function, ... )                          \
    {                                                                   \
        uint64_t timing_start = SCOREP_GetClockTicks();                 \
        function( __VA_ARGS__ );                                        \
        if ( function ## _ >= 0 && function ## _ < scorep_timing_num_entries ) \
        {                                                               \
            scorep_timing_sendbuf_durations[ function ## _ ] = SCOREP_GetClockTicks() - timing_start; \
        }                                                               \
    }

#define SCOREP_TIME_START_TIMING( function )                            \
    uint64_t timing_start_ ## function               = SCOREP_GetClockTicks()

#define SCOREP_TIME_STOP_TIMING( function )                             \
    scorep_timing_sendbuf_durations[ function ## _ ] = SCOREP_GetClockTicks() - timing_start_ ## function

#define SCOREP_TIME_MEASUREMENT_START()                                 \
    timing_start_MeasurementDuration                 = SCOREP_GetClockTicks()

#define SCOREP_TIME_PRINT_TIMINGS()                                     \
    if ( SCOREP_Mpi_GetRank() == 0 )                                    \
    {                                                                   \
        if ( SCOREP_Mpi_HasMpi() )                                      \
        {                                                               \
            printf( "[Score-P] Score-P runtime-management timings: function    min[s]         max[s]\n" ); \
        }                                                               \
        else                                                            \
        {                                                               \
            printf( "[Score-P] Score-P runtime-management timings: function    time[s]\n" ); \
        }                                                               \
                                                                        \
                                                                        \
        for ( int i = 0; i < scorep_timing_num_entries; ++i )           \
        {                                                               \
            if ( SCOREP_Mpi_HasMpi() )                                  \
            {                                                           \
                /* special handling for times that are measured only */ \
                /* after PMPI_Finalize, i.e. they don't take part in */ \
                /* the reduce step and always belong to rank 0. */      \
                if ( scorep_timing_recvbuf_durations_min[ i ] == 0.0 && \
                     scorep_timing_recvbuf_durations_max[ i ] == 0.0 )  \
                {                                                       \
                    printf( "[Score-P] %-48s%-15e%-15e\n",              \
                            scorep_timing_function_names[ i ],          \
                            scorep_timing_sendbuf_durations[ i ] / ( double )SCOREP_GetClockResolution(), \
                            scorep_timing_sendbuf_durations[ i ] / ( double )SCOREP_GetClockResolution() ); \
                }                                                       \
                else                                                    \
                {                                                       \
                    printf( "[Score-P] %-48s%-15e%-15e\n",              \
                            scorep_timing_function_names[ i ],          \
                            scorep_timing_recvbuf_durations_min[ i ] / ( double )SCOREP_GetClockResolution(), \
                            scorep_timing_recvbuf_durations_max[ i ] / ( double )SCOREP_GetClockResolution() ); \
                }                                                        \
            }                                                           \
            else                                                        \
            {                                                           \
                printf( "[Score-P] %-48s%-15e\n",                       \
                        scorep_timing_function_names[ i ],              \
                        scorep_timing_sendbuf_durations[ i ] / ( double )SCOREP_GetClockResolution() ); \
            }                                                           \
        }                                                               \
    }

#else

#define SCOREP_TIME( function ) function()
#define SCOREP_TIME_WITH_ARGS( function, ... ) function( __VA_ARGS__ )
#define SCOREP_TIME_START_TIMING( function )
#define SCOREP_TIME_STOP_TIMING( function )
#define SCOREP_TIME_MEASUREMENT_START()
#define SCOREP_TIME_PRINT_TIMINGS()

#endif


#endif /* SCOREP_TIMING_INTERNAL_H */
