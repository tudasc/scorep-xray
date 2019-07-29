/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2015,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * Copyright (c) 2015,
 * Technische Universitaet Darmstadt, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */



/**
 * @file
 *
 *
 *
 */

#include <config.h>
#include <scorep_profile_cluster.h>
#include <scorep_profile_definition.h>
#include <scorep_profile_node.h>
#include <scorep_profile_location.h>
#include <scorep_profile_cube4_writer.h>
#include <scorep_profile_mpi_events.h>
#include <SCOREP_Memory.h>
#include <SCOREP_Metric_Management.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Mutex.h>
#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME PROFILE
#include <UTILS_Debug.h>
#include <scorep_environment.h>
#include <scorep_ipc.h>
#include <SCOREP_Thread_Mgmt.h>
#include <math.h>                      /* sqrt() and log10() */
#include <float.h>                     /* DBL_MAX */
#include <string.h>                    /* strlen() */
#include <stdlib.h>                    /* malloc() */
#include <stdio.h>
#include <inttypes.h>

#include <SCOREP_Timer_Utils.h>

enum
{
    MET_EXECUTION_TIME,     /* inclusive time */
    MET_VISITS,             /* visits */

    MET_MPI_TIME,           /* mpi time */
    //met_mpi_sync_time,      /* mpi synchronization time */
    MET_MPI_COLL_SYNC_TIME, /* mpi collective synchronization time */
    MET_MPI_COMM_TIME,      /* mpi communication time */
    MET_MPI_P2P_COMM_TIME,  /* mpi point-to-point communication time */
    MET_MPI_COLL_COMM_TIME, /* mpi collective communication time */

    MET_COLL_BSENT,         /* collective bytes sent */
    MET_COLL_BRCVD,         /* collective bytes received */
    MET_COLL_SYNCS,         /* collective sync accumulator */
    //met_coll_c_dst,         /* collective comm as destination */
    //met_coll_c_src,         /* collective comm as source */
    //met_coll_c_xch,         /* collective comm as src & dest */
    MET_SEND_BYTES,         /* sent bytes accumulator */
    MET_SEND_COMMS,         /* message send accumulator */
    //met_send_syncs,         /* zero-sized sends */
    MET_RECV_BYTES,         /* received bytes accumulator */
    MET_RECV_COMMS,         /* message recv accumulator */
    //met_recv_syncs,         /* zero-sized recvs */

    MET_P2P_BYTES,          /* p2p bytes transferred */
    MET_COLL_BYTES,         /* collective bytes transferred */
    MET_BYTES,              /* bytes transferred */
    MET_P2P_COMM,           /* p2p communication count */
    MET_COLL_COMM,          /* collective communication count */
    MET_COMM,               /* communication count */
    //met_p2p_syncs,          /* p2p synchronization count */
    //met_syncs,              /* synchronization count */

    MET_DENSE_START         /* Number of predefined metrics */
};

static uint64_t cluster_metric_number = MET_DENSE_START;

/* Type declarations */

/**
   This represents the inclusive metric values of a cluster
   stored for distance calculation for a given cluster.
 */
typedef struct scorep_metric_data_t
{
    uint64_t* dense_metrics;
} scorep_metric_data_t;

/**
   The clusterers metric reference values, used for normalization during
   distance calculations. Exists once per clusterer.
 */
typedef struct scorep_reference_t
{
    double*  dense_metrics;
    uint64_t count;
} scorep_reference_t;

struct scorep_cluster_t;

/**
   This represents an clustered iteration. It is used to keep track
   which iterations belong to a cluster. Iterations of a cluster
   form a so called disjoint set which is a tree where the call-tree
   of the root iteration represents the cluster. Other iterations
   point to their root itertaions.
 */
typedef struct scorep_clustered_iteration_t
{
    struct scorep_cluster_t*             cluster;         /* Pointer to the cluster (if root) */
    uint32_t                             iteration_id;    /* The id of the iteration */
    uint32_t                             rank;            /* Rank in disjoint sets tree. */
    struct scorep_clustered_iteration_t* next;            /* Next element of the linked list */
    struct scorep_clustered_iteration_t* parent;          /* Parent in the disjoint sets
                                                             tree. This is a root if NULL */
} scorep_clustered_iteration_t;

/**
   Elements of this type form a linked list which stores the cluster distances. */
typedef struct scorep_cluster_distance_t
{
    double                            value; /* Distance value */
    struct scorep_cluster_distance_t* next;  /* Pointer to the next element of
                                                the list */
} scorep_cluster_distance_t;

/**
   Objects of this type define a cluster.
 */
typedef struct scorep_cluster_t
{
    scorep_profile_node*       root;               /* Root call path of the cluster */
    uint32_t                   cluster_id;         /* Cluster id. 0 if not defined yet */
    uint32_t                   hash;               /* The hash of the associated tree */
    scorep_cluster_distance_t* d;                  /* List of distances from all
                                                      the equivalent groups that
                                                      appear later in the list */
    double                        min_val;         /* Lowest distance value */
    scorep_cluster_distance_t*    min_pos;         /* Points to where min_val was found */
    uint32_t                      iteration_count; /* Number of iterations in cluster */
    scorep_metric_data_t          mets_sum;        /* Sum of inclusive metric
                                                      values for distance computation */
    scorep_metric_data_t          mets_avg;        /* Average inclusive metric
                                                      values for distance computation */
    scorep_clustered_iteration_t* disjoint_set;    /* Corresponding disjoint set */
    struct scorep_cluster_t*      next;            /* Next cluster in the list of
                                                      equivalent clusters */
} scorep_cluster_t;

/**
   This is a list of clusters that have equivalent call trees. Only clusters of
   wquivalent call trees are merged.
 */
typedef struct scorep_equiv_cluster_list_t
{
    scorep_cluster_t*                   head;    /* Head of the list */
    double                              min_val; /* Lowest distance value here */
    scorep_cluster_t*                   min_pos; /* Position of the lowest distance */
    struct scorep_equiv_cluster_list_t* next;    /* Next element in the linked list */
} scorep_equiv_cluster_list_t;

/**
   This stores the call-tree equivalence classes.
 */
typedef struct scorep_equiv_cluster_list_set_t
{
    scorep_equiv_cluster_list_t* head;    /* This is just a linked list to be able
                                             to iterate through the lists */
    double                       min_val; /* Lowest distance value */
    scorep_equiv_cluster_list_t* min_pos; /* Position of the lowest distance */
} scorep_equiv_cluster_list_set_t;

/**
   An element of the cluster queue represented by @a scorep_cluster_queue_t.
 */
typedef struct scorep_cluster_queue_elem_t
{
    scorep_profile_node*                path; /* Root call path of the iteration */
    struct scorep_cluster_queue_elem_t* next; /* Next element in the queue */
} scorep_cluster_queue_elem_t;

/**
   The queue which stores iterations to be clustered. Whenever a iteration is complete,
   it is enquened for clustering. This allows to delay the actual clustering.
 */
typedef struct scorep_cluster_queue_t
{
    scorep_cluster_queue_elem_t*  begin;
    scorep_cluster_queue_elem_t** end;
} scorep_cluster_queue_t;

/**
   The data that is required for clustering a dynamic region.
 */
typedef struct scorep_clusterer_t
{
    uint32_t clustering_buffer;                                /* How many iterations can
                                                                  be buffered before
                                                                  processing queue */
    uint32_t                         max_cl_count;             /* Maximum number of clusters */
    uint32_t                         cl_count;                 /* Current number of clusters */
    uint32_t                         cl_it_count;              /* Number of clustered_iterations */
    scorep_clustered_iteration_t*    cl_it_head;               /* Head of the linked list of
                                                                  clustered_iterations */
    scorep_clustered_iteration_t**   cl_it_tail;               /* Tail of the linked list of
                                                                  clustered iterations */
    scorep_equiv_cluster_list_set_t* eq_cl_lt_set;             /* Data structure storing
                                                                  the actual clusters */
    scorep_reference_t*              ref_vals;                 /* Reference data for
                                                                  normalization */
    scorep_cluster_queue_t*          queue;                    /* Queue of iterations to be
                                                                  clustered */
    scorep_cluster_distance_t*       free_cluster_distances;   /* free objects list */
    scorep_cluster_t*                free_clusters;            /* free objects list */
    scorep_cluster_queue_elem_t*     free_cluster_queue_elems; /* free objects list */
} scorep_clusterer_t;

/* Variable and constant declarations */

/* Whether or not to measure time spent in clustering */
#define MEASURE_CLUSTERING_TIME

/**
   The one global clusterer object.
 */
scorep_clusterer_t* scorep_clusterer;

/* Mutex */
static SCOREP_Mutex cluster_distance_lock;
static SCOREP_Mutex cluster_lock;
static SCOREP_Mutex cluster_queue_elem_lock;

/* Flag whether clustering is initialized and enabled */
static bool clustering_enabled = false;

/* Stores a pointer to the root node of the clustered dynamic region.
   Used for fast identification whether a node should be clustered. */
static scorep_profile_node* cluster_parent = NULL;


/* Some prototypes */
static uint32_t
calculate_hash( scorep_profile_node* path );

static void
calculate_derived_metrics( scorep_cluster_t*    cluster,
                           scorep_profile_node* path );

static void
post_process_derived_metrics( scorep_cluster_t* cluster );


