#define shmem_clear_lock_L shmem_clear_lock
#define shmem_clear_lock_U SHMEM_CLEAR_LOCK

#define shmem_set_lock_L shmem_set_lock
#define shmem_set_lock_U SHMEM_SET_LOCK

#define shmem_test_lock_L shmem_test_lock
#define shmem_test_lock_U SHMEM_TEST_LOCK



/* *INDENT-OFF* */

#define SHMEM_FORTRAN_LOCK_1( FUNCNAME )    \
    void                                    \
    FSUB( FUNCNAME )( long *lock )          \
    {                                       \
        SCOREP_IN_MEASUREMENT_INCREMENT();  \
        FUNCNAME( lock );                   \
        SCOREP_IN_MEASUREMENT_DECREMENT();  \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_CLEAR_LOCK )
SHMEM_FORTRAN_LOCK_1( shmem_clear_lock )
#endif
#if SHMEM_HAVE_DECL( SHMEM_SET_LOCK )
SHMEM_FORTRAN_LOCK_1( shmem_set_lock )
#endif


/* *INDENT-OFF* */

#define SHMEM_FORTRAN_LOCK_2( FUNCNAME )    \
    int                                     \
    FSUB( FUNCNAME )( long *lock )          \
    {                                       \
        SCOREP_IN_MEASUREMENT_INCREMENT();  \
                                            \
        int ret;                            \
        ret = FUNCNAME( lock );             \
                                            \
        SCOREP_IN_MEASUREMENT_DECREMENT();  \
        return ret;                         \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_TEST_LOCK )
SHMEM_FORTRAN_LOCK_2( shmem_test_lock )
#endif
