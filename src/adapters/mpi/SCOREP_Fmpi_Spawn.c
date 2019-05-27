/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2016, 2018-2019,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * Copyright (c) 2016,
 * Technische Universitaet Darmstadt, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2011                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2010-2011                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/



/**
 * @file
 * @ingroup    MPI_Wrapper
 *
 * @brief Fortran interface wrappers for process creation and management
 *        functions (spawning interface)
 */

#include <config.h>
#include "SCOREP_Fmpi.h"

#include <UTILS_Error.h>

/* uppercase defines */
/** @def MPI_Close_port_U
    Exchange MPI_Close_port_U by MPI_CLOSE_PORT.
    It is used for the Fortran wrappers of MPI_Close_port.
 */
#define MPI_Close_port_U MPI_CLOSE_PORT

/** @def MPI_Comm_accept_U
    Exchange MPI_Comm_accept_U by MPI_COMM_ACCEPT.
    It is used for the Fortran wrappers of MPI_Comm_accept.
 */
#define MPI_Comm_accept_U MPI_COMM_ACCEPT

/** @def MPI_Comm_connect_U
    Exchange MPI_Comm_connect_U by MPI_COMM_CONNECT.
    It is used for the Fortran wrappers of MPI_Comm_connect.
 */
#define MPI_Comm_connect_U MPI_COMM_CONNECT

/** @def MPI_Comm_disconnect_U
    Exchange MPI_Comm_disconnect_U by MPI_COMM_DISCONNECT.
    It is used for the Fortran wrappers of MPI_Comm_disconnect.
 */
#define MPI_Comm_disconnect_U MPI_COMM_DISCONNECT

/** @def MPI_Comm_get_parent_U
    Exchange MPI_Comm_get_parent_U by MPI_COMM_GET_PARENT.
    It is used for the Fortran wrappers of MPI_Comm_get_parent.
 */
#define MPI_Comm_get_parent_U MPI_COMM_GET_PARENT

/** @def MPI_Comm_join_U
    Exchange MPI_Comm_join_U by MPI_COMM_JOIN.
    It is used for the Fortran wrappers of MPI_Comm_join.
 */
#define MPI_Comm_join_U MPI_COMM_JOIN

/** @def MPI_Comm_spawn_U
    Exchange MPI_Comm_spawn_U by MPI_COMM_SPAWN.
    It is used for the Fortran wrappers of MPI_Comm_spawn.
 */
#define MPI_Comm_spawn_U MPI_COMM_SPAWN

/** @def MPI_Comm_spawn_multiple_U
    Exchange MPI_Comm_spawn_multiple_U by MPI_COMM_SPAWN_MULTIPLE.
    It is used for the Fortran wrappers of MPI_Comm_spawn_multiple.
 */
#define MPI_Comm_spawn_multiple_U MPI_COMM_SPAWN_MULTIPLE

/** @def MPI_Lookup_name_U
    Exchange MPI_Lookup_name_U by MPI_LOOKUP_NAME.
    It is used for the Fortran wrappers of MPI_Lookup_name.
 */
#define MPI_Lookup_name_U MPI_LOOKUP_NAME

/** @def MPI_Open_port_U
    Exchange MPI_Open_port_U by MPI_OPEN_PORT.
    It is used for the Fortran wrappers of MPI_Open_port.
 */
#define MPI_Open_port_U MPI_OPEN_PORT

/** @def MPI_Publish_name_U
    Exchange MPI_Publish_name_U by MPI_PUBLISH_NAME.
    It is used for the Fortran wrappers of MPI_Publish_name.
 */
#define MPI_Publish_name_U MPI_PUBLISH_NAME

/** @def MPI_Unpublish_name_U
    Exchange MPI_Unpublish_name_U by MPI_UNPUBLISH_NAME.
    It is used for the Fortran wrappers of MPI_Unpublish_name.
 */
#define MPI_Unpublish_name_U MPI_UNPUBLISH_NAME


/* lowercase defines */
/** @def MPI_Close_port_L
    Exchanges MPI_Close_port_L by mpi_close_port.
    It is used for the Fortran wrappers of MPI_Close_port.
 */
#define MPI_Close_port_L mpi_close_port

/** @def MPI_Comm_accept_L
    Exchanges MPI_Comm_accept_L by mpi_comm_accept.
    It is used for the Fortran wrappers of MPI_Comm_accept.
 */
#define MPI_Comm_accept_L mpi_comm_accept

/** @def MPI_Comm_connect_L
    Exchanges MPI_Comm_connect_L by mpi_comm_connect.
    It is used for the Fortran wrappers of MPI_Comm_connect.
 */
#define MPI_Comm_connect_L mpi_comm_connect

/** @def MPI_Comm_disconnect_L
    Exchanges MPI_Comm_disconnect_L by mpi_comm_disconnect.
    It is used for the Fortran wrappers of MPI_Comm_disconnect.
 */
#define MPI_Comm_disconnect_L mpi_comm_disconnect

/** @def MPI_Comm_get_parent_L
    Exchanges MPI_Comm_get_parent_L by mpi_comm_get_parent.
    It is used for the Fortran wrappers of MPI_Comm_get_parent.
 */
#define MPI_Comm_get_parent_L mpi_comm_get_parent

