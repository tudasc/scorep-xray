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
 * @file       silc_runtime_management_internal.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status ALPHA
 *
 */


#include "silc_runtime_management_internal.h"

#include <SILC_Timing.h>
#include <SILC_Error.h>

#include <stdio.h>
#include <sys/stat.h>


/* *INDENT-OFF* */
extern void silc_create_experiment_dir(char* dirName, int dirNameSize, void (*createDir) (const char*) );
static void silc_create_directory(const char* dirname);
static void silc_create_experiment_dir_name();
/* *INDENT-ON* */


#define dir_name_size  6 + L_tmpnam

extern char silc_experiment_dir_name[ dir_name_size ];

static bool       silc_is_experiment_dir_created = false;

OTF2_ArchiveData* silc_otf2_archive = 0;

bool              silc_profiling_enabled = false;

bool              silc_tracing_enabled = true;

bool              flush_done = false;

void
SILC_CreateExperimentDir()
{
    silc_create_experiment_dir_name();
    silc_create_experiment_dir( silc_experiment_dir_name,
                                dir_name_size,
                                silc_create_directory );
}


void
silc_create_experiment_dir_name()
{
    strncpy( silc_experiment_dir_name, "silc_", 5 );
    char* not_used = tmpnam( &( silc_experiment_dir_name[ 5 ] ) );
}


bool
SILC_ExperimentDirIsCreated()
{
    return silc_is_experiment_dir_created;
}


void
silc_create_directory( const char* dirname )
{
    mode_t mode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
    if ( mkdir( silc_experiment_dir_name, mode ) == -1 )
    {
        SILC_ERROR_POSIX( "Can't create experiment directory \"%s\".",
                          silc_experiment_dir_name );
        _Exit( EXIT_FAILURE );
    }
    silc_is_experiment_dir_created = true;
}


uint64_t
silc_on_trace_post_flush( void )
{
    /* remember that we have flushed the first time
     * after this point, we can't switch into MPI mode anymore
     */
    flush_done = true;

    return SILC_GetClockTicks();
}

/* char* */
/* silc_on_trace_pre_flush( const char* filePath ) */
/* { */
/*     return 0; */
/* } */
