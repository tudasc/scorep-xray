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

/**
 * @file scorep_profile_tausnap_writer.c
 *
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @status alpha
 * Implements a profile writer in TAU snapshot format.
 */

#include <config.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>

#include <SCOREP_Memory.h>
#include <UTILS_CStr.h>
#include <UTILS_Debug.h>
#include <UTILS_Error.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Timing.h>

#include <scorep_profile_definition.h>
#include <scorep_definitions.h>
#include <scorep_mpi.h>
#include <scorep_runtime_management.h>
#include <scorep_profile_metric.h>
#include <scorep_profile_location.h>

extern SCOREP_DefinitionManager* scorep_unified_definition_manager;

/* Forward declaration */
static void
write_node_tau( scorep_profile_node* node,
                char*                parentpath,
                FILE*                file,
                uint64_t*            callpath_counter );

/**
   Helper function for the profile writer in TAU snapshot format.
   Writes callpath definition to a file.
   @param path String,     containing the callpath.
   @param file Pointer     to the file to which the data is written.
   @param callpath_counter counter of the encountered callpathes. The
                           counter is used to enumerate the callpathes and
                           serves as an unique id to map callpath definitions
                           to callpath values. The value of this variable gets
                           updated inside this function according to the
                           number of processed callpathes.
 */
static void
write_tausnap_def( char*     path,
                   FILE*     file,
                   uint64_t* callpath_counter )
{
    fprintf( file,
             "<event id=\"%" PRIu64 "\"><name>%s</name></event>\n",
             *callpath_counter,
             path );
    ( *callpath_counter )++;
}

/**
   Helper function for the profile writer in TAU snapshot format.
   Writes callpath definition for a regular region.
   @param node       Pointer to the current node. Can not be NULL because the
                     calling function would not be able to determine the type.
   @param parentpath String which contains the callpath for its parent node.
   @param file       Pointer to the file to which the data is written.
   @param callpath_counter counter of the encountered callpathes. The
                           counter is used to enumerate the callpathes and
                           serves as an unique id to map callpath definitions
                           to callpath values. The value of this variable gets
                           updated inside this function according to the
                           number of processed callpathes.
 */
static void
write_region_tau( scorep_profile_node* node,
                  char*                parentpath,
                  FILE*                file,
                  uint64_t*            callpath_counter )
{
    /* Construct callpath name */
    const char* name   = SCOREP_Region_GetName( scorep_profile_type_get_region_handle( node->type_specific_data ) );
    int         length = strlen( name ) + 1;
    if ( parentpath )
    {
        length += strlen( parentpath ) + 7;
    }
    char* path = SCOREP_Memory_AllocForProfile( length );
    if ( parentpath == NULL )
    {
        strcpy( path, name );
    }
    else
    {
        sprintf( path, "%s =&gt; %s", parentpath, name );
    }

    /* write definition */
    if ( SCOREP_Region_GetType( scorep_profile_type_get_region_handle( node->type_specific_data ) ) != SCOREP_REGION_DYNAMIC )
    {
        write_tausnap_def( path, file, callpath_counter );
    }

    if ( node->callpath_handle != SCOREP_INVALID_CALLPATH )
    {
        /* invoke children */
        scorep_profile_node* child = node->first_child;
        while ( child != NULL )
        {
            write_node_tau( child, path, file, callpath_counter );
            child = child->next_sibling;
        }
    }
}

/**
   Helper function for the profile writer in TAU snapshot format.
   Writes callpath definition for a parameter string node.
   @param node       Pointer to the current node.
   @param parentpath String which contains the callpath for its parent node.
   @param file       Pointer to the file to which the data is written.
   @param callpath_counter counter of the encountered callpathes. The
                           counter is used to enumerate the callpathes and
                           serves as an unique id to map callpath definitions
                           to callpath values. The value of this variable gets
                           updated inside this function according to the
                           number of processed callpathes.
 */