/** @def MPI_Comm_join_L
    Exchanges MPI_Comm_join_L by mpi_comm_join.
    It is used for the Fortran wrappers of MPI_Comm_join.
 */
#define MPI_Comm_join_L mpi_comm_join

/** @def MPI_Comm_spawn_L
    Exchanges MPI_Comm_spawn_L by mpi_comm_spawn.
    It is used for the Fortran wrappers of MPI_Comm_spawn.
 */
#define MPI_Comm_spawn_L mpi_comm_spawn

/** @def MPI_Comm_spawn_multiple_L
    Exchanges MPI_Comm_spawn_multiple_L by mpi_comm_spawn_multiple.
    It is used for the Fortran wrappers of MPI_Comm_spawn_multiple.
 */
#define MPI_Comm_spawn_multiple_L mpi_comm_spawn_multiple

/** @def MPI_Lookup_name_L
    Exchanges MPI_Lookup_name_L by mpi_lookup_name.
    It is used for the Fortran wrappers of MPI_Lookup_name.
 */
#define MPI_Lookup_name_L mpi_lookup_name

/** @def MPI_Open_port_L
    Exchanges MPI_Open_port_L by mpi_open_port.
    It is used for the Fortran wrappers of MPI_Open_port.
 */
#define MPI_Open_port_L mpi_open_port

/** @def MPI_Publish_name_L
    Exchanges MPI_Publish_name_L by mpi_publish_name.
    It is used for the Fortran wrappers of MPI_Publish_name.
 */
#define MPI_Publish_name_L mpi_publish_name

/** @def MPI_Unpublish_name_L
    Exchanges MPI_Unpublish_name_L by mpi_unpublish_name.
    It is used for the Fortran wrappers of MPI_Unpublish_name.
 */
#define MPI_Unpublish_name_L mpi_unpublish_name


/**
 * @name Fortran wrappers
 * @{
 */

#ifndef NEED_F2C_CONV

/* If MPI_Comm_f2c is HAVE as a simple macro (which typically does nothing)
 * as it would require a "real" function if it is really needed
 * => we can save the f2c and c2s conversions */

#if HAVE( DECL_PMPI_COMM_SPAWN_MULTIPLE ) && !defined( SCOREP_MPI_NO_SPAWN ) && !defined( SCOREP_MPI_NO_EXTRA )
/**
 * Manual measurement wrapper for MPI_Comm_spawn_multiple
 * @note Manually adapted wrapper
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup spawn
 */
