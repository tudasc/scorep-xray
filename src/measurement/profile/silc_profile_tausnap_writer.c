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

/**
 * @file silc_profile_tausnap_writer.c
 *
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @status ALPHA
 * Implements a profile writer in TAU snapshot format.
 */

#include <stdint.h>
#include <inttypes.h>
#include <sys/stat.h>

#include "SILC_Memory.h"
#include "SILC_Utils.h"
#include "SILC_Definitions.h"
#include "SILC_Timing.h"

#include "silc_profile_definition.h"
#include "silc_definitions.h"
#include "silc_mpi.h"
#include "silc_runtime_management.h"

/* Forward declaration */
static void
silc_profile_write_node_tau( silc_profile_node* node,
                             char*              parentpath,
                             FILE*              file,
                             uint64_t*          callpath_counter );

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
silc_profile_write_tausnap_def( char*     path,
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
silc_profile_write_region_tau( silc_profile_node* node,
                               char*              parentpath,
                               FILE*              file,
                               uint64_t*          callpath_counter )
{
    /* Construct callpath name */
    const char* name   = SILC_Region_GetName( SILC_PROFILE_DATA2REGION( node->type_specific_data ) );
    int         length = strlen( name ) + 2;
    if ( parentpath )
    {
        length += strlen( parentpath );
    }
    char* path = SILC_Memory_AllocForProfile( length );
    if ( parentpath == NULL )
    {
        strcpy( path, name );
    }
    else
    {
        sprintf( path, "%s =&gt; %s", parentpath, name );
    }

    /* write definition */
    silc_profile_write_tausnap_def( path, file, callpath_counter );

    /* invoke children */
    silc_profile_node* child = node->first_child;
    while ( child != NULL )
    {
        silc_profile_write_node_tau( child, path, file, callpath_counter );
        child = child->next_sibling;
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
silc_profile_write_paramstring_tau( silc_profile_node* node,
                                    char*              parentpath,
                                    FILE*              file,
                                    uint64_t*          callpath_counter )
{
    /** TODO: implement paramter string definition writing */
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
silc_profile_write_paramint_tau( silc_profile_node* node,
                                 char*              parentpath,
                                 FILE*              file,
                                 uint64_t*          callpath_counter )
{
    /** TODO: implement paramter integer definition writing */
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
silc_profile_write_node_tau( silc_profile_node* node,
                             char*              parentpath,
                             FILE*              file,
                             uint64_t*          callpath_counter )
{
    if ( node == NULL )
    {
        return;
    }

    switch ( node->node_type )
    {
        case silc_profile_node_regular_region:
            silc_profile_write_region_tau( node, parentpath, file, callpath_counter );
            break;

        case silc_profile_node_parameter_string:
            silc_profile_write_paramstring_tau( node, parentpath, file, callpath_counter );
            break;

        case silc_profile_node_parameter_integer:
            silc_profile_write_paramint_tau( node, parentpath, file, callpath_counter );
            break;

        default:
            SILC_ERROR( SILC_ERROR_PROFILE_INCONSISTENT,
                        "Node type %d encountered in subtree during writing",
                        node->node_type );
    }
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
silc_profile_write_data_tau( silc_profile_node* node,
                             FILE*              file,
                             uint64_t*          callpath_counter )
{
    uint64_t tps = SILC_GetClockResolution();

    /* Write data in format:
       <callpath id> <number of calls> <child calls> <exclusive time> <inclusive time>
     */
    fprintf( file,
             "%" PRIu64 " %" PRIu64 " %" PRIu64 " %" PRIu64 " %" PRIu64 "\n",
             *callpath_counter, node->count,
             silc_profile_get_number_of_child_calls( node ),
             ( silc_profile_get_exclusive_time( node ) * 1000000llu / tps ),
             ( node->inclusive_time.sum * 1000000llu / tps ) );
    ( *callpath_counter )++;

    /* invoke children */
    silc_profile_node* child = node->first_child;
    while ( child != NULL )
    {
        silc_profile_write_data_tau( child, file, callpath_counter );
        child = child->next_sibling;
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
silc_profile_write_thread_tau( silc_profile_node* node,
                               uint64_t           threadnum,
                               FILE*              file )
{
    /* The counter is used to enumerate the callpathes and
       serves as an unique id to map callpath definitions
       to callpath values.*/
    uint64_t callpath_counter = 0;

    SILC_ASSERT( node != NULL );

    /* Write thread definition */
    fprintf( file,
             "<thread id=\"%d.0.%" PRIu64 ".0\" node=\"%d"
             "\" context=\"0\" thread=\"%" PRIu64 "\">\n",
             SILC_Mpi_GetRank(), threadnum, SILC_Mpi_GetRank(), threadnum );
    fprintf( file, "</thread>\n\n" );
    fprintf( file, "<definitions thread=\"%d.0.%" PRIu64 ".0\">\n",
             SILC_Mpi_GetRank(), threadnum );
    fprintf( file, "<metric id=\"0\"><name>TIME</name>\n" );
    fprintf( file, "<units>ms</units>\n" );
    fprintf( file, "</metric>\n" );
    fprintf( file, "</definitions>\n\n" );

    /* Write callpath definition */
    fprintf( file, "<definitions thread=\"%d.0.%" PRIu64 ".0\">\n",
             SILC_Mpi_GetRank(), threadnum );
    silc_profile_node* child = node->first_child;
    callpath_counter = 0;
    while ( child != NULL )
    {
        silc_profile_write_node_tau( child, NULL, file, &callpath_counter );
        child = child->next_sibling;
    }
    fprintf( file, "</definitions>\n\n" );

    /* Write metrics data */
    fprintf( file, "<profile thread=\"%d.0.%" PRIu64 ".0\">\n",
             SILC_Mpi_GetRank(), threadnum );
    fprintf( file, "<name>final</name>\n" );
    fprintf( file, "<interval_data metrics=\"0\">\n" );
    child            = node->first_child;
    callpath_counter = 0;
    while ( child != NULL )
    {
        silc_profile_write_data_tau( child, file, &callpath_counter );
        child = child->next_sibling;
    }
    fprintf( file, "</interval_data>\n" );
    fprintf( file, "</profile>\n\n" );
}

/* Implemetation of the top function for writing a TAU snapshot profile.
 */
void
silc_profile_write_tau_snapshot()
{
    uint64_t           threadnum = 0;
    silc_profile_node* thread    = silc_profile.first_root_node;

    SILC_DEBUG_PRINTF( SILC_DEBUG_PROFILE, "Write profile in TAU snapshot format" );
    FILE* file;
    char  filename[ 600 ];
    char  dirname[ 500 ];

    /* Create sub directory 'tau' */
    sprintf( dirname, "%s/tau", SILC_GetExperimentDirName() );
    if ( mkdir( dirname, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH ) != 0 )
    {
        if ( errno != EEXIST )
        {
            SILC_ERROR_POSIX( "Unable to create directory for snapshot profile" );
            return;
        }
    }

    /* Open file */
    sprintf( filename, "%s/snapshot.%d.0.0", dirname, SILC_Mpi_GetRank() );
    file = fopen( filename, "w" );
    if ( !file )
    {
        SILC_ERROR_POSIX( "Failed to write profile. Unable to open file" );
        return;
    }

    /* Write starting tag */
    fprintf( file, "<profile_xml>\n" );

    /* Write thread data */
    while ( thread != NULL )
    {
        silc_profile_write_thread_tau( thread, threadnum, file );
        thread = thread->next_sibling;
        threadnum++;
    }

    /* Write closing tag and close file */
    fprintf( file, "</profile_xml>\n" );
    fclose( file );
}
