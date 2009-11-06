/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
**  KOJAK       http://www.fz-juelich.de/jsc/kojak/                        **
*****************************************************************************
**  Copyright (c) 1998-2009                                                **
**  Forschungszentrum Juelich, Juelich Supercomputing Centre               **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

/**
 * @file  SILC_Fmpi_Spawn.c
 *
 * @brief Fortran interface wrappers for process creation and management
 *        functions (spawning interface)
 */

/* uppercase defines */
/** @def MPI_Close_port_U
    Exchange MPI_Close_port_U by MPI_CLOSE_PORT.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Close_port_U MPI_CLOSE_PORT

/** @def MPI_Comm_accept_U
    Exchange MPI_Comm_accept_U by MPI_COMM_ACCEPT.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Comm_accept_U MPI_COMM_ACCEPT

/** @def MPI_Comm_connect_U
    Exchange MPI_Comm_connect_U by MPI_COMM_CONNECT.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Comm_connect_U MPI_COMM_CONNECT

/** @def MPI_Comm_disconnect_U
    Exchange MPI_Comm_disconnect_U by MPI_COMM_DISCONNECT.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Comm_disconnect_U MPI_COMM_DISCONNECT

/** @def MPI_Comm_get_parent_U
    Exchange MPI_Comm_get_parent_U by MPI_COMM_GET_PARENT.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Comm_get_parent_U MPI_COMM_GET_PARENT

/** @def MPI_Comm_join_U
    Exchange MPI_Comm_join_U by MPI_COMM_JOIN.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Comm_join_U MPI_COMM_JOIN

/** @def MPI_Comm_spawn_U
    Exchange MPI_Comm_spawn_U by MPI_COMM_SPAWN.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Comm_spawn_U MPI_COMM_SPAWN

/** @def MPI_Comm_spawn_multiple_U
    Exchange MPI_Comm_spawn_multiple_U by MPI_COMM_SPAWN_MULTIPLE.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Comm_spawn_multiple_U MPI_COMM_SPAWN_MULTIPLE

/** @def MPI_Lookup_name_U
    Exchange MPI_Lookup_name_U by MPI_LOOKUP_NAME.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Lookup_name_U MPI_LOOKUP_NAME

/** @def MPI_Open_port_U
    Exchange MPI_Open_port_U by MPI_OPEN_PORT.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Open_port_U MPI_OPEN_PORT

/** @def MPI_Publish_name_U
    Exchange MPI_Publish_name_U by MPI_PUBLISH_NAME.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Publish_name_U MPI_PUBLISH_NAME

/** @def MPI_Unpublish_name_U
    Exchange MPI_Unpublish_name_U by MPI_UNPUBLISH_NAME.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Unpublish_name_U MPI_UNPUBLISH_NAME


/* lowercase defines */
/** @def MPI_Close_port_L
    Exchanges MPI_Close_port_L by mpi_close_port.
    It is used for the Forran wrappers of me).
 */
#define MPI_Close_port_L mpi_close_port

/** @def MPI_Comm_accept_L
    Exchanges MPI_Comm_accept_L by mpi_comm_accept.
    It is used for the Forran wrappers of me).
 */
#define MPI_Comm_accept_L mpi_comm_accept

/** @def MPI_Comm_connect_L
    Exchanges MPI_Comm_connect_L by mpi_comm_connect.
    It is used for the Forran wrappers of me).
 */
#define MPI_Comm_connect_L mpi_comm_connect

/** @def MPI_Comm_disconnect_L
    Exchanges MPI_Comm_disconnect_L by mpi_comm_disconnect.
    It is used for the Forran wrappers of me).
 */
#define MPI_Comm_disconnect_L mpi_comm_disconnect

/** @def MPI_Comm_get_parent_L
    Exchanges MPI_Comm_get_parent_L by mpi_comm_get_parent.
    It is used for the Forran wrappers of me).
 */
#define MPI_Comm_get_parent_L mpi_comm_get_parent

