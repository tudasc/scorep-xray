#ifndef SCOREP_SHMEM_PROCESS_FUNC
#error Macro 'SCOREP_SHMEM_PROCESS_FUNC' was not defined
#endif

/*
 * Initialization routines
 */
#if SHMEM_HAVE_DECL( START_PES )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     void,        start_pes, ( int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_INIT )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     void,        shmem_init, ( void ) )
#endif
#if HAVE( SHMEM_INIT_THREAD_CRAY_ONE_ARG_VARIANT )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     int,         shmem_init_thread, ( int ) )
#elif HAVE( SHMEM_INIT_THREAD_CRAY_TWO_ARGS_VARIANT )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     void,        shmem_init_thread, ( int, int* ) )
#endif

/*
 * Query routines
 */
#if SHMEM_HAVE_DECL( MY_PE )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     int,         my_pe, ( void ) )
#endif
#if SHMEM_HAVE_DECL( _MY_PE )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     int,         _my_pe, ( void ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_MY_PE )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     int,         shmem_my_pe, ( void ) )
#endif
#if SHMEM_HAVE_DECL( NUM_PES )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     int,         num_pes, ( void ) )
#endif
#if SHMEM_HAVE_DECL( _NUM_PES )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     int,         _num_pes, ( void ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_N_PES )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     int,         shmem_n_pes, ( void ) )
#endif

/*
 * Data cache routines
 */
#if SHMEM_HAVE_DECL( SHMEM_CLEAR_CACHE_INV )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     void,         shmem_clear_cache_inv, ( void ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_SET_CACHE_INV )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     void,         shmem_set_cache_inv, ( void ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_UDCFLUSH )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     void,         shmem_udcflush, ( void ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_CLEAR_CACHE_LINE_INV )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     void,         shmem_clear_cache_line_inv, ( void* ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_SET_CACHE_LINE_INV )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     void,         shmem_set_cache_line_inv, ( void* ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_UDCFLUSH_LINE )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     void,         shmem_udcflush_line, ( void* ) )
#endif

/*
 * Accessability routines
 */
#if SHMEM_HAVE_DECL( SHMEM_PE_ACCESSIBLE )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     int,         shmem_pe_accessible, ( int pe ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_ADDR_ACCESSIBLE ) && defined( SCOREP_SHMEM_ADDR_ACCESSIBLE_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     int,         shmem_addr_accessible, SCOREP_SHMEM_ADDR_ACCESSIBLE_PROTO_ARGS )
#endif

/*
 * Symmetric heap routines
 */
#if SHMEM_HAVE_DECL( SHMALLOC )
SCOREP_SHMEM_PROCESS_FUNC( ALLOCATE,     void*,       shmalloc, ( size_t size ) )
#endif
#if SHMEM_HAVE_DECL( SHMEMALIGN )
SCOREP_SHMEM_PROCESS_FUNC( ALLOCATE,     void*,       shmemalign, ( size_t, size_t ) )
#endif
#if SHMEM_HAVE_DECL( SHREALLOC )
SCOREP_SHMEM_PROCESS_FUNC( REALLOCATE,   void*,       shrealloc, ( void* , size_t ) )
#endif
#if SHMEM_HAVE_DECL( SHFREE )
SCOREP_SHMEM_PROCESS_FUNC( DEALLOCATE,   void,        shfree, ( void* ptr ) )
#endif

/*
 * Symmetric heap routines (Fortran)
 */
#if SHMEM_HAVE_DECL( SHPALLOC )
SCOREP_SHMEM_PROCESS_FUNC( ALLOCATE,     void,        shpalloc, ( void**, int*, long*, int* ) )
#endif
#if SHMEM_HAVE_DECL( SHPDEALLC )
SCOREP_SHMEM_PROCESS_FUNC( DEALLOCATE,   void*,       shpdeallc, ( void**, long*, int* ) )
#endif
#if SHMEM_HAVE_DECL( SHPCLMOVE )
SCOREP_SHMEM_PROCESS_FUNC( REALLOCATE,   void*,       shpclmove, ( int*, int*, long*, int* ) )
#endif

/*
 * Remote pointer operations
 */
#if SHMEM_HAVE_DECL( SHMEM_PTR ) && defined( SCOREP_SHMEM_PTR_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     void*,       shmem_ptr, SCOREP_SHMEM_PTR_PROTO_ARGS )
#endif

