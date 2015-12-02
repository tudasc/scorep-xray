#define shmem_broadcast4_L shmem_broadcast4
#define shmem_broadcast4_U SHMEM_BROADCAST4

#define shmem_broadcast8_L shmem_broadcast8
#define shmem_broadcast8_U SHMEM_BROADCAST8

#define shmem_broadcast32_L shmem_broadcast32
#define shmem_broadcast32_U SHMEM_BROADCAST32

#define shmem_broadcast64_L shmem_broadcast64
#define shmem_broadcast64_U SHMEM_BROADCAST64



/* *INDENT-OFF* */

#define SHMEM_FORTRAN_BROADCAST_ROUTINE( F_FUNCNAME, C_FUNCNAME )   \
    void                                                            \
    FSUB( shmem_ ## F_FUNCNAME )( void       *target,               \
                                  const void *source,               \
                                  int        *nelems,               \
                                  int        *PE_root,              \
                                  int        *PE_start,             \
                                  int        *logPE_stride,         \
                                  int        *PE_size,              \
                                  int        *pSync )               \
    {                                                               \
        SCOREP_IN_MEASUREMENT_INCREMENT();                          \
        shmem_ ## C_FUNCNAME( target,                               \
                              source,                               \
                              *nelems,                              \
                              *PE_root,                             \
                              *PE_start,                            \
                              *logPE_stride,                        \
                              *PE_size,                             \
                              ( long * ) pSync );                   \
        SCOREP_IN_MEASUREMENT_DECREMENT();                          \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_BROADCAST32 )
SHMEM_FORTRAN_BROADCAST_ROUTINE( broadcast4,  broadcast32 )
SHMEM_FORTRAN_BROADCAST_ROUTINE( broadcast32, broadcast32 )
#endif
#if SHMEM_HAVE_DECL( SHMEM_BROADCAST64 )
SHMEM_FORTRAN_BROADCAST_ROUTINE( broadcast8,  broadcast64 )
SHMEM_FORTRAN_BROADCAST_ROUTINE( broadcast64, broadcast64 )
#endif
