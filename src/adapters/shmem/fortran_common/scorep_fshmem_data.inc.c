#define shmem_character_put_L shmem_character_put
#define shmem_character_put_U SHMEM_CHARACTER_PUT

// #define shmem_complex_put_L shmem_complex_put
// #define shmem_complex_put_U SHMEM_COMPLEX_PUT

#define shmem_double_put_L shmem_double_put
#define shmem_double_put_U SHMEM_DOUBLE_PUT

#define shmem_integer_put_L shmem_integer_put
#define shmem_integer_put_U SHMEM_INTEGER_PUT

#define shmem_logical_put_L shmem_logical_put
#define shmem_logical_put_U SHMEM_LOGICAL_PUT

#define shmem_real_put_L shmem_real_put
#define shmem_real_put_U SHMEM_REAL_PUT

// #define shmem_put_L shmem_put
// #define shmem_put_U SHMEM_PUT

#define shmem_put4_L shmem_put4
#define shmem_put4_U SHMEM_PUT4

#define shmem_put8_L shmem_put8
#define shmem_put8_U SHMEM_PUT8

#define shmem_put32_L shmem_put32
#define shmem_put32_U SHMEM_PUT32

#define shmem_put64_L shmem_put64
#define shmem_put64_U SHMEM_PUT64

#define shmem_put128_L shmem_put128
#define shmem_put128_U SHMEM_PUT128

#define shmem_putmem_L shmem_putmem
#define shmem_putmem_U SHMEM_PUTMEM

#define shmem_character_iput_L shmem_character_iput
#define shmem_character_iput_U SHMEM_CHARACTER_IPUT

#define shmem_double_iput_L shmem_double_iput
#define shmem_double_iput_U SHMEM_DOUBLE_IPUT

#define shmem_integer_iput_L shmem_integer_iput
#define shmem_integer_iput_U SHMEM_INTEGER_IPUT

#define shmem_logical_iput_L shmem_logical_iput
#define shmem_logical_iput_U SHMEM_LOGICAL_IPUT

#define shmem_real_iput_L shmem_real_iput
#define shmem_real_iput_U SHMEM_REAL_IPUT

// #define shmem_complex_iput_L shmem_complex_iput
// #define shmem_complex_iput_U SHMEM_COMPLEX_IPUT

// #define shmem_iput_L shmem_iput
// #define shmem_iput_U SHMEM_IPUT

#define shmem_iput4_L shmem_iput4
#define shmem_iput4_U SHMEM_IPUT4

#define shmem_iput8_L shmem_iput8
#define shmem_iput8_U SHMEM_IPUT8

#define shmem_iput32_L shmem_iput32
#define shmem_iput32_U SHMEM_IPUT32

#define shmem_iput64_L shmem_iput64
#define shmem_iput64_U SHMEM_IPUT64

#define shmem_iput128_L shmem_iput128
#define shmem_iput128_U SHMEM_IPUT128

#define shmem_character_get_L shmem_character_get
#define shmem_character_get_U SHMEM_CHARACTER_GET

// #define shmem_complex_get_L shmem_complex_get
// #define shmem_complex_get_U SHMEM_COMPLEX_GET

#define shmem_double_get_L shmem_double_get
#define shmem_double_get_U SHMEM_DOUBLE_GET

#define shmem_integer_get_L shmem_integer_get
#define shmem_integer_get_U SHMEM_INTEGER_GET

#define shmem_logical_get_L shmem_logical_get
#define shmem_logical_get_U SHMEM_LOGICAL_GET

#define shmem_real_get_L shmem_real_get
#define shmem_real_get_U SHMEM_REAL_GET

// #define shmem_get_L shmem_get
// #define shmem_get_U SHMEM_GET

#define shmem_get4_L shmem_get4
#define shmem_get4_U SHMEM_GET4

#define shmem_get8_L shmem_get8
#define shmem_get8_U SHMEM_GET8

