#include "silc_profile_types.h"
#include "silc_profile_tree.h"
#include "SILC_Utils.h"
#include <stdbool.h>

#define SILC_PROFILE_ASSURE_INITIALIZED if ( !silc_profile_is_initialized ) { return; }
#define SILC_PROFILE_STOP silc_profile_is_initialized = false;

static silc_profile_definition silc_profile;
static bool                    silc_profile_is_initialized = false;

/*----------------------------------------------------------------------------------------
   Thread handling not yet implemented
   ----------------------------------------------------------------------------------------*/
static silc_profile_node* silc_profile_current_node = NULL;

inline silc_profile_node*
silc_profile_get_current_node( SILC_Thread_Data* thread )
{
    return silc_profile_current_node;
}

inline void
silc_profile_set_current_node( SILC_Thread_Data*  thread,
                               silc_profile_node* node )
{
    silc_profile_current_node = node;
}

/*----------------------------------------------------------------------------------------
   Constructors / destriuctors
   ----------------------------------------------------------------------------------------*/
void
silc_profile_init_definition( int32_t             numDenseMetrics,
                              SILC_CounterHandle* metrics )
{
    int i;
    silc_profile.first_root_node      = NULL;
    silc_profile.num_of_dense_metrics = numDenseMetrics;
    silc_profile.dense_metrics        = malloc( numDenseMetrics * sizeof( SILC_CounterHandle ) );

    for ( i = 0; i < numDenseMetrics; i++ )
    {
        silc_profile.dense_metrics[ i ] = metrics[ i ];
    }
}

void
silc_profile_delete_definition()
{
    silc_profile.first_root_node      = NULL;
    silc_profile.num_of_dense_metrics = 0;
    if ( silc_profile.dense_metrics )
    {
        free( silc_profile.dense_metrics );
        silc_profile.dense_metrics = NULL;
    }
}

void
silc_profile_init_dense_metric( silc_profile_dense_metric* metric )
{
    metric->sum         = 0;
    metric->min         = UINT64_MAX;
    metric->max         = 0;
    metric->squares     = 0;
    metric->start_value = 0;
}

silc_profile_node*
silc_profile_create_region_node( silc_profile_node* parent,
                                 SILC_RegionHandle  region,
                                 uint64_t           timestamp )
{
    int i;

    /* Reserve space for the node record and dense metrics */
    silc_profile_node* node = ( silc_profile_node* )malloc( sizeof( silc_profile_node )
                                                            + silc_profile.num_of_dense_metrics
                                                            * sizeof( silc_profile_dense_metric ) );
    if ( !node )
    {
        SILC_ERROR_POSIX();
        return NULL;
    }

    /* Space for dense metrics are reserved after the node struct */
    node->dense_metrics =
        ( silc_profile_dense_metric* )( ( void* )node + sizeof( silc_profile_node ) );

    /* Initialize values */
    node->parent              = parent;
    node->first_child         = NULL;
    node->next_sibling        = NULL;
    node->first_double_sparse = NULL;
    node->first_int_sparse    = NULL;
    node->count               = 0; /* Is increased to one during SILC_Profile_Enter() */
    node->first_enter_time    = timestamp;
    node->last_exit_time      = timestamp;
    node->node_type           = silc_profile_node_regular_region;
    node->type_specific_data  = region;

    /* Initialize dense metric values */
    silc_profile_init_dense_metric( &node->implicit_time );
    for ( i = 0; i < silc_profile.num_of_dense_metrics; i++ )
    {
        silc_profile_init_dense_metric( &node->dense_metrics[ i ] );
    }

    return node;
}

/*----------------------------------------------------------------------------------------
   Helper functions
   ----------------------------------------------------------------------------------------*/
inline void
silc_profile_update_dense_metric( silc_profile_dense_metric* metric,
                                  uint64_t                   end_value )
{
    double diff = end_value - metric->start_value;
    metric->sum += diff;
    if ( metric->min > diff )
    {
        metric->min = diff;
    }
    if ( metric->max < diff )
    {
        metric->max = diff;
    }
    metric->squares += diff * diff;
}

/*----------------------------------------------------------------------------------------
   Interface implementation
   ----------------------------------------------------------------------------------------*/

