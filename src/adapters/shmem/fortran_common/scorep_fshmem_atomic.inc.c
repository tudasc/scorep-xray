#define shmem_int4_swap_L shmem_int4_swap
#define shmem_int4_swap_U SHMEM_INT4_SWAP

#define shmem_int8_swap_L shmem_int8_swap
#define shmem_int8_swap_U SHMEM_INT8_SWAP

#define shmem_real4_swap_L shmem_real4_swap
#define shmem_real4_swap_U SHMEM_REAL4_SWAP

#define shmem_real8_swap_L shmem_real8_swap
#define shmem_real8_swap_U SHMEM_REAL8_SWAP

#define shmem_swap_L shmem_swap
#define shmem_swap_U SHMEM_SWAP

#define shmem_int4_cswap_L shmem_int4_cswap
#define shmem_int4_cswap_U SHMEM_INT4_CSWAP

#define shmem_int8_cswap_L shmem_int8_cswap
#define shmem_int8_cswap_U SHMEM_INT8_CSWAP

#define shmem_int4_fadd_L shmem_int4_fadd
#define shmem_int4_fadd_U SHMEM_INT4_FADD

#define shmem_int8_fadd_L shmem_int8_fadd
#define shmem_int8_fadd_U SHMEM_INT8_FADD

#define shmem_int4_finc_L shmem_int4_finc
#define shmem_int4_finc_U SHMEM_INT4_FINC

#define shmem_int8_finc_L shmem_int8_finc
#define shmem_int8_finc_U SHMEM_INT8_FINC

#define shmem_int4_add_L shmem_int4_add
#define shmem_int4_add_U SHMEM_INT4_ADD

#define shmem_int8_add_L shmem_int8_add
#define shmem_int8_add_U SHMEM_INT8_ADD

#define shmem_int4_inc_L shmem_int4_inc
#define shmem_int4_inc_U SHMEM_INT4_INC

#define shmem_int8_inc_L shmem_int8_inc
#define shmem_int8_inc_U SHMEM_INT8_INC



/* *INDENT-OFF* */

#define SHMEM_FORTRAN_ATOMIC_ROUTINE_1( F_FUNCNAME, DATATYPE, C_FUNCNAME )  \
    DATATYPE                                                                \
    FSUB( shmem_ ## F_FUNCNAME )( DATATYPE *target,                         \
                                  DATATYPE *value,                          \
                                  int      *pe )                            \
    {                                                                       \
        SCOREP_IN_MEASUREMENT_INCREMENT();                                  \
                                                                            \
        DATATYPE ret;                                                       \
        ret = shmem_ ## C_FUNCNAME ( target,                                \
                                      *value,                               \
                                      *pe );                                \
                                                                            \
        SCOREP_IN_MEASUREMENT_DECREMENT();                                  \
        return ret;                                                         \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_INT_SWAP )
SHMEM_FORTRAN_ATOMIC_ROUTINE_1( int4_swap,  int,    int_swap )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONG_SWAP )
SHMEM_FORTRAN_ATOMIC_ROUTINE_1( int8_swap,  long,   long_swap )
#endif
#if SHMEM_HAVE_DECL( SHMEM_FLOAT_SWAP )
SHMEM_FORTRAN_ATOMIC_ROUTINE_1( real4_swap, float,  float_swap )
#endif
#if SHMEM_HAVE_DECL( SHMEM_DOUBLE_SWAP )
SHMEM_FORTRAN_ATOMIC_ROUTINE_1( real8_swap, double, double_swap )
#endif
/* @TODO: How to determine the default integer size in Fortran?
 * This information is needed to map shmem_swap correctly
 * to its corresponding C function. */
#if SHMEM_HAVE_DECL( SHMEM_LONG_SWAP )
SHMEM_FORTRAN_ATOMIC_ROUTINE_1( swap,       long,   long_swap )
#endif

#if SHMEM_HAVE_DECL( SHMEM_INT_FADD )
SHMEM_FORTRAN_ATOMIC_ROUTINE_1( int4_fadd,  int,    int_fadd )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONG_FADD )
SHMEM_FORTRAN_ATOMIC_ROUTINE_1( int8_fadd,  long,   long_fadd )
#endif


