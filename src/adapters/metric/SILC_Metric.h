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

/** @file SILC_Metric.h

    This file contains the interface for the hardware counter access. This module can
    be build with different implementations supporting different hardware counter
    libraries. All of these libraries are accessed through the same interface.

    In the first step, only PAPI support is implemented. Later on, one could build this
    module also with NEC or Solaris support.
 */

#ifndef SILC_METRIC_PAPI_H
#define SILC_METRIC_PAPI_H

#include <stdint.h>

/* ************************************************************************
 * Counter properties
 * ***********************************************************************/

/** @def SILC_METRIC_TYPE_UINT64
    Specifies that the counter data type is an unsigned 64 bit integer.
 */
#define SILC_METRIC_TYPE_UINT64 0

/** @def SILC_METRIC_TYPE_INT64
   Specifies that the counter data type is an signed 64 bit integer.
 */
#define SILC_METRIC_TYPE_INT64  1

/** @def SILC_METRIC_TYPE_DOUBLE
    Specifies that the counter data type is a double.
 */
#define SILC_METRIC_TYPE_DOUBLE 2

/** @def SILC_METRIC_TYPE_DOUBLE
    Specifes that the value of the counter is the accumulated value from the
    start of measurement.
 */
#define SILC_METRIC_INTERVALL_START 0

/** @def SILC_METRIC_INTERVALL_LAST
    Specifies that the value of the counter is given as the difference from
    the last time it was obtained.
 */
#define SILC_METRIC_INTERVALL_LAST  4

/** @def SILC_METRIC_MODE_COUNTER
    Specifies that this metric is a monotonical increasing counter value.
 */
#define SILC_METRIC_MODE_COUNTER 0

/** @def SILC_METRIC_MODE_RATE
    Specifies that this metric is given as a rate (value per time).
 */
#define SILC_METRIC_MODE_RATE 8

/** @def SILC_METRIC_MODE_SAMPLE
    Specifies that this metric consists of independent samples.
 */
#define SILC_METRIC_MODE_SAMPLE 16

/* **************************************************************************
 * typedefs
 * *************************************************************************/

/** This type encodes the properties of a metric. Different properties are
    encoded in different bits of this type.
    @li The first two bytes encode the data type. Possible values are:
        SILC_METRIC_TYPE_UINT64, SILC_METRIC_TYPE_INT64, and
        SILC_METRIC_TYPE_DOUBLE.</li>
    @li The third byte encode the inteval type. Possible values are:
        SILC_METRIC_INTERVALL_START and SILC_METRIC_INTERVALL_LAST. </li>
    @li The forth and fifth byte encode the mode. Possible values are:
        SILC_METRIC_MODE_COUNTER, SILC_METRIC_MODE_RATE, and
        SILC_METRIC_MODE_SAMPLE.
 */
typedef uint32_t SILC_Metric_Properties;

/** Type for specifing the generic loading of the metric specification file.
    If the metric specification file is read in, different parts are of
    interest, dependent which tool is loading the specification. For every
    entry type a separate bit is used.
 */
typedef enum
{
    /** Unknown entry type */
    SILC_METRIC_MATCHMAP_UNKNOWN = 0x0,

    /** Entry which define alias names for a metric. */
    SILC_METRIC_MATCHMAP_MEASURE = 0x1,

    /** Entry which define a group of counters. */
    SILC_METRIC_MATCHMAP_AGGROUP = 0x2,

    /** Entry which defines a metric which is a calculated from other counters */
    SILC_METRIC_MATCHMAP_COMPOSE = 0x4,

    /** Indicates a computed metrics */
    SILC_METRIC_MATCHMAP_COMPUTE = 0x8,

    /** Used to define an invalid type or choice. */
    SILC_METRIC_MATCHMAP_INVALID = 0x10,
} SILC_Metric_MatchMapType;

/** This type defines an entry in the metric definitions. It builds a linked
    list which contains the loaded metric specifications
 */
typedef struct SILC_Metric_Map
{
    /** Type of this configurtion entry */
    SILC_Metric_MatchMapType type;

    /** Native name of the metric */
    char* eventName;

    /** Given or alias name */
    char* aliasName;

    /** Pointer to the next entry */
    struct SILC_Metric_Map* next;
} SILC_Metric_Map;

/** This struct contains the information about the set of counters, which
    are currently measured. The layout of this struct varies depending on the
    used hardware counter library. Thus, it is only declared as an incomplete
    type at this point. The full definition is then contained in the actual
    implementation file.
 */
struct SILC_Metric_EventSet;

/* **************************************************************************
 * Functions
 * *************************************************************************/

/** Reads the configuration from environment variables and configuration
    files and initializes the performance counter library. It must be called
    before other functions of the library are used by the measurement system.
    @return It returns the number of metrics in the event set.
 */
extern int32_t
SILC_Metric_Open
    ();

/** Finalizes the performance counter adapter. Frees memory allocated by
    SILC_Metric_Open.
 */
extern void
SILC_Metric_Close
    ();

/** Create per-thread counter sets
 */
extern struct SILC_Metric_EventSet*
SILC_Metric_Create
    ();

/** Free per-thread counter sets
    @param eventSet The event set that defines the measured counters
                    which should be freed
 */
extern void
SILC_Metric_Free
(
    struct SILC_Metric_EventSet* eventSet
);

/** Register thread
    @param getThreadIdFunc supply pthread_self() or omp_get_thread_num()
 */
extern void
SILC_Metric_ThreadInit
(
    int64_t ( * getThreadIdFunc )
    (
        void
    )
);

/** Unregister thread
 */
extern void
SILC_Metric_ThreadFini
    ();

/** Reads values of counters relative to the time of SILC_Metric_Open()
    @param eventSet An event set, that contains the definition of the counters
                    that should be measured.
    @param values   An array, to which the counter values are written.
 */
extern void
SILC_Metric_Read
(
    struct SILC_Metric_EventSet* eventSet,
    uint64_t                     values[]
);

/** Returns number of counters
 */
extern int32_t
SILC_Metric_Num
    ();

/** Returns name of counter i
    @param i Index of the counter.
 */
extern const char*
SILC_Metric_Name
(
    int32_t i
);

/** Returns description of counter i
    @param i Index of the counter.
 */
extern const char*
SILC_Metric_Descr
(
    int32_t i
);

/** Returns a string containing a representation of the unit of counter i
    @param i Index of the counter.
 */
extern const char*
SILC_Metric_Unit
(
    int32_t i
);

/** Returns the properties of counter i. The Property is a combination of e
    USER_METRIC_TYPE_*, USER_METRIC_INTERVALL, and USER_METRIC_MODE_*
    macro.
    @param i Index of the counter.
 */
extern SILC_Metric_Properties
SILC_Metric_Props
(
    int32_t i
);

/** Returns the clock rate.
 */
extern uint64_t
SILC_Metric_ClockRate
    ();

/** Reads the spec file
    @param match Defines the entries of the spec file which are read in.
                 Only entrys in the spec file for which the bit is set,
                 will be read in.
    @return Returns a pointer to the specification data structure.
 */
extern SILC_Metric_Map*
SILC_Metric_MapInit
(
    SILC_Metric_MatchMapType match
);

/** Releases memory allocated for a metric map
    @param metricMap The metric specification which should be freed.
 */
extern void
SILC_Metric_MapFree
(
    SILC_Metric_Map* metricMap
);


#endif // SILC_METRIC_PAPI_H