static void
write_paramstring_tau( scorep_profile_node* node,
                       char*                parentpath,
                       FILE*                file,
                       uint64_t*            callpath_counter )
{
    /* Construct callpath name */
    char*       path;
    const char* param_name  = SCOREP_Parameter_GetName( node->type_specific_data.handle );
    const char* param_value = SCOREP_String_Get( node->type_specific_data.value );
    if ( parentpath == NULL )
    {
        const char* parentname = SCOREP_Parameter_GetName( node->parent->type_specific_data.handle );
        parentpath = UTILS_CStr_dup( parentname );
    }
    /* Length is "<path> (<name> = <value>)" plus terminating '\0' */
    int length = strlen( parentpath ) + strlen( param_name ) + 24 + strlen( param_value ) + 1;
    path = ( char* )malloc( length );
// [ &lt;%s&gt; = &lt;%" PRIi64 "&gt; ]
    sprintf( path, "%s [ &lt;%s&gt; = &lt;%s&gt; ]", parentpath, param_name, param_value );

    /* write definition */
    write_tausnap_def( path, file, callpath_counter );

    if ( node->callpath_handle != SCOREP_INVALID_CALLPATH )
    {
        /* invoke children */
        scorep_profile_node* child = node->first_child;
        while ( child != NULL )
        {
            write_node_tau( child, path, file, callpath_counter );
            child = child->next_sibling;
        }
    }
}

/**
   Helper function for the profile writer in TAU snapshot format.
   Writes callpath definition for a parameter integer node.
   @param node       Pointer to the current node.
   @param parentpath String which contains the callpath for its parent node.
   @param file       Pointer to the file to which the data is written.
   @param callpath_counter counter of the encountered callpathes. The
                           counter is used to enumerate the callpathes and
                           serves as an unique id to map callpath definitions
                           to callpath values. The value of this variable gets
                           updated inside this function according to the
                           number of processed callpathes.
 */
static void
write_paramint_tau( scorep_profile_node* node,
                    char*                parentpath,
                    FILE*                file,
                    uint64_t*            callpath_counter )
{
    /* Construct callpath name */
    char*                  path;
    SCOREP_ParameterHandle param =
        scorep_profile_type_get_parameter_handle( node->type_specific_data );
    const char* param_name = SCOREP_Parameter_GetName( param );
    if ( parentpath == NULL )
    {
        const char* parentname = SCOREP_Parameter_GetName( node->parent->type_specific_data.handle );
        parentpath = UTILS_CStr_dup( parentname );
    }
    if ( param == scorep_profile_param_instance )
    {
        /* 12 digit max data length. */
        int length = strlen( parentpath ) + 12 + 3 + 1;
        path = ( char* )malloc( length );
        sprintf( path, "%s [%" PRIu64 "]", parentpath,
                 node->type_specific_data.value );
    }
    else
    {
        /* 12 digit max data length. */
        int length = strlen( parentpath ) +
                     strlen( SCOREP_Parameter_GetName( param ) ) +
                     21 + 6 + 1;
        SCOREP_ParameterType type = SCOREP_Parameter_GetType( param );
        path = ( char* )malloc( length );
//![CDATA[int f2(int) [{simpleTau.cpp} {5,1}-{10,1}]   [ <y> = <2> ]]]
        if ( type == SCOREP_PARAMETER_INT64 )
        {
            sprintf( path, "%s [ &lt;%s&gt; = &lt;%" PRIi64 "&gt; ]", parentpath,
                     param_name,
                     scorep_profile_type_get_int_value( node->type_specific_data ) );
        }
        else
        {
            sprintf( path, "%s [ &lt;%s&gt; = &lt;%" PRIi64 "&gt; ]", parentpath,
                     param_name,
                     scorep_profile_type_get_int_value( node->type_specific_data ) );
        }
    }
    /* write definition */
    write_tausnap_def( path, file, callpath_counter );

    /* invoke children */
    if ( node->callpath_handle != SCOREP_INVALID_CALLPATH )
    {
        scorep_profile_node* child = node->first_child;
        while ( child != NULL )
        {
            write_node_tau( child, path, file, callpath_counter );
            child = child->next_sibling;
        }
    }
}