#define shmem_get32_L shmem_get32
#define shmem_get32_U SHMEM_GET32

#define shmem_get64_L shmem_get64
#define shmem_get64_U SHMEM_GET64

#define shmem_get128_L shmem_get128
#define shmem_get128_U SHMEM_GET128

#define shmem_getmem_L shmem_getmem
#define shmem_getmem_U SHMEM_GETMEM

#define shmem_character_iget_L shmem_character_iget
#define shmem_character_iget_U SHMEM_CHARACTER_IGET

#define shmem_double_iget_L shmem_double_iget
#define shmem_double_iget_U SHMEM_DOUBLE_IGET

#define shmem_integer_iget_L shmem_integer_iget
#define shmem_integer_iget_U SHMEM_INTEGER_IGET

#define shmem_logical_iget_L shmem_logical_iget
#define shmem_logical_iget_U SHMEM_LOGICAL_IGET

#define shmem_real_iget_L shmem_real_iget
#define shmem_real_iget_U SHMEM_REAL_IGET

// #define shmem_complex_iget_L shmem_complex_iget
// #define shmem_complex_iget_U SHMEM_COMPLEX_IGET

// #define shmem_iget_L shmem_iget
// #define shmem_iget_U SHMEM_IGET

#define shmem_iget4_L shmem_iget4
#define shmem_iget4_U SHMEM_IGET4

#define shmem_iget8_L shmem_iget8
#define shmem_iget8_U SHMEM_IGET8

#define shmem_iget32_L shmem_iget32
#define shmem_iget32_U SHMEM_IGET32

#define shmem_iget64_L shmem_iget64
#define shmem_iget64_U SHMEM_IGET64

#define shmem_iget128_L shmem_iget128
#define shmem_iget128_U SHMEM_IGET128


/* *INDENT-OFF* */

#define SHMEM_FORTRAN_PUT( F_FUNCNAME, C_FUNCNAME, DATATYPE )       \
    void                                                            \
    FSUB( shmem_ ## F_FUNCNAME ## _put )( DATATYPE       *target,   \
                                          const DATATYPE *src,      \
                                          int            *size,     \
                                          int            *pe )      \
    {                                                               \
        SCOREP_IN_MEASUREMENT_INCREMENT();                          \
        shmem_ ## C_FUNCNAME ## _put( target,                       \
                                      src,                          \
                                      *size,                        \
                                      *pe );                        \
        SCOREP_IN_MEASUREMENT_DECREMENT();                          \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_CHAR_PUT )
SHMEM_FORTRAN_PUT( character, char,     char )
#endif
#if SHMEM_HAVE_DECL( SHMEM_DOUBLE_PUT )
SHMEM_FORTRAN_PUT( double,    double,   double )
#endif
#if SHMEM_HAVE_DECL( SHMEM_INT_PUT )
SHMEM_FORTRAN_PUT( integer,   int,      int )
#endif
#if SHMEM_HAVE_DECL( SHMEM_INT_PUT )
SHMEM_FORTRAN_PUT( logical,   int,      int )
#endif
#if SHMEM_HAVE_DECL( SHMEM_INT_PUT )
SHMEM_FORTRAN_PUT( real,      int,      int )
#endif
// #if SHMEM_HAVE_DECL( SHMEM_COMPLEXF_PUT )
// SHMEM_FORTRAN_PUT( complex,   complexf, COMPLEXIFY( float ) )
// #endif


/* *INDENT-OFF* */

#define SHMEM_FORTRAN_PUT_SIZE( SIZE, C_FUNCNAME, DATATYPE )    \
    void                                                        \
    FSUB( shmem_put ## SIZE )( DATATYPE       *target,          \
                               const DATATYPE *src,             \
                               int            *size,            \
                               int            *pe )             \
    {                                                           \
        SCOREP_IN_MEASUREMENT_INCREMENT();                      \
        shmem_## C_FUNCNAME ##_put( target,                     \
                                    src,                        \
                                    *size,                      \
                                    *pe);                       \
        SCOREP_IN_MEASUREMENT_DECREMENT();                      \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_INT_PUT )
