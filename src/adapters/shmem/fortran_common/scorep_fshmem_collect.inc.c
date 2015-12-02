#define shmem_collect4_L shmem_collect4
#define shmem_collect4_U SHMEM_COLLECT4

#define shmem_collect8_L shmem_collect8
#define shmem_collect8_U SHMEM_COLLECT8

#define shmem_collect32_L shmem_collect32
#define shmem_collect32_U SHMEM_COLLECT32

#define shmem_collect64_L shmem_collect64
#define shmem_collect64_U SHMEM_COLLECT64

#define shmem_fcollect4_L shmem_fcollect4
#define shmem_fcollect4_U SHMEM_FCOLLECT4

#define shmem_fcollect8_L shmem_fcollect8
#define shmem_fcollect8_U SHMEM_FCOLLECT8

#define shmem_fcollect32_L shmem_fcollect32
#define shmem_fcollect32_U SHMEM_FCOLLECT32

#define shmem_fcollect64_L shmem_fcollect64
#define shmem_fcollect64_U SHMEM_FCOLLECT64



/* *INDENT-OFF* */

#define SHMEM_FORTRAN_COLLECT_ROUTINE( F_FUNCNAME, C_FUNCNAME ) \
    void                                                        \
    FSUB( shmem_ ## F_FUNCNAME )( void       *target,           \
                                  const void *source,           \
                                  size_t     *nelems,           \
                                  int        *PE_start,         \
                                  int        *logPE_stride,     \
                                  int        *PE_size,          \
                                  int        *pSync )           \
    {                                                           \
        SCOREP_IN_MEASUREMENT_INCREMENT();                      \
        shmem_ ## C_FUNCNAME ( target,                          \
                               source,                          \
                               *nelems,                         \
                               *PE_start,                       \
                               *logPE_stride,                   \
                               *PE_size,                        \
                               ( long * ) pSync );              \
        SCOREP_IN_MEASUREMENT_DECREMENT();                      \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_COLLECT32 )
SHMEM_FORTRAN_COLLECT_ROUTINE( collect4,   collect32 )
SHMEM_FORTRAN_COLLECT_ROUTINE( collect32,  collect32 )
#endif
#if SHMEM_HAVE_DECL( SHMEM_COLLECT64 )
SHMEM_FORTRAN_COLLECT_ROUTINE( collect8,   collect64 )
SHMEM_FORTRAN_COLLECT_ROUTINE( collect64,  collect64 )
#endif
#if SHMEM_HAVE_DECL( SHMEM_FCOLLECT32 )
SHMEM_FORTRAN_COLLECT_ROUTINE( fcollect4,  fcollect32 )
SHMEM_FORTRAN_COLLECT_ROUTINE( fcollect32, fcollect32 )
#endif
#if SHMEM_HAVE_DECL( SHMEM_FCOLLECT64 )
SHMEM_FORTRAN_COLLECT_ROUTINE( fcollect8,  fcollect64 )
SHMEM_FORTRAN_COLLECT_ROUTINE( fcollect64, fcollect64 )
#endif