static int
compare_call_trees( scorep_profile_node* a,
                    scorep_profile_node* b,
                    uint32_t             itCntA,
                    uint32_t             itCntB );

static void
add_iter_to_clustering( SCOREP_Profile_LocationData* location,
                        scorep_profile_node*         path,
                        scorep_clusterer_t*          clusterer );

/* **************************************************************************
   Local helper functions
****************************************************************************/

/**
   Find root of disjoint set from an iteration and speed up future searches.
   @param itertaion Pointer to an itertaion which belongs to the disjoint set.
   @returns the root iteration of the disjoint set.
 */
static scorep_clustered_iteration_t*
disjoint_set_get_root( scorep_clustered_iteration_t* iteration )
{
    if ( NULL == iteration->parent )
    {
        return iteration;
    }
    return iteration->parent = disjoint_set_get_root( iteration->parent );
}

/**
   This function merges two sets in the disjoint sets datastructure, which is
   implemented here in the scorep_clustered_iteration_t objects.  a and b are
   the two distjoint sets to be merged, while cluster is the cluster that they
   will be associated with in the future.
   @param a       An iteration of the disjoint set into which we merge b.
   @param b       An iteration of the disjoint set which is merged into a.
   @param cluster The cluster with which a and b will be associated with.
 */
static void
disjoint_set_merge( scorep_clustered_iteration_t* a,
                    scorep_clustered_iteration_t* b,
                    scorep_cluster_t*             cluster )
{
    a = disjoint_set_get_root( a );
    b = disjoint_set_get_root( b );
    if ( a->rank < b->rank )
    {
        scorep_clustered_iteration_t* temp;
        temp = a;
        a    = b;
        b    = temp;
    }
    else if ( a->rank == b->rank )
    {
        a->rank++;
    }
    b->parent                = a;
    a->cluster               = cluster;
    a->cluster->disjoint_set = a;
}

/**
   Make sure the reference is updated before this function is called,
   as there is only an implicit check for zero reference values (I
   assume that if the diff here is non-zero, then the reference cannot
   be zero.)
   @param a         The first cluster for the distance calculation.
   @param b         The second cluste for the distance calculation.
   @param reference The metric reference set of the clusterer, required
                    for normalization.
   @returns a the distance of a and b.
 */
static double
calculate_cluster_distance( const scorep_cluster_t* a,
                            const scorep_cluster_t* b,
                            scorep_reference_t*     reference )
{
    /*  scorep_warning("DISTANCE BEGIN");*/
    /* Calculate basic distance value */
    double distance = 0.0;
    int    mr;
    for ( mr = 0; mr < cluster_metric_number; mr++ )
    {
        double diff = a->mets_avg.dense_metrics[ mr ] - b->mets_avg.dense_metrics[ mr ];
        if ( 0.0 == diff )
        {
            continue;
        }
        if ( 0.0 < diff )
        {
            distance += diff / reference->dense_metrics[ mr ];
        }
        else
        {
            distance -= diff / reference->dense_metrics[ mr ];
        }
        /*    scorep_warning("DISTANCE: Time. mr: %d, diff: %lf, ref: %lf, div: %lf, "
                    "dist: %lf", mr, diff, reference->timeb[mr],
                    diff/reference->timeb[mr], distance);*/
    }

    /* Apply multiplier */
    double multiplier = 0.4 + 0.05 * ( a->iteration_count + b->iteration_count );
    /*  scorep_warning("DISTANCE END: distance: %lf, multiplier: %lf, ret_val: %lf",
                distance, (1.0>=multiplier?multiplier:sqrt(multiplier)),
                distance * (1.0>=multiplier?multiplier:sqrt(multiplier)));*/
    if ( 1.0 >= multiplier )
    {
        distance *= multiplier;
    }
    else
    {
        distance *= sqrt( multiplier );
    }

    return distance;
}

/**
   Returns a new (or previously used) scorep_cluster_distance_t object.
   @param location  The Score-P location object.
   @param clusterer The clusterer object.
   @returns the new object. It will automatically deleted when the
            profile pages are freed.
 */
static scorep_cluster_distance_t*
new_cluster_distance( SCOREP_Location*    location,
                      scorep_clusterer_t* clusterer )
{
    scorep_cluster_distance_t* cluster_distance;
    SCOREP_MutexLock( cluster_distance_lock );

    if ( NULL == clusterer->free_cluster_distances )
    {
        clusterer->free_cluster_distances =
            SCOREP_Location_AllocForProfile( location,
                                             sizeof( scorep_cluster_distance_t ) );
        /* No next element in the list */
        clusterer->free_cluster_distances->next = NULL;
    }
    cluster_distance                  = clusterer->free_cluster_distances;
    clusterer->free_cluster_distances = cluster_distance->next;
    cluster_distance->next            = NULL;

    SCOREP_MutexUnlock( cluster_distance_lock );
    return cluster_distance;
}

/**
   Adds a scorep_cluster_distance_t object to the list of unused.
   @param distanceObj  The distance object that will be recycled.
   @param clusterer    The associated clusterer.
   @returns the next object in the list (which might be useful when
            deleting a list of objects).
 */
static scorep_cluster_distance_t*
delete_cluster_distance( scorep_cluster_distance_t* distanceObj,
                         scorep_clusterer_t*        clusterer )
{
    scorep_cluster_distance_t* next;
    SCOREP_MutexLock( cluster_distance_lock );

    next                              = distanceObj->next;
    distanceObj->next                 = clusterer->free_cluster_distances;
    clusterer->free_cluster_distances = distanceObj;

    SCOREP_MutexUnlock( cluster_distance_lock );
    return next;
}

/**
   Returns a new (or previously used) scorep_cluster_t object.
   @param location            The Score-P location object of the executing
                              location.
   @param clusterer           Pointer to the clusterer object
   @param clustered iteration Pointer to the initial (first)
                              iteration of the new cluster
   @param root                Pointer to the root node in the
                              call tree which represents the
                              clustered iterations.
   @returns the new object.
 */
static scorep_cluster_t*
new_cluster( SCOREP_Location*              location,
             scorep_clusterer_t*           clusterer,
             scorep_clustered_iteration_t* clustered_iteration,
             scorep_profile_node*          root )
{
    scorep_cluster_t* cluster;
    SCOREP_MutexLock( cluster_lock );

    /* Create structural parts */
    if ( NULL == clusterer->free_clusters )
    {
        clusterer->free_clusters =
            SCOREP_Location_AllocForProfile( location, sizeof( scorep_cluster_t ) );
        /* No next element in the list */
        clusterer->free_clusters->next = NULL;
    }
    cluster                  = clusterer->free_clusters;
    clusterer->free_clusters = clusterer->free_clusters->next;

    /* Initialize */
    cluster->root            = root;
    cluster->cluster_id      = clusterer->cl_it_count + 1;
    cluster->hash            = 0;
    cluster->d               = NULL;
    cluster->min_val         = DBL_MAX;
    cluster->min_pos         = NULL;
    cluster->iteration_count = 1;

    /* Create metric part */
    size_t metric_array_size = sizeof( uint64_t ) * cluster_metric_number;
    cluster->mets_sum.dense_metrics = SCOREP_Location_AllocForProfile( location, metric_array_size );
    cluster->mets_avg.dense_metrics = SCOREP_Location_AllocForProfile( location, metric_array_size );
    for ( int mr = 0; mr < cluster_metric_number; mr++ )
    {
        cluster->mets_sum.dense_metrics[ mr ] = 0.0;
        cluster->mets_avg.dense_metrics[ mr ] = 0.0;
    }

    cluster->disjoint_set               = clustered_iteration;
    cluster->disjoint_set->cluster      = cluster;
    cluster->disjoint_set->iteration_id = clusterer->cl_it_count + 1;
    cluster->disjoint_set->rank         = 0;
    cluster->disjoint_set->next         = NULL;
    cluster->disjoint_set->parent       = NULL;
    cluster->next                       = NULL;

    SCOREP_MutexUnlock( cluster_lock );
    return cluster;
}

/**
   Recycles a scorep_cluster_t object by adding it to the list
   of unused objects.
   @param cluster   Cluster obeject that will be recycled.
   @param clusterer The clusterer object
   @returns the next cluster after the deleted one, in the
            cluster's equvalence list.
 */
static scorep_cluster_t*
delete_cluster( scorep_cluster_t*   cluster,
                scorep_clusterer_t* clusterer )
{
    scorep_cluster_t* next;
    SCOREP_MutexLock( cluster_lock );

    next = cluster->next;
    while ( NULL != cluster->d )
    {
        cluster->d = delete_cluster_distance( cluster->d, clusterer );
    }
    cluster->next            = clusterer->free_clusters;
    clusterer->free_clusters = cluster;

    SCOREP_MutexUnlock( cluster_lock );
    return next;
}

/**
   Create a new scorep_clustered_iteration_t object.
   @param location  The Score-P location object of the executing location.
   @retuns the new object.
 */
static scorep_clustered_iteration_t*
new_clustered_iteration( SCOREP_Location* location )
{
    scorep_clustered_iteration_t* clustered_iteration;
    clustered_iteration =
        SCOREP_Location_AllocForProfile( location, sizeof( scorep_clustered_iteration_t ) );
    /* Initialize */
    memset( clustered_iteration, 0, sizeof( scorep_clustered_iteration_t ) );

    return clustered_iteration;
}

