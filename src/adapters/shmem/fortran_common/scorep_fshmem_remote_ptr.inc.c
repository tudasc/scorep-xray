#define shmem_ptr_L shmem_ptr
#define shmem_ptr_U SHMEM_PTR


/* *INDENT-OFF* */

#define SHMEM_FORTRAN_REMOTE_PTR( FUNCNAME )    \
    void*                                       \
    FSUB( shmem_ ## FUNCNAME ) ( void *target,  \
                                 int  *pe )     \
    {                                           \
        SCOREP_IN_MEASUREMENT_INCREMENT();      \
                                                \
        void* ret = shmem_ ## FUNCNAME( target, \
                                        *pe );  \
                                                \
        SCOREP_IN_MEASUREMENT_DECREMENT();      \
                                                \
        return ret;                             \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_PTR )
SHMEM_FORTRAN_REMOTE_PTR( ptr )
#endif