/** @def MPI_Comm_join_L
    Exchanges MPI_Comm_join_L by mpi_comm_join.
    It is used for the Forran wrappers of me).
 */
#define MPI_Comm_join_L mpi_comm_join

/** @def MPI_Comm_spawn_L
    Exchanges MPI_Comm_spawn_L by mpi_comm_spawn.
    It is used for the Forran wrappers of me).
 */
#define MPI_Comm_spawn_L mpi_comm_spawn

/** @def MPI_Comm_spawn_multiple_L
    Exchanges MPI_Comm_spawn_multiple_L by mpi_comm_spawn_multiple.
    It is used for the Forran wrappers of me).
 */
#define MPI_Comm_spawn_multiple_L mpi_comm_spawn_multiple

/** @def MPI_Lookup_name_L
    Exchanges MPI_Lookup_name_L by mpi_lookup_name.
    It is used for the Forran wrappers of me).
 */
#define MPI_Lookup_name_L mpi_lookup_name

/** @def MPI_Open_port_L
    Exchanges MPI_Open_port_L by mpi_open_port.
    It is used for the Forran wrappers of me).
 */
#define MPI_Open_port_L mpi_open_port

/** @def MPI_Publish_name_L
    Exchanges MPI_Publish_name_L by mpi_publish_name.
    It is used for the Forran wrappers of me).
 */
#define MPI_Publish_name_L mpi_publish_name

/** @def MPI_Unpublish_name_L
    Exchanges MPI_Unpublish_name_L by mpi_unpublish_name.
    It is used for the Forran wrappers of me).
 */
#define MPI_Unpublish_name_L mpi_unpublish_name


/**
 * @name Fortran wrappers
 * @{
 */

#ifndef NEED_F2C_CONV

/* If MPI_Comm_f2c is defined as a simple macro (which typically does nothing)
 * as it would require a "real" function if it is really needed
 * => we can save the f2c and c2s conversions */

/**
 * Manual measurement wrapper for MPI_Comm_spawn_multiple
 * @ingroup manual_wrapper
 * @ingroup interface_fortran
 * @ingroup mpi_version_2
 * @ingroup mpi_enabled_proc
 */
void
FSUB
(
    MPI_Comm_spawn_multiple
) ( MPI_Fint * count,
    char* array_of_commands[],
    char* array_of_argv[],
    MPI_Fint * array_of_maxprocs,
    MPI_Fint * array_of_info,
    MPI_Fint * root,
    MPI_Fint * comm,
    MPI_Fint * intercomm,
    MPI_Fint * array_of_errcodes,
    MPI_Fint * ierr,
    MPI_Fint array_of_commands_len[] )
{
    MPI_Fint i;
    char*    c_array_of_commands[] = NULL;
    char*    c_array_of_argv[]     = NULL;

    /* allocate and fill local array_of_commands */
    c_array_of_commands = ( char** )malloc( count * sizeof( char* ) );
    if ( !c_array_of_commands )
    {
        exit( EXIT_FAILURE );
    }
    for ( i = 0; i < count; ++i )
    {
        c_array_of_commands[ i ] =
            ( char* )malloc( ( array_of_commands_len[ i ] + 1 ) * sizeof( char ) );
        if ( !( c_array_of_commands[ i ] ) )
        {
            exit( EXIT_FAILURE );
        }
        strncpy( c_array_of_commands[ i ], array_of_commands[ i ],
                 array_of_commands_len[ i ] );
        /* null terminate character array */
        c_array_of_commands[ i ][ array_of_commands_len[ i ] ] = '\0';
    }

    /* allocate and fill local array_of_argv */
    c_array_of_argv = ( char** )malloc( count * sizeof( char* ) );
    if ( !c_array_of_argv )
    {
        exit( EXIT_FAILURE );
    }
    for ( i = 0; i < count; ++i )
    {
        c_array_of_argv[ i ] =
            ( char* )malloc( ( array_of_argv_len[ i ] + 1 ) * sizeof( char ) );
        if ( !( c_array_of_argv[ i ] ) )
        {
            exit( EXIT_FAILURE );
        }
        strncpy( c_array_of_argv[ i ], array_of_argv[ i ],
                 array_of_argv_len[ i ] );
        /* null terminate character array */
        c_array_of_argv[ i ][ array_of_argv_len[ i ] ] = '\0';
    }

    *ierr = MPI_Comm_spawn_multiple( *count, c_array_of_commands,
                                     c_array_of_argv, array_of_maxprocs,
                                     array_of_info, *root, *comm,
                                     intercomm, array_of_errcodes );

    for ( i = 0; i < count; ++i )
    {
        free( c_array_of_commands[ i ] );
        free( c_array_of_argv[ i ] );
    }
    free( c_array_of_commands );
    free( c_array_of_argv );
}