/**
   Create a new scorep_equiv_cluster_list_t object
   @param location  The Score-P location object of the executing location.
   @retuns the new object.
 */
static scorep_equiv_cluster_list_t*
new_equiv_cluster_list( SCOREP_Location* location )
{
    scorep_equiv_cluster_list_t* list;
    list          = SCOREP_Location_AllocForProfile( location, sizeof( scorep_equiv_cluster_list_t ) );
    list->min_val = DBL_MAX;
    list->min_pos = NULL;
    list->head    = NULL;
    list->next    = NULL;

    return list;
}

/**
   Create a new scorep_equiv_cluster_list_set_t object
   @param location  The Score-P location object of the executing location.
   @retuns the new object.
 */
static scorep_equiv_cluster_list_set_t*
new_equiv_cluster_list_set( SCOREP_Location* location )
{
    scorep_equiv_cluster_list_set_t* list_set;
    list_set = SCOREP_Location_AllocForProfile( location, sizeof( scorep_equiv_cluster_list_set_t ) );
    /* Initialize */
    list_set->min_val = DBL_MAX;
    list_set->min_pos = NULL;
    list_set->head    = NULL;

    return list_set;
}

/**
   Create a new scorep_reference_t object
   @param location  The Score-P location object of the executing location.
   @retuns the new object.
 */
static scorep_reference_t*
new_reference( SCOREP_Location* location )
{
    uint32_t            mr;
    scorep_reference_t* reference;
    reference = SCOREP_Location_AllocForProfile( location, sizeof( scorep_reference_t ) );
    /* Initialize */
    reference->count         = 0;
    reference->dense_metrics = SCOREP_Location_AllocForProfile( location, sizeof( uint64_t ) * cluster_metric_number );
    for ( mr = 0; mr < cluster_metric_number; mr++ )
    {
        reference->dense_metrics[ mr ] = 0.0;
    }

    return reference;
}

/**
   Create a new scorep_cluster_queue_elem_t object
   @param location  The Score-P location object of the executing location.
   @retuns the new object.
 */
static scorep_cluster_queue_elem_t*
new_cluster_queue_elem( SCOREP_Location*     location,
                        scorep_clusterer_t*  clusterer,
                        scorep_profile_node* path )
{
    scorep_cluster_queue_elem_t* elem;
    SCOREP_MutexLock( cluster_queue_elem_lock );

    if ( NULL == clusterer->free_cluster_queue_elems )
    {
        clusterer->free_cluster_queue_elems =
            SCOREP_Location_AllocForProfile( location,
                                             sizeof( scorep_cluster_queue_elem_t ) );
        clusterer->free_cluster_queue_elems->next = NULL;
    }
    elem                                = clusterer->free_cluster_queue_elems;
    clusterer->free_cluster_queue_elems = elem->next;
    SCOREP_MutexUnlock( cluster_queue_elem_lock );

    /* Initialize */
    elem->path = path;
    elem->next = NULL;

    return elem;
}

/**
   Delete an scorep_cluster_queue_elem_t object
   @param elem      The scorep_cluster_queue_elem_t object that will be recycled.
   @param clusterer The associated clusterer.
 */
static void
delete_cluster_queue_elem( scorep_cluster_queue_elem_t* elem,
                           scorep_clusterer_t*          clusterer )
{
    SCOREP_MutexLock( cluster_queue_elem_lock );

    elem->next                          = clusterer->free_cluster_queue_elems;
    clusterer->free_cluster_queue_elems = elem;

    SCOREP_MutexUnlock( cluster_queue_elem_lock );
}

/**
   Create a new scorep_cluster_queue_t object
   @param location  The Score-P location object of the executing location.
   @retuns the new object.
 */
static scorep_cluster_queue_t*
new_cluster_queue( SCOREP_Location* location )
{
    scorep_cluster_queue_t* queue;
    queue = SCOREP_Location_AllocForProfile( location, sizeof( scorep_cluster_queue_t ) );
    /* Initialize */
    queue->begin = NULL;
    queue->end   = &( queue->begin );

    return queue;
}

/**
   Create a new scorep_clusterer_t object
   @param location        The Score-P location object of the executing location.
   @param maxClusterCount The maximum number of clusters.
   @retuns the new object.
 */
static scorep_clusterer_t*
new_clusterer( SCOREP_Location* location,
               uint32_t         maxClusterCount )
{
    scorep_clusterer_t* clusterer;
    clusterer = SCOREP_Location_AllocForProfile( location, sizeof( scorep_clusterer_t ) );

    /* Initialize */
    clusterer->max_cl_count             = maxClusterCount;
    clusterer->clustering_buffer        = scorep_profile_get_cluster_count();
    clusterer->cl_count                 = 0;
    clusterer->cl_it_count              = 0;
    clusterer->cl_it_head               = NULL;
    clusterer->cl_it_tail               = &( clusterer->cl_it_head );
    clusterer->eq_cl_lt_set             = new_equiv_cluster_list_set( location );
    clusterer->ref_vals                 = new_reference( location );
    clusterer->queue                    = new_cluster_queue( location );
    clusterer->free_cluster_distances   = NULL;
    clusterer->free_clusters            = NULL;
    clusterer->free_cluster_queue_elems = NULL;

    return clusterer;
}

/**
   Update reference values with the data from the given iteration.
   @param reference The metric reference object of the associated
                    clusterer
   @param iteration The iteration object.
 */
static void
update_reference_values( scorep_reference_t*           reference,
                         scorep_clustered_iteration_t* iteration )
{
    scorep_cluster_t* cluster = iteration->cluster;
    double            count   = reference->count;

    for ( int mr = 0; mr < cluster_metric_number; mr++ )
    {
        reference->dense_metrics[ mr ] *= count / ( count + 1.0 );
        reference->dense_metrics[ mr ] += cluster->mets_avg.dense_metrics[ mr ] / ( count + 1.0 );
    }

    /* Increment the number of samples used for the reference */
    reference->count++;
}

/**
   This function assumes that this cluster has no distance data at all and
   needs to calculate all distances to clusters after itself in the list
   @param location  The Score-P location object of the executing location.
   @param cluster   The cluster object for which the distances are calculated
   @param list      Pointer to the equivalenz cluster list.
   @param clusterer The clusterer object.
 */
static void
calculate_distances_for_new_cluster( SCOREP_Location*             location,
                                     scorep_cluster_t*            cluster,
                                     scorep_equiv_cluster_list_t* list,
                                     scorep_clusterer_t*          clusterer )
{
    scorep_cluster_distance_t** tail = &( cluster->d );
    /* Set up a shortcut to the list set */
    scorep_equiv_cluster_list_set_t* list_set = clusterer->eq_cl_lt_set;
    /* Fill distance data of new cluster */
    scorep_cluster_t* cl;
    for ( cl = cluster->next; cl != NULL; cl = cl->next )
    {
        /* Calculate distance */
        scorep_cluster_distance_t* distance =
            new_cluster_distance( location, clusterer );
        distance->value = calculate_cluster_distance( cluster, cl,
                                                      clusterer->ref_vals );
        /* Insert distance value */
        *tail = distance;
        tail  = &( distance->next );

        /* Update minima */
        if ( cluster->min_val > distance->value )
        {
            cluster->min_val = distance->value;
            cluster->min_pos = distance;
        }
    }

    /* Update minima in the higher parts of the data structure */
    if ( list->min_val > cluster->min_val )
    {
        list->min_val = cluster->min_val;
        list->min_pos = cluster;

        if ( list_set->min_val > list->min_val )
        {
            list_set->min_val = list->min_val;
            list_set->min_pos = list;
        }
    }
}

/**
   A merge sort algorithm on linked lists of call paths. I'll assume that len
   means the number of elements in the list, and it's right. No checks.
   head is an in/out, tail is an out only parameter. len is in only.
   @param head  Pointer to memory location which contains the pointer
                to the first element of nodes that will be sorted. The
                nodes must be linked to a list via the @a next_sibling
                struct member. After the function returnes, the memory
                location to which head points will contain the first
                node of the sorted list.
   @param tail  Pointer to a memory location into which a pointer to
                the last element of the sorted list is written.
   @param len   The number of elements that are sorted.
 */
static void
merge_sort_call_paths( scorep_profile_node** head,
                       scorep_profile_node** tail,
                       int                   len )
{
    /* If it's a single element list, return that list */
    if ( 1 >= len )
    {
        *tail = *head;
        return;
    }
    int                   pos, pos_end = len / 2;
    scorep_profile_node*  head_tail;
    scorep_profile_node*  mid;
    scorep_profile_node** last;
    /* Find the middle of the list */
    for ( pos = 0, mid = *head; pos < pos_end; pos++ )
    {
        last = &( mid->next_sibling );
        mid  = *last;
    }

    /* Cut the list in the middle */
    *last = NULL;
    /* Sort the two sub-lists */
    merge_sort_call_paths( head, &head_tail, pos_end );
    merge_sort_call_paths( &mid, tail, len - pos_end );
    /* See if it's a merge or a concatenation */
    if ( scorep_profile_node_less_than( head_tail, mid ) )
    {
        /* Concatenate the two lists */
        head_tail->next_sibling = mid;
    }
    else
    {
        /* Merge the two lists */
        last      = head;
        head_tail = *last;
        while ( NULL != mid && NULL != head_tail )
        {
            if ( scorep_profile_node_less_than( head_tail, mid ) )
            {
                /* Just advance head_tail */
                last      = &( head_tail->next_sibling );
                head_tail = *last;
            }
            else
            {
                /* Insert the element from mid before head_tail and advance mid */
                scorep_profile_node* mid_next_sibling = mid->next_sibling;
                *last = mid;
                last  = &( mid->next_sibling );
                *last = head_tail;
                mid   = mid_next_sibling;
            }
        }
        if ( NULL != mid )
        {
            /* Concatenate. tail is already at the end of the list */
            *last = mid;
        }
        else if ( NULL != head_tail )
        {
            /* Bring tail to the end of the list */
            while ( NULL != head_tail->next_sibling )
            {
                head_tail = head_tail->next_sibling;
            }
            *tail = head_tail;
        }
    }
}

