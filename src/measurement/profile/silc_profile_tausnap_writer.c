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

#include "SILC_Memory.h"
#include "SILC_Utils.h"
#include "SILC_Definitions.h"
#include "SILC_Timing.h"

#include "silc_profile_definition.h"
#include "silc_definitions.h"
#include "silc_mpi.h"

static uint64_t callpath_counter = 0;

static void
silc_profile_write_node_tau( silc_profile_node* node,
                             char*              parentpath,
                             FILE*              file );

static void
silc_profile_write_tausnap_def( char* path,
                                FILE* file )
{
    fprintf( file,
             "<event id=\"%d\"><name>%s</name></event>\n",
             callpath_counter,
             path );
    callpath_counter++;
}

/**
   Writes a callpath definition for a region in TAU snapshot format.
   node cannot be NULL because the calling function would not be able to
   determine the type.
 */
static void
silc_profile_write_region_tau( silc_profile_node* node,
                               char*              parentpath,
                               FILE*              file )
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
        sprintf( path, "%s/%s", parentpath, name );
    }

    /* write definition */
    silc_profile_write_tausnap_def( path, file );

    /* invoke children */
    silc_profile_node* child = node->first_child;
    while ( child != NULL )
    {
        silc_profile_write_node_tau( child, path, file );
        child = child->next_sibling;
    }
}

static void
silc_profile_write_paramstring_tau( silc_profile_node* node,
                                    char*              parentpath,
                                    FILE*              file )
{
}

static void
silc_profile_write_paramint_tau( silc_profile_node* node,
                                 char*              parentpath,
                                 FILE*              file )
{
}

static void
silc_profile_write_node_tau( silc_profile_node* node,
                             char*              parentpath,
                             FILE*              file )
{
    if ( node == NULL )
    {
        return;
    }

    switch ( node->node_type )
    {
        case silc_profile_node_regular_region:
            silc_profile_write_region_tau( node, parentpath, file );
            break;

        case silc_profile_node_parameter_string:
            silc_profile_write_paramstring_tau( node, parentpath, file );
            break;

        case silc_profile_node_parameter_integer:
            silc_profile_write_paramint_tau( node, parentpath, file );
            break;

        default:
            SILC_ERROR( SILC_ERROR_PROFILE_INCONSISTENT,
                        "Node type %d encountered in subtree during writing",
                        node->node_type );
    }
}

static void
silc_profile_write_data_tau( silc_profile_node* node,
                             FILE*              file )
{
    uint64_t tps = SILC_GetClockResolution();

    fprintf( file, "%d %d %d %llu %llu\n", callpath_counter, node->count,
             silc_profile_get_number_of_child_calls( node ),
             ( silc_profile_get_exclusive_time( node ) * 1000000llu / tps ),
             ( node->inclusive_time.sum * 1000000llu / tps ) );
    callpath_counter++;

    /* invoke children */
    silc_profile_node* child = node->first_child;
    while ( child != NULL )
    {
        silc_profile_write_data_tau( child, file );
        child = child->next_sibling;
    }
}

static void
silc_profile_write_thread_tau( silc_profile_node* node,
                               int32_t            threadnum,
                               FILE*              file )
{
    SILC_ASSERT( node != NULL );

    /* Write thread definition */
    fprintf( file,
             "<thread id=\"%d.0.%d.0\" node=\"%d\" context=\"0\" thread=\"%d\">\n",
             SILC_Mpi_GetRank(), threadnum, SILC_Mpi_GetRank(), threadnum );
    fprintf( file, "</thread>\n\n" );
    fprintf( file, "<definitions thread=\"%d.0.%d.0\">\n", SILC_Mpi_GetRank(),
             threadnum );
    fprintf( file, "<metric id=\"0\"><name>TIME</name>\n" );
    fprintf( file, "<units>ms</units>\n" );
    fprintf( file, "</metric>\n" );
    fprintf( file, "</definitions>\n\n" );

    /* Write callpath definition */
    fprintf( file, "<definitions thread=\"%d.0.%d.0\">\n", SILC_Mpi_GetRank(),
             threadnum );
    silc_profile_node* child = node->first_child;
    callpath_counter = 0;
    while ( child != NULL )
    {
        silc_profile_write_node_tau( child, NULL, file );
        child = child->next_sibling;
    }
    fprintf( file, "</definitions>\n\n" );

    /* Write metrics data */
    fprintf( file, "<profile thread=\"%d.0.%d.0\">\n", SILC_Mpi_GetRank(),
             threadnum );
    fprintf( file, "<name>final</name>\n" );
    fprintf( file, "<interval_data metrics=\"0\">\n" );
    child            = node->first_child;
    callpath_counter = 0;
    while ( child != NULL )
    {
        silc_profile_write_data_tau( child, file );
        child = child->next_sibling;
    }
    fprintf( file, "</interval_data>\n" );
    fprintf( file, "</profile>\n\n" );
}

void
silc_profile_write_tau_snapshot()
{
    int32_t            threadnum = 0;
    silc_profile_node* thread    = silc_profile.first_root_node;

    SILC_DEBUG_PRINTF( SILC_DEBUG_PROFILE, "Write profile in TAU snapshot format" );
    /* Open file */
    FILE* file;
    char  filename[ 500 ];
    sprintf( filename, "%s.%d.0.0.0", silc_profile_basename, SILC_Mpi_GetRank() );
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
