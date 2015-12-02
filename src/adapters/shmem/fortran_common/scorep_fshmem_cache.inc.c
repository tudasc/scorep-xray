#define shmem_clear_cache_inv_L shmem_clear_cache_inv
#define shmem_clear_cache_inv_U SHMEM_CLEAR_CACHE_INV

#define shmem_set_cache_inv_L shmem_set_cache_inv
#define shmem_set_cache_inv_U SHMEM_SET_CACHE_INV

#define shmem_udcflush_L shmem_udcflush
#define shmem_udcflush_U SHMEM_UDCFLUSH

#define shmem_clear_cache_line_inv_L shmem_clear_cache_line_inv
#define shmem_clear_cache_line_inv_U SHMEM_CLEAR_CACHE_LINE_INV

#define shmem_set_cache_line_inv_L shmem_set_cache_line_inv
#define shmem_set_cache_line_inv_U SHMEM_SET_CACHE_LINE_INV

#define shmem_udcflush_line_L shmem_udcflush_line
#define shmem_udcflush_line_U SHMEM_UDCFLUSH_LINE


/* *INDENT-OFF* */

#define SHMEM_FORTRAN_DATA_CACHE_ROUTINE( FUNCNAME )    \
    void                                                \
    FSUB( FUNCNAME )( void )                            \
    {                                                   \
        SCOREP_IN_MEASUREMENT_INCREMENT();              \
        FUNCNAME ( );                                   \
        SCOREP_IN_MEASUREMENT_DECREMENT();              \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_CLEAR_CACHE_INV )
SHMEM_FORTRAN_DATA_CACHE_ROUTINE( shmem_clear_cache_inv )
#endif
#if SHMEM_HAVE_DECL( SHMEM_SET_CACHE_INV )
SHMEM_FORTRAN_DATA_CACHE_ROUTINE( shmem_set_cache_inv )
#endif
#if SHMEM_HAVE_DECL( SHMEM_UDCFLUSH )
SHMEM_FORTRAN_DATA_CACHE_ROUTINE( shmem_udcflush )
#endif


/* *INDENT-OFF* */

#define SHMEM_FORTRAN_DATA_CACHE_ROUTINE_WITH_ARG( FUNCNAME )   \
    void                                                        \
    FSUB( FUNCNAME )( void * target )                           \
    {                                                           \
        SCOREP_IN_MEASUREMENT_INCREMENT();                      \
        FUNCNAME ( target );                                    \
        SCOREP_IN_MEASUREMENT_DECREMENT();                      \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_CLEAR_CACHE_LINE_INV )
SHMEM_FORTRAN_DATA_CACHE_ROUTINE_WITH_ARG( shmem_clear_cache_line_inv )
#endif
#if SHMEM_HAVE_DECL( SHMEM_SET_CACHE_LINE_INV )
SHMEM_FORTRAN_DATA_CACHE_ROUTINE_WITH_ARG( shmem_set_cache_line_inv )
#endif
#if SHMEM_HAVE_DECL( SHMEM_UDCFLUSH_LINE )
SHMEM_FORTRAN_DATA_CACHE_ROUTINE_WITH_ARG( shmem_udcflush_line )
#endif
