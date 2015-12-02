#define my_pe_L my_pe
#define my_pe_U MY_PE

#define _my_pe_L _my_pe
#define _my_pe_U _MY_PE

#define shmem_my_pe_L shmem_my_pe
#define shmem_my_pe_U SHMEM_MY_PE

#define num_pes_L num_pes
#define num_pes_U NUM_PES

#define _num_pes_L _num_pes
#define _num_pes_U _NUM_PES

#define shmem_n_pes_L shmem_n_pes
#define shmem_n_pes_U SHMEM_N_PES

#define shmem_addr_accessible_L shmem_addr_accessible
#define shmem_addr_accessible_U SHMEM_ADDR_ACCESSIBLE

#define shmem_pe_accessible_L shmem_pe_accessible
#define shmem_pe_accessible_U SHMEM_PE_ACCESSIBLE



/* *INDENT-OFF* */

#define SHMEM_FORTRAN_QUERY_PE( F_FUNCNAME, C_FUNCNAME )    \
    int                                                     \
    FSUB( F_FUNCNAME )( void )                              \
    {                                                       \
        SCOREP_IN_MEASUREMENT_INCREMENT();                  \
                                                            \
        int ret;                                            \
        ret = C_FUNCNAME();                                 \
                                                            \
        SCOREP_IN_MEASUREMENT_DECREMENT();                  \
        return ret;                                         \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( _MY_PE )
SHMEM_FORTRAN_QUERY_PE( my_pe, _my_pe )
#endif
#if SHMEM_HAVE_DECL( _NUM_PES )
SHMEM_FORTRAN_QUERY_PE( num_pes, _num_pes )
#endif
#if SHMEM_HAVE_DECL( _MY_PE )
SHMEM_FORTRAN_QUERY_PE( _my_pe, _my_pe )
#endif
#if SHMEM_HAVE_DECL( _NUM_PES )
SHMEM_FORTRAN_QUERY_PE( _num_pes, _num_pes )
#endif
#if SHMEM_HAVE_DECL( SHMEM_MY_PE )
SHMEM_FORTRAN_QUERY_PE( shmem_my_pe, shmem_my_pe )
#endif
#if SHMEM_HAVE_DECL( SHMEM_N_PES )
SHMEM_FORTRAN_QUERY_PE( shmem_n_pes, shmem_n_pes )
#endif


/* *INDENT-OFF* */

#define SHMEM_FORTRAN_ADDR_ACCESSIBLE( F_FUNCNAME, C_FUNCNAME ) \
    int                                                         \
    FSUB( F_FUNCNAME )( void *addr,                             \
                        int  *pe )                              \
    {                                                           \
        SCOREP_IN_MEASUREMENT_INCREMENT();                      \
                                                                \
        int ret;                                                \
        ret= C_FUNCNAME( addr, *pe );                           \
                                                                \
        SCOREP_IN_MEASUREMENT_DECREMENT();                      \
        return ret;                                             \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_ADDR_ACCESSIBLE )
SHMEM_FORTRAN_ADDR_ACCESSIBLE( shmem_addr_accessible, shmem_addr_accessible )
#endif


/* *INDENT-OFF* */

#define SHMEM_FORTRAN_PE_ACCESSIBLE( F_FUNCNAME, C_FUNCNAME )   \
    int                                                         \
    FSUB( F_FUNCNAME )( int  *pe )                              \
    {                                                           \
        SCOREP_IN_MEASUREMENT_INCREMENT();                      \
                                                                \
        int ret;                                                \
        ret = C_FUNCNAME( *pe );                                \
                                                                \
        SCOREP_IN_MEASUREMENT_DECREMENT();                      \
        return ret;                                             \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_PE_ACCESSIBLE )
SHMEM_FORTRAN_PE_ACCESSIBLE( shmem_pe_accessible, shmem_pe_accessible )
#endif
