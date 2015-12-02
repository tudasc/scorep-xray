/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012, 2015,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012, 2013
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012, 2014
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */



/**
 * @file
 * @ingroup    TAU
 *
 * @brief Implementation of the TAU adapter functions.
 */


#include <config.h>
#include <scorep/SCOREP_Tau.h>
#include <SCOREP_Types.h>
#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_InMeasurement.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Events.h>
#include <UTILS_Error.h>
#include <stdio.h>


/**
 * Converts TAU paradigm types to Score-P paradigm types.
 */
static SCOREP_ParadigmType
scorep_tau_convert_paradigm_type( SCOREP_Tau_ParadigmType paradigm )
{
    switch ( paradigm )
    {
        case SCOREP_TAU_PARADIGM_USER:
            return SCOREP_PARADIGM_USER;

        case SCOREP_TAU_PARADIGM_COMPILER:
            return SCOREP_PARADIGM_COMPILER;

        case SCOREP_TAU_PARADIGM_MPI:
            return SCOREP_PARADIGM_MPI;

        case SCOREP_TAU_PARADIGM_OPENMP:
            return SCOREP_PARADIGM_OPENMP;

        case SCOREP_TAU_PARADIGM_CUDA:
            return SCOREP_PARADIGM_CUDA;

        case SCOREP_TAU_PARADIGM_MEASUREMENT:
            return SCOREP_PARADIGM_MEASUREMENT;

        case SCOREP_TAU_PARADIGM_SHMEM:
            return SCOREP_PARADIGM_SHMEM;

        case SCOREP_TAU_PARADIGM_PTHREAD:
            return SCOREP_PARADIGM_PTHREAD;

        case SCOREP_TAU_PARADIGM_MPP:
        case SCOREP_TAU_PARADIGM_THREAD_FORK_JOIN:
        case SCOREP_TAU_PARADIGM_THREAD_CREATE_WAIT:
        case SCOREP_TAU_PARADIGM_ACCELERATOR:
            UTILS_BUG( "These paradigm classes are deprecated and should not be used as an paradigm: %u", paradigm );
            break;

        default:
            UTILS_BUG( "Failed to convert TAU paradigm to Score-P paradigm." );
            break;
    }
    return SCOREP_INVALID_PARADIGM_TYPE;
}


#define SCOREP_REGION_TYPE( NAME, name_str ) \
    case SCOREP_TAU_REGION_ ## NAME: \
        return SCOREP_REGION_ ## NAME;

/**
 * Converts TAU region types to Score-P region types.
 */
static SCOREP_RegionType
scorep_tau_convert_region_type( SCOREP_Tau_RegionType region_type )
{
    switch ( region_type )
    {
        case SCOREP_TAU_REGION_UNKNOWN:
            return SCOREP_REGION_UNKNOWN;
        case SCOREP_TAU_REGION_FUNCTION:
            return SCOREP_REGION_FUNCTION;
        case SCOREP_TAU_REGION_LOOP:
            return SCOREP_REGION_LOOP;
        case SCOREP_TAU_REGION_USER:
            return SCOREP_REGION_USER;
        case SCOREP_TAU_REGION_CODE:
            return SCOREP_REGION_CODE;

        case SCOREP_TAU_REGION_PHASE:
            return SCOREP_REGION_PHASE;
        case SCOREP_TAU_REGION_DYNAMIC:
            return SCOREP_REGION_DYNAMIC;
        case SCOREP_TAU_REGION_DYNAMIC_PHASE:
            return SCOREP_REGION_DYNAMIC_PHASE;
        case SCOREP_TAU_REGION_DYNAMIC_LOOP:
            return SCOREP_REGION_DYNAMIC_LOOP;
        case SCOREP_TAU_REGION_DYNAMIC_FUNCTION:
            return SCOREP_REGION_DYNAMIC_FUNCTION;
        case SCOREP_TAU_REGION_DYNAMIC_LOOP_PHASE:
            return SCOREP_REGION_DYNAMIC_LOOP_PHASE;

            SCOREP_REGION_TYPES

        default:
            UTILS_BUG( "Failed to convert TAU region type to Score-P region type." );
    }
    return SCOREP_INVALID_REGION_TYPE;
}
#undef SCOREP_REGION_TYPE

/** @ingroup TAU
    @{
 */
/* **************************************************************************************
 *                                                                   TAU event functions
 ***************************************************************************************/

/**
 * Initialize the measurement system from the adapter layer. This function
 * needs to be called at least once by an (arbitrary) adapter before any other
 * measurement API function is called. Calling other API functions before is
 * seen as undefined behaviour. The first call to this function triggers the
 * initialization of all adapters in use.
 *
 * For performance reasons the adapter should keep track of it's
 * initialization status and call this function only once.
 *
 * Calling this function several times does no harm to the measurement system.
 *
 * Each arising error leads to a fatal abortion of the program.
 *
 * @note The MPI adapter needs special treatment, see
 * SCOREP_InitMppMeasurement().
 *
 * @see SCOREP_FinalizeMeasurement()
 */

