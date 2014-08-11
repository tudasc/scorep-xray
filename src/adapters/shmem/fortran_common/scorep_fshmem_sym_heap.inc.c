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
                                                                            \
        if ( SCOREP_SHMEM_IS_EVENT_GEN_ON )                                 \
        {                                                                   \
            SCOREP_SHMEM_EVENT_GEN_OFF();                                   \
                                                                            \
            SCOREP_EnterRegion( scorep_shmem_region__ ## FUNCNAME );        \
                                                                            \
            SCOREP_LIBWRAP_FUNC_CALL( lw, FUNCNAME, ( addr, length,         \
                                                      errcode, abort ) );   \
                                                                            \
            SCOREP_ExitRegion( scorep_shmem_region__ ## FUNCNAME );         \
                                                                            \
            SCOREP_SHMEM_EVENT_GEN_ON();                                    \
        }                                                                   \
        else                                                                \
        {                                                                   \
            SCOREP_LIBWRAP_FUNC_CALL( lw, FUNCNAME, ( addr, length,         \
                                                      errcode, abort ) );   \
        }                                                                   \
                                                                            \
        return ret;                                                         \
    }

/* *INDENT-ON* */

SHMEM_FORTRAN_SHPALLOC( shpalloc )


/* *INDENT-OFF* */

#define SHMEM_FORTRAN_SHPDEALLOC( FUNCNAME )                                \
    void                                                                    \
    SCOREP_LIBWRAP_FUNC_NAME( FSUB( FUNCNAME ) ) ( void **addr,             \
                                                   long *errcode,           \
                                                   int  *abort )            \
                                                                            \
        if ( SCOREP_SHMEM_IS_EVENT_GEN_ON )                                 \
        {                                                                   \
            SCOREP_SHMEM_EVENT_GEN_OFF();                                   \
                                                                            \
            SCOREP_EnterRegion( scorep_shmem_region__ ## FUNCNAME );        \
                                                                            \
            SCOREP_LIBWRAP_FUNC_CALL( lw, FUNCNAME, ( addr, errcode,        \
                                                      abort ) );            \
                                                                            \
            SCOREP_ExitRegion( scorep_shmem_region__ ## FUNCNAME );         \
                                                                            \
            SCOREP_SHMEM_EVENT_GEN_ON();                                    \
        }                                                                   \
        else                                                                \
        {                                                                   \
            SCOREP_LIBWRAP_FUNC_CALL( lw, FUNCNAME, ( addr, errcode,        \
                                                      abort ) );            \
        }                                                                   \
                                                                            \
        return ret;                                                         \
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
                                                                            \
        if ( SCOREP_SHMEM_IS_EVENT_GEN_ON )                                 \
        {                                                                   \
            SCOREP_SHMEM_EVENT_GEN_OFF();                                   \
                                                                            \
            SCOREP_EnterRegion( scorep_shmem_region__ ## FUNCNAME );        \
                                                                            \
            SCOREP_LIBWRAP_FUNC_CALL( lw, FUNCNAME, ( addr, length,         \
                                                      errcode, abort ) );   \
                                                                            \
            SCOREP_ExitRegion( scorep_shmem_region__ ## FUNCNAME );         \
                                                                            \
            SCOREP_SHMEM_EVENT_GEN_ON();                                    \
        }                                                                   \
        else                                                                \
        {                                                                   \
            SCOREP_LIBWRAP_FUNC_CALL( lw, FUNCNAME, ( addr, length,         \
                                                      errcode, abort ) );   \
        }                                                                   \
                                                                            \
        return ret;                                                         \
    }

/* *INDENT-ON* */

SHMEM_FORTRAN_SHPCLMOVE( shpclmove )
