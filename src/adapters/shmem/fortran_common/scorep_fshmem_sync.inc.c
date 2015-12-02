#define shmem_int4_wait_L shmem_int4_wait
#define shmem_int4_wait_U SHMEM_INT4_WAIT

#define shmem_int8_wait_L shmem_int8_wait
#define shmem_int8_wait_U SHMEM_INT8_WAIT

#define shmem_wait_L shmem_wait
#define shmem_wait_U SHMEM_WAIT

#define shmem_int4_wait_until_L shmem_int4_wait_until
#define shmem_int4_wait_until_U SHMEM_INT4_WAIT_UNTIL

#define shmem_int8_wait_until_L shmem_int8_wait_until
#define shmem_int8_wait_until_U SHMEM_INT8_WAIT_UNTIL

#define shmem_wait_until_L shmem_wait_until
#define shmem_wait_until_U SHMEM_WAIT_UNTIL

#define shmem_fence_L shmem_fence
#define shmem_fence_U SHMEM_FENCE

#define shmem_quiet_L shmem_quiet
#define shmem_quiet_U SHMEM_QUIET

#define shmem_barrier_all_L shmem_barrier_all
#define shmem_barrier_all_U SHMEM_BARRIER_ALL

#define shmem_barrier_L shmem_barrier
#define shmem_barrier_U SHMEM_BARRIER



/* *INDENT-OFF* */

#define SHMEM_FORTRAN_P2P_WAIT( F_FUNCNAME, DATATYPE )  \
    void                                                \
    FSUB( shmem_ ## F_FUNCNAME )( DATATYPE *var,        \
                                  DATATYPE *value )     \
    {                                                   \
        SCOREP_IN_MEASUREMENT_INCREMENT();              \
                                                        \
        shmem_ ## DATATYPE ## _wait( var,               \
                                     *value );          \
                                                        \
        SCOREP_IN_MEASUREMENT_DECREMENT();              \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_INT_WAIT )
SHMEM_FORTRAN_P2P_WAIT( int4_wait, int )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONG_WAIT )
SHMEM_FORTRAN_P2P_WAIT( int8_wait, long )
#endif
/* @TODO: How to determine the default integer size in Fortran?
 * This information is needed to map shmem_wait correctly
 * to its corresponding C function. */
#if SHMEM_HAVE_DECL( SHMEM_LONG_WAIT )
SHMEM_FORTRAN_P2P_WAIT( wait,      long )
#endif


/* *INDENT-OFF* */

#define SHMEM_FORTRAN_P2P_WAIT_UNTIL( FUNCNAME, DATATYPE )  \
    void                                                    \
    FSUB( shmem_ ## FUNCNAME )( DATATYPE *var,              \
                                int      *cmp,              \
                                DATATYPE *value )           \
    {                                                       \
        SCOREP_IN_MEASUREMENT_INCREMENT();                  \
                                                            \
        shmem_ ## DATATYPE ## _wait_until( var,             \
                                           *cmp,            \
                                           *value );        \
                                                            \
        SCOREP_IN_MEASUREMENT_DECREMENT();                  \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_INT_WAIT_UNTIL )
SHMEM_FORTRAN_P2P_WAIT_UNTIL( int4_wait_until, int )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONG_WAIT_UNTIL )
SHMEM_FORTRAN_P2P_WAIT_UNTIL( int8_wait_until, long )
#endif
/* @TODO: How to determine the default integer size in Fortran?
 * This information is needed to map shmem_wait_until
 * correctly to its corresponding C function. */
#if SHMEM_HAVE_DECL( SHMEM_LONG_WAIT_UNTIL )
SHMEM_FORTRAN_P2P_WAIT_UNTIL( wait_until,      long )
#endif


/* *INDENT-OFF* */

#define SHMEM_FORTRAN_SYNC_ROUTINE_1( FUNCNAME )    \
    void                                            \
    FSUB( shmem_ ## FUNCNAME )( void )              \
    {                                               \
        SCOREP_IN_MEASUREMENT_INCREMENT();          \
                                                    \
        shmem_ ## FUNCNAME();                       \
                                                    \
        SCOREP_IN_MEASUREMENT_DECREMENT();          \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_FENCE )
SHMEM_FORTRAN_SYNC_ROUTINE_1( fence )
#endif
#if SHMEM_HAVE_DECL( SHMEM_QUIET )
SHMEM_FORTRAN_SYNC_ROUTINE_1( quiet )
#endif
#if SHMEM_HAVE_DECL( SHMEM_BARRIER_ALL )
SHMEM_FORTRAN_SYNC_ROUTINE_1( barrier_all )
#endif


/* *INDENT-OFF* */

#define SHMEM_FORTRAN_BARRIER_ROUTINE( FUNCNAME )   \
    void                                            \
    FSUB (shmem_## FUNCNAME )( int *PE_start,       \
                               int *logPE_stride,   \
                               int *PE_size,        \
                               int *pSync )         \
    {                                               \
        SCOREP_IN_MEASUREMENT_INCREMENT();          \
                                                    \
        shmem_ ## FUNCNAME( *PE_start,              \
                            *logPE_stride,          \
                            *PE_size,               \
                            ( long * ) pSync );     \
                                                    \
        SCOREP_IN_MEASUREMENT_DECREMENT();          \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_BARRIER )
SHMEM_FORTRAN_BARRIER_ROUTINE( barrier )
#endif