/**
   Searches for a a thread start node to a given fork
   in a specific location subtree.
   @param root  The root node of a location subtree.
   @param fork  The node where the fork happended.
   @return The thread start node forked at @a fork if it exists.
           If no matching node is found, the function returns NULL.
 */
static scorep_profile_node*
get_thread_start_for_fork( scorep_profile_node* root,
                           scorep_profile_node* fork )
{
    scorep_profile_node* child = root->first_child;
    while ( child != NULL )
    {
        if ( ( child->node_type == SCOREP_PROFILE_NODE_THREAD_START ) &&
             ( scorep_profile_type_get_fork_node( child->type_specific_data ) == fork ) )
        {
            return child;
        }

        child = child->next_sibling;
    }
    return NULL;
}

/**
   Sorts the children of every node of the subtree rooted in @a path.
   This functions decents also in forked threads.
   @param path  The root node of the subtree that is sorted.
 */
static void
sort_subtree( scorep_profile_node* path )
{
    uint32_t             ch, len;
    scorep_profile_node* curr;
    /* Sort children */
    for ( curr = path->first_child, len = 0; NULL != curr;
          curr = curr->next_sibling, len++ )
    {
        ;
    }
    merge_sort_call_paths( &( path->first_child ), &curr, len );

    /* Sort the subtrees of the children */
    for ( curr = path->first_child, ch = 0;
          NULL != curr;
          curr = curr->next_sibling, ch++ )
    {
        sort_subtree( curr );
    }

    /* Sort the subtrees of the fork children */
    if ( scorep_profile_is_fork_node( path ) )
    {
        scorep_profile_node* root = scorep_profile.first_root_node;
        while ( root != NULL )
        {
            curr = get_thread_start_for_fork( root, path );
            if ( curr != NULL )
            {
                sort_subtree( curr );
            }

            root = root->next_sibling;
        }
    }
}

/**
   Searches for a sparse metric which stores the number of received bytes.
   @param node  The node for which the sparse metric is searched.
   @return A pointer to the sparse metric object.
 */
static scorep_profile_sparse_metric_int*
get_recv_bytes( scorep_profile_node* node )
{
    SCOREP_MetricHandle metric_handle = scorep_profile_get_bytes_recv_metric_handle();
    if ( metric_handle == SCOREP_INVALID_METRIC )
    {
        return NULL;
    }

    for ( scorep_profile_sparse_metric_int* sparse = node->first_int_sparse;
          sparse != NULL;
          sparse = sparse->next_metric )
    {
        if ( sparse->metric == metric_handle )
        {
            return sparse;
        }
    }
    return NULL;
}

/**
   Searches for a sparse metric which stores the number of send bytes.
   @param node  The node for which the sparse metric is searched.
   @return A pointer to the sparse metric object.
 */
static scorep_profile_sparse_metric_int*
get_send_bytes( scorep_profile_node* node )
{
    SCOREP_MetricHandle metric_handle = scorep_profile_get_bytes_send_metric_handle();
    if ( metric_handle == SCOREP_INVALID_METRIC )
    {
        return NULL;
    }

    for ( scorep_profile_sparse_metric_int* sparse = node->first_int_sparse;
          sparse != NULL;
          sparse = sparse->next_metric )
    {
        if ( sparse->metric == metric_handle )
        {
            return sparse;
        }
    }
    return NULL;
}

/**
   Checks whether a node represents an MPI function.
   @param node  Pointer to the profile node under investigation.
   @returns true is @a node represents an MPI function. Else it
            returns false.
 */
static inline bool
is_mpi_node( scorep_profile_node* node )
{
    return node->node_type == SCOREP_PROFILE_NODE_REGULAR_REGION &&
           SCOREP_RegionHandle_GetParadigmType( scorep_profile_type_get_region_handle( node->type_specific_data ) ) == SCOREP_PARADIGM_MPI;
}

/**
   Adds up the metric values of exclusive metrics from the given subtree
   in the given cluster.
   Called during intialization of a new cluster, representing a new iteration.
   @param cluster  The cluster representing the new iteration.
   @param path     Pointer to the root node of the subtree for which the
                   derived metrics are calculated.
 */
static void
calculate_derived_metrics( scorep_cluster_t*    cluster,
                           scorep_profile_node* path )
{
    scorep_profile_node* curr;

    /* Get the visits */
    cluster->mets_sum.dense_metrics[ MET_VISITS ] += path->count;

    /* Calculate derived MPI metrics */
    if ( is_mpi_node( path ) )
    {
        scorep_profile_sparse_metric_int* send_bytes;
        scorep_profile_sparse_metric_int* recv_bytes;

        uint64_t          visits         = path->count;
        uint64_t          execution_time = path->inclusive_time.sum;
        SCOREP_RegionType region_type    = SCOREP_RegionHandle_GetType( scorep_profile_type_get_region_handle( path->type_specific_data ) );

        cluster->mets_sum.dense_metrics[ MET_MPI_TIME ] += execution_time;
        switch ( region_type )
        {
            case SCOREP_REGION_BARRIER:
                cluster->mets_sum.dense_metrics[ MET_MPI_COLL_SYNC_TIME ] += execution_time;
                cluster->mets_sum.dense_metrics[ MET_COLL_SYNCS ]         += visits;
                break;
            case SCOREP_REGION_COLL_ONE2ALL:
            case SCOREP_REGION_COLL_ALL2ONE:
            case SCOREP_REGION_COLL_ALL2ALL:
            case SCOREP_REGION_COLL_OTHER:
                cluster->mets_sum.dense_metrics[ MET_MPI_COLL_COMM_TIME ] += execution_time;
                cluster->mets_sum.dense_metrics[ MET_COLL_COMM ]          += visits;
                send_bytes                                                 = get_send_bytes( path );
                if ( send_bytes != NULL )
                {
                    cluster->mets_sum.dense_metrics[ MET_COLL_BSENT ] += send_bytes->sum;
                }
                recv_bytes = get_recv_bytes( path );
                if ( recv_bytes != NULL )
                {
                    cluster->mets_sum.dense_metrics[ MET_COLL_BRCVD ] += recv_bytes->sum;
                }
                break;
            case SCOREP_REGION_POINT2POINT:
                cluster->mets_sum.dense_metrics[ MET_MPI_P2P_COMM_TIME ] += execution_time;
                cluster->mets_sum.dense_metrics[ MET_P2P_COMM ]          += visits;
                send_bytes                                                = get_send_bytes( path );
                if ( send_bytes != NULL )
                {
                    cluster->mets_sum.dense_metrics[ MET_SEND_BYTES ] += send_bytes->sum;
                    cluster->mets_sum.dense_metrics[ MET_SEND_COMMS ] += send_bytes->count;
                }
                recv_bytes = get_recv_bytes( path );
                if ( recv_bytes != NULL )
                {
                    cluster->mets_sum.dense_metrics[ MET_RECV_BYTES ] += recv_bytes->sum;
                    cluster->mets_sum.dense_metrics[ MET_RECV_COMMS ] += recv_bytes->count;
                }
                break;
            default:
                cluster->mets_sum.dense_metrics[ MET_MPI_TIME ] += execution_time;
        }
    }

    /* Call children */
    for ( curr = path->first_child; NULL != curr; curr = curr->next_sibling )
    {
        calculate_derived_metrics( cluster, curr );
    }

    /* Call fork children */
    if ( scorep_profile_is_fork_node( path ) )
    {
        for ( scorep_profile_node* root = scorep_profile.first_root_node;
              root != NULL;
              root = root->next_sibling )
        {
            curr = get_thread_start_for_fork( root, path );
            if ( curr != NULL )
            {
                calculate_derived_metrics( cluster, curr );
            }
        }
    }
}

/**
   Calculates metric values that are calculated from other inclusive metrics.
   Called during intialization of a new cluster, representing a new iteration.
   @param cluster  The cluster representing the new iteration.
 */
