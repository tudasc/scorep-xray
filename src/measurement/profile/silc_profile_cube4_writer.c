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
 * @file silc_profile_cube4_writer.c
 *
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @status ALPHA
 * Implements a profile writer in Cube 4 format.
 */

#include <sys/stat.h>

#include "SILC_Memory.h"
#include "SILC_Utils.h"
#include "SILC_Definitions.h"

#include "silc_profile_definition.h"
#include "silc_definition_cube4.h"
#include "silc_mpi.h"
#include "silc_runtime_management.h"

#include "cube.h"
#include "cubew.h"
#include "services.h"

void
silc_profile_write_data_to_cube4( cube_t*                     my_cube,
                                  silc_cube4_definitions_map* map )
{
}

void
silc_profile_write_cube4()
{
    uint64_t                    number_of_writers = 1; /* Initially one writer, in MPI case it can
                                                          be more */
    uint64_t                    number_of_threads = silc_profile_get_number_of_threads();
    uint64_t                    my_rank           = SILC_Mpi_GetRank();
    cube_t*                     my_cube           = NULL; /* The cube structure */
    CubeWriter*                 cube_writer       = NULL; /* The cube writer */
    char*                       filename          = NULL; /* Contains the basename for the cube file */
    char*                       dirname           = SILC_GetExperimentDirName();
    silc_cube4_definitions_map* map               = NULL;

    SILC_DEBUG_PRINTF( SILC_DEBUG_PROFILE,
                       "Writing profile in Cube 4 format ..." );

    /* Create definition mapping tables */
    map = silc_cube4_create_definitions_map();
    if ( map == NULL )
    {
        SILC_ERROR( SILC_ERROR_MEM_ALLOC_FAILED,
                    "Failed to allocat ememory for defintion mapping\n"
                    "Failed to write Cube 4 profile" );
        return;
    }

    /* Allocate memory for full filename */
    filename = ( char* )malloc( strlen( dirname ) +             /* Directory     */
                                6 +                             /* adding /cube/ */
                                strlen( silc_profile_basename ) /* basename      */
                                + 1 );                          /* trailing '0'  */
    if ( filename == NULL )
    {
        SILC_ERROR_POSIX( "Failed to allocate memory for filename.\n"
                          "Failed to write Cube 4 profile" );
        goto cleanup;
    }

    /* Create sub directory 'cube' */
    sprintf( filename, "%s/cube", dirname );
    if ( mkdir( filename, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH ) != 0 )
    {
        if ( errno != EEXIST )
        {
            SILC_ERROR_POSIX( "Unable to create directory for snapshot profile\n"
                              "Failed to write Cube 4 profile" );
            goto cleanup;
        }
    }

    /* Create full filename */
    sprintf( filename, "%s/%s", filename, silc_profile_basename );

    /* Create writer object. FALSE -> no zlib compression */
    cube_writer
        = cubew_create( my_rank,           /* rank of this node */
                        number_of_threads, /* sum of all threads of all nodes */
                        number_of_writers, /* number of parallel writers */
                        filename,          /* base file name */
                        FALSE );           /* zlib compression */

    /* Retrieve the cube object to which all data is written */
    my_cube = cubew_get_cube( cube_writer );

    /* generate header */
    cube_def_mirror( my_cube, "http://icl.cs.utk.edu/software/kojak/" );
    cube_def_mirror( my_cube, "http://www.fz-juelich.de/jsc/kojak/" );
    cube_def_attr( my_cube, "description", "need a description" );
    cube_def_attr( my_cube, "experiment time", "need a date" );

    /* Write definitions to cube */
    silc_write_definitions_to_cube4( my_cube, map );

    /* Write data to cube */
    silc_profile_write_data_to_cube4( my_cube, map );
    cubew_finalize( cube_writer );

    /* Clean up */
cleanup:
    if ( map )
    {
        silc_cube4_delete_definitions_map( map );
    }
    if ( filename )
    {
        free( filename );
    }
}