void
SILC_Profile_Initialize( int32_t             numDenseMetrics,
                         SILC_CounterHandle* metrics )
{
    if ( silc_profile_is_initialized )
    {
        return;
    }

    silc_profile_is_initialized = true;

    silc_profile_init_definition( numDenseMetrics, metrics );
}

void
SILC_Profile_Finalize()
{
    silc_profile_is_initialized = false;

    silc_profile_delete_definition();
}

void
SILC_Profile_Enter( SILC_Thread_Data* thread,
                    SILC_RegionHandle region,
                    SILC_RegionType   type,
                    uint64_t          timestamp,
                    uint64_t*         metrics )
{
    silc_profile_node* node  = silc_profile_get_current_node( thread );
    silc_profile_node* child = NULL;
    int                i;

    SILC_PROFILE_ASSURE_INITIALIZED;

    /* Find or create child node:
       Test if its the first region of the profile or thread */
    if ( node == NULL )
    {
        if ( silc_profile.first_root_node == NULL )
        {
            node                         = silc_profile_create_region_node( NULL, region, timestamp );
            silc_profile.first_root_node = node;
        }
        else
        {
            SILC_ERROR( SILC_ERROR_PROFILE_UNINITIALIZED_THREAD,
                        "Enter event occured for thread with invalid current node" );
            SILC_PROFILE_STOP;
            return;
        }
    }
    /* It is a subnode of an existing tree: Traverse childen */
    else
    {
        child = node->first_child;
        /* It is the first child */
        if ( child == NULL )
        {
            node->first_child = silc_profile_create_region_node( node, region, timestamp );
            node              = node->first_child;
        }
        /* At least one child exists: Search if the region was entered before */
        else
        {
            while ( ( child->next_sibling != NULL ) ||
                    ( child->node_type != silc_profile_node_regular_region ) ||
                    ( child->type_specific_data != region ) )
            {
                child = child->next_sibling;
            }
            /* No matching region found: Create a new sibling */
            if ( ( child->node_type != silc_profile_node_regular_region ) ||
                 ( child->type_specific_data != region ) )
            {
                child->next_sibling = silc_profile_create_region_node( node, region, timestamp );
                node                = child->next_sibling;
            }
            /* Call path was already visited */
            else
            {
                node = child;
            }
        }
    }
    /* Now node points to the newly entered region */

    /* Store start values for dense metrics */
    node->count++;
    node->implicit_time.start_value = timestamp;
    for ( i = 0; i < silc_profile.num_of_dense_metrics; i++ )
    {
        node->dense_metrics[ i ].start_value = metrics[ i ];
    }

    /* Update current node pointer */
    silc_profile_set_current_node( thread, node );
}

/** Exit a region */
void
SILC_Profile_Exit( SILC_Thread_Data* thread,
                   SILC_RegionHandle region,
                   uint64_t          timestamp,
                   uint64_t*         metrics )
{
    int                i;
    silc_profile_node* node   = silc_profile_get_current_node( thread );
    silc_profile_node* parent = node;

    SILC_PROFILE_ASSURE_INITIALIZED;

    if ( node == NULL )
    {
        SILC_ERROR( SILC_ERROR_PROFILE_INCONSISTENT,
                    "Exit event occured in a thread which never entered a region" );
        SILC_PROFILE_STOP;
        return;
    }

    /* Exit all parameters and the region itself. Thus, more than one node may be exited.
       Initialize loop: start with this node. Further iterations should work on the
       parent. */
    parent = node;
    do
    {
        node = parent;

        /* Update metrics */
        node->count++;
        node->last_exit_time = timestamp;
        silc_profile_update_dense_metric( &node->implicit_time, timestamp );
        for ( i = 0; i < silc_profile.num_of_dense_metrics; i++ )
        {
            silc_profile_update_dense_metric( &node->dense_metrics[ i ], metrics[ i ] );
        }
        parent = node->parent;
    }
    while ( ( node->node_type != silc_profile_node_regular_region ) &&
            ( parent != NULL ) );

    /* If this was a parameter node also exit next level node */
    if ( ( node->node_type != silc_profile_node_regular_region ) &&
         ( node->type_specific_data != region ) )
    {
        SILC_ERROR( SILC_ERROR_PROFILE_INCONSISTENT,
                    "Exit event for other than current region occured" );
        SILC_PROFILE_STOP;
        return;
    }

    /* Update current node */
    silc_profile_set_current_node( thread, node );
}