#if defined( HAS_MPI2_SPAWN ) && defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Close_port
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 */
void
FSUB
(
    MPI_Close_port
) ( char* port_name, int* ierr, int port_name_len )
{
    char* c_port_name = NULL;
    c_port_name = ( char* )malloc( ( port_name_len + 1 ) * sizeof( char ) );
    if ( !c_port_name )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_port_name, port_name, port_name_len );
    c_port_name[ port_name_len ] = '\0';


    *ierr = MPI_Close_port( c_port_name );

    free( c_port_name );
}
#endif
#if defined( HAS_MPI2_SPAWN ) && defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Comm_accept
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 */
void
FSUB
(
    MPI_Comm_accept
) ( char* port_name, MPI_Info * info, int* root, MPI_Comm * comm, MPI_Comm * newcomm, int* ierr, int port_name_len )
{
    char* c_port_name = NULL;
    c_port_name = ( char* )malloc( ( port_name_len + 1 ) * sizeof( char ) );
    if ( !c_port_name )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_port_name, port_name, port_name_len );
    c_port_name[ port_name_len ] = '\0';


    *ierr = MPI_Comm_accept( c_port_name, *info, *root, *comm, newcomm );

    free( c_port_name );
}
#endif
#if defined( HAS_MPI2_SPAWN ) && defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Comm_connect
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 */
void
FSUB
(
    MPI_Comm_connect
) ( char* port_name, MPI_Info * info, int* root, MPI_Comm * comm, MPI_Comm * newcomm, int* ierr, int port_name_len )
{
    char* c_port_name = NULL;
    c_port_name = ( char* )malloc( ( port_name_len + 1 ) * sizeof( char ) );
    if ( !c_port_name )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_port_name, port_name, port_name_len );
    c_port_name[ port_name_len ] = '\0';


    *ierr = MPI_Comm_connect( c_port_name, *info, *root, *comm, newcomm );

    free( c_port_name );
}
#endif
#if defined( HAS_MPI2_SPAWN ) && defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Comm_disconnect
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 */
void
FSUB
(
    MPI_Comm_disconnect
) ( MPI_Comm * comm, int* ierr )
{
    *ierr = MPI_Comm_disconnect( comm );
}
#endif
#if defined( HAS_MPI2_SPAWN ) && defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Comm_get_parent
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 */
void
FSUB
(
    MPI_Comm_get_parent
) ( MPI_Comm * parent, int* ierr )
{
    *ierr = MPI_Comm_get_parent( parent );
}
#endif
#if defined( HAS_MPI2_SPAWN ) && defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Comm_join
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 */
void
FSUB
(
    MPI_Comm_join
) ( int* fd, MPI_Comm * intercomm, int* ierr )
{
    *ierr = MPI_Comm_join( *fd, intercomm );
}
#endif
#if defined( HAS_MPI2_SPAWN ) && defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Comm_spawn
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 */
void
FSUB
(
    MPI_Comm_spawn
) ( char* command, char* argv[], int* maxprocs, MPI_Info * info, int* root, MPI_Comm * comm, MPI_Comm * intercomm, int array_of_errcodes[], int* ierr, int command_len, int argv_len )
{
    char* c_command = NULL;
    char* c_argv    = NULL;
    c_command = ( char* )malloc( ( command_len + 1 ) * sizeof( char ) );
    if ( !c_command )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_command, command, command_len );
    c_command[ command_len ] = '\0';

    c_argv = ( char* )malloc( ( argv_len + 1 ) * sizeof( char ) );
    if ( !c_argv )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_argv, argv, argv_len );
    c_argv[ argv_len ] = '\0';


    *ierr = MPI_Comm_spawn( c_command, c_argv, *maxprocs, *info, *root, *comm, intercomm, array_of_errcodes );

    free( c_command );
    free( c_argv );
}
#endif
#if defined( HAS_MPI2_SPAWN ) && defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Lookup_name
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 */
void
FSUB
(
    MPI_Lookup_name
) ( char* service_name, MPI_Info * info, char* port_name, int* ierr, int service_name_len, int port_name_len )
{
    char* c_service_name = NULL;
    char* c_port_name    = NULL;
    c_service_name = ( char* )malloc( ( service_name_len + 1 ) * sizeof( char ) );
    if ( !c_service_name )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_service_name, service_name, service_name_len );
    c_service_name[ service_name_len ] = '\0';

    c_port_name = ( char* )malloc( ( port_name_len + 1 ) * sizeof( char ) );
    if ( !c_port_name )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_port_name, port_name, port_name_len );
    c_port_name[ port_name_len ] = '\0';


    *ierr = MPI_Lookup_name( c_service_name, *info, c_port_name );

    free( c_service_name );
    free( c_port_name );
}
#endif
#if defined( HAS_MPI2_SPAWN ) && defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Open_port
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 */
void
FSUB
(
    MPI_Open_port
) ( MPI_Info * info, char* port_name, int* ierr, int port_name_len )
{
    char* c_port_name = NULL;
    c_port_name = ( char* )malloc( ( port_name_len + 1 ) * sizeof( char ) );
    if ( !c_port_name )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_port_name, port_name, port_name_len );
    c_port_name[ port_name_len ] = '\0';


    *ierr = MPI_Open_port( *info, c_port_name );

    free( c_port_name );
}
#endif
#if defined( HAS_MPI2_SPAWN ) && defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Publish_name
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 */
void
FSUB
(
    MPI_Publish_name
) ( char* service_name, MPI_Info * info, char* port_name, int* ierr, int service_name_len, int port_name_len )
{
    char* c_service_name = NULL;
    char* c_port_name    = NULL;
    c_service_name = ( char* )malloc( ( service_name_len + 1 ) * sizeof( char ) );
    if ( !c_service_name )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_service_name, service_name, service_name_len );
    c_service_name[ service_name_len ] = '\0';

    c_port_name = ( char* )malloc( ( port_name_len + 1 ) * sizeof( char ) );
    if ( !c_port_name )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_port_name, port_name, port_name_len );
    c_port_name[ port_name_len ] = '\0';


    *ierr = MPI_Publish_name( c_service_name, *info, c_port_name );

    free( c_service_name );
    free( c_port_name );
}
#endif
#if defined( HAS_MPI2_SPAWN ) && defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Unpublish_name
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 */
void
FSUB
(
    MPI_Unpublish_name
) ( char* service_name, MPI_Info * info, char* port_name, int* ierr, int service_name_len, int port_name_len )
{
    char* c_service_name = NULL;
    char* c_port_name    = NULL;
    c_service_name = ( char* )malloc( ( service_name_len + 1 ) * sizeof( char ) );
    if ( !c_service_name )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_service_name, service_name, service_name_len );
    c_service_name[ service_name_len ] = '\0';

    c_port_name = ( char* )malloc( ( port_name_len + 1 ) * sizeof( char ) );
    if ( !c_port_name )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_port_name, port_name, port_name_len );
    c_port_name[ port_name_len ] = '\0';


    *ierr = MPI_Unpublish_name( c_service_name, *info, c_port_name );

    free( c_service_name );
    free( c_port_name );
}
#endif