static void
post_process_derived_metrics( scorep_cluster_t* cluster )
{
    scorep_profile_node* path = cluster->root;

    /* Time */
    cluster->mets_sum.dense_metrics[ MET_EXECUTION_TIME ] = path->inclusive_time.sum;

    /* Set iteration_count to 1 */
    cluster->iteration_count = 1;

    /* Derived MPI metrics */
    cluster->mets_sum.dense_metrics[ MET_MPI_COMM_TIME ]
        = cluster->mets_sum.dense_metrics[ MET_MPI_P2P_COMM_TIME ]
          + cluster->mets_sum.dense_metrics[ MET_MPI_COLL_COMM_TIME ];

    cluster->mets_sum.dense_metrics[ MET_MPI_TIME ]
        += cluster->mets_sum.dense_metrics[ MET_MPI_COMM_TIME ]
           + cluster->mets_sum.dense_metrics[ MET_MPI_COLL_SYNC_TIME ];

    cluster->mets_sum.dense_metrics[ MET_P2P_BYTES ]
        = cluster->mets_sum.dense_metrics[ MET_SEND_BYTES ]
          + cluster->mets_sum.dense_metrics[ MET_RECV_BYTES ];

    cluster->mets_sum.dense_metrics[ MET_COLL_BYTES ]
        = cluster->mets_sum.dense_metrics[ MET_COLL_BRCVD ]
          + cluster->mets_sum.dense_metrics[ MET_COLL_BSENT ];

    cluster->mets_sum.dense_metrics[ MET_BYTES ]
        = cluster->mets_sum.dense_metrics[ MET_P2P_BYTES ]
          + cluster->mets_sum.dense_metrics[ MET_COLL_BYTES ];

    cluster->mets_sum.dense_metrics[ MET_COLL_COMM ]
        += cluster->mets_sum.dense_metrics[ MET_COLL_SYNCS ];

    cluster->mets_sum.dense_metrics[ MET_COMM ]
        = cluster->mets_sum.dense_metrics[ MET_P2P_COMM ]
          + cluster->mets_sum.dense_metrics[ MET_COLL_COMM ];

    /* Dense metrics */
    uint32_t dense_end = MET_DENSE_START + SCOREP_Metric_GetNumberOfStrictlySynchronousMetrics();
    for ( uint32_t mr = MET_DENSE_START; mr < dense_end; mr++ )
    {
        cluster->mets_sum.dense_metrics[ mr ] = path->dense_metrics[ mr - MET_DENSE_START ].sum;
    }

    /* Set avg data based on sum data. This function is only
       called from init_cluster, thus, the iteration count is 1 */
    for ( int mr = 0; mr < cluster_metric_number; mr++ )
    {
        cluster->mets_avg.dense_metrics[ mr ] = cluster->mets_sum.dense_metrics[ mr ];
    }
}

/**
   Initialize a new clustered iteration.
   @param cluster The newly created cluster object.
 */
static void
init_cluster( scorep_cluster_t* cluster )
{
    UTILS_DEBUG_ENTRY( "" );

    /* Let's do the sorting upfront */
    sort_subtree( cluster->root );

    /* Calculate hash value */
    cluster->hash = calculate_hash( cluster->root );

    /* The node in the root is always different, so remove it from the hash */
    cluster->hash -= scorep_profile_node_hash( cluster->root );

    /* Calculate derived metrics */
    calculate_derived_metrics( cluster, cluster->root );
    post_process_derived_metrics( cluster );

    UTILS_DEBUG_EXIT( "" );
}

/**
   Inserts a new cluster into the data structures. It finds the
   appropriate list in the equiv_cluster_list_set. If not found,
   creates it. The return value is the list used.
   @param location  The Score-P location object of the executing location.
   @param cluster   The cluster that is inserted.
   @param listSet   The list of equivalence sets of the associated clusterer.
   @returns the equivalence class into which the cluster was inserted.
 */
static scorep_equiv_cluster_list_t*
insert_cluster( SCOREP_Location*                 location,
                scorep_cluster_t*                cluster,
                scorep_equiv_cluster_list_set_t* listSet )
{
    /* We assume that the equivalence list is sorted by their hash value.
       Traverse the equivalence cluster list until last points to
       a lower value and list to a higher or equal value.
     */
    scorep_equiv_cluster_list_t* list, ** last = &( listSet->head );
    for ( list = *last; NULL != list && list->head->hash < cluster->hash;
          last = &( list->next ), list = *last )
    {
        ;
    }

    /* Compare the clusters call tree with all clusters with an equal
       hash value
     */
    for (; NULL != list && list->head->hash == cluster->hash;
         last = &( list->next ), list = *last )
    {
        if ( 0 == compare_call_trees( list->head->root, cluster->root,
                                      list->head->iteration_count,
                                      cluster->iteration_count ) )
        {
            //fprintf( stderr, "insert_cluster: Trees match\n" );
            /* Insert cluster */
            cluster->next = list->head;
            list->head    = cluster;
            return list;
        }

        //fprintf( stderr, "insert_cluster: Trees don't match\n" );
    }
    /*  scorep_cntl_msg("No match found, creating new list");*/
    /* List not found, create new list */
    list = new_equiv_cluster_list( location );
    /* Insert new list */
    list->next = *last;
    *last      = list;
    /* Insert cluster */
    list->head = cluster;

    return list;
}

/**
   Appends an element at the end of the queue
   @param queue  The queue into which the element is pushed.
   @param elem   The queue element that is appended.
 */
static void
cluster_queue_push( scorep_cluster_queue_t*      queue,
                    scorep_cluster_queue_elem_t* elem )
{
    *( queue->end ) = elem;
    elem->next      = NULL;
    queue->end      = &( elem->next );
}

/**
   Retrieves an element from the beginning of the queue and removes
   it from the queue.
   @param queue  The queue from which the first element is retrieved.
   @returns the first queue element. This element is removed from the
            queue.
 */
static scorep_cluster_queue_elem_t*
cluster_queue_pop( scorep_cluster_queue_t* queue )
{
    scorep_cluster_queue_elem_t* first = queue->begin;
    if ( NULL != first )
    {
        if ( NULL == ( queue->begin = queue->begin->next ) )
        {
            queue->end = &( queue->begin );
        }
    }

    return first;
}

/**
   Puts an iteration to the end of the clustering queue. The idea behind
   enqueueing new iterations is, that it might allow deferred clustering
   (e.g. when waiting at a barrier anyway).
   @param location   The Score-P location object of the executing location.
   @param clusterer  The associated clusterer object.
   @param path       The profile node that is the root of the new iteration.
 */
static void
add_iter_to_queue( SCOREP_Location*     location,
                   scorep_clusterer_t*  clusterer,
                   scorep_profile_node* path )
{
    scorep_cluster_queue_elem_t* elem =
        new_cluster_queue_elem( location, clusterer, path );
    cluster_queue_push( clusterer->queue, elem );
}

/**
   Adds all iterations from the queue to the clustering. The idea behind
   enqueueing new iterations is, that it might allow deferred clustering
   (e.g. when waiting at a barrier anyway). However, we currently
   process the queue content immidately after iteration exit.
   @param clusterer  The associated clusterer.
   @param location   The executing location.
 */
static void
process_queue( scorep_clusterer_t*          clusterer,
               SCOREP_Profile_LocationData* location )
{
    scorep_cluster_queue_elem_t* elem;
    while ( ( elem = cluster_queue_pop( clusterer->queue ) ) )
    {
        add_iter_to_clustering( location, elem->path, clusterer );
    }
}

/**
   Integrates a new iteration into the clustering.
   @param location  The executing location.
   @param path      The profile node that is the root of the new iteration.
   @param clusterer The associated clusterer.
 */
