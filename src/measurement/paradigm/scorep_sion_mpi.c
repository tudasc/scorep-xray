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
 * @file       src/measurement/paradigm/scorep_sion_mpi.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include <config.h>


#include <stdio.h>
#include <assert.h>


#include <mpi.h>
#include <sion.h>


#include <otf2/otf2.h>


#include <scorep_environment.h>
#include <scorep_mpi.h>
#include <tracing/SCOREP_Tracing.h>
#include <tracing/scorep_tracing_internal.h>
#include <SCOREP_RuntimeManagement.h>

static int
scorep_sion_open( void*          userData,
                  OTF2_FileType  fileType,
                  uint64_t       locationId,
                  const char*    fileName,
                  const char*    fileMode,
                  long long int* chunkSize,
                  int*           fileSystemBlockSize,
                  FILE**         filePointer )
{
    assert( scorep_tracing_nlocations_per_sion_file > 0 );
    int         number_of_sion_files = SCOREP_Mpi_GetCommWorldSize() / scorep_tracing_nlocations_per_sion_file;
    const char* file_mode            = "bw";
    char*       new_file_name        = NULL;
    MPI_Comm    comm_world           = MPI_COMM_WORLD;
    int         global_rank          = SCOREP_Mpi_GetRank();
    SCOREP_MPI_EVENT_GEN_OFF();

    #if defined( __bgp__ )
    /* MPIX_Pset_same_comm_create creates a communicator such that all
       nodes in the same communicator are served by the same I/O node. For
       each I/O node we will create one sion file that comprises the files
       of all tasks of this communicator. To activate this, set
       number_of_sion_files <= 0. */
    MPI_Comm commSame;
    MPIX_Pset_same_comm_create( &commSame );
    number_of_sion_files = 0;
    #endif

    PMPI_Barrier( comm_world );
    int sion_file_handle = sion_paropen_mpi( ( char* )fileName,
                                             file_mode,
                                             &number_of_sion_files,
                                             comm_world,
                                             #if defined( __bgp__ )
                                             &commSame,
                                             #else
                                             &comm_world,
                                             #endif
                                             chunkSize,
                                             fileSystemBlockSize,
                                             &global_rank,
                                             filePointer,
                                             &new_file_name );
    PMPI_Barrier( comm_world );

    SCOREP_MPI_EVENT_GEN_ON();
    assert( sion_file_handle != -1 );
    return sion_file_handle;
}


static int
scorep_sion_close( void*         userData,
                   OTF2_FileType fileType,
                   uint64_t      locationId,
                   int           sionFileHandle )
{
    SCOREP_MPI_EVENT_GEN_OFF();
    int close_status = sion_parclose_mpi( sionFileHandle );
    SCOREP_MPI_EVENT_GEN_ON();
    assert( close_status == 1 );
    return close_status;
}


static OTF2_FileSionCallbacks scorep_sion_callbacks =
{
    .otf2_file_sion_open  = scorep_sion_open,
    .otf2_file_sion_close = scorep_sion_close
};


void
scorep_tracing_register_sion_callbacks( OTF2_Archive* archive )
{
    assert( archive );

    if ( scorep_tracing_use_sion )
    {
        SCOREP_Error_Code status = OTF2_Archive_SetFileSionCallbacks(
            archive,
            &scorep_sion_callbacks,
            NULL );
        assert( status == SCOREP_SUCCESS );
    }
}


OTF2_FileSubstrate
scorep_tracing_get_file_substrate()
{
    OTF2_FileSubstrate substrate = OTF2_SUBSTRATE_POSIX;
    if ( scorep_tracing_use_sion )
    {
        substrate = OTF2_SUBSTRATE_SION;
    }
    return substrate;
}