void
FSUB( MPI_Comm_spawn_multiple )( int*      count,
                                 char*     array_of_commands,
                                 char*     array_of_argv,
                                 int*      array_of_maxprocs,
                                 MPI_Info* array_of_info,
                                 int*      root,
                                 MPI_Comm* comm,
                                 MPI_Comm* intercomm,
                                 int*      array_of_errcodes,
                                 int*      ierr,
                                 int       array_of_commands_len,
                                 int       array_of_argv_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Fint cur_cmd, cur_arg;
    char**   c_array_of_commands = NULL;
    char***  c_array_of_argv     = NULL;
    int      array_size          = *count + 1; /* additional space needed for empty element */
    /* allocate and fill local array_of_commands */

    c_array_of_commands = ( char** )malloc( *count * sizeof( char* ) );
    if ( !c_array_of_commands )
    {
        UTILS_ERROR_POSIX();
    }

    for ( cur_cmd = 0; cur_cmd < *count; ++cur_cmd )
    {
        char* start_ptr = array_of_commands + cur_cmd * array_of_commands_len;
        char* ptr       = start_ptr + array_of_commands_len - 1;
        int   len       = 0;

        /* Fortran strings are 'blank' padded */
        while ( *ptr == ' ' && ptr > start_ptr )
        {
            ptr--;
        }
        len                            = ptr - start_ptr;
        c_array_of_commands[ cur_cmd ] = ( char* )malloc( ( len + 1 ) * sizeof( char ) );
        if ( !( c_array_of_commands[ cur_cmd ] ) )
        {
            UTILS_ERROR_POSIX();
        }
        /* copy contents */
        strncpy( c_array_of_commands[ cur_cmd ], start_ptr, len );
        /* null terminate string */
        c_array_of_commands[ cur_cmd ][ len ] = '\0';
    }

    /* check for special case of no arguments */
    if ( ( char*** )array_of_argv == MPI_ARGVS_NULL )
    {
        c_array_of_argv = MPI_ARGVS_NULL;
    }
    else
    {
        /*
         * we need to convert Fortran argument list to C
         * NOTE: Argument list is a column-major 2D Fortran array with fixed
         *       width entries.
         */

        /* allocate and fill local array_of_argv */
        c_array_of_argv = ( char*** )malloc( array_size * sizeof( char** ) );
        if ( !c_array_of_argv )
        {
            UTILS_ERROR_POSIX();
        }

        for ( cur_cmd = 0; cur_cmd < *count; ++cur_cmd )
        {
            int   arg_count = 0; /* keep track of number of arguments */
            int   arg_size  = 0; /* keep track of total storage needed for arguments */
            int   arg_len   = 0;
            char* start_ptr = array_of_argv + cur_cmd * array_of_argv_len;
            char* ptr       = NULL;
            char* c_arg_ptr = 0; /* pointer used for copying arguments into array */

            for ( cur_arg = 0; 1; ++cur_arg )
            {
                /* start at first column entry of command */
                ptr = start_ptr + array_of_argv_len - 1;

                /* track number of arguments */
                arg_count = cur_arg;

                /* truncate padding */
                while ( *ptr == ' ' && ptr > start_ptr )
                {
                    ptr--;
                }
                if ( ptr == start_ptr )
                {
                    /* terminating empty argument was found */
                    break;
                }

                arg_size += ptr - start_ptr + 1; /* add space for null termination */

                /* move start_ptr to next column */
                start_ptr += *count * array_of_argv_len;
            }

            /* allocate target array */
            c_array_of_argv[ cur_cmd ] = ( char** )malloc( ( arg_count + 1 ) * sizeof( char* ) );
            if ( !( c_array_of_argv[ cur_cmd ] ) )
            {
                UTILS_ERROR_POSIX();
            }

            /* allocate a single block with sufficient capacity to minimize
             * system calls */
            c_array_of_argv[ cur_cmd ][ 0 ] = ( char* )malloc( arg_size * sizeof( char ) );
            if ( !( c_array_of_argv[ cur_cmd ][ 0 ] ) )
            {
                UTILS_ERROR_POSIX();
            }
            /* set index to beginning of the block */
            c_arg_ptr = c_array_of_argv[ cur_cmd ][ 0 ];

            /* reset start_ptr */
            start_ptr = array_of_argv + cur_cmd * array_of_argv_len;
            for ( cur_arg = 0; cur_arg < arg_count; ++cur_arg )
            {
                /* truncate padding */
                ptr = start_ptr + array_of_argv_len - 1;
                while ( *ptr == ' ' && ptr > start_ptr )
                {
                    ptr--;
                }
                arg_len = ptr - start_ptr;
                strncpy( c_arg_ptr, start_ptr, arg_len );
                /* terminate string */
                c_arg_ptr[ arg_len ] = '\0';
                /* save start of argument to c_array_of_argv */
                c_array_of_argv[ cur_cmd ][ cur_arg ] = c_arg_ptr;
                /* advance c_arg_ptr to next free position */
                c_arg_ptr += arg_len + 1;
                /* proceed to next column */
                start_ptr += *count * array_of_argv_len;
            }
            /* null terminate argument array for command */
            c_array_of_argv[ cur_cmd ][ arg_count ] = NULL;
        }
        /* null terminate complete argument array */
        c_array_of_argv[ *count ] = NULL;
    }

    *ierr = MPI_Comm_spawn_multiple( *count, c_array_of_commands,
                                     c_array_of_argv, array_of_maxprocs,
                                     array_of_info, *root, *comm,
                                     intercomm, array_of_errcodes );

    /* free array of commands */
    for ( cur_cmd = 0; cur_cmd < *count; ++cur_cmd )
    {
        free( c_array_of_commands[ cur_cmd ] );
        free( c_array_of_argv[ cur_cmd ][ 0 ] );
        free( c_array_of_argv[ cur_cmd ] );
    }
    free( c_array_of_commands );
    free( c_array_of_argv );

    SCOREP_IN_MEASUREMENT_DECREMENT();
} /* FSUB(MPI_Comm_spawn_multiple) */

#endif