static void
add_iter_to_clustering( SCOREP_Profile_LocationData* location,
                        scorep_profile_node*         path,
                        scorep_clusterer_t*          clusterer )
{
    UTILS_DEBUG_ENTRY( "" );
    /* Create new clustered_iteration */
    scorep_clustered_iteration_t* clustered_iteration =
        new_clustered_iteration( location->location_data );

    /* Create new cluster */
    scorep_cluster_t* cluster = new_cluster( location->location_data,
                                             clusterer,
                                             clustered_iteration,
                                             path );
    /* Initialize cluster */
    init_cluster( cluster );

    /* Insert cluster. If there is no list with a tree like this, a new list
       will be created */
    scorep_equiv_cluster_list_t* list = insert_cluster( location->location_data,
                                                        cluster,
                                                        clusterer->eq_cl_lt_set );

    /* Update reference data */
    update_reference_values( clusterer->ref_vals, clustered_iteration );

    /* Insert new iteration into the iteration list */
    *( clusterer->cl_it_tail ) = clustered_iteration;
    clusterer->cl_it_tail      = &( clustered_iteration->next );
    /* Increment number of clustered iterations */
    clusterer->cl_it_count++;

    if ( clusterer->cl_it_count > clusterer->max_cl_count )
    {
        if ( clusterer->cl_it_count == clusterer->max_cl_count + 1 )
        {
            scorep_equiv_cluster_list_t* lt;
            scorep_cluster_t*            cl;

            /* This is the first time we calculate distances, just before
               the first potential merge. This is to be able to have as many
               iterations in the reference data already as possible */
            for ( lt = clusterer->eq_cl_lt_set->head; NULL != lt; lt = lt->next )
            {
                for ( cl = lt->head; NULL != cl; cl = cl->next )
                {
                    calculate_distances_for_new_cluster( location->location_data,
                                                         cl, lt, clusterer );
                }
            }
        }
        else
        {
            calculate_distances_for_new_cluster( location->location_data,
                                                 cluster, list, clusterer );
        }
    }

    /* Increment cluster count */
    clusterer->cl_count++;

    if ( clusterer->max_cl_count < clusterer->cl_count )
    {
        /* We have to merge clusters as we have more than the allowed maximum */
        if ( DBL_MAX == clusterer->eq_cl_lt_set->min_val )
        {
            /* All cluster lists contain only a single cluster by now.
               This is because there are too many structural equivalence classes.
               Let's be flexible and increase max_cl_count then. */
            fprintf( stderr, "Increase max_cl_count\n" );
            clusterer->max_cl_count++;
        }
        else
        {
            /* It's possible to merge two clusters */

            /* Go to the clusters with the lowest distance */

            list = clusterer->eq_cl_lt_set->min_pos;
            int mr;

            scorep_cluster_t* cl_a = list->min_pos;

            scorep_cluster_distance_t*  shortest_dist = cl_a->min_pos;
            scorep_cluster_distance_t** last;

            scorep_cluster_distance_t* temp_dist = cl_a->d;
            int                        ctr_a, ctr_b, ctr_d, ctr_diff, ctr_cl;
            scorep_cluster_t*          cl;
            scorep_cluster_t*          before_cl_b = cl_a; /* cl_b is after cl_a in the list */

            scorep_cluster_t*            cl_b = before_cl_b->next;
            scorep_equiv_cluster_list_t* lt;

            for ( ctr_diff = 1; temp_dist != shortest_dist;
                  temp_dist = temp_dist->next, ctr_diff++ )
            {
                before_cl_b = cl_b;
                cl_b        = cl_b->next;
            }

            UTILS_DEBUG( "cl_a: iteration_count: %d, hash: %d, cluster_id: %d",
                         cl_a->iteration_count, cl_a->hash, cl_a->cluster_id );
            UTILS_DEBUG( "cl_b: iteration_count: %d, hash: %d, cluster_id: %d",
                         cl_b->iteration_count, cl_b->hash, cl_b->cluster_id );

            /* Add up metric values from the two clusters in cl_a */
            cl_a->iteration_count += cl_b->iteration_count;
            for ( mr = 0; mr < cluster_metric_number; mr++ )
            {
                cl_a->mets_sum.dense_metrics[ mr ] += cl_b->mets_sum.dense_metrics[ mr ];
                cl_a->mets_avg.dense_metrics[ mr ]  =
                    cl_a->mets_sum.dense_metrics[ mr ] / cl_a->iteration_count;
            }

            /* Remove it from its parant list. If it contains a parallel regions,
               the merge takes care of removing the forked threads. */
            scorep_profile_remove_node( cl_b->root );

            /* Add up the two call trees */
            scorep_profile_merge_subtree( location, cl_a->root, cl_b->root );

            /* Merge the disjoint sets corresponding to these clusters */
            disjoint_set_merge( cl_a->disjoint_set, cl_b->disjoint_set, cl_a );

            /* Count the number of elements in the list up to cl_a */
            for ( cl = list->head, ctr_a = 0; cl != cl_a; cl = cl->next, ctr_a++ )
            {
                ;
            }

            /* Update all the distance values in all clusters before cl_a */
            for ( cl = list->head, ctr_cl = 1; cl_a != cl; cl = cl->next, ctr_cl++ )
            {
                /* First, find the distance from cl_a and update it */
                for ( last = &( cl->d ), temp_dist = *last, ctr_d = ctr_a - ctr_cl; 0 != ctr_d;
                      last = &( temp_dist->next ), temp_dist = *last, ctr_d-- )
                {
                    ;
                }
                temp_dist->value =
                    calculate_cluster_distance( cl, cl_a, clusterer->ref_vals );
                /* Update cluster minimum */
                if ( cl->min_val > temp_dist->value )
                {
                    cl->min_val = temp_dist->value;
                    cl->min_pos = temp_dist;
                }
                else if ( cl->min_pos == temp_dist )
                {
                    /* It was exactly this distance that had the lowest value
                       and it didn't get lower, so check if it's still the lowest */
                    scorep_cluster_distance_t* d;
                    cl->min_val = DBL_MAX;
                    for ( d = cl->d; NULL != d; d = d->next )
                    {
                        if ( cl->min_val > d->value )
                        {
                            cl->min_val = d->value;
                            cl->min_pos = d;
                        }
                    }
                }
                /* Second, find the distance from cl_b and delete it */
                for ( ctr_d = ctr_diff; 0 != ctr_d;
                      last = &( temp_dist->next ), temp_dist = *last, ctr_d-- )
                {
                    ;
                }
                /* Unlink and delete the distance */
                if ( cl->min_pos == temp_dist )
                {
                    /* It was exactly this distance that had the lowest value, and
                       now it's being deleted. So let's update the minimum */
                    scorep_cluster_distance_t* d;
                    temp_dist->value = DBL_MAX;
                    cl->min_val      = DBL_MAX;
                    for ( d = cl->d; NULL != d; d = d->next )
                    {
                        if ( cl->min_val > d->value )
                        {
                            cl->min_val = d->value;
                            cl->min_pos = d;
                        }
                    }
                }
                *last = delete_cluster_distance( temp_dist, clusterer );
            }

            /* Remove the element for cl_b from the distance lists of the
               clusters between cl_a and cl_b, including cl_a */
            for ( cl = cl_a, ctr_cl = 1; cl_b != cl; cl = cl->next, ctr_cl++ )
            {
                /* Find the distance from cl_b and delete it */
                for ( last = &( cl->d ), temp_dist = *last, ctr_d = ctr_diff - ctr_cl; 0 != ctr_d;
                      last = &( temp_dist->next ), temp_dist = *last, ctr_d-- )
                {
                    ;
                }
                /* Unlink and delete the distance */
                if ( cl->min_pos == temp_dist )
                {
                    /* It was exactly this distance that had the lowest value, and
                       now it's being deleted. So let's update the minimum */
                    scorep_cluster_distance_t* d;
                    temp_dist->value = DBL_MAX;
                    cl->min_val      = DBL_MAX;
                    for ( d = cl->d; NULL != d; d = d->next )
                    {
                        if ( cl->min_val > d->value )
                        {
                            cl->min_val = d->value;
                            cl->min_pos = d;
                        }
                    }
                }
                *last = delete_cluster_distance( temp_dist, clusterer );
            }

            /* Unlink and delete cl_b */
            before_cl_b->next = delete_cluster( cl_b, clusterer );

            /* Update all the distance values in cl_a */
            cl_a->min_val = DBL_MAX;
            for ( temp_dist = cl_a->d, cl = cl_a->next; temp_dist != NULL;
                  temp_dist = temp_dist->next, cl = cl->next )
            {
                temp_dist->value =
                    calculate_cluster_distance( cl_a, cl, clusterer->ref_vals );
                /* Update minimum */
                if ( cl_a->min_val > temp_dist->value )
                {
                    cl_a->min_val = temp_dist->value;
                    cl_a->min_pos = temp_dist;
                }
            }

            /* Ivalidate min_val at the list level and recalculate the minimum */
            list->min_val = DBL_MAX;
            for ( cl = list->head; NULL != cl; cl = cl->next )
            {
                if ( list->min_val > cl->min_val )
                {
                    list->min_val = cl->min_val;
                    list->min_pos = cl;
                }
            }

            /* Re-calculate global minimum */
            clusterer->eq_cl_lt_set->min_val = DBL_MAX;
            for ( lt = clusterer->eq_cl_lt_set->head; NULL != lt; lt = lt->next )
            {
                if ( clusterer->eq_cl_lt_set->min_val > lt->min_val )
                {
                    clusterer->eq_cl_lt_set->min_val = lt->min_val;
                    clusterer->eq_cl_lt_set->min_pos = lt;
                }
            }

            /* Merge done, decrement cluster count */
            clusterer->cl_count--;
        }
    }
    UTILS_DEBUG_EXIT( "" );
}

/**
   Evaluates whether we need to consider the visit count for a @a node
   when conidering equivalence classes.
   @param clusterMode  The clustering mode.
   @param node         The profile node.
   @returns true if we need to consider the visit count for @a node.
 */
static inline bool
consider_visit_count( int clusterMode, scorep_profile_node* node )
{
    if ( clusterMode == SCOREP_PROFILE_CLUSTER_SUBTREE_VISITS ||
         clusterMode == SCOREP_PROFILE_CLUSTER_MPI_VISITS_ALL ||
         ( clusterMode == SCOREP_PROFILE_CLUSTER_MPI_VISITS && is_mpi_node( node ) ) )
    {
        if ( node->node_type == SCOREP_PROFILE_NODE_REGULAR_REGION )
        {
            const char* name = SCOREP_RegionHandle_GetName( scorep_profile_type_get_region_handle( node->type_specific_data ) );
            if ( ( 0 == strncmp( "MPI_Probe", name, 9 ) ) ||
                 ( 0 == strncmp( "MPI_Iprobe", name, 10 ) ) ||
                 ( 0 == strncmp( "MPI_Test", name, 8 ) ) )
            {
                return false;
            }
        }
        return true;
    }
    return false;
}

/**
   Compares the structure of two call trees according to the structural
   clustering mode. The clustering mode can be set via the environment
   variable SCOREP_PROFILE_CLUSTERING_MODE. The following modes exist:
   0 - off, do not enforce matching call-trees.
   1 - full structure.
   2 - full structure plus visits.
   3 - MPI and COM structure.
   4 - MPI and COM structure, MPI visits.
   5 - MPI and COM structure, MPI and COM visits.
   @param a        Pointer to the root of the first call tree.
   @param b        Pointer to the root of the second call tree.
   @param itCntA   Number of iterations contained in a.
   @param itCntB   Number of iterations conatined in b.
   @returns 0 if the structure of a and b matches according to the
            rules of the clustering mode. Otherwise non-zero is
            returned.
 */
