/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
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
 * @file       scorep_sion_mpi.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include <config.h>
#include <scorep_sion.h>
#include <stdio.h>
#include <sion.h>
#include <otf2/OTF2_File_Sion_Ext.h>
#include <assert.h>
#include <scorep_environment.h>
#include <mpi.h>


/* *INDENT-OFF* */
static int scorep_sion_open(const char*, const char*, const long long int, int, FILE**);
static int scorep_sion_close(int);
/* *INDENT-ON*  */


void
SCOREP_RegisterSionCallbacks()
{
    OTF2_File_Sion_Register_Open_Callback( 0 );
    OTF2_File_Sion_Register_Close_Callback( 0 );

    if ( SCOREP_Env_UseSionSubstrate() )
    {
        #if HAVE( SION )
        OTF2_File_Sion_Register_Open_Callback( scorep_sion_open );
        OTF2_File_Sion_Register_Close_Callback( scorep_sion_close );
        #endif
    }
}


static int
scorep_sion_open( const char*         fileName,
                  const char*         fileMode,
                  const long long int chunkSize,
                  int                 fileSystemBlockSize,
                  FILE**              filePointer )
{
    int number_of_sion_files  = 1; /* on BlueGene one should increase this
                                      number for applications using >16k
                                      processes. Maybe this will be done
                                      automagically by sion in upcoming
                                      releases. */
    const char* file_mode     = "wb";
    const char* new_file_name = 0;
    MPI_Comm    comm_world    = MPI_COMM_WORLD;


    int sion_file_handle = sion_paropen_mpi( fileName,
                                             file_mode,
                                             number_of_sion_files,
                                             comm_world,
                                             &comm_world,
                                             chunkSize,
                                             fileSystemBlockSize,
                                             SCOREP_Mpi_GetRank(),
                                             filePointer,
                                             new_file_name );
    assert( sion_file_handle != -1 );
    return sion_file_handle;
}


static int
scorep_sion_close( int sionFileHandle )
{
    int close_status = sion_parclose_mpi( sionFileHandle );
    assert( close_status == 1 );
    return close_status;
}