/*
 * Elemental put routines
 */
#if SHMEM_HAVE_DECL( SHMEM_CHAR_P )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_char_p, ( char* , char , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_SHORT_P )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_short_p,( short* , short , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_INT_P )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_int_p, ( int* , int , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONG_P )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_long_p, ( long* , long , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_FLOAT_P )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_float_p, ( float* , float , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_DOUBLE_P )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_double_p, ( double* addr, double , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONGLONG_P )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_longlong_p, ( long long* , long long , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONGDOUBLE_P )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_longdouble_p, ( long double* , long double , int ) )
#endif

/*
 * Block data put routines
 */
#if SHMEM_HAVE_DECL( SHMEM_CHAR_PUT )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_char_put, ( char* , const char* , size_t , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_SHORT_PUT )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_short_put, ( short* , const short* , size_t , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_INT_PUT )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_int_put, ( int* , const int* , size_t , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONG_PUT )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_long_put, ( long* , const long* src, size_t , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_FLOAT_PUT )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_float_put, ( float* , const float* , size_t , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_DOUBLE_PUT )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_double_put, ( double* , const double* , size_t , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONGLONG_PUT )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_longlong_put, ( long long* , const long long* src, size_t , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONGDOUBLE_PUT ) && defined( SCOREP_SHMEM_LONGDOUBLE_PUT_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_longdouble_put, SCOREP_SHMEM_LONGDOUBLE_PUT_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_PUT16 )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_put16, ( void* , const void* , size_t , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_PUT32 )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_put32, ( void* , const void* , size_t , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_PUT64 )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_put64, ( void* , const void* , size_t , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_PUT128 )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_put128, ( void* , const void* , size_t , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_PUTMEM )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_putmem, ( void* , const void* , size_t , int ) )
#endif

/*
 * Strided put routines
 */
#if SHMEM_HAVE_DECL( SHMEM_SHORT_IPUT )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_short_iput, ( short* , const short* , ptrdiff_t , ptrdiff_t , size_t , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_INT_IPUT )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_int_iput, ( int* , const int* , ptrdiff_t , ptrdiff_t , size_t , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONG_IPUT )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_long_iput, ( long* , const long* , ptrdiff_t , ptrdiff_t , size_t , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_FLOAT_IPUT )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_float_iput, ( float* , const float* , ptrdiff_t , ptrdiff_t , size_t , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_DOUBLE_IPUT )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_double_iput, ( double* , const double* , ptrdiff_t , ptrdiff_t , size_t , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONGLONG_IPUT )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_longlong_iput, ( long long* , const long long* , ptrdiff_t , ptrdiff_t , size_t , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONGDOUBLE_IPUT ) && defined( SCOREP_SHMEM_LONGDOUBLE_IPUT_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_longdouble_iput, SCOREP_SHMEM_LONGDOUBLE_IPUT_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_IPUT16 )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_iput16, ( void* , const void* , ptrdiff_t , ptrdiff_t , size_t , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_IPUT32 )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_iput32, ( void* , const void* , ptrdiff_t , ptrdiff_t , size_t , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_IPUT64 )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_iput64, ( void* , const void* , ptrdiff_t , ptrdiff_t , size_t , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_IPUT128 )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_iput128, ( void* , const void* , ptrdiff_t , ptrdiff_t , size_t , int ) )
#endif

/*
 * Elemental get routines
 */
#if SHMEM_HAVE_DECL( SHMEM_CHAR_G ) && defined( SCOREP_SHMEM_CHAR_G_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          char,        shmem_char_g, SCOREP_SHMEM_CHAR_G_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_SHORT_G ) && defined( SCOREP_SHMEM_SHORT_G_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          short,       shmem_short_g, SCOREP_SHMEM_SHORT_G_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_INT_G ) && defined( SCOREP_SHMEM_INT_G_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          int,         shmem_int_g, SCOREP_SHMEM_INT_G_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONG_G ) && defined( SCOREP_SHMEM_LONG_G_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          long,        shmem_long_g, SCOREP_SHMEM_LONG_G_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_FLOAT_G ) && defined( SCOREP_SHMEM_FLOAT_G_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          float,       shmem_float_g, SCOREP_SHMEM_FLOAT_G_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_DOUBLE_G ) && defined( SCOREP_SHMEM_DOUBLE_G_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          double,      shmem_double_g, SCOREP_SHMEM_DOUBLE_G_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONGLONG_G ) && defined( SCOREP_SHMEM_LONGLONG_G_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          long long,   shmem_longlong_g, SCOREP_SHMEM_LONGLONG_G_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONGDOUBLE_G ) && defined( SCOREP_SHMEM_LONGDOUBLE_G_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          long double, shmem_longdouble_g, SCOREP_SHMEM_LONGDOUBLE_G_PROTO_ARGS )
#endif