SHMEM_FORTRAN_PUT_SIZE( 4,   int,        int )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONG_PUT )
SHMEM_FORTRAN_PUT_SIZE( 8,   long,       long )
#endif
#if SHMEM_HAVE_DECL( SHMEM_INT_PUT )
SHMEM_FORTRAN_PUT_SIZE( 32,  int,        int )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONG_PUT )
SHMEM_FORTRAN_PUT_SIZE( 64,  long,       long )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONGDOUBLE_PUT )
SHMEM_FORTRAN_PUT_SIZE( 128, longdouble, long double )
#endif


/* *INDENT-OFF* */

#define SHMEM_FORTRAN_PUT_MEM                   \
    void                                        \
    FSUB( shmem_putmem )( void       *target,   \
                          const void *src,      \
                          int        *size,     \
                          int        *pe )      \
    {                                           \
        SCOREP_IN_MEASUREMENT_INCREMENT();      \
        shmem_putmem( target,                   \
                      src,                      \
                      *size,                    \
                      *pe );                    \
        SCOREP_IN_MEASUREMENT_DECREMENT();      \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_PUTMEM )
SHMEM_FORTRAN_PUT_MEM
#endif


// #if SHMEM_HAVE_DECL( SHMEM_LONG_PUT ) && #defined( shmem_put )
// void
// FSUB( shmem_put )( long       *target,
//                    const long *source,
//                    int        *size,
//                    int        *pe )
// {
//     shmem_long_put( target,
//                     source,
//                     *size,
//                     *pe );
// }
// #endif


/* *INDENT-OFF* */

#define SHMEM_FORTRAN_IPUT( F_FUNCNAME, C_FUNCNAME, DATATYPE )      \
    void                                                            \
    FSUB( shmem_## F_FUNCNAME ## _iput )( DATATYPE       *target,   \
                                          const DATATYPE *src,      \
                                          int            *tst,      \
                                          int            *sst,      \
                                          int            *size,     \
                                          int            *pe)       \
    {                                                               \
        SCOREP_IN_MEASUREMENT_INCREMENT();                          \
        shmem_ ## C_FUNCNAME ## _iput( target,                      \
                                       src,                         \
                                       *tst,                        \
                                       *sst,                        \
                                       *size,                       \
                                       *pe );                       \
        SCOREP_IN_MEASUREMENT_DECREMENT();                          \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_CHAR_IPUT )
SHMEM_FORTRAN_IPUT( character, char,     char )
#endif
#if SHMEM_HAVE_DECL( SHMEM_DOUBLE_IPUT )
SHMEM_FORTRAN_IPUT( double,    double,   double )
#endif
#if SHMEM_HAVE_DECL( SHMEM_INT_IPUT )
SHMEM_FORTRAN_IPUT( integer,   int,      int )
#endif
#if SHMEM_HAVE_DECL( SHMEM_INT_IPUT )
SHMEM_FORTRAN_IPUT( logical,   int,      int )
#endif
#if SHMEM_HAVE_DECL( SHMEM_INT_IPUT )
SHMEM_FORTRAN_IPUT( real,      int,      int )
#endif
// #if SHMEM_HAVE_DECL( SHMEM_COMPLEXF_IPUT )
// SHMEM_FORTRAN_IPUT( complex,   complexf, COMPLEXIFY( float ) )
// #endif


/* *INDENT-OFF* */

#define SHMEM_FORTRAN_IPUT_SIZE( SIZE, C_FUNCNAME, DATATYPE )   \
    void                                                        \
    FSUB( shmem_iput ## SIZE )( DATATYPE       *target,         \
                                const DATATYPE *src,            \
                                int            *tst,            \
                                int            *sst,            \
                                int            *size,           \
                                int            *pe)             \
    {                                                           \
        SCOREP_IN_MEASUREMENT_INCREMENT();                      \
        shmem_ ## C_FUNCNAME ## _iput( target,                  \
                                       src,                     \
                                       *tst,                    \
                                       *sst,                    \
                                       *size,                   \
                                       *pe );                   \
        SCOREP_IN_MEASUREMENT_DECREMENT();                      \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_INT_IPUT )