static int
compare_call_trees( scorep_profile_node* a, scorep_profile_node* b,
                    uint32_t itCntA, uint32_t itCntB )
{
    scorep_profile_node*       a_tail;
    scorep_profile_node*       b_tail;
    SCOREP_Profile_ClusterMode clustering_mode = scorep_profile_get_cluster_mode();
    if ( SCOREP_PROFILE_CLUSTER_NONE == clustering_mode )
    {
        return 0;
    }

    /* No need to sort children, they should already be sorted by now */

    /* Compare children */
    for ( a_tail = a->first_child, b_tail = b->first_child; NULL != a_tail && NULL != b_tail;
          a_tail = a_tail->next_sibling, b_tail = b_tail->next_sibling )
    {
        /* Search forward until the next child that has MPI in its subtree */
        if ( clustering_mode >= SCOREP_PROFILE_CLUSTER_MPI )
        {
            while ( NULL != a_tail && !scorep_profile_is_mpi_in_subtree( a_tail ) )
            {
                a_tail = a_tail->next_sibling;
            }
            while ( NULL != b_tail && !scorep_profile_is_mpi_in_subtree( b_tail ) )
            {
                b_tail = b_tail->next_sibling;
            }
            if ( NULL == a_tail || NULL == b_tail )
            {
                break;
            }
        }

        if ( scorep_profile_compare_nodes( a_tail, b_tail ) &&           /* Same node and region types and */
             ( !consider_visit_count( clustering_mode, a_tail ) ||       /* visits do not matter or */
               ( a_tail->count / itCntA ==  b_tail->count / itCntB ) ) ) /* visits match */
        {
            int res = compare_call_trees( a_tail, b_tail, itCntA, itCntB );
            if ( 0 != res )
            {
                return res;
            }
        }
        else
        {
            fprintf( stderr, "Warning: the trees don't match at comparison!" );
            if ( scorep_profile_node_less_than( a_tail, b_tail ) )
            {
                return -1;
            }
            else if ( scorep_profile_node_less_than( b_tail, a_tail ) )
            {
                return 1;
            }
            else if ( a_tail->count / itCntA <
                      b_tail->count / itCntB )
            {
                return -1;
            }
            else /* a_tail->record->count / itCntA >
                    b_tail->record->count / itCntB */
            {
                return 1;
            }
        }
    }

    /* Do this check, just in case */
    if ( SCOREP_PROFILE_CLUSTER_MPI <= clustering_mode )
    {
        while ( NULL != a_tail && !scorep_profile_is_mpi_in_subtree( a_tail ) )
        {
            a_tail = a_tail->next_sibling;
        }
        while ( NULL != b_tail && !scorep_profile_is_mpi_in_subtree( b_tail ) )
        {
            b_tail = b_tail->next_sibling;
        }
    }

    /* Check if there are children left */
    if ( NULL != a_tail )
    {
        return 1;
    }
    if ( NULL != b_tail )
    {
        return -1;
    }

    /* Compare fork children */
    if ( scorep_profile_is_fork_node( a ) || scorep_profile_is_fork_node( b ) )
    {
        if ( !scorep_profile_is_fork_node( a ) )
        {
            UTILS_WARNING( "fork children don't match at comparison A!" );
            return -1;
        }
        else if ( !scorep_profile_is_fork_node( b ) )
        {
            UTILS_WARNING( "fork children don't match at comparison B!" );
            return 1;
        }

        for ( scorep_profile_node* root = scorep_profile.first_root_node;
              root != NULL;
              root = root->next_sibling )
        {
            scorep_profile_node* child_a = get_thread_start_for_fork( root, a );
            scorep_profile_node* child_b = get_thread_start_for_fork( root, b );

            if ( child_a == NULL && child_b == NULL )
            {
                /* Unused location */
                continue;
            }

            if ( child_a == NULL && child_b != NULL )
            {
                UTILS_WARNING( "fork children don't match at comparison! C" );
                return -1;
            }

            if ( child_a != NULL && child_b == NULL )
            {
                UTILS_WARNING( "fork children don't match at comparison! D" );
                return 1;
            }

            if ( ( SCOREP_PROFILE_CLUSTER_MPI <= clustering_mode )
                 && !scorep_profile_is_mpi_in_subtree( child_a )
                 && !scorep_profile_is_mpi_in_subtree( child_b ) )
            {
                continue;
            }

            int res = compare_call_trees( child_a, child_b, itCntA, itCntB );
            if ( 0 != res )
            {
                fprintf( stderr, "comparison failed, due to forked children\n" );
                fflush( stderr );
                return res;
            }
        }
    }

    /* No difference was found */
    return 0;
}

/**
   Calculate a hash value based on the nodeids and the structure of the tree.
   Also set mpi_in_subtree value in each node of the tree.
   @param path The root node of the profile tree.
   @returns the hash value.
 */
static uint32_t
calculate_hash( scorep_profile_node* path )
{
    uint32_t                   val = 0;
    uint32_t                   ch, used_ch;
    scorep_profile_node*       curr;
    SCOREP_Profile_ClusterMode clustering_mode = scorep_profile_get_cluster_mode();

    if ( SCOREP_PROFILE_CLUSTER_NONE == clustering_mode )
    {
        return scorep_profile_node_hash( path ); /* this is subtracted later, so this is 0 */
    }
    scorep_profile_set_mpi_in_subtree( path, is_mpi_node( path ) );

    /* No need to sort the children, they should be sorted by now */

    /* Calculate hash for children */
    for ( curr = path->first_child, ch = 0, used_ch = 0; NULL != curr; curr = curr->next_sibling, ch++ )
    {
        /* The call also sets mpi_in_subtree in the child */
        int child_val = calculate_hash( curr );
        /* In these modes, only the subtrees with MPI count */
        if ( SCOREP_PROFILE_CLUSTER_MPI <= clustering_mode && !scorep_profile_is_mpi_in_subtree( curr ) )
        {
            continue;
        }
        if ( scorep_profile_is_mpi_in_subtree( curr ) )
        {
            scorep_profile_set_mpi_in_subtree( path, true );
        }
        val += ++used_ch;
        val  = ( val >> 1 ) | ( val << 31 );
        val += child_val;
    }

    /* Calculate hash for fork children */
    if ( scorep_profile_is_fork_node( path ) )
    {
        int thread_count = 1;
        for ( scorep_profile_node* root = scorep_profile.first_root_node;
              root != NULL;
              root = root->next_sibling )
        {
            curr = get_thread_start_for_fork( root, path );
            if ( curr == NULL )
            {
                continue;
            }
            thread_count++;

            int child_val = calculate_hash( curr ) - scorep_profile_node_hash( curr );
            /* In these modes, only the subtrees with MPI count */
            if ( SCOREP_PROFILE_CLUSTER_MPI <= clustering_mode &&
                 !scorep_profile_is_mpi_in_subtree( curr ) )
            {
                continue;
            }
            if ( scorep_profile_is_mpi_in_subtree( curr ) )
            {
                scorep_profile_set_mpi_in_subtree( path, true );
            }
            val += thread_count + 1;
            val  = ( val >> 1 ) | ( val << 31 );
            val += child_val;
        }
    }

    /* Calculate hash from local data */
    if ( consider_visit_count( clustering_mode, path ) )
    {
        val += path->count;
        val  = ( val >> 1 ) | ( val << 31 );
    }

    /* Add the nodeid of the current node */
    val += scorep_profile_node_hash( path );
    return val;
}

/**
   Writes the mapping data for one iteration into Cube.
   @param writeData   The Cube writing data object.
   @param lineno      The iteration number.
   @param entry_count The number of ranks.
   @param entries     An array containing the cnode ids of the iteration for
                      each rank. The ith entry represent the cnode id of
                      the cluster for the ith rank. A value of zero
                      means that this iteration does not exist on the particular
                      rank.
 */
static void
cluster_write_line( scorep_cube_writing_data* writeData,
                    uint32_t                  lineno,
                    uint32_t                  entry_count,
                    uint32_t*                 entries )
{
    /* Calculate string length */
    size_t length = 0;
    for (  uint32_t it = 0; it < entry_count; it++ )
    {
        length += 2 + ( entries[ it ] == 0 ? 0 : log10( entries[ it ] ) );
    }
    length++; /* Add space for the terminating '\0' */

    /* Create comma separated list of cluster number */
    char* line        = ( char* )calloc( length, sizeof( char ) );
    char* current_pos = line;
    if ( line == NULL )
    {
        UTILS_ERROR_POSIX( "Failed to allocate memory to write mapping." );
        abort();
    }

    const char* sep = "";
    for ( uint32_t it = 0; it < entry_count; it++ )
    {
        current_pos += sprintf( current_pos, "%s%" PRIu32, sep, entries[ it ] );
        sep          = ",";
    }

    /* Create key string */
    char key[ 32 ];
    sprintf( key, "CLUSTER MAPPING %" PRIu32, lineno );
    cube_def_attr( writeData->my_cube, key, line );

    free( line );
}

/* **************************************************************************
   External visible functions
****************************************************************************/