/*
 * Block data get routines
 */
#if SHMEM_HAVE_DECL( SHMEM_CHAR_GET )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_char_get, ( char* , const char* , size_t , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_SHORT_GET )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_short_get, ( short* , const short* , size_t , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_INT_GET )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_int_get, ( int* , const int* , size_t , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONG_GET )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_long_get, ( long* , const long* , size_t , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_FLOAT_GET )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_float_get, ( float* , const float* , size_t , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_DOUBLE_GET )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_double_get, ( double* , const double* src, size_t , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONGLONG_GET )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_longlong_get, ( long long* , const long long* , size_t , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONGDOUBLE_GET ) && defined( SCOREP_SHMEM_LONGDOUBLE_GET_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_longdouble_get, SCOREP_SHMEM_LONGDOUBLE_GET_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_GET16 )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_get16, ( void* , const void* , size_t , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_GET32 )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_get32, ( void* , const void* , size_t , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_GET64 )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_get64, ( void* , const void* , size_t , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_GET128 )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_get128, ( void* , const void* , size_t , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_GETMEM )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_getmem, ( void* , const void* , size_t , int ) )
#endif

/*
 * Strided get routines
 */
#if SHMEM_HAVE_DECL( SHMEM_SHORT_IGET )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_short_iget, ( short* , const short* , ptrdiff_t , ptrdiff_t , size_t , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_INT_IGET )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_int_iget, ( int* , const int* , ptrdiff_t , ptrdiff_t , size_t , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONG_IGET )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_long_iget, ( long* , const long* , ptrdiff_t , ptrdiff_t , size_t , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_FLOAT_IGET )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_float_iget, ( float* , const float* , ptrdiff_t , ptrdiff_t , size_t , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_DOUBLE_IGET )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_double_iget,     ( double* , const double* , ptrdiff_t , ptrdiff_t , size_t , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONGLONG_IGET )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_longlong_iget, ( long long* , const long long* , ptrdiff_t , ptrdiff_t , size_t , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONGDOUBLE_IGET ) && defined( SCOREP_SHMEM_LONGDOUBLE_IGET_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_longdouble_iget, SCOREP_SHMEM_LONGDOUBLE_IGET_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_IGET16 )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_iget16, ( void* , const void* , ptrdiff_t , ptrdiff_t , size_t , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_IGET32 )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_iget32, ( void* , const void* , ptrdiff_t , ptrdiff_t , size_t , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_IGET64 )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_iget64, ( void* , const void* , ptrdiff_t , ptrdiff_t , size_t , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_IGET128 )
SCOREP_SHMEM_PROCESS_FUNC( RMA,          void,        shmem_iget128, ( void* , const void* , ptrdiff_t , ptrdiff_t , size_t , int ) )
#endif

/*
 * Atomic operations
 */