/**
   Helper function for the profile writer in TAU snapshot format.
   Dependent on the node type, switches to the particular function
   for writing callpath definitions.
   @param node       Pointer to the current node.
   @param parentpath String which contains the callpath for its parent node.
   @param file       Pointer to the file to which the data is written.
   @param callpath_counter counter of the encountered callpathes. The
                           counter is used to enumerate the callpathes and
                           serves as an unique id to map callpath definitions
                           to callpath values. The value of this variable gets
                           updated inside this function according to the
                           number of processed callpathes.
 */
static void
write_node_tau( scorep_profile_node* node,
                char*                parentpath,
                FILE*                file,
                uint64_t*            callpath_counter )
{
    if ( node == NULL )
    {
        return;
    }
    switch ( node->node_type )
    {
        case scorep_profile_node_regular_region:
            write_region_tau( node, parentpath, file, callpath_counter );
            break;

        case scorep_profile_node_parameter_string:
            write_paramstring_tau( node, parentpath, file, callpath_counter );
            break;

        case scorep_profile_node_parameter_integer:
            write_paramint_tau( node, parentpath, file, callpath_counter );
            break;

        default:
            UTILS_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                         "Node type %d encountered in subtree during writing",
                         node->node_type );
    }
}


static uint64_t
get_metrics_sum_from_array( scorep_profile_node* node, int index )
{
    return node->dense_metrics[ index ].sum;
}

static uint64_t
get_metrics_start_from_array( scorep_profile_node* node, int index )
{
    return node->dense_metrics[ index ].start_value;
}

static void
write_metric_data_tau( scorep_profile_node*      node,
                       FILE*                     file,
                       SCOREP_DefinitionManager* manager )
{
    for ( int i = 0; i  < scorep_profile.num_of_dense_metrics; i++ )
    {
        fprintf( file,
                 " %" PRIu64 " %" PRIu64,
                 get_metrics_sum_from_array( node,  i ),
                 get_metrics_start_from_array( node, i ) );
    }
}

