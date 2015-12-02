#define shmem_clear_event_L shmem_clear_event
#define shmem_clear_event_U SHMEM_CLEAR_EVENT

#define shmem_set_event_L shmem_set_event
#define shmem_set_event_U SHMEM_SET_EVENT

#define shmem_wait_event_L shmem_wait_event
#define shmem_wait_event_U SHMEM_WAIT_EVENT

#define shmem_test_event_L shmem_test_event
#define shmem_test_event_U SHMEM_TEST_EVENT


/* *INDENT-OFF* */

#define SHMEM_FORTRAN_EVENT( FUNCNAME )    \
    void                                   \
    FSUB( FUNCNAME ) ( long * event )      \
    {                                      \
        SCOREP_IN_MEASUREMENT_INCREMENT(); \
        FUNCNAME( event );                 \
        SCOREP_IN_MEASUREMENT_DECREMENT(); \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_CLEAR_EVENT )
SHMEM_FORTRAN_EVENT( shmem_clear_event )
#endif
#if SHMEM_HAVE_DECL( SHMEM_SET_EVENT )
SHMEM_FORTRAN_EVENT( shmem_set_event )
#endif
#if SHMEM_HAVE_DECL( SHMEM_WAIT_EVENT )
SHMEM_FORTRAN_EVENT( shmem_wait_event )
#endif


/* *INDENT-OFF* */

#define SHMEM_FORTRAN_EVENT_WITH_RET_VAL( FUNCNAME )    \
    int                                                 \
    FSUB( FUNCNAME ) ( long * event )                   \
    {                                                   \
        SCOREP_IN_MEASUREMENT_INCREMENT();              \
                                                        \
        int ret;                                        \
        ret = FUNCNAME( event );                        \
                                                        \
        SCOREP_IN_MEASUREMENT_DECREMENT();              \
        return ret;                                     \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_TEST_EVENT )
SHMEM_FORTRAN_EVENT_WITH_RET_VAL( shmem_test_event )
#endif