#if HAVE( MPI_COMM_SPAWN ) && !defined( NO_MPI_SPAWN ) \
    && !defined( NO_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Comm_spawn
 * @note Manually adapter wrapper
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 */
void
FSUB( MPI_Comm_spawn )( char*     command,
                        char*     argv,
                        int*      maxprocs,
                        MPI_Info* info,
                        int*      root,
                        MPI_Comm* comm,
                        MPI_Comm* newcomm,
                        int*      array_of_errcodes,
                        int*      ierr,
                        int       command_len,
                        int       argv_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    char*  c_command = NULL;
    char** c_argv    = NULL;
    int    arg_count = 0;
    int    arg_size  = 0;
    int    arg_len   = 0;
    int    cur_arg   = 0;
    char*  start_ptr = NULL;
    char*  ptr       = NULL;
    char*  c_arg_ptr = NULL;

    /* convert command */
    c_command = ( char* )malloc( ( command_len + 1 ) * sizeof( char ) );
    if ( !c_command )
    {
        UTILS_ERROR_POSIX();
    }

    strncpy( c_command, command, command_len );
    c_command[ command_len ] = '\0';

    if ( ( char** )argv == MPI_ARGV_NULL )
    {
        c_argv = MPI_ARGV_NULL;
    }
    else
    {
        /* convert array of arguments */
        for ( cur_arg = 0; 1; ++cur_arg )
        {
            /* count arguments and track overall size */
            start_ptr = argv + cur_arg * argv_len;
            ptr       = start_ptr + argv_len - 1;

            while ( *ptr == ' ' && ptr > start_ptr )
            {
                ptr--;
            }
            arg_size += ptr - start_ptr + 1;

            if ( ptr == start_ptr )
            {
                /* found terminating empty argument */
                arg_count = cur_arg;
                break;
            }
        }

        c_argv = ( char** )malloc( ( arg_count + 1 ) * sizeof( char ) );
        if ( !c_argv )
        {
            UTILS_ERROR_POSIX();
        }

        c_argv[ 0 ] = ( char* )malloc( arg_size * sizeof( char ) );
        if ( !( c_argv[ 0 ] ) )
        {
            UTILS_ERROR_POSIX();
        }

        c_arg_ptr = c_argv[ 0 ];
        for ( cur_arg = 0; cur_arg < arg_count; ++cur_arg )
        {
            /* count arguments and track overall size */
            start_ptr = argv + cur_arg * argv_len;
            ptr       = start_ptr + argv_len - 1;

            while ( *ptr == ' ' && ptr > start_ptr )
            {
                ptr--;
            }
            arg_len = ptr - start_ptr;

            strncpy( c_arg_ptr, start_ptr, argv_len );
            c_arg_ptr[ arg_len ] = '\0';
            c_argv[ cur_arg ]    = c_arg_ptr;
            c_arg_ptr           += arg_len + 1;
        }
    }

    *ierr = MPI_Comm_spawn( c_command, c_argv, *maxprocs,
                            *info, *root, *comm, newcomm,
                            array_of_errcodes );

    free( c_command );
    free( c_argv );
    SCOREP_IN_MEASUREMENT_DECREMENT();
} /* FSUB(MPI_Comm_spawn) */
#endif

#if HAVE( DECL_PMPI_CLOSE_PORT ) && !defined( SCOREP_MPI_NO_SPAWN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Close_port )
/**
 * Measurement wrapper for MPI_Close_port
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 */
void
FSUB( MPI_Close_port )( char* port_name, int* ierr, scorep_fortran_charlen_t port_name_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    char* c_port_name = NULL;
    c_port_name = scorep_mpi_f2c_string( port_name, port_name_len );



    *ierr = MPI_Close_port( c_port_name );

    free( c_port_name );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_COMM_ACCEPT ) && !defined( SCOREP_MPI_NO_SPAWN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Comm_accept )
/**
 * Measurement wrapper for MPI_Comm_accept
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 */
void
FSUB( MPI_Comm_accept )( char* port_name, MPI_Info* info, int* root, MPI_Comm* comm, MPI_Comm* newcomm, int* ierr, scorep_fortran_charlen_t port_name_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    char* c_port_name = NULL;
    c_port_name = scorep_mpi_f2c_string( port_name, port_name_len );



    *ierr = MPI_Comm_accept( c_port_name, *info, *root, *comm, newcomm );

    free( c_port_name );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_COMM_CONNECT ) && !defined( SCOREP_MPI_NO_SPAWN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Comm_connect )
/**
 * Measurement wrapper for MPI_Comm_connect
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 */
void
FSUB( MPI_Comm_connect )( char* port_name, MPI_Info* info, int* root, MPI_Comm* comm, MPI_Comm* newcomm, int* ierr, scorep_fortran_charlen_t port_name_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    char* c_port_name = NULL;
    c_port_name = scorep_mpi_f2c_string( port_name, port_name_len );



    *ierr = MPI_Comm_connect( c_port_name, *info, *root, *comm, newcomm );

    free( c_port_name );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_COMM_DISCONNECT ) && !defined( SCOREP_MPI_NO_SPAWN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Comm_disconnect )
/**
 * Measurement wrapper for MPI_Comm_disconnect
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 */
void
FSUB( MPI_Comm_disconnect )( MPI_Comm* comm, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Comm_disconnect( comm );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_COMM_GET_PARENT ) && !defined( SCOREP_MPI_NO_SPAWN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Comm_get_parent )
/**
 * Measurement wrapper for MPI_Comm_get_parent
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 */
void
FSUB( MPI_Comm_get_parent )( MPI_Comm* parent, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Comm_get_parent( parent );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_COMM_JOIN ) && !defined( SCOREP_MPI_NO_SPAWN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Comm_join )
/**
 * Measurement wrapper for MPI_Comm_join
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 */
void
FSUB( MPI_Comm_join )( int* fd, MPI_Comm* newcomm, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Comm_join( *fd, newcomm );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_LOOKUP_NAME ) && !defined( SCOREP_MPI_NO_SPAWN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Lookup_name )
/**
 * Measurement wrapper for MPI_Lookup_name
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 */
void
FSUB( MPI_Lookup_name )( char* service_name, MPI_Info* info, char* port_name, int* ierr, scorep_fortran_charlen_t service_name_len, scorep_fortran_charlen_t port_name_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    char*  c_service_name  = NULL;
    char*  c_port_name     = NULL;
    size_t c_port_name_len = 0;
    c_service_name = scorep_mpi_f2c_string( service_name, service_name_len );

    c_port_name = ( char* )malloc( ( port_name_len + 1 ) * sizeof( char ) );
    if ( !c_port_name )
    {
        exit( EXIT_FAILURE );
    }



    *ierr = MPI_Lookup_name( c_service_name, *info, c_port_name );

    free( c_service_name );

    c_port_name_len = strlen( c_port_name );
    strncpy( port_name, c_port_name, c_port_name_len );
    memset( port_name + c_port_name_len, ' ', port_name_len - c_port_name_len );
    free( c_port_name );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_OPEN_PORT ) && !defined( SCOREP_MPI_NO_SPAWN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Open_port )
/**
 * Measurement wrapper for MPI_Open_port
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 */
void
FSUB( MPI_Open_port )( MPI_Info* info, char* port_name, int* ierr, scorep_fortran_charlen_t port_name_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    char*  c_port_name     = NULL;
    size_t c_port_name_len = 0;
    c_port_name = ( char* )malloc( ( port_name_len + 1 ) * sizeof( char ) );
    if ( !c_port_name )
    {
        exit( EXIT_FAILURE );
    }



    *ierr = MPI_Open_port( *info, c_port_name );


    c_port_name_len = strlen( c_port_name );
    strncpy( port_name, c_port_name, c_port_name_len );
    memset( port_name + c_port_name_len, ' ', port_name_len - c_port_name_len );
    free( c_port_name );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_PUBLISH_NAME ) && !defined( SCOREP_MPI_NO_SPAWN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Publish_name )
/**
 * Measurement wrapper for MPI_Publish_name
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 */
void
FSUB( MPI_Publish_name )( char* service_name, MPI_Info* info, char* port_name, int* ierr, scorep_fortran_charlen_t service_name_len, scorep_fortran_charlen_t port_name_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    char* c_service_name = NULL;
    char* c_port_name    = NULL;
    c_service_name = scorep_mpi_f2c_string( service_name, service_name_len );

    c_port_name = scorep_mpi_f2c_string( port_name, port_name_len );



    *ierr = MPI_Publish_name( c_service_name, *info, c_port_name );

    free( c_service_name );
    free( c_port_name );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_UNPUBLISH_NAME ) && !defined( SCOREP_MPI_NO_SPAWN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Unpublish_name )
/**
 * Measurement wrapper for MPI_Unpublish_name
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 */
void
FSUB( MPI_Unpublish_name )( char* service_name, MPI_Info* info, char* port_name, int* ierr, scorep_fortran_charlen_t service_name_len, scorep_fortran_charlen_t port_name_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    char* c_service_name = NULL;
    char* c_port_name    = NULL;
    c_service_name = scorep_mpi_f2c_string( service_name, service_name_len );

    c_port_name = scorep_mpi_f2c_string( port_name, port_name_len );



    *ierr = MPI_Unpublish_name( c_service_name, *info, c_port_name );

    free( c_service_name );
    free( c_port_name );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#else /* !NEED_F2C_CONV */

#if HAVE( DECL_PMPI_COMM_SPAWN_MULTIPLE ) && !defined( SCOREP_MPI_NO_SPAWN ) && !defined( SCOREP_MPI_NO_EXTRA )
/**
 * Manual measurement wrapper for MPI_Comm_spawn_multiple
 * @ingroup manual_wrapper
 * @ingroup interface_fortran
 * @ingroup mpi_version_2
 * @ingroup mpi_enabled_proc
 */
void
FSUB( MPI_Comm_spawn_multiple )( MPI_Fint* count,
                                 char*     array_of_commands,
                                 char*     array_of_argv,
                                 MPI_Fint* array_of_maxprocs,
                                 MPI_Fint* array_of_info,
                                 MPI_Fint* root,
                                 MPI_Fint* comm,
                                 MPI_Fint* intercomm,
                                 MPI_Fint* array_of_errcodes,
                                 MPI_Fint* ierr,
                                 MPI_Fint  array_of_commands_len,
                                 MPI_Fint  array_of_argv_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Fint  cur_cmd, cur_arg;
    char**    c_array_of_commands = NULL;
    char***   c_array_of_argv     = NULL;
    MPI_Info* c_array_of_info     = NULL;
    MPI_Comm  c_intercomm, c_comm;
    int       array_size = *count + 1;        // additional space needed for empty element

    /* allocate and fill local array_of_commands */
    c_array_of_commands = ( char** )malloc( *count * sizeof( char* ) );
    if ( !c_array_of_commands )
    {
        UTILS_ERROR_POSIX();
    }

    for ( cur_cmd = 0; cur_cmd < *count; ++cur_cmd )
    {
        char* start_ptr = array_of_commands + cur_cmd * array_of_commands_len;
        char* ptr       = start_ptr + array_of_commands_len - 1;
        int   len       = 0;

        /* Fortran strings are 'blank' padded */
        while ( *ptr == ' ' && ptr > start_ptr )
        {
            ptr--;
        }
        len                            = ptr - start_ptr;
        c_array_of_commands[ cur_cmd ] = ( char* )malloc( ( len + 1 ) * sizeof( char ) );
        if ( !( c_array_of_commands[ cur_cmd ] ) )
        {
            UTILS_ERROR_POSIX();
        }
        /* copy contents */
        strncpy( c_array_of_commands[ cur_cmd ], start_ptr, len );
        /* null terminate string */
        c_array_of_commands[ cur_cmd ][ len ] = '\0';
    }

    /* check for special case of no arguments */
    if ( ( char*** )array_of_argv == MPI_ARGVS_NULL )
    {
        c_array_of_argv = MPI_ARGVS_NULL;
    }
    else
    {
        /*
         * we need to convert Fortran argument list to C
         * NOTE: Argument list is a column-major 2D Fortran array with fixed
         *       width entries.
         */

        /* allocate and fill local array_of_argv */
        c_array_of_argv = ( char*** )malloc( array_size * sizeof( char** ) );
        if ( !c_array_of_argv )
        {
            UTILS_ERROR_POSIX();
        }

        for ( cur_cmd = 0; cur_cmd < *count; ++cur_cmd )
        {
            int   arg_count = 0; /* keep track of number of arguments */
            int   arg_size  = 0; /* keep track of total storage needed for arguments */
            int   arg_len   = 0;
            char* start_ptr = array_of_argv + cur_cmd * array_of_argv_len;
            char* ptr       = NULL;
            char* c_arg_ptr = NULL; /* pointer used for copying arguments into array */

            for ( cur_arg = 0; 1; ++cur_arg )
            {
                /* start at first column entry of command */
                ptr = start_ptr + array_of_argv_len - 1;

                /* track number of arguments */
                arg_count = cur_arg;

                /* truncate padding */
                while ( *ptr == ' ' && ptr > start_ptr )
                {
                    ptr--;
                }
                if ( ptr == start_ptr )
                {
                    /* terminating empty argument was found */
                    break;
                }

                arg_size += ptr - start_ptr + 1; /* add space for null termination */

                /* move start_ptr to next column */
                start_ptr += *count * array_of_argv_len;
            }

            /* allocate target array */
            c_array_of_argv[ cur_cmd ] = ( char** )malloc( ( arg_count + 1 ) * sizeof( char* ) );
            if ( !( c_array_of_argv[ cur_cmd ] ) )
            {
                UTILS_ERROR_POSIX();
            }

            /* allocate a single block with sufficient capacity to minimize
             * system calls */
            c_array_of_argv[ cur_cmd ][ 0 ] = ( char* )malloc( arg_size * sizeof( char ) );
            if ( !( c_array_of_argv[ cur_cmd ][ 0 ] ) )
            {
                UTILS_ERROR_POSIX();
            }
            /* set index to beginning of the block */
            c_arg_ptr = c_array_of_argv[ cur_cmd ][ 0 ];

            /* reset start_ptr */
            start_ptr = array_of_argv + cur_cmd * array_of_argv_len;
            for ( cur_arg = 0; cur_arg < arg_count; ++cur_arg )
            {
                /* truncate padding */
                ptr = start_ptr + array_of_argv_len - 1;
                while ( *ptr == ' ' && ptr > start_ptr )
                {
                    ptr--;
                }
                arg_len = ptr - start_ptr;
                strncpy( c_arg_ptr, start_ptr, arg_len );
                /* terminate string */
                c_arg_ptr[ arg_len ] = '\0';
                /* save start of argument to c_array_of_argv */
                c_array_of_argv[ cur_cmd ][ cur_arg ] = c_arg_ptr;
                /* advance c_arg_ptr to next free position */
                c_arg_ptr += arg_len + 1;
                /* proceed to next column */
                start_ptr += *count * array_of_argv_len;
            }
            /* null terminate argument array for command */
            c_array_of_argv[ cur_cmd ][ arg_count ] = NULL;
        }
        /* null terminate complete argument array */
        c_array_of_argv[ *count ] = NULL;
    }

    /* convert info objects */
    c_array_of_info = ( MPI_Info* )malloc( *count * sizeof( MPI_Info ) );
    if ( !c_array_of_info )
    {
        UTILS_ERROR_POSIX();
    }
    for ( cur_cmd = 0; cur_cmd < *count; ++cur_cmd )
    {
        c_array_of_info[ cur_cmd ] = PMPI_Info_f2c( array_of_info[ cur_cmd ] );
    }

    /* convert incoming communicator */
    c_comm = PMPI_Comm_f2c( *comm );

    *ierr = MPI_Comm_spawn_multiple( *count, c_array_of_commands,
                                     c_array_of_argv, array_of_maxprocs,
                                     c_array_of_info, *root, c_comm,
                                     &c_intercomm, array_of_errcodes );

    /* convert outgoing communicator */
    *intercomm = PMPI_Comm_c2f( c_intercomm );

    /* free array of commands */
    for ( cur_cmd = 0; cur_cmd < *count; ++cur_cmd )
    {
        free( c_array_of_commands[ cur_cmd ] );
        free( c_array_of_argv[ cur_cmd ][ 0 ] );
        free( c_array_of_argv[ cur_cmd ] );
    }
    free( c_array_of_commands );
    free( c_array_of_argv );
    free( c_array_of_info );

    SCOREP_IN_MEASUREMENT_DECREMENT();
} /* FSUB(MPI_Comm_spawn_multiple) */
#endif

#if HAVE( PMPI_COMM_SPAWN ) && !defined( NO_MPI_SPAWN ) \
    && !defined( NO_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Comm_spawn
 * @note Manually adapted wrapper
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 */
void
FSUB( MPI_Comm_spawn )( char*     command,
                        char*     argv,
                        MPI_Fint* maxprocs,
                        MPI_Fint* info,
                        MPI_Fint* root,
                        MPI_Fint* comm,
                        MPI_Fint* newcomm,
                        MPI_Fint* array_of_errcodes,
                        MPI_Fint* ierr,
                        MPI_Fint  command_len,
                        MPI_Fint  argv_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    char*    c_command = NULL;
    char**   c_argv    = NULL;
    MPI_Comm c_newcomm = MPI_COMM_NULL;
    int      arg_count = 0;
    int      arg_size  = 0;
    int      arg_len   = 0;
    int      cur_arg   = 0;
    char*    start_ptr = NULL;
    char*    ptr       = NULL;
    char*    c_arg_ptr = NULL;

    /* convert command */
    c_command = ( char* )malloc( ( command_len + 1 ) * sizeof( char ) );
    if ( !c_command )
    {
        UTILS_ERROR_POSIX();
    }

    strncpy( c_command, command, command_len );
    c_command[ command_len ] = '\0';

    if ( ( char** )argv == MPI_ARGV_NULL )
    {
        c_argv = MPI_ARGV_NULL;
    }
    else
    {
        /* convert array of arguments */
        for ( cur_arg = 0; 1; ++cur_arg )
        {
            /* count arguments and track overall size */
            start_ptr = argv + cur_arg * argv_len;
            ptr       = start_ptr + argv_len - 1;

            while ( *ptr == ' ' && ptr > start_ptr )
            {
                ptr--;
            }
            arg_size += ptr - start_ptr + 1;

            if ( ptr == start_ptr )
            {
                /* found terminating empty argument */
                arg_count = cur_arg;
                break;
            }
        }

        c_argv = ( char** )malloc( ( arg_count + 1 ) * sizeof( char ) );
        if ( !c_argv )
        {
            UTILS_ERROR_POSIX();
        }

        c_argv[ 0 ] = ( char* )malloc( arg_size * sizeof( char ) );
        if ( !( c_argv[ 0 ] ) )
        {
            SCOREP_ERROR_POSIX;
        }

        c_arg_ptr = c_argv[ 0 ];
        for ( cur_arg = 0; cur_arg < arg_count; ++cur_arg )
        {
            /* count arguments and track overall size */
            start_ptr = argv + cur_arg * argv_len;
            ptr       = start_ptr + argv_len - 1;

            while ( *ptr == ' ' && ptr > start_ptr )
            {
                ptr--;
            }
            arg_len = ptr - start_ptr;

            strncpy( c_arg_ptr, start_ptr, argv_len );
            c_arg_ptr[ arg_len ] = '\0';
            c_argv[ cur_arg ]    = c_arg_ptr;
            c_arg_ptr           += arg_len + 1;
        }
    }

    *ierr = MPI_Comm_spawn( c_command, c_argv, *maxprocs,
                            PMPI_Info_f2c( *info ), *root,
                            PMPI_Comm_f2c( *comm ), &c_newcomm,
                            array_of_errcodes );

    /* convert communicator handle */
    *newcomm = PMPI_Comm_c2f( c_newcomm );

    free( c_command );
    free( c_argv );
    SCOREP_IN_MEASUREMENT_DECREMENT();
} /* FSUB(MPI_Comm_spawn) */
#endif

#if HAVE( DECL_PMPI_CLOSE_PORT ) && !defined( SCOREP_MPI_NO_SPAWN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Close_port )
/**
 * Measurement wrapper for MPI_Close_port
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 * For the order of events see @ref MPI_Close_port
 */
void
FSUB( MPI_Close_port )( char* port_name, MPI_Fint* ierr, scorep_fortran_charlen_t port_name_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    char* c_port_name = NULL;
    c_port_name = scorep_mpi_f2c_string( port_name, port_name_len );



    *ierr = MPI_Close_port( c_port_name );

    free( c_port_name );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_COMM_ACCEPT ) && !defined( SCOREP_MPI_NO_SPAWN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Comm_accept )
/**
 * Measurement wrapper for MPI_Comm_accept
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 * For the order of events see @ref MPI_Comm_accept
 */
void
FSUB( MPI_Comm_accept )( char* port_name, MPI_Fint* info, MPI_Fint* root, MPI_Fint* comm, MPI_Fint* newcomm, MPI_Fint* ierr, scorep_fortran_charlen_t port_name_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    char*    c_port_name = NULL;
    MPI_Comm c_newcomm;
    c_port_name = scorep_mpi_f2c_string( port_name, port_name_len );



    *ierr = MPI_Comm_accept( c_port_name, PMPI_Info_f2c( *info ), *root, PMPI_Comm_f2c( *comm ), &c_newcomm );

    free( c_port_name );
    *newcomm = PMPI_Comm_c2f( c_newcomm );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_COMM_CONNECT ) && !defined( SCOREP_MPI_NO_SPAWN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Comm_connect )
/**
 * Measurement wrapper for MPI_Comm_connect
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 * For the order of events see @ref MPI_Comm_connect
 */
void
FSUB( MPI_Comm_connect )( char* port_name, MPI_Fint* info, MPI_Fint* root, MPI_Fint* comm, MPI_Fint* newcomm, MPI_Fint* ierr, scorep_fortran_charlen_t port_name_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    char*    c_port_name = NULL;
    MPI_Comm c_newcomm;
    c_port_name = scorep_mpi_f2c_string( port_name, port_name_len );



    *ierr = MPI_Comm_connect( c_port_name, PMPI_Info_f2c( *info ), *root, PMPI_Comm_f2c( *comm ), &c_newcomm );

    free( c_port_name );
    *newcomm = PMPI_Comm_c2f( c_newcomm );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_COMM_DISCONNECT ) && !defined( SCOREP_MPI_NO_SPAWN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Comm_disconnect )
/**
 * Measurement wrapper for MPI_Comm_disconnect
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 * For the order of events see @ref MPI_Comm_disconnect
 */
void
FSUB( MPI_Comm_disconnect )( MPI_Fint* comm, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Comm c_comm = PMPI_Comm_f2c( *comm );


    *ierr = MPI_Comm_disconnect( &c_comm );

    *comm = PMPI_Comm_c2f( c_comm );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_COMM_GET_PARENT ) && !defined( SCOREP_MPI_NO_SPAWN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Comm_get_parent )
/**
 * Measurement wrapper for MPI_Comm_get_parent
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 * For the order of events see @ref MPI_Comm_get_parent
 */
void
FSUB( MPI_Comm_get_parent )( MPI_Fint* parent, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Comm c_parent;


    *ierr = MPI_Comm_get_parent( &c_parent );

    *parent = PMPI_Comm_c2f( c_parent );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_COMM_JOIN ) && !defined( SCOREP_MPI_NO_SPAWN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Comm_join )
/**
 * Measurement wrapper for MPI_Comm_join
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 * For the order of events see @ref MPI_Comm_join
 */
void
FSUB( MPI_Comm_join )( MPI_Fint* fd, MPI_Fint* newcomm, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Comm c_newcomm;


    *ierr = MPI_Comm_join( *fd, &c_newcomm );

    *newcomm = PMPI_Comm_c2f( c_newcomm );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_LOOKUP_NAME ) && !defined( SCOREP_MPI_NO_SPAWN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Lookup_name )
/**
 * Measurement wrapper for MPI_Lookup_name
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 * For the order of events see @ref MPI_Lookup_name
 */
void
FSUB( MPI_Lookup_name )( char* service_name, MPI_Fint* info, char* port_name, MPI_Fint* ierr, scorep_fortran_charlen_t service_name_len, scorep_fortran_charlen_t port_name_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    char*  c_service_name  = NULL;
    char*  c_port_name     = NULL;
    size_t c_port_name_len = 0;
    c_service_name = scorep_mpi_f2c_string( service_name, service_name_len );

    c_port_name = ( char* )malloc( ( port_name_len + 1 ) * sizeof( char ) );
    if ( !c_port_name )
    {
        exit( EXIT_FAILURE );
    }



    *ierr = MPI_Lookup_name( c_service_name, PMPI_Info_f2c( *info ), c_port_name );

    free( c_service_name );

    c_port_name_len = strlen( c_port_name );
    strncpy( port_name, c_port_name, c_port_name_len );
    memset( port_name + c_port_name_len, ' ', port_name_len - c_port_name_len );
    free( c_port_name );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_OPEN_PORT ) && !defined( SCOREP_MPI_NO_SPAWN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Open_port )
/**
 * Measurement wrapper for MPI_Open_port
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 * For the order of events see @ref MPI_Open_port
 */
void
FSUB( MPI_Open_port )( MPI_Fint* info, char* port_name, MPI_Fint* ierr, scorep_fortran_charlen_t port_name_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    char*  c_port_name     = NULL;
    size_t c_port_name_len = 0;
    c_port_name = ( char* )malloc( ( port_name_len + 1 ) * sizeof( char ) );
    if ( !c_port_name )
    {
        exit( EXIT_FAILURE );
    }



    *ierr = MPI_Open_port( PMPI_Info_f2c( *info ), c_port_name );


    c_port_name_len = strlen( c_port_name );
    strncpy( port_name, c_port_name, c_port_name_len );
    memset( port_name + c_port_name_len, ' ', port_name_len - c_port_name_len );
    free( c_port_name );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_PUBLISH_NAME ) && !defined( SCOREP_MPI_NO_SPAWN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Publish_name )
/**
 * Measurement wrapper for MPI_Publish_name
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 * For the order of events see @ref MPI_Publish_name
 */
void
FSUB( MPI_Publish_name )( char* service_name, MPI_Fint* info, char* port_name, MPI_Fint* ierr, scorep_fortran_charlen_t service_name_len, scorep_fortran_charlen_t port_name_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    char* c_service_name = NULL;
    char* c_port_name    = NULL;
    c_service_name = scorep_mpi_f2c_string( service_name, service_name_len );

    c_port_name = scorep_mpi_f2c_string( port_name, port_name_len );



    *ierr = MPI_Publish_name( c_service_name, PMPI_Info_f2c( *info ), c_port_name );

    free( c_service_name );
    free( c_port_name );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_UNPUBLISH_NAME ) && !defined( SCOREP_MPI_NO_SPAWN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Unpublish_name )
/**
 * Measurement wrapper for MPI_Unpublish_name
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 * For the order of events see @ref MPI_Unpublish_name
 */
void
FSUB( MPI_Unpublish_name )( char* service_name, MPI_Fint* info, char* port_name, MPI_Fint* ierr, scorep_fortran_charlen_t service_name_len, scorep_fortran_charlen_t port_name_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    char* c_service_name = NULL;
    char* c_port_name    = NULL;
    c_service_name = scorep_mpi_f2c_string( service_name, service_name_len );

    c_port_name = scorep_mpi_f2c_string( port_name, port_name_len );



    *ierr = MPI_Unpublish_name( c_service_name, PMPI_Info_f2c( *info ), c_port_name );

    free( c_service_name );
    free( c_port_name );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#endif

/**
 * @}
 */