static void
write_metrics_tau( scorep_profile_node*      node,
                   FILE*                     file,
                   SCOREP_DefinitionManager* manager )
{
    SCOREP_Metric_Definition* metric_definition;
    char*                     metric_name;
    char*                     metric_unit;
    char*                     metric_description;
    int                       i = 1;

    SCOREP_DEFINITION_FOREACH_DO( manager, Metric, metric )
    {
        /* Only want to write dense metrics here*/
        if ( definition->source_type == SCOREP_METRIC_SOURCE_TYPE_PAPI )
        {
            metric_name = SCOREP_UNIFIED_HANDLE_DEREF( definition->name_handle,
                                                       String )->string_data;
            metric_unit = SCOREP_UNIFIED_HANDLE_DEREF( definition->unit_handle,
                                                       String )->string_data;
            metric_description = SCOREP_UNIFIED_HANDLE_DEREF( definition->description_handle,
                                                              String )->string_data;

            fprintf( file, "<metric id=\"%d\"><name>%s</name>\n", i, metric_name );
            fprintf( file, "<units>%s</units>\n", metric_unit );
            fprintf( file, "</metric>\n" );
            i++;
        }
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}


/**
   Helper function for the profile writer in TAU snapshot format.
   Writes the metric data for the runtime of a node and process its children
   recursively.
   @param node Pointer to the node which time data is processed.
   @param file A pointer to an open file to which the data is written.
   @param callpath_counter counter of the encountered callpathes. The
                           counter is used to enumerate the callpathes and
                           serves as an unique id to map callpath definitions
                           to callpath values. The value of this variable gets
                           updated inside this function according to the
                           number of processed callpathes.
 */
static void
write_data_tau( scorep_profile_node*      node,
                FILE*                     file,
                uint64_t*                 callpath_counter,
                SCOREP_DefinitionManager* manager )

{
    uint64_t tps = SCOREP_GetClockResolution();

    /* Write data in format:
       <callpath id> <number of calls> <child calls> <exclusive time> <inclusive time>
     */
    if ( node->node_type != scorep_profile_node_regular_region || SCOREP_Region_GetType( scorep_profile_type_get_region_handle( node->type_specific_data ) ) != SCOREP_REGION_DYNAMIC )
    {
        fprintf( file,
                 "%" PRIu64 " %" PRIu64 " %" PRIu64 " %" PRIu64 " %" PRIu64,
                 *callpath_counter, node->count,
                 scorep_profile_get_number_of_child_calls( node ),
                 ( scorep_profile_get_exclusive_time( node ) * 1000000 / tps ),
                 ( node->inclusive_time.sum * 1000000 / tps ) );
        write_metric_data_tau( node, file, manager );
        fprintf( file, "\n" );
        ( *callpath_counter )++;
    }
    if ( node->callpath_handle != SCOREP_INVALID_CALLPATH )
    {
        /* invoke children */
        scorep_profile_node* child = node->first_child;
        while ( child != NULL )
        {
            write_data_tau( child, file, callpath_counter, manager );
            child = child->next_sibling;
        }
    }
}
struct list_el
{
    int             val;
    int             index;
    struct list_el* next;
};


typedef struct list_el item;
item* curr, * head, * tail;
/**
   Helper function for the profile writer in TAU snapshot format.
   It writes the user event names and eventID  for a thread to a given file.
   @param node      Pointer to the root node of the thread.
   @param threadnum Number of the thread on this node. The number must be
                    unique on each node.
   @param file      A pointer to an open file to which the data is written.
 */

static void
write_atomicdata_tau( scorep_profile_node*      node,
                      FILE*                     file,
                      uint64_t*                 callpath_counter,
                      SCOREP_DefinitionManager* manager )
{
    scorep_profile_sparse_metric_double* metric = node->first_double_sparse;

    while ( metric != NULL )
    {
/* Write data in format:
   <EventID> <NumSamples> <MaxValue> <MinValue> <MeanValue> <Sum of Squares>
 */
        int eventID = -1;

        curr = head;

        while ( curr != NULL )
        {
            if ( curr->val == ( int )metric->metric )
            {
                eventID = curr->index;
            }
            curr = curr->next;
        }

        fprintf( file, "%d %" PRIu64 " %.16G %.16G %.16G %.16G\n",
                 eventID, metric->count, metric->max, metric->min,
                 metric->sum / metric->count, metric->squares );
        metric = metric->next_metric;
    }

    if ( node->callpath_handle != SCOREP_INVALID_CALLPATH )
    {
        /* invoke children */
        scorep_profile_node* child = node->first_child;
        while ( child != NULL )
        {
            write_atomicdata_tau( child, file, callpath_counter, manager );
            child = child->next_sibling;
        }
    }
}
/**
   Helper function for the profile writer in TAU snapshot format.
   It writes the atomic event data for a thread to a given file.
   @param node      Pointer to the root node of the thread.
   @param file      A pointer to an open file to which the data is written.
 */

static void
write_userevent_data_metric_tau( scorep_profile_node*      node,
                                 char*                     parentpath,
                                 FILE*                     file,
                                 SCOREP_DefinitionManager* manager )

{
    scorep_profile_sparse_metric_double* metric = node->first_double_sparse;
    while ( metric != NULL )
    {
        curr = head;
        int eventID = -1;
        while ( curr != NULL )
        {
            if ( ( int )metric->metric == curr->val )
            {
                eventID = curr->index;
                curr    = NULL;
            }
            if ( curr != NULL )
            {
                curr = curr->next;
            }
        }
        if ( eventID == -1 )
        {
            if ( tail == NULL )
            {
                eventID = 0;
            }
            else
            {
                eventID = tail->index + 1;
            }
            item* new = ( item* )malloc( sizeof( item ) );
            new->val   = metric->metric;
            new->index = eventID;
            new->next  = NULL;


            if ( head == NULL )
            {
                head = new;
            }
            if ( tail != NULL )
            {
                tail->next = new;
            }
            tail = new;


            SCOREP_Metric_Definition* metric_definition;
            char*                     metric_name;
            metric_definition = SCOREP_LOCAL_HANDLE_DEREF( metric->metric, Metric );
            metric_name       = SCOREP_UNIFIED_HANDLE_DEREF( metric_definition->name_handle,
                                                             String )->string_data;
//In the future context events should somehow be marked
            char* found = strchr( metric_name, ':' );
            if ( found != NULL )
            {
                int   length     = strlen( metric_name ) + 1 + strlen( parentpath ) + 2;
                char* metricpath = SCOREP_Memory_AllocForProfile( length );
                sprintf( metricpath, "%s %s", metric_name, parentpath );
                metric_name = metricpath;
            }
            fprintf( file, "<userevent id=\"%d\"><name>%s</name>", eventID, metric_name );
            fprintf( file, "</userevent>\n" );
        }
        metric = metric->next_metric;
    }

    if ( node->callpath_handle != SCOREP_INVALID_CALLPATH )
    {
        /* invoke children */
        scorep_profile_node* child = node->first_child;
        if ( child != NULL )
        {
            /* Construct callpath name */
            const char* name   = SCOREP_Region_GetName( scorep_profile_type_get_region_handle( child->type_specific_data ) );
            int         length = strlen( name ) + 1;
            if ( parentpath )
            {
                length += strlen( parentpath ) + 7;
            }
            char* path = SCOREP_Memory_AllocForProfile( length );
            if ( parentpath == NULL )
            {
                strcpy( path, name );
            }
            else
            {
                sprintf( path, "%s =&gt; %s", parentpath, name );
            }
            parentpath = path;
        }
        while ( child != NULL )
        {
            write_userevent_data_metric_tau( child, parentpath, file,  manager );
            child = child->next_sibling;
        }
    }
}
/**
   Helper function for the profile writer in TAU snapshot format.
   It calls the function to write out the user events.
   @param node      Pointer to the root node of the thread.
   @param threadnum Number of the thread on this node. The number must be
                    unique on each node.
   @param file      A pointer to an open file to which the data is written.
 */

static void
write_userevent_data_tau( scorep_profile_node*      child,
                          uint64_t                  threadnum,
                          FILE*                     file,
                          SCOREP_DefinitionManager* manager )
{
    head = NULL;
    tail = NULL;
    char* parentpath = NULL;
    /*Write User Events Definitions*/
    scorep_profile_node* node = child;
    while ( node != NULL )
    {
        while ( child != NULL )
        {
            /* Construct callpath name */
            const char* name   = SCOREP_Region_GetName( scorep_profile_type_get_region_handle( node->type_specific_data ) );
            int         length = strlen( name ) + 1;
            if ( parentpath )
            {
                length += strlen( parentpath ) + 7;
            }
            char* path = SCOREP_Memory_AllocForProfile( length );
            if ( parentpath == NULL )
            {
                strcpy( path, name );
            }
            else
            {
                sprintf( path, "%s =&gt; %s", parentpath, name );
            }


            parentpath = path;
            write_userevent_data_metric_tau( child, path, file, manager );
            child = child->next_sibling;
        }
        child = node->first_child;
        node  = node->first_child;
    }
}



/**
   Helper function for the profile writer in TAU snapshot format.
   It writes the metadata for a thread to a given file and process the regions
   recursively.
   @param node      Pointer to the root node of the thread.
   @param threadnum Number of the thread on this node. The number must be
                    unique on each node.
   @param file      A pointer to an open file to which the data is written.
 */
static void
write_thread_tau( scorep_profile_node*      node,
                  uint64_t                  threadnum,
                  FILE*                     file,
                  SCOREP_DefinitionManager* manager )
{
    /* The counter is used to enumerate the callpathes and
       serves as an unique id to map callpath definitions
       to callpath values.*/
    uint64_t callpath_counter = 0;

    UTILS_ASSERT( node != NULL );

    /* Write thread definition */
    fprintf( file,
             "<thread id=\"%d.0.%" PRIu64 ".0\" node=\"%d"
             "\" context=\"0\" thread=\"%" PRIu64 "\">\n",
             SCOREP_Mpi_GetRank(), threadnum, SCOREP_Mpi_GetRank(), threadnum );
    fprintf( file, "</thread>\n\n" );
    fprintf( file, "<definitions thread=\"%d.0.%" PRIu64 ".0\">\n",
             SCOREP_Mpi_GetRank(), threadnum );
    fprintf( file, "<metric id=\"0\"><name>TIME</name>\n" );
    fprintf( file, "<units>ms</units>\n" );
    fprintf( file, "</metric>\n" );
    write_metrics_tau( node, file, manager );
    fprintf( file, "</definitions>\n\n" );


    scorep_profile_node* child = node->first_child;

    fprintf( file, "<definitions thread=\"%d.0.%" PRIu64 ".0\">\n",
             SCOREP_Mpi_GetRank(), threadnum );
    write_userevent_data_tau( child, threadnum, file,  manager );

    fprintf( file, "</definitions>\n\n" );

    /* Write callpath definition */
    fprintf( file, "<definitions thread=\"%d.0.%" PRIu64 ".0\">\n",
             SCOREP_Mpi_GetRank(), threadnum );
    callpath_counter = 0;

    while ( child != NULL )
    {
        write_node_tau( child, NULL, file, &callpath_counter );
        child = child->next_sibling;
    }
    fprintf( file, "</definitions>\n\n" );


    /* Write metrics data */
    fprintf( file, "<profile thread=\"%d.0.%" PRIu64 ".0\">\n",
             SCOREP_Mpi_GetRank(), threadnum );
    fprintf( file, "<name>final</name>\n" );
    fprintf( file, "<interval_data metrics=\"0" );
    for ( int i = 0; i < scorep_profile.num_of_dense_metrics; i++ )
    {
        fprintf( file, " %d", i + 1 );
    }
    fprintf( file, "\">\n" );
    child            = node->first_child;
    callpath_counter = 0;
    while ( child != NULL )
    {
        write_data_tau( child, file, &callpath_counter, manager );
        child = child->next_sibling;
    }
    fprintf( file, "</interval_data>\n" );
/*Write Atomic Data*/
    fprintf( file, "<atomic_data>\n" );
    child            = node->first_child;
    callpath_counter = 0;
    if ( child->next_sibling != NULL )
    {
        child = child->next_sibling;
    }

    while ( child != NULL )
    {
        write_atomicdata_tau( child, file, &callpath_counter, manager );
        child = child->next_sibling;
    }
    fprintf( file, "</atomic_data>\n" );

    fprintf( file, "</profile>\n\n" );
}

/**
   Adds nodes to the callpath, as siblings of the root, to represent the nodes of
   the callgraph. It also adds "dummy" nodes as children to these nodes.
   The dummy nodes are used to correctly calculate
   exclusive time and child counts. This function is a processing function
   for a scorep_profile_for_all call.
   @param node  Pointer to the current profile node.
   @param param Pointer to the list of added nodes.
 */
static void
write_tau_merge_callpath_nodes( scorep_profile_node* node,
                                void*                param )
{
    scorep_profile_node*         parent   = ( scorep_profile_node* )param;
    scorep_profile_node*         list     = parent;
    SCOREP_Profile_LocationData* location =
        scorep_profile_type_get_location_data( scorep_profile.first_root_node->type_specific_data );

    bool root = scorep_profile_compare_nodes( parent, node );
    //We are at the root of the call graph so there is no need to merge any nodes
    if ( root )
    {
        return;
    }
    bool same = false;

    while ( list != NULL )
    {
        same = scorep_profile_compare_nodes( list, node );

        if ( same )
        {
            //If the node is found in the list, break and merge the nodes
            break;
        }
        if ( list->next_sibling == NULL )
        {
            //if we reached the end of the list without finding the node, add a new one
            scorep_profile_node* copy = scorep_profile_copy_node( location, node );
            copy->next_sibling = NULL;
            list->next_sibling = copy;
            //Need the parent to properly name parameter regions.
            copy->parent = node->parent;

            //need to collect exclusive time and child calls so that the information can be printed correctly
            scorep_profile_node* dummy = scorep_profile_copy_node( location, copy );
            dummy->inclusive_time.sum = copy->inclusive_time.sum  - scorep_profile_get_exclusive_time( node );
            dummy->count              = scorep_profile_get_number_of_child_calls( node );
            dummy->next_sibling       = NULL;
            copy->first_child         = dummy;
            dummy->callpath_handle    = SCOREP_INVALID_CALLPATH;
            break;
        }
        else
        {
            list = list->next_sibling;
        }
    }
    if ( same )
    {
        //need to add exclusive time and child calls to the dummy node
        scorep_profile_node* dummy = list->first_child;
        dummy->inclusive_time.sum += node->inclusive_time.sum  - scorep_profile_get_exclusive_time( node );
        dummy->count              += scorep_profile_get_number_of_child_calls( node );

        //merge the metrics
        scorep_profile_merge_node_dense( list, node );
        scorep_profile_merge_node_sparse( location, list, node );
    }
}

/* Adds nodes to the callpath for the TAU snapshot profile.
 * @param node Pointer to the root of the callpath.
 */
static void
write_tau_add_callpath_nodes( scorep_profile_node* parent )
{
    scorep_profile_node* node = parent->first_child;

    //Have to count the siblings because the merged callpath nodes are added as siblings
    //and we don't want to merge the merged callpathn nodes
    //but we do want to merge the phases
    int sibling_count = 0;
    while ( node->next_sibling != NULL )
    {
        sibling_count++;
        node = node->next_sibling;
    }


    node = parent->first_child;
    scorep_profile_for_all( node,
                            write_tau_merge_callpath_nodes,
                            node );
    while ( sibling_count > 0 )
    {
        //if there are phases they will be here so merge them
        node = node->next_sibling;
        scorep_profile_for_all( node,
                                write_tau_merge_callpath_nodes,
                                node );
        sibling_count--;
    }
}

/* rImplemetatio  of the top function for writing a TAU snapshot profile.
 */
void
scorep_profile_write_tau_snapshot()
{
    uint64_t             threadnum = 0;
    scorep_profile_node* thread    = scorep_profile.first_root_node;

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE, "Write profile in TAU snapshot format" );
    FILE* file;
    char  filename[ 600 ];
    char  dirname[ 500 ];

    /* The unification is always processed, even in serial case. Thus, we have
        always access to the unified definitions on rank 0.
        In non-mpi case SCOREP_Mpi_GetRank() returns always 0. Thus, we need only
        to test for the rank. */
    SCOREP_DefinitionManager* manager = scorep_unified_definition_manager;
    if ( SCOREP_Mpi_GetRank() == 0 )
    {
        assert( scorep_unified_definition_manager );
    }

    /* Create sub directory 'tau' */
    sprintf( dirname, "%s/tau", SCOREP_GetExperimentDirName() );
    if ( mkdir( dirname, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH ) != 0 )
    {
        if ( errno != EEXIST )
        {
            UTILS_ERROR_POSIX( "Unable to create directory for snapshot profile" );
            return;
        }
    }

    /* Open file */
    sprintf( filename, "%s/snapshot.%d.0.0", dirname, SCOREP_Mpi_GetRank() );
    file = fopen( filename, "w" );
    if ( !file )
    {
        UTILS_ERROR_POSIX( "Failed to write profile. Unable to open file" );
        return;
    }
    /*Add the summary nodes to the calltree*/
    scorep_profile_node* root = scorep_profile.first_root_node;
    write_tau_add_callpath_nodes( root );

    /* Write starting tag */
    fprintf( file, "<profile_xml>\n" );

    /* Write thread data */
    while ( thread != NULL )
    {
        write_thread_tau( thread, threadnum, file, manager );
        thread = thread->next_sibling;
        threadnum++;
    }

    /* Write closing tag and close file */
    fprintf( file, "</profile_xml>\n" );

    fclose( file );
}