void
scorep_cluster_on_enter_dynamic(  SCOREP_Profile_LocationData* location,
                                  scorep_profile_node*         node )
{
    /* If we do not cluster or the only supported cluster is already identified
       nothing is to do. */
    if ( !clustering_enabled || cluster_parent != NULL )
    {
        return;
    }


    const char* clustered_region = scorep_profile_get_clustered_region();
    const char* current_region   = SCOREP_RegionHandle_GetName( scorep_profile_type_get_region_handle( node->type_specific_data ) );

    /* If no region is specified, cluster the first dynamic region. */
    if ( strcmp( clustered_region, "" ) == 0 ||
         strcmp( clustered_region, current_region ) == 0 )
    {
        /* Cannot cluster regions inside parallel regions. */
        if ( SCOREP_Thread_InParallel() )
        {
            UTILS_WARNING( "Cannot cluster regions that appear inside "
                           "of parallel regions. Clustering disabled." );
            clustering_enabled = false;
            return;
        }

        /* This will be the region that we cluster */
        cluster_parent = node;
    }
}


/* NOTE: This function should only be called outside parallel
   regions. In general, clustering dynamic regions is only supported when the
   root of the dynamic region is not in a parallel region. However, parallel
   regions inside a dynamic region are allowed. */
void
scorep_cluster_if_necessary( SCOREP_Profile_LocationData* location,
                             scorep_profile_node*         node )
{
    /* Check if we plan to cluster this dynamic region or not */
    if ( !clustering_enabled || node->parent != cluster_parent )
    {
        return;
    }

    /* TODO: I use only one clusterer here, but it would be easy to create
       separate clusterers for different dynamic regions and store them in an
       array. Still, don't use clustering on nested dynamic regions, it would
       probably lead to undefined/unexpected behavior */

    /* Check if the clusterer exists, and create it if necessary */
    if ( NULL == scorep_clusterer )
    {
        /* Initialize also the number of dense metrics */
        cluster_metric_number = MET_DENSE_START
                                + SCOREP_Metric_GetNumberOfStrictlySynchronousMetrics();

        /* Create clusterer */
        scorep_clusterer = new_clusterer( location->location_data,
                                          scorep_profile_get_cluster_count() );
    }

    /* Add the iteration to the queue */
    add_iter_to_queue( location->location_data, scorep_clusterer, node );

    /* Process the queue. This is only one of the possibilities, processing the
       queue here, immediately. It could be done somewhere later, e.g. at a
       collective synchronization point, where it could be completely hidden
       between two barriers or such... */
    process_queue( scorep_clusterer, location );
}

void
scorep_cluster_initialize( void )
{
    SCOREP_MutexCreate( &cluster_distance_lock );
    SCOREP_MutexCreate( &cluster_lock );
    SCOREP_MutexCreate( &cluster_queue_elem_lock );

    if ( !scorep_profile_do_clustering() )
    {
        return;
    }

    /* Sanity checks */
    if ( scorep_profile_get_cluster_count() < 1 )
    {
        UTILS_WARNING( "Max number of clusters is zero. Clustering disabled. "
                       "Set SCOREP_CLUSTER_COUNT to a value greater than 0 to enable "
                       "clustering" );
        return;
    }

    if ( scorep_profile_get_cluster_mode() > SCOREP_PROFILE_CLUSTER_MPI_VISITS_ALL )
    {
        /* Clustering switched off */
        UTILS_WARNING( "Unknown clustering mode %d set in "
                       "SCOREP_CLUSTERING_MODE. Possible modes values are 0 to 5. "
                       "Read the manual for the meaning of the mode values.",
                       scorep_profile_get_cluster_mode() );
        UTILS_WARNING( "Disable clustering." );
        return;
    }

    clustering_enabled = true;
}

void
scorep_cluster_finalize( void )
{
    clustering_enabled = false;

    SCOREP_MutexDestroy( &cluster_queue_elem_lock );
    SCOREP_MutexDestroy( &cluster_lock );
    SCOREP_MutexDestroy( &cluster_distance_lock );
}

void
scorep_cluster_postprocess( void )
{
    if ( scorep_clusterer == NULL || !clustering_enabled )
    {
        return;
    }

    /* Enumerate all clusters from 1 to n */
    uint64_t counter = 1;
    for ( scorep_equiv_cluster_list_t* list = scorep_clusterer->eq_cl_lt_set->head;
          list != NULL;
          list = list->next )
    {
        for ( scorep_cluster_t* cluster = list->head;
              cluster != NULL;
              cluster = cluster->next )
        {
            scorep_profile_type_set_int_value( &cluster->root->type_specific_data,
                                               counter );
            counter++;
        }
    }
}

void
scorep_cluster_write_cube4( scorep_cube_writing_data* writeData )
{
    char                 value[ 12 ];
    int*                 cluster_ids = NULL;
    scorep_profile_node* root        = NULL;

    /* Write flag whether clustering happened */
    int32_t has_cluster_local  = ( scorep_clusterer == NULL ? 0 : 1 );
    int32_t has_cluster_global = 0;
    SCOREP_Ipc_Allreduce( &has_cluster_local,
                          &has_cluster_global,
                          1, SCOREP_IPC_INT32_T,
                          SCOREP_IPC_SUM );
    if ( has_cluster_global == 0 )
    {
        if ( writeData->my_rank == writeData->root_rank )
        {
            cube_def_attr( writeData->my_cube, "CLUSTERING", "OFF" );
        }
        return; /* No clusters */
    }

    if ( writeData->my_rank == writeData->root_rank )
    {
        cube_def_attr( writeData->my_cube, "CLUSTERING", "ON" );

        /* Write cluster root */
        root = disjoint_set_get_root( scorep_clusterer->cl_it_head )->cluster->root->parent;
        SCOREP_CallpathHandle handle =
            SCOREP_CallpathHandle_GetUnified( root->callpath_handle );
        uint32_t root_id =
            cube_cnode_get_id( scorep_get_cube4_callpath( writeData->map, handle ) );
        sprintf( value, "%" PRIu32, root_id );
        cube_def_attr( writeData->my_cube, "CLUSTER ROOT CNODE ID", value );

        /* Write number of processes */
        sprintf( value, "%" PRIu32, writeData->ranks_number );
        cube_def_attr( writeData->my_cube, "CLUSTER PROCESS NUM", value );
    }

    /* Create mapping from cluster number to Cube's cnode id.
       Clusters are enumerated from 1 to n.
       Cluster number 0 means invalid cluster */
    if ( writeData->my_rank == writeData->root_rank )
    {
        SCOREP_CallpathHandle handle =
            SCOREP_CallpathHandle_GetUnified( root->callpath_handle );
        cube_cnode* root_cn     = scorep_get_cube4_callpath( writeData->map, handle );
        uint32_t    cluster_num = cube_cnode_num_children( root_cn );
        cluster_ids = ( int* )malloc( sizeof( int ) * cluster_num );
        UTILS_ASSERT( cluster_ids );

        for ( int i = 0; i < cluster_num; i++ )
        {
            cube_cnode* child_cn = cube_cnode_get_child( root_cn, i );
            const char* name     = cube_region_get_name( cube_cnode_get_callee( child_cn ) );
            uint64_t    index    = atoi( &name[ 9 ] );                /* Format is 'instance=<number>' */
            cluster_ids[ index - 1 ] = cube_cnode_get_id( child_cn ); /* Enumeration starts with 1 */
        }
    }

    /* Determine maximum number of iterations */
    uint32_t global_it_count = 0;
    SCOREP_Ipc_Allreduce( &scorep_clusterer->cl_it_count,
                          &global_it_count,
                          1, SCOREP_IPC_UINT32_T,
                          SCOREP_IPC_MAX );

    if ( writeData->my_rank == writeData->root_rank )
    {
        sprintf( value, "%" PRIu32, global_it_count );
        cube_def_attr( writeData->my_cube, "CLUSTER ITERATION COUNT", value );
    }

    /* Collect array with local iterations.
       Iterations are enumerated from 1 to n. */
    uint32_t* it_map = ( uint32_t* )calloc( global_it_count, sizeof( uint32_t ) );
    UTILS_ASSERT( it_map );
    for (  scorep_clustered_iteration_t* it = scorep_clusterer->cl_it_head;
           it != NULL;
           it = it->next )
    {
        scorep_profile_node* cluster = disjoint_set_get_root( it )->cluster->root;
        const char*          name    = SCOREP_RegionHandle_GetName( scorep_profile_type_get_region_handle( cluster->type_specific_data ) );
        it_map[ it->iteration_id - 1 ] = atoi( &name[ 9 ] ); /* Format is 'instance=<number>' */
    }

    /* Collect data to rank 0 */
    /* Variant that writes one iterations for all process in one line */
    uint32_t* line = NULL;
    if ( writeData->my_rank == writeData->root_rank )
    {
        line = ( uint32_t* )malloc( writeData->ranks_number * sizeof( uint32_t ) );
    }

    for ( uint32_t i = 0; i < global_it_count; i++ )
    {
        SCOREP_Ipc_Gather( &it_map[ i ], line,
                           1, SCOREP_IPC_UINT32_T,
                           0 );
        SCOREP_Ipc_Barrier();

        if ( writeData->my_rank == writeData->root_rank )
        {
            /* Replace cluster number by cnode id
               cnode id 0 means that the iteations did not appear on this rank. */
            for ( int i = 0; i < writeData->ranks_number; i++ )
            {
                line[ i ] = ( line[ i ] == 0 ? 0 : cluster_ids[ line[ i ] - 1 ] );
            }
            cluster_write_line( writeData, i, writeData->ranks_number, line );
        }
    }
    if ( writeData->my_rank == writeData->root_rank )
    {
        free( line );
        free( cluster_ids );
    }

    free( it_map );
}