SHMEM_FORTRAN_IPUT_SIZE( 4,   int,        int )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONG_IPUT )
SHMEM_FORTRAN_IPUT_SIZE( 8,   long,       long )
#endif
#if SHMEM_HAVE_DECL( SHMEM_INT_IPUT )
SHMEM_FORTRAN_IPUT_SIZE( 32,  int,        int )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONG_IPUT )
SHMEM_FORTRAN_IPUT_SIZE( 64,  long,       long )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONGDOUBLE_IPUT )
SHMEM_FORTRAN_IPUT_SIZE( 128, longdouble, long double )
#endif


/* *INDENT-OFF* */

#define SHMEM_FORTRAN_GET( F_FUNCNAME, C_FUNCNAME, DATATYPE )       \
    void                                                            \
    FSUB( shmem_ ## F_FUNCNAME ## _get )( DATATYPE       *target,   \
                                          const DATATYPE *src,      \
                                          int            *size,     \
                                          int            *pe)       \
    {                                                               \
        SCOREP_IN_MEASUREMENT_INCREMENT();                          \
        shmem_## C_FUNCNAME ## _get( target,                        \
                                     src,                           \
                                     *size,                         \
                                     *pe );                         \
        SCOREP_IN_MEASUREMENT_DECREMENT();                          \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_CHAR_GET )
SHMEM_FORTRAN_GET( character, char,     char )
#endif
#if SHMEM_HAVE_DECL( SHMEM_DOUBLE_GET )
SHMEM_FORTRAN_GET( double,    double,   double )
#endif
#if SHMEM_HAVE_DECL( SHMEM_INT_GET )
SHMEM_FORTRAN_GET( integer,   int,      int )
#endif
#if SHMEM_HAVE_DECL( SHMEM_INT_GET )
SHMEM_FORTRAN_GET( logical,   int,      int )
#endif
#if SHMEM_HAVE_DECL( SHMEM_INT_GET )
SHMEM_FORTRAN_GET( real,      int,      int )
#endif
// #if SHMEM_HAVE_DECL( SHMEM_COMPLEXF_GET )
// SHMEM_FORTRAN_GET( complex,   complexf, COMPLEXIFY( float ) )
// #endif


/* *INDENT-OFF* */

#define SHMEM_FORTRAN_GET_SIZE( SIZE, C_FUNCNAME, DATATYPE )    \
    void                                                        \
    FSUB( shmem_get ## SIZE )( DATATYPE       *target,          \
                               const DATATYPE *src,             \
                               int            *size,            \
                               int            *pe)              \
    {                                                           \
        SCOREP_IN_MEASUREMENT_INCREMENT();                      \
        shmem_## C_FUNCNAME ## _get( target,                    \
                                     src,                       \
                                     *size,                     \
                                     *pe );                     \
        SCOREP_IN_MEASUREMENT_DECREMENT();                      \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_INT_GET )
SHMEM_FORTRAN_GET_SIZE( 4,   int,        int )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONG_GET )
SHMEM_FORTRAN_GET_SIZE( 8,   long,       long )
#endif
#if SHMEM_HAVE_DECL( SHMEM_INT_GET )
SHMEM_FORTRAN_GET_SIZE( 32,  int,        int )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONG_GET )
SHMEM_FORTRAN_GET_SIZE( 64,  long,       long )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONGDOUBLE_GET )
SHMEM_FORTRAN_GET_SIZE( 128, longdouble, long double )
#endif


/* *INDENT-OFF* */

