#define shpalloc_L shpalloc
#define shpalloc_U SHPALLOC

#define shpdeallc_L shpdeallc
#define shpdeallc_U SHPDEALLC

#define shpclmove_L shpclmove
#define shpclmove_U SHPCLMOVE


/* *INDENT-OFF* */

#define SHMEM_FORTRAN_SHPALLOC( FUNCNAME )                                  \
    void                                                                    \
    SCOREP_LIBWRAP_FUNC_NAME( FSUB( FUNCNAME ) ) ( void **addr,             \
                                                   int  *length,            \
                                                   long *errcode,           \
                                                   int  *abort )            \
    {                                                                       \
        SCOREP_IN_MEASUREMENT_INCREMENT();                                  \
                                                                            \
        const int event_gen_active = SCOREP_SHMEM_IS_EVENT_GEN_ON;          \
                                                                            \
        if ( event_gen_active )                                             \
        {                                                                   \
            SCOREP_SHMEM_EVENT_GEN_OFF();                                   \
                                                                            \
            SCOREP_EnterWrappedRegion( scorep_shmem_region__ ## FUNCNAME ); \
        }                                                                   \
                                                                            \
        SCOREP_ENTER_WRAPPED_REGION();                                      \
        SCOREP_LIBWRAP_FUNC_CALL( FUNCNAME, ( addr, length,                 \
                                                  errcode, abort ) );       \
        SCOREP_EXIT_WRAPPED_REGION();                                       \
                                                                            \
        if ( event_gen_active )                                             \
        {                                                                   \
            SCOREP_ExitRegion( scorep_shmem_region__ ## FUNCNAME );         \
                                                                            \
            SCOREP_SHMEM_EVENT_GEN_ON();                                    \
        }                                                                   \
                                                                            \
        SCOREP_IN_MEASUREMENT_DECREMENT();                                  \
    }

/* *INDENT-ON* */

SHMEM_FORTRAN_SHPALLOC( shpalloc )


/* *INDENT-OFF* */

#define SHMEM_FORTRAN_SHPDEALLOC( FUNCNAME )                                \
    void                                                                    \
    SCOREP_LIBWRAP_FUNC_NAME( FSUB( FUNCNAME ) ) ( void **addr,             \
                                                   long *errcode,           \
                                                   int  *abort )            \
    {                                                                       \
        SCOREP_IN_MEASUREMENT_INCREMENT();                                  \
                                                                            \
        const int event_gen_active = SCOREP_SHMEM_IS_EVENT_GEN_ON;          \
                                                                            \
        if ( event_gen_active )                                             \
        {                                                                   \
            SCOREP_SHMEM_EVENT_GEN_OFF();                                   \
                                                                            \
            SCOREP_EnterWrappedRegion( scorep_shmem_region__ ## FUNCNAME ); \
        }                                                                   \
                                                                            \
        SCOREP_ENTER_WRAPPED_REGION();                                      \
        SCOREP_LIBWRAP_FUNC_CALL( FUNCNAME, ( addr, errcode,                \
                                                  abort ) );                \
        SCOREP_EXIT_WRAPPED_REGION();                                       \
                                                                            \
        if ( event_gen_active )                                             \
        {                                                                   \
            SCOREP_ExitRegion( scorep_shmem_region__ ## FUNCNAME );         \
                                                                            \
            SCOREP_SHMEM_EVENT_GEN_ON();                                    \
        }                                                                   \
                                                                            \
        SCOREP_IN_MEASUREMENT_DECREMENT();                                  \
    }

/* *INDENT-ON* */

SHMEM_FORTRAN_SHPDEALLOC( shpdealloc )


/* *INDENT-OFF* */

#define SHMEM_FORTRAN_SHPCLMOVE( FUNCNAME )                                 \
    void                                                                    \
    SCOREP_LIBWRAP_FUNC_NAME( FSUB( FUNCNAME ) ) ( void *addr,              \
                                                   int  *length,            \
                                                   long *errcode,           \
                                                   int  *abort )            \
    {                                                                       \
        SCOREP_IN_MEASUREMENT_INCREMENT();                                  \
                                                                            \
        const int event_gen_active = SCOREP_SHMEM_IS_EVENT_GEN_ON;          \
                                                                            \
        if ( event_gen_active )                                             \
        {                                                                   \
            SCOREP_SHMEM_EVENT_GEN_OFF();                                   \
                                                                            \
            SCOREP_EnterWrappedRegion( scorep_shmem_region__ ## FUNCNAME ); \
        }                                                                   \
                                                                            \
        SCOREP_ENTER_WRAPPED_REGION();                                      \
        SCOREP_LIBWRAP_FUNC_CALL( FUNCNAME, ( addr, length,                 \
                                                  errcode, abort ) );       \
        SCOREP_EXIT_WRAPPED_REGION();                                       \
                                                                            \
        if ( event_gen_active )                                             \
        {                                                                   \
            SCOREP_ExitRegion( scorep_shmem_region__ ## FUNCNAME );         \
                                                                            \
            SCOREP_SHMEM_EVENT_GEN_ON();                                    \
        }                                                                   \
                                                                            \
        SCOREP_IN_MEASUREMENT_DECREMENT();                                  \
    }

/* *INDENT-ON* */

SHMEM_FORTRAN_SHPCLMOVE( shpclmove )
