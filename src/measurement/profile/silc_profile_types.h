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

#ifndef SILC_PROFILE_TYPES_H
#define SILC_PROFILE_TYPES_H

/**
 * @file        silc_profile_types.h
 * @maintainer  Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @brief Types needed for the profile call tree
 *
 */

#include "SILC_Types.h"
#include <stdint.h>
typedef void* SILC_CallpathHandle;


/** Contains the data for one dens metric */
typedef struct
{
    uint64_t sum;
    uint64_t min;
    uint64_t max;
    uint64_t squares;
    uint64_t start_value;
} silc_profile_dense_metric;

/** Contains the data for a sparse metric of integer values */
typedef struct silc_profile_sparse_metric_int_struct
{
    SILC_CounterHandle                            metric;
    uint64_t                                      count;
    uint64_t                                      sum;
    uint64_t                                      min;
    uint64_t                                      max;
    uint64_t                                      squares;
    struct silc_profile_sparse_metric_int_struct* next_metric;
} silc_profile_sparse_metric_int;

/** Contains the data for a sparse metric of double values */
typedef struct silc_profile_sparse_metric_double_struct
{
    SILC_CounterHandle                               metric;
    uint64_t                                         count;
    uint64_t                                         sum;
    uint64_t                                         min;
    uint64_t                                         max;
    uint64_t                                         squares;
    struct silc_profile_sparse_metric_double_struct* next_metric;
} silc_profile_sparse_metric_double;

/** List of profile node types */
typedef enum
{
    silc_profile_node_regular_region,
    silc_profile_node_parameter_string,
    silc_profile_node_parameter_integer,
    silc_profile_node_thread_create,
    silc_profile_node_thread_start
} silc_profile_node_type;

/** Contains all data for one profile node */
typedef struct silc_profile_node_struct
{
    SILC_CallpathHandle                callpath_handle;
    struct silc_profile_node_struct*   parent;
    struct silc_profile_node_struct*   first_child;
    struct silc_profile_node_struct*   next_sibling;
    silc_profile_dense_metric*         dense_metrics;
    silc_profile_sparse_metric_double* first_double_sparse;
    silc_profile_sparse_metric_int*    first_int_sparse;
    silc_profile_dense_metric          implicit_time;
    uint64_t                           count;              // For dense metrics
    uint64_t                           first_enter_time;   // Required by Scalasca
    uint64_t                           last_exit_time;     // Required by Scalasca
    silc_profile_node_type             node_type;
    uint64_t                           type_specific_data;
} silc_profile_node;

/** Thread local data for the profiling system */
typedef struct
{
    silc_profile_node* current_node;
    silc_profile_node* creation_node;
} SILC_Profile_ThreadData;

/** Global profile definition data */
typedef struct
{
    /* Points to the first root node. Further root nodes are added as siblings
       to a root node. */
    silc_profile_node* first_root_node;

    /* Number of metrics in dense representation. All enter/exit events expect this
       number of metrics. */
    uint32_t num_of_dense_metrics;

    /* Array containing the Metric defintion handle for the metrics in dense
       representation. All enter/exit events expect the metrics in this order. */
    SILC_CounterHandle* dense_metrics;
} silc_profile_definition;

#endif // SILC_PROFILE_TYPES_H