#else /* !NEED_F2C_CONV */

/**
 * Manual measurement wrapper for MPI_Comm_spawn_multiple
 * @ingroup manual_wrapper
 * @ingroup interface_fortran
 * @ingroup mpi_version_2
 * @ingroup mpi_enabled_proc
 */
void
FSUB
(
    MPI_Comm_spawn_multiple
) ( MPI_Fint * count,
    char* array_of_commands[],
    char* array_of_argv[],
    MPI_Fint * array_of_maxprocs,
    MPI_Fint * array_of_info,
    MPI_Fint * root,
    MPI_Fint * comm,
    MPI_Fint * intercomm,
    MPI_Fint * array_of_errcodes,
    MPI_Fint * ierr,
    MPI_Fint array_of_commands_len[] )
{
    MPI_Fint  i;
    char*     c_array_of_commands[] = NULL;
    char*     c_array_of_argv[]     = NULL;
    MPI_Info* c_array_of_info       = NULL;

    /* allocate and fill local array_of_commands */
    c_array_of_commands = ( char** )malloc( count * sizeof( char* ) );
    if ( !c_array_of_commands )
    {
        exit( EXIT_FAILURE );
    }
    for ( i = 0; i < count; ++i )
    {
        c_array_of_commands[ i ] =
            ( char* )malloc( ( array_of_commands_len[ i ] + 1 ) * sizeof( char ) );
        if ( !( c_array_of_commands[ i ] ) )
        {
            exit( EXIT_FAILURE );
        }
        strncpy( c_array_of_commands[ i ], array_of_commands[ i ],
                 array_of_commands_len[ i ] );
        /* null terminate character array */
        c_array_of_commands[ i ][ array_of_commands_len[ i ] ] = '\0';
    }

    /* allocate and fill local array_of_argv */
    c_array_of_argv = ( char** )malloc( count * sizeof( char* ) );
    if ( !c_array_of_argv )
    {
        exit( EXIT_FAILURE );
    }
    for ( i = 0; i < count; ++i )
    {
        c_array_of_argv[ i ] =
            ( char* )malloc( ( array_of_argv_len[ i ] + 1 ) * sizeof( char ) );
        if ( !( c_array_of_argv[ i ] ) )
        {
            exit( EXIT_FAILURE );
        }
        strncpy( c_array_of_argv[ i ], array_of_argv[ i ],
                 array_of_argv_len[ i ] );
        /* null terminate character array */
        c_array_of_argv[ i ][ array_of_argv_len[ i ] ] = '\0';
    }

    /* allocate and fill local array_of_info */
    c_array_of_info = ( MPI_Info* )malloc( count * sizeof( MPI_Info ) );
    if ( !( c_array_of_info ) )
    {
        exit( EXIT_FAILURE );
    }
    for ( i = 0; i < count; ++i )
    {
        c_array_of_info = PMPI_Info_f2c( array_of_info[ i ] );
    }

    *ierr = MPI_Comm_spawn_multiple( *count, c_array_of_commands,
                                     c_array_of_argv, array_of_maxprocs,
                                     c_array_of_info, *root, *comm,
                                     intercomm, array_of_errcodes );

    for ( i = 0; i < count; ++i )
    {
        free( c_array_of_commands[ i ] );
        free( c_array_of_argv[ i ] );
    }
    free( c_array_of_commands );
    free( c_array_of_argv );
    free( c_array_of_info );
}