#define SHMEM_FORTRAN_GET_MEM                   \
    void                                        \
    FSUB( shmem_getmem )( void       *target,   \
                          const void *src,      \
                          int        *size,     \
                          int        *pe )      \
    {                                           \
        SCOREP_IN_MEASUREMENT_INCREMENT();      \
        shmem_getmem( target,                   \
                      src,                      \
                      *size,                    \
                      *pe );                    \
        SCOREP_IN_MEASUREMENT_DECREMENT();      \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_GETMEM )
SHMEM_FORTRAN_GET_MEM
#endif


// #if SHMEM_HAVE_DECL( SHMEM_LONG_GET ) && !defined( shmem_get )
// void
// FSUB( shmem_get )( long       *target,
//                    const long *source,
//                    int        *size,
//                    int        *pe )
// {
//     shmem_long_get( target,
//                     source,
//                     *size,
//                     *pe );
// }
// #endif


/* *INDENT-OFF* */

#define SHMEM_FORTRAN_IGET( F_FUNCNAME, C_FUNCNAME, DATATYPE )      \
    void                                                            \
    FSUB( shmem_ ## F_FUNCNAME ## _iget )( DATATYPE       *target,  \
                                           const DATATYPE *src,     \
                                           int            *tst,     \
                                           int            *sst,     \
                                           int            *size,    \
                                           int            *pe )     \
    {                                                               \
        SCOREP_IN_MEASUREMENT_INCREMENT();                          \
        shmem_## C_FUNCNAME ## _iget( target,                       \
                                      src,                          \
                                      *tst,                         \
                                      *sst,                         \
                                      *size,                        \
                                      *pe );                        \
        SCOREP_IN_MEASUREMENT_DECREMENT();                          \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_CHAR_IGET )
SHMEM_FORTRAN_IGET( character, char,     char )
#endif
#if SHMEM_HAVE_DECL( SHMEM_DOUBLE_IGET )
SHMEM_FORTRAN_IGET( double,    double,   double )
#endif
#if SHMEM_HAVE_DECL( SHMEM_INT_IGET )
SHMEM_FORTRAN_IGET( integer,   int,      int )
#endif
#if SHMEM_HAVE_DECL( SHMEM_INT_IGET )
SHMEM_FORTRAN_IGET( logical,   int,      int )
#endif
#if SHMEM_HAVE_DECL( SHMEM_INT_IGET )
SHMEM_FORTRAN_IGET( real,      int,      int )
#endif
// #if SHMEM_HAVE_DECL( SHMEM_COMPLEXF_IGET )
// SHMEM_FORTRAN_IGET( complex,   complexf, COMPLEXIFY( float ) )
// #endif


/* *INDENT-OFF* */

#define SHMEM_FORTRAN_IGET_SIZE( SIZE, C_FUNCNAME, DATATYPE )   \
    void                                                        \
    FSUB( shmem_iget ## SIZE )( DATATYPE       *target,         \
                                const DATATYPE *src,            \
                                int            *tst,            \
                                int            *sst,            \
                                int            *size,           \
                                int            *pe )            \
    {                                                           \
        SCOREP_IN_MEASUREMENT_INCREMENT();                      \
        shmem_## C_FUNCNAME ## _iget( target,                   \
                                      src,                      \
                                      *tst,                     \
                                      *sst,                     \
                                      *size,                    \
                                      *pe );                    \
        SCOREP_IN_MEASUREMENT_DECREMENT();                      \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_INT_IGET )
SHMEM_FORTRAN_IGET_SIZE( 4,   int,        int )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONG_IGET )
SHMEM_FORTRAN_IGET_SIZE( 8,   long,       long )
#endif
#if SHMEM_HAVE_DECL( SHMEM_INT_IGET )
SHMEM_FORTRAN_IGET_SIZE( 32,  int,        int )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONG_IGET )
SHMEM_FORTRAN_IGET_SIZE( 64,  long,       long )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONGDOUBLE_IGET )
SHMEM_FORTRAN_IGET_SIZE( 128, longdouble, long double )
#endif