void
SCOREP_Tau_InitMeasurement( void )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    SCOREP_InitMeasurement();
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

/**
 * Register a function that can close the callstack. This is invoked by
 * the SCOREP routine that is called by atexit. Before flushing the data to
 * disk, all the open timers are closed by invoking the function callback.
 *
 * @param: callback, a function pointer. It points to the routine
 * Tau_profile_exit_all_threads.
 */

void
SCOREP_Tau_RegisterExitCallback( SCOREP_Tau_ExitCallback callback )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    SCOREP_RegisterExitCallback( callback );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}


/**
 * Associate a code region with a process unique file handle.
 *
 * @param regionName A meaningful name for the region, e.g. a function
 * name. The string will be copied.
 *
 * @param fileHandle A previously defined SCOREP_SourceFileHandle or
 * SCOREP_INVALID_SOURCE_FILE.
 *
 * @param beginLine The file line number where the region starts or
 * SCOREP_INVALID_LINE_NO.
 *
 * @param endLine The file line number where the region ends or
 * SCOREP_INVALID_LINE_NO.
 *
 * @param paradigm The paradigm of the adapter that is calling.
 *
 * @param regionType The type of the region. Until now, the @a regionType was
 * not used during the measurement but during analysis. This @e may change in
 * future with e.g. dynamic regions or parameter based profiling. In the first
 * run, we can implement at least dynamic regions in the adapter.
 *
 * @note The name of e.g. Java classes, previously provided as a string to the
 * region description, should now be encoded in the region name. The region
 * description field is replaced by the adapter type as that was it's primary
 * use.
 *
 * @note During unification, we compare @a regionName, @a fileHandle, @a
 * beginLine, @a endLine and @a adapter of regions from different
 * processes. If all values are equal, we consider the regions to be equal. We
 * don't check for uniqueness of this tuple in this function, i.e. during
 * measurement, this is the adapters responsibility, but we require that every
 * call defines a unique and distinguishable region.
 *
 * @return A process unique region handle to be used in calls to
 * SCOREP_EnterRegion() and SCOREP_ExitRegion().
 *
 */

SCOREP_Tau_RegionHandle
SCOREP_Tau_DefineRegion( const char*                 regionName,
                         SCOREP_Tau_SourceFileHandle fileHandle,
                         SCOREP_Tau_LineNo           beginLine,
                         SCOREP_Tau_LineNo           endLine,
                         SCOREP_Tau_ParadigmType     tauParadigm,
                         SCOREP_Tau_RegionType       tauRegionType )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    SCOREP_ParadigmType paradigm    = scorep_tau_convert_paradigm_type( tauParadigm );
    SCOREP_RegionType   region_type = scorep_tau_convert_region_type( tauRegionType );

    SCOREP_Tau_RegionHandle handle =
        SCOREP_Definitions_NewRegion( regionName, NULL, fileHandle,
                                      beginLine, endLine, paradigm, region_type );

    SCOREP_IN_MEASUREMENT_DECREMENT();

    return handle;
}


/**
 * Generate a region enter event in the measurement system.
 *
 * @param regionHandle The corresponding region for the enter event.
 */
void
SCOREP_Tau_EnterRegion( SCOREP_Tau_RegionHandle regionHandle )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    SCOREP_EnterRegion( ( SCOREP_RegionHandle )regionHandle );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

/**
 * Generate a region exit event in the measurement system.
 *
 * @param regionHandle The corresponding region for the enter event.
 */
void
SCOREP_Tau_ExitRegion( SCOREP_Tau_RegionHandle regionHandle )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    SCOREP_ExitRegion( ( SCOREP_RegionHandle )regionHandle );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}


void
SCOREP_Tau_Metric( SCOREP_Tau_MetricHandle* metricHandle )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    *metricHandle = SCOREP_INVALID_SAMPLING_SET;
    SCOREP_IN_MEASUREMENT_DECREMENT();
}




void
SCOREP_Tau_InitMetric( SCOREP_Tau_MetricHandle* metricHandle,
                       const char*              name,
                       const char*              unit )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    SCOREP_User_InitMetric( ( SCOREP_SamplingSetHandle* )metricHandle, name, unit,
                            SCOREP_USER_METRIC_TYPE_DOUBLE,  SCOREP_USER_METRIC_CONTEXT_GLOBAL );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
SCOREP_Tau_TriggerMetricDouble( SCOREP_Tau_MetricHandle metricHandle,
                                double                  value )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    SCOREP_User_TriggerMetricDouble( ( SCOREP_SamplingSetHandle )metricHandle, value );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
SCOREP_Tau_Parameter_INT64( SCOREP_Tau_ParamHandle* paramHandle,
                            const char*             name,
                            int64_t                 value )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    SCOREP_User_ParameterInt64( ( SCOREP_User_ParameterHandle* )paramHandle, name, value );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
SCOREP_Tau_AddLocationProperty( const char* name, const char* value )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    SCOREP_AddLocationProperty( name, value );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

/* *INDENT-OFF* */
/* *INDENT-ON*  */
/** @} */