/* Atomic swap */
#if SHMEM_HAVE_DECL( SHMEM_SWAP )
SCOREP_SHMEM_PROCESS_FUNC( ATOMIC,       long,        shmem_swap, ( long* , long , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_SHORT_SWAP )
SCOREP_SHMEM_PROCESS_FUNC( ATOMIC,       short,       shmem_short_swap, ( short* , short , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_INT_SWAP )
SCOREP_SHMEM_PROCESS_FUNC( ATOMIC,       int,         shmem_int_swap, ( int* , int , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONG_SWAP )
SCOREP_SHMEM_PROCESS_FUNC( ATOMIC,       long,        shmem_long_swap, ( long* , long , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONGLONG_SWAP )
SCOREP_SHMEM_PROCESS_FUNC( ATOMIC,       long long,   shmem_longlong_swap, ( long long* , long long , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_FLOAT_SWAP )
SCOREP_SHMEM_PROCESS_FUNC( ATOMIC,       float,       shmem_float_swap, ( float* , float , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_DOUBLE_SWAP )
SCOREP_SHMEM_PROCESS_FUNC( ATOMIC,       double,      shmem_double_swap, ( double* , double , int ) )
#endif

/* Atomic conditional swap */
#if SHMEM_HAVE_DECL( SHMEM_SHORT_CSWAP )
SCOREP_SHMEM_PROCESS_FUNC( ATOMIC,       short,       shmem_short_cswap, ( short* , short , short , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_INT_CSWAP )
SCOREP_SHMEM_PROCESS_FUNC( ATOMIC,       int,         shmem_int_cswap, ( int* , int , int , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONG_CSWAP )
SCOREP_SHMEM_PROCESS_FUNC( ATOMIC,       long,        shmem_long_cswap,( long* , long , long  , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONGLONG_CSWAP )
SCOREP_SHMEM_PROCESS_FUNC( ATOMIC,       long long,   shmem_longlong_cswap, ( long long* , long long , long long , int ) )
#endif

/* Atomic Fetch and Add */
#if SHMEM_HAVE_DECL( SHMEM_SHORT_FADD )
SCOREP_SHMEM_PROCESS_FUNC( ATOMIC,       short,       shmem_short_fadd, ( short* , short , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_INT_FADD )
SCOREP_SHMEM_PROCESS_FUNC( ATOMIC,       int,         shmem_int_fadd, ( int* , int , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONG_FADD )
SCOREP_SHMEM_PROCESS_FUNC( ATOMIC,       long,        shmem_long_fadd, ( long* , long , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONGLONG_FADD )
SCOREP_SHMEM_PROCESS_FUNC( ATOMIC,       long long,   shmem_longlong_fadd, ( long long* , long long , int ) )
#endif

/* Atomic Fetch and Inc */
#if SHMEM_HAVE_DECL( SHMEM_SHORT_FINC )
SCOREP_SHMEM_PROCESS_FUNC( ATOMIC,       short,       shmem_short_finc, ( short* , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_INT_FINC )
SCOREP_SHMEM_PROCESS_FUNC( ATOMIC,       int,         shmem_int_finc, ( int* , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONG_FINC )
SCOREP_SHMEM_PROCESS_FUNC( ATOMIC,       long,        shmem_long_finc, ( long* , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONGLONG_FINC )
SCOREP_SHMEM_PROCESS_FUNC( ATOMIC,       long long,   shmem_longlong_finc, ( long long* , int ) )
#endif

/* Atomic Add */
#if SHMEM_HAVE_DECL( SHMEM_SHORT_ADD )
SCOREP_SHMEM_PROCESS_FUNC( ATOMIC,       void,        shmem_short_add, ( short* , short , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_INT_ADD )
SCOREP_SHMEM_PROCESS_FUNC( ATOMIC,       void,        shmem_int_add, ( int* , int , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONG_ADD )
SCOREP_SHMEM_PROCESS_FUNC( ATOMIC,       void,        shmem_long_add, ( long* , long , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONGLONG_ADD )
SCOREP_SHMEM_PROCESS_FUNC( ATOMIC,       void,        shmem_longlong_add, ( long long* , long long , int ) )
#endif

/* Atomic Inc */
#if SHMEM_HAVE_DECL( SHMEM_SHORT_INC )
SCOREP_SHMEM_PROCESS_FUNC( ATOMIC,       void,        shmem_short_inc, ( short* , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_INT_INC )
SCOREP_SHMEM_PROCESS_FUNC( ATOMIC,       void,        shmem_int_inc, ( int* , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONG_INC )
SCOREP_SHMEM_PROCESS_FUNC( ATOMIC,       void,        shmem_long_inc, ( long* , int ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONGLONG_INC )
SCOREP_SHMEM_PROCESS_FUNC( ATOMIC,       void,        shmem_longlong_inc, ( long long* , int ) )
#endif

/*
 * Lock functions
 */
#if SHMEM_HAVE_DECL( SHMEM_SET_LOCK ) && defined( SCOREP_SHMEM_SET_LOCK_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     void,        shmem_set_lock, SCOREP_SHMEM_SET_LOCK_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_CLEAR_LOCK ) && defined( SCOREP_SHMEM_CLEAR_LOCK_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     void,        shmem_clear_lock, SCOREP_SHMEM_CLEAR_LOCK_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_TEST_LOCK ) && defined( SCOREP_SHMEM_TEST_LOCK_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     int,         shmem_test_lock, SCOREP_SHMEM_TEST_LOCK_PROTO_ARGS )
#endif

/*
 * P2P sync routines
 */
#if SHMEM_HAVE_DECL( SHMEM_SHORT_WAIT ) && defined( SCOREP_SHMEM_SHORT_WAIT_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     void,        shmem_short_wait, SCOREP_SHMEM_SHORT_WAIT_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_INT_WAIT ) && defined( SCOREP_SHMEM_INT_WAIT_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     void,        shmem_int_wait, SCOREP_SHMEM_INT_WAIT_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONG_WAIT ) && defined( SCOREP_SHMEM_LONG_WAIT_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     void,        shmem_long_wait, SCOREP_SHMEM_LONG_WAIT_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONGLONG_WAIT ) && defined( SCOREP_SHMEM_LONGLONG_WAIT_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     void,        shmem_longlong_wait, SCOREP_SHMEM_LONGLONG_WAIT_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_WAIT ) && defined( SCOREP_SHMEM_WAIT_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     void,        shmem_wait, SCOREP_SHMEM_WAIT_PROTO_ARGS )
#endif

#if SHMEM_HAVE_DECL( SHMEM_SHORT_WAIT_UNTIL ) && defined( SCOREP_SHMEM_SHORT_WAIT_UNTIL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     void,        shmem_short_wait_until, SCOREP_SHMEM_SHORT_WAIT_UNTIL_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_INT_WAIT_UNTIL ) && defined( SCOREP_SHMEM_INT_WAIT_UNTIL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     void,        shmem_int_wait_until, SCOREP_SHMEM_INT_WAIT_UNTIL_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONG_WAIT_UNTIL ) && defined( SCOREP_SHMEM_LONG_WAIT_UNTIL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     void,        shmem_long_wait_until, SCOREP_SHMEM_LONG_WAIT_UNTIL_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONGLONG_WAIT_UNTIL ) && defined( SCOREP_SHMEM_LONGLONG_WAIT_UNTIL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     void,        shmem_longlong_wait_until, SCOREP_SHMEM_LONGLONG_WAIT_UNTIL_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_WAIT_UNTIL ) && defined( SCOREP_SHMEM_WAIT_UNTIL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     void,        shmem_wait_until, SCOREP_SHMEM_WAIT_UNTIL_PROTO_ARGS )
#endif

/*
 * Barrier sync routines
 */
#if SHMEM_HAVE_DECL( SHMEM_BARRIER )
SCOREP_SHMEM_PROCESS_FUNC( BARRIER,      void,        shmem_barrier, ( int , int , int , long* ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_BARRIER_ALL )
SCOREP_SHMEM_PROCESS_FUNC( BARRIER,      void,        shmem_barrier_all, ( void ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_FENCE )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     void,        shmem_fence, ( void ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_QUIET )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     void,        shmem_quiet, ( void ) )
#endif

/*
 * Collective routines
 */
#if SHMEM_HAVE_DECL( SHMEM_BROADCAST32 )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ONE2ALL, void,        shmem_broadcast32, ( void* , const void* , size_t , int , int , int , int , long* ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_BROADCAST64 )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ONE2ALL, void,        shmem_broadcast64, ( void* , const void* , size_t , int , int , int , int , long* ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_COLLECT32 )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_collect32, ( void* , const void* , size_t , int , int , int , long* ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_COLLECT64 )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_collect64, ( void* , const void* , size_t , int , int , int , long* ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_FCOLLECT32 )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_fcollect32, ( void* , const void* , size_t , int , int , int , long* ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_FCOLLECT64 )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_fcollect64, ( void* , const void* , size_t , int , int , int , long* ) )
#endif

/*
 * Reduction routines
 */
#if SHMEM_HAVE_DECL( SHMEM_SHORT_AND_TO_ALL ) && defined( SCOREP_SHMEM_SHORT_AND_TO_ALL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_short_and_to_all, SCOREP_SHMEM_SHORT_AND_TO_ALL_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_INT_AND_TO_ALL ) && defined( SCOREP_SHMEM_INT_AND_TO_ALL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_int_and_to_all, SCOREP_SHMEM_INT_AND_TO_ALL_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONG_AND_TO_ALL ) && defined( SCOREP_SHMEM_LONG_AND_TO_ALL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_long_and_to_all, SCOREP_SHMEM_LONG_AND_TO_ALL_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONGLONG_AND_TO_ALL ) && defined( SCOREP_SHMEM_LONGLONG_AND_TO_ALL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_longlong_and_to_all, SCOREP_SHMEM_LONGLONG_AND_TO_ALL_PROTO_ARGS )
#endif


#if SHMEM_HAVE_DECL( SHMEM_SHORT_OR_TO_ALL ) && defined( SCOREP_SHMEM_SHORT_OR_TO_ALL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_short_or_to_all, SCOREP_SHMEM_SHORT_OR_TO_ALL_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_INT_OR_TO_ALL ) && defined( SCOREP_SHMEM_INT_OR_TO_ALL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_int_or_to_all, SCOREP_SHMEM_INT_OR_TO_ALL_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONG_OR_TO_ALL ) && defined( SCOREP_SHMEM_LONG_OR_TO_ALL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_long_or_to_all, SCOREP_SHMEM_LONG_OR_TO_ALL_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONGLONG_OR_TO_ALL ) && defined( SCOREP_SHMEM_LONGLONG_OR_TO_ALL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_longlong_or_to_all, SCOREP_SHMEM_LONGLONG_OR_TO_ALL_PROTO_ARGS )
#endif


#if SHMEM_HAVE_DECL( SHMEM_SHORT_XOR_TO_ALL ) && defined( SCOREP_SHMEM_SHORT_XOR_TO_ALL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_short_xor_to_all, SCOREP_SHMEM_SHORT_XOR_TO_ALL_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_INT_XOR_TO_ALL ) && defined( SCOREP_SHMEM_INT_XOR_TO_ALL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_int_xor_to_all, SCOREP_SHMEM_INT_XOR_TO_ALL_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONG_XOR_TO_ALL ) && defined( SCOREP_SHMEM_LONG_XOR_TO_ALL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_long_xor_to_all, SCOREP_SHMEM_LONG_XOR_TO_ALL_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONGLONG_XOR_TO_ALL ) && defined( SCOREP_SHMEM_LONGLONG_XOR_TO_ALL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_longlong_xor_to_all, SCOREP_SHMEM_LONGLONG_XOR_TO_ALL_PROTO_ARGS )
#endif


#if SHMEM_HAVE_DECL( SHMEM_SHORT_MAX_TO_ALL ) && defined( SCOREP_SHMEM_SHORT_MAX_TO_ALL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_short_max_to_all, SCOREP_SHMEM_SHORT_MAX_TO_ALL_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_INT_MAX_TO_ALL ) && defined( SCOREP_SHMEM_INT_MAX_TO_ALL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_int_max_to_all, SCOREP_SHMEM_INT_MAX_TO_ALL_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONG_MAX_TO_ALL ) && defined( SCOREP_SHMEM_LONG_MAX_TO_ALL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_long_max_to_all, SCOREP_SHMEM_LONG_MAX_TO_ALL_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONGLONG_MAX_TO_ALL ) && defined( SCOREP_SHMEM_LONGLONG_MAX_TO_ALL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_longlong_max_to_all, SCOREP_SHMEM_LONGLONG_MAX_TO_ALL_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_FLOAT_MAX_TO_ALL ) && defined( SCOREP_SHMEM_FLOAT_MAX_TO_ALL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_float_max_to_all, SCOREP_SHMEM_FLOAT_MAX_TO_ALL_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_DOUBLE_MAX_TO_ALL ) && defined( SCOREP_SHMEM_DOUBLE_MAX_TO_ALL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_double_max_to_all, SCOREP_SHMEM_DOUBLE_MAX_TO_ALL_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONGDOUBLE_MAX_TO_ALL ) && defined( SCOREP_SHMEM_LONGDOUBLE_MAX_TO_ALL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_longdouble_max_to_all, SCOREP_SHMEM_LONGDOUBLE_MAX_TO_ALL_PROTO_ARGS )
#endif


#if SHMEM_HAVE_DECL( SHMEM_SHORT_MIN_TO_ALL ) && defined( SCOREP_SHMEM_SHORT_MIN_TO_ALL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_short_min_to_all, SCOREP_SHMEM_SHORT_MIN_TO_ALL_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_INT_MIN_TO_ALL ) && defined( SCOREP_SHMEM_INT_MIN_TO_ALL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_int_min_to_all, SCOREP_SHMEM_INT_MIN_TO_ALL_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONG_MIN_TO_ALL ) && defined( SCOREP_SHMEM_LONG_MIN_TO_ALL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_long_min_to_all, SCOREP_SHMEM_LONG_MIN_TO_ALL_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONGLONG_MIN_TO_ALL ) && defined( SCOREP_SHMEM_LONGLONG_MIN_TO_ALL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_longlong_min_to_all, SCOREP_SHMEM_LONGLONG_MIN_TO_ALL_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_FLOAT_MIN_TO_ALL ) && defined( SCOREP_SHMEM_FLOAT_MIN_TO_ALL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_float_min_to_all, SCOREP_SHMEM_FLOAT_MIN_TO_ALL_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_DOUBLE_MIN_TO_ALL ) && defined( SCOREP_SHMEM_DOUBLE_MIN_TO_ALL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_double_min_to_all, SCOREP_SHMEM_DOUBLE_MIN_TO_ALL_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONGDOUBLE_MIN_TO_ALL ) && defined( SCOREP_SHMEM_LONGDOUBLE_MIN_TO_ALL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_longdouble_min_to_all, SCOREP_SHMEM_LONGDOUBLE_MIN_TO_ALL_PROTO_ARGS )
#endif


#if SHMEM_HAVE_DECL( SHMEM_SHORT_SUM_TO_ALL ) && defined( SCOREP_SHMEM_SHORT_SUM_TO_ALL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_short_sum_to_all, SCOREP_SHMEM_SHORT_SUM_TO_ALL_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_INT_SUM_TO_ALL ) && defined( SCOREP_SHMEM_INT_SUM_TO_ALL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_int_sum_to_all, SCOREP_SHMEM_INT_SUM_TO_ALL_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONG_SUM_TO_ALL ) && defined( SCOREP_SHMEM_LONG_SUM_TO_ALL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_long_sum_to_all, SCOREP_SHMEM_LONG_SUM_TO_ALL_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONGLONG_SUM_TO_ALL ) && defined( SCOREP_SHMEM_LONGLONG_SUM_TO_ALL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_longlong_sum_to_all, SCOREP_SHMEM_LONGLONG_SUM_TO_ALL_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_FLOAT_SUM_TO_ALL ) && defined( SCOREP_SHMEM_FLOAT_SUM_TO_ALL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_float_sum_to_all, SCOREP_SHMEM_FLOAT_SUM_TO_ALL_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_DOUBLE_SUM_TO_ALL ) && defined( SCOREP_SHMEM_DOUBLE_SUM_TO_ALL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_double_sum_to_all, SCOREP_SHMEM_DOUBLE_SUM_TO_ALL_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONGDOUBLE_SUM_TO_ALL ) && defined( SCOREP_SHMEM_LONGDOUBLE_SUM_TO_ALL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_longdouble_sum_to_all, SCOREP_SHMEM_LONGDOUBLE_SUM_TO_ALL_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_COMPLEXF_SUM_TO_ALL ) && defined( SCOREP_SHMEM_COMPLEXF_SUM_TO_ALL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_complexf_sum_to_all, SCOREP_SHMEM_COMPLEXF_SUM_TO_ALL_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_COMPLEXD_SUM_TO_ALL ) && defined( SCOREP_SHMEM_COMPLEXD_SUM_TO_ALL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_complexd_sum_to_all, SCOREP_SHMEM_COMPLEXD_SUM_TO_ALL_PROTO_ARGS )
#endif


#if SHMEM_HAVE_DECL( SHMEM_SHORT_PROD_TO_ALL ) && defined( SCOREP_SHMEM_SHORT_PROD_TO_ALL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_short_prod_to_all, SCOREP_SHMEM_SHORT_PROD_TO_ALL_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_INT_PROD_TO_ALL ) && defined( SCOREP_SHMEM_INT_PROD_TO_ALL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_int_prod_to_all, SCOREP_SHMEM_INT_PROD_TO_ALL_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONG_PROD_TO_ALL ) && defined( SCOREP_SHMEM_LONG_PROD_TO_ALL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_long_prod_to_all, SCOREP_SHMEM_LONG_PROD_TO_ALL_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONGLONG_PROD_TO_ALL ) && defined( SCOREP_SHMEM_LONGLONG_PROD_TO_ALL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_longlong_prod_to_all, SCOREP_SHMEM_LONGLONG_PROD_TO_ALL_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_FLOAT_PROD_TO_ALL ) && defined( SCOREP_SHMEM_FLOAT_PROD_TO_ALL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_float_prod_to_all, SCOREP_SHMEM_FLOAT_PROD_TO_ALL_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_DOUBLE_PROD_TO_ALL ) && defined( SCOREP_SHMEM_DOUBLE_PROD_TO_ALL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_double_prod_to_all, SCOREP_SHMEM_DOUBLE_PROD_TO_ALL_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_LONGDOUBLE_PROD_TO_ALL ) && defined( SCOREP_SHMEM_LONGDOUBLE_PROD_TO_ALL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_longdouble_prod_to_all, SCOREP_SHMEM_LONGDOUBLE_PROD_TO_ALL_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_COMPLEXF_PROD_TO_ALL ) && defined( SCOREP_SHMEM_COMPLEXF_PROD_TO_ALL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_complexf_prod_to_all, SCOREP_SHMEM_COMPLEXF_PROD_TO_ALL_PROTO_ARGS )
#endif
#if SHMEM_HAVE_DECL( SHMEM_COMPLEXD_PROD_TO_ALL ) && defined( SCOREP_SHMEM_COMPLEXD_PROD_TO_ALL_PROTO_ARGS )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_complexd_prod_to_all, SCOREP_SHMEM_COMPLEXD_PROD_TO_ALL_PROTO_ARGS )
#endif

/*
 * Team routines
 */
#if SHMEM_HAVE_DECL( SHMEM_TEAM_SPLIT )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     void,        shmem_team_split, ( shmem_team_t, int, int, shmem_team_t* ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_TEAM_CREATE_STRIDED )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     void,        shmem_team_create_strided, ( int, int, int, shmem_team_t* ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_TEAM_FREE )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     void,        shmem_team_free, ( shmem_team_t* ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_TEAM_NPES )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     int,         shmem_team_npes, ( shmem_team_t ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_TEAM_MYPE )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     int,         shmem_team_mype, ( shmem_team_t ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_TEAM_TRANSLATE_PE )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     int,         shmem_team_translate_pe, ( shmem_team_t, int, shmem_team_t ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_TEAM_BARRIER )
SCOREP_SHMEM_PROCESS_FUNC( BARRIER,      void,        shmem_team_barrier, ( shmem_team_t, long* ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_TEAM_ALLTOALL )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_team_alltoall, ( void* , const void*, size_t, shmem_team_t, long* ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_TEAM_ALLTOALLV )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_team_alltoallv, ( void*, size_t*, size_t*, const void*, size_t*, size_t*, shmem_team_t, long* ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_TEAM_ALLTOALLV_PACKED )
SCOREP_SHMEM_PROCESS_FUNC( COLL_ALL2ALL, void,        shmem_team_alltoallv_packed, ( void*, size_t, size_t*, const void*, size_t*, size_t*, shmem_team_t, long* ) )
#endif

/*
 * Event routines
 */
#if SHMEM_HAVE_DECL( SHMEM_CLEAR_EVENT )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     void,        shmem_clear_event, ( long* ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_SET_EVENT )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     void,        shmem_set_event, ( long* ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_TEST_EVENT )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     int,         shmem_test_event, ( long* ) )
#endif
#if SHMEM_HAVE_DECL( SHMEM_WAIT_EVENT )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     void,        shmem_wait_event, ( long* ) )
#endif

/*
 * This funciton is not part of the OpenSHMEM standard but
 * it is highly recommended to explicitly finalize the
 * SHMEM environment by calling this function.
 */
#if HAVE( SHMEM_FINALIZE_COMPLIANT ) || !HAVE( DECL_SHMEM_FINALIZE )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     void,        shmem_finalize, ( void ) )
#elif HAVE( SHMEM_FINALIZE_OPENMPI_VARIANT )
SCOREP_SHMEM_PROCESS_FUNC( FUNCTION,     int,         shmem_finalize, ( void ) )
#endif

#undef SCOREP_SHMEM_PROCESS_FUNC
