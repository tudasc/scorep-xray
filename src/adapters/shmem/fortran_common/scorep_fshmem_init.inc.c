#define start_pes_L start_pes
#define start_pes_U START_PES

#define shmem_init_L shmem_init
#define shmem_init_U SHMEM_INIT

#define shmem_finalize_L shmem_finalize
#define shmem_finalize_U SHMEM_FINALIZE


/* *INDENT-OFF* */

#define SHMEM_FORTRAN_INITIALIZATION_WITH_ARG( FUNCNAME )   \
    void                                                    \
    FSUB( FUNCNAME )( int *npes )                           \
    {                                                       \
        SCOREP_IN_MEASUREMENT_INCREMENT();                  \
        FUNCNAME( *npes );                                  \
        SCOREP_IN_MEASUREMENT_DECREMENT();                  \
    }

/* *INDENT-ON* */


#if SHMEM_HAVE_DECL( START_PES )
SHMEM_FORTRAN_INITIALIZATION_WITH_ARG( start_pes )
#endif


/* *INDENT-OFF* */

#define SHMEM_FORTRAN_INITIALIZATION( FUNCNAME )    \
    void                                            \
    FSUB( FUNCNAME )( void )                        \
    {                                               \
        SCOREP_IN_MEASUREMENT_INCREMENT();          \
        FUNCNAME();                                 \
        SCOREP_IN_MEASUREMENT_DECREMENT();          \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_INIT )
SHMEM_FORTRAN_INITIALIZATION( shmem_init )
#endif


/* *INDENT-OFF* */

#define SHMEM_FORTRAN_FINALIZATION( FUNCNAME )  \
    void                                        \
    FSUB( FUNCNAME )( void )                    \
    {                                           \
        SCOREP_IN_MEASUREMENT_INCREMENT();      \
        FUNCNAME();                             \
        SCOREP_IN_MEASUREMENT_DECREMENT();      \
    }

/* *INDENT-ON* */

/*
 * Please note that 'shmem_finalize' is not part of the OpenSHMEM standard 1.0.
 * However, we need this function call to write events like RMA window destruction.
 * That's why we always wrap 'shmem_finalize'.
 * The Score-P measurement system always implements this function.
 * If the SHMEM implementation also provides 'shmem_finalize' we will pass this call to the SHMEM library.
 */
SHMEM_FORTRAN_FINALIZATION( shmem_finalize )