/* *INDENT-OFF* */

#define SHMEM_FORTRAN_ATOMIC_ADD_ROUTINE( F_DATATYPE, C_DATATYPE )  \
    void                                                            \
    FSUB( shmem_ ## F_DATATYPE ## _add )( C_DATATYPE *target,       \
                                          C_DATATYPE *value,        \
                                          int        *pe )          \
    {                                                               \
        SCOREP_IN_MEASUREMENT_INCREMENT();                          \
        shmem_ ## C_DATATYPE ## _add ( target,                      \
                                       *value,                      \
                                       *pe );                       \
        SCOREP_IN_MEASUREMENT_DECREMENT();                          \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_INT_ADD )
SHMEM_FORTRAN_ATOMIC_ADD_ROUTINE( int4, int )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONG_ADD )
SHMEM_FORTRAN_ATOMIC_ADD_ROUTINE( int8, long )
#endif


/* *INDENT-OFF* */

#define SHMEM_FORTRAN_ATOMIC_INCREMENT_ROUTINE( F_DATATYPE, C_DATATYPE )    \
    void                                                                    \
    FSUB( shmem_ ## F_DATATYPE ## _inc )( C_DATATYPE *target,               \
                                          int        *pe )                  \
    {                                                                       \
        SCOREP_IN_MEASUREMENT_INCREMENT();                                  \
        shmem_ ## C_DATATYPE ## _inc ( target,                              \
                                       *pe );                               \
        SCOREP_IN_MEASUREMENT_DECREMENT();                                  \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_INT_INC )
SHMEM_FORTRAN_ATOMIC_INCREMENT_ROUTINE( int4, int )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONG_INC )
SHMEM_FORTRAN_ATOMIC_INCREMENT_ROUTINE( int8, long )
#endif


/* *INDENT-OFF* */

#define SHMEM_FORTRAN_ATOMIC_CSWAP_ROUTINE( F_DATATYPE, C_DATATYPE )    \
    C_DATATYPE                                                          \
    FSUB( shmem_ ## F_DATATYPE ## _cswap )( C_DATATYPE *target,         \
                                            C_DATATYPE *cond,           \
                                            C_DATATYPE *value,          \
                                            int        *pe )            \
    {                                                                   \
        SCOREP_IN_MEASUREMENT_INCREMENT();                              \
                                                                        \
        C_DATATYPE ret;                                                 \
        ret = shmem_ ## C_DATATYPE ## _cswap ( target,                  \
                                                *cond,                  \
                                                *value,                 \
                                                *pe );                  \
                                                                        \
        SCOREP_IN_MEASUREMENT_DECREMENT();                              \
        return ret;                                                     \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_INT_CSWAP )
SHMEM_FORTRAN_ATOMIC_CSWAP_ROUTINE( int4, int )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONG_CSWAP )
SHMEM_FORTRAN_ATOMIC_CSWAP_ROUTINE( int8, long )
#endif


/* *INDENT-OFF* */

#define SHMEM_FORTRAN_ATOMIC_FETCH_INC_ROUTINE( F_DATATYPE, C_DATATYPE )    \
    C_DATATYPE                                                              \
    FSUB( shmem_ ## F_DATATYPE ## _finc )( C_DATATYPE *target,              \
                                           int        *pe )                 \
    {                                                                       \
        SCOREP_IN_MEASUREMENT_INCREMENT();                                  \
                                                                            \
        C_DATATYPE ret;                                                     \
        ret = shmem_ ## C_DATATYPE ## _finc ( target,                       \
                                               *pe );                       \
                                                                            \
        SCOREP_IN_MEASUREMENT_DECREMENT();                                  \
        return ret;                                                         \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_INT_FINC )
SHMEM_FORTRAN_ATOMIC_FETCH_INC_ROUTINE( int4, int )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONG_FINC )
SHMEM_FORTRAN_ATOMIC_FETCH_INC_ROUTINE( int8, long )
#endif