#if defined( HAS_MPI2_SPAWN ) && defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Close_port
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 */
void
FSUB
(
    MPI_Close_port
) ( char* port_name, int* ierr, int port_name_len )
{
    char* c_port_name = NULL;
    c_port_name = ( char* )malloc( ( port_name_len + 1 ) * sizeof( char ) );
    if ( !c_port_name )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_port_name, port_name, port_name_len );
    c_port_name[ port_name_len ] = '\0';

    *ierr = MPI_Close_port( port_name );
    free( c_port_name );
}
#endif
#if defined( HAS_MPI2_SPAWN ) && defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Comm_accept
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 */
void
FSUB
(
    MPI_Comm_accept
) ( char* port_name, MPI_Fint * info, MPI_Fint * root, MPI_Fint * comm, MPI_Fint * newcomm, int* ierr, int port_name_len )
{
    char* c_port_name = NULL;
    c_port_name = ( char* )malloc( ( port_name_len + 1 ) * sizeof( char ) );
    if ( !c_port_name )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_port_name, port_name, port_name_len );
    c_port_name[ port_name_len ] = '\0';

    MPI_Comm c_newcomm;
    *ierr = MPI_Comm_accept( port_name, PMPI_Info_f2c( *info ), *root, PMPI_Comm_f2c( *comm ), &c_newcomm );
    free( c_port_name );
    *newcomm = PMPI_Comm_c2f( c_newcomm );
}
#endif
#if defined( HAS_MPI2_SPAWN ) && defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Comm_connect
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 */
void
FSUB
(
    MPI_Comm_connect
) ( char* port_name, MPI_Fint * info, MPI_Fint * root, MPI_Fint * comm, MPI_Fint * newcomm, int* ierr, int port_name_len )
{
    char* c_port_name = NULL;
    c_port_name = ( char* )malloc( ( port_name_len + 1 ) * sizeof( char ) );
    if ( !c_port_name )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_port_name, port_name, port_name_len );
    c_port_name[ port_name_len ] = '\0';

    MPI_Comm c_newcomm;
    *ierr = MPI_Comm_connect( port_name, PMPI_Info_f2c( *info ), *root, PMPI_Comm_f2c( *comm ), &c_newcomm );
    free( c_port_name );
    *newcomm = PMPI_Comm_c2f( c_newcomm );
}
#endif
#if defined( HAS_MPI2_SPAWN ) && defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Comm_disconnect
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 */
void
FSUB
(
    MPI_Comm_disconnect
) ( MPI_Fint * comm, int* ierr )
{
    MPI_Comm c_comm = PMPI_Comm_f2c( *comm );
    *ierr = MPI_Comm_disconnect( &c_comm );
    *comm = PMPI_Comm_c2f( c_comm );
}
#endif
#if defined( HAS_MPI2_SPAWN ) && defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Comm_get_parent
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 */
void
FSUB
(
    MPI_Comm_get_parent
) ( MPI_Fint * parent, int* ierr )
{
    MPI_Comm c_parent;
    *ierr   = MPI_Comm_get_parent( &c_parent );
    *parent = PMPI_Comm_c2f( c_parent );
}
#endif
#if defined( HAS_MPI2_SPAWN ) && defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Comm_join
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 */
void
FSUB
(
    MPI_Comm_join
) ( MPI_Fint * fd, MPI_Fint * intercomm, int* ierr )
{
    MPI_Comm c_intercomm;
    *ierr      = MPI_Comm_join( *fd, &c_intercomm );
    *intercomm = PMPI_Comm_c2f( c_intercomm );
}
#endif
#if defined( HAS_MPI2_SPAWN ) && defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Comm_spawn
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 */
void
FSUB
(
    MPI_Comm_spawn
) ( char* command, char* argv, MPI_Fint * maxprocs, MPI_Fint * info, MPI_Fint * root, MPI_Fint * comm, MPI_Fint * intercomm, MPI_Fint * array_of_errcodes, int* ierr, int command_len, int argv_len )
{
    char* c_command = NULL;
    char* c_argv    = NULL;
    c_command = ( char* )malloc( ( command_len + 1 ) * sizeof( char ) );
    if ( !c_command )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_command, command, command_len );
    c_command[ command_len ] = '\0';

    c_argv = ( char* )malloc( ( argv_len + 1 ) * sizeof( char ) );
    if ( !c_argv )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_argv, argv, argv_len );
    c_argv[ argv_len ] = '\0';

    MPI_Comm c_intercomm;
    *ierr = MPI_Comm_spawn( command, argv, *maxprocs, PMPI_Info_f2c( *info ), *root, PMPI_Comm_f2c( *comm ), &c_intercomm, array_of_errcodes );
    free( c_command );
    free( c_argv );
    *intercomm = PMPI_Comm_c2f( c_intercomm );
}
#endif
#if defined( HAS_MPI2_SPAWN ) && defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Lookup_name
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 */
void
FSUB
(
    MPI_Lookup_name
) ( char* service_name, MPI_Fint * info, char* port_name, int* ierr, int service_name_len, int port_name_len )
{
    char* c_service_name = NULL;
    char* c_port_name    = NULL;
    c_service_name = ( char* )malloc( ( service_name_len + 1 ) * sizeof( char ) );
    if ( !c_service_name )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_service_name, service_name, service_name_len );
    c_service_name[ service_name_len ] = '\0';

    c_port_name = ( char* )malloc( ( port_name_len + 1 ) * sizeof( char ) );
    if ( !c_port_name )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_port_name, port_name, port_name_len );
    c_port_name[ port_name_len ] = '\0';

    *ierr = MPI_Lookup_name( service_name, PMPI_Info_f2c( *info ), port_name );
    free( c_service_name );
    free( c_port_name );
}
#endif
#if defined( HAS_MPI2_SPAWN ) && defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Open_port
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 */
void
FSUB
(
    MPI_Open_port
) ( MPI_Fint * info, char* port_name, int* ierr, int port_name_len )
{
    char* c_port_name = NULL;
    c_port_name = ( char* )malloc( ( port_name_len + 1 ) * sizeof( char ) );
    if ( !c_port_name )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_port_name, port_name, port_name_len );
    c_port_name[ port_name_len ] = '\0';

    *ierr = MPI_Open_port( PMPI_Info_f2c( *info ), port_name );
    free( c_port_name );
}
#endif
#if defined( HAS_MPI2_SPAWN ) && defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Publish_name
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 */
void
FSUB
(
    MPI_Publish_name
) ( char* service_name, MPI_Fint * info, char* port_name, int* ierr, int service_name_len, int port_name_len )
{
    char* c_service_name = NULL;
    char* c_port_name    = NULL;
    c_service_name = ( char* )malloc( ( service_name_len + 1 ) * sizeof( char ) );
    if ( !c_service_name )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_service_name, service_name, service_name_len );
    c_service_name[ service_name_len ] = '\0';

    c_port_name = ( char* )malloc( ( port_name_len + 1 ) * sizeof( char ) );
    if ( !c_port_name )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_port_name, port_name, port_name_len );
    c_port_name[ port_name_len ] = '\0';

    *ierr = MPI_Publish_name( service_name, PMPI_Info_f2c( *info ), port_name );
    free( c_service_name );
    free( c_port_name );
}
#endif
#if defined( HAS_MPI2_SPAWN ) && defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Unpublish_name
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup spawn
 */
void
FSUB
(
    MPI_Unpublish_name
) ( char* service_name, MPI_Fint * info, char* port_name, int* ierr, int service_name_len, int port_name_len )
{
    char* c_service_name = NULL;
    char* c_port_name    = NULL;
    c_service_name = ( char* )malloc( ( service_name_len + 1 ) * sizeof( char ) );
    if ( !c_service_name )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_service_name, service_name, service_name_len );
    c_service_name[ service_name_len ] = '\0';

    c_port_name = ( char* )malloc( ( port_name_len + 1 ) * sizeof( char ) );
    if ( !c_port_name )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_port_name, port_name, port_name_len );
    c_port_name[ port_name_len ] = '\0';

    *ierr = MPI_Unpublish_name( service_name, PMPI_Info_f2c( *info ), port_name );
    free( c_service_name );
    free( c_port_name );
}
#endif

#endif

/**
 * @}
 */
