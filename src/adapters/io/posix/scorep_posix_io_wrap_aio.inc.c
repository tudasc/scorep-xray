/*
 * Asynchronous I/O
 */

static inline void
aio_add_request( struct aiocb*          aiocbp,
                 SCOREP_IoOperationMode mode )
{
    SCOREP_MutexLock( scorep_posix_io_aio_request_table_mutex );

    SCOREP_Hashtab_InsertUint64( scorep_posix_io_aio_request_table,
                                 ( void* )aiocbp,
                                 mode,
                                 NULL );

    SCOREP_MutexUnlock( scorep_posix_io_aio_request_table_mutex );
}

static inline void
aio_delete_request( const struct aiocb* aiocbp )
{
    SCOREP_MutexLock( scorep_posix_io_aio_request_table_mutex );

    SCOREP_Hashtab_Remove( scorep_posix_io_aio_request_table,
                           ( const void* )aiocbp,
                           SCOREP_Hashtab_DeleteNone,
                           SCOREP_Hashtab_DeleteNone,
                           NULL );

    SCOREP_MutexUnlock( scorep_posix_io_aio_request_table_mutex );
}

static inline int
aio_find_request( const struct aiocb*     aiocbp,
                  SCOREP_IoOperationMode* mode )
{
    SCOREP_MutexLock( scorep_posix_io_aio_request_table_mutex );
    SCOREP_Hashtab_Entry* e = SCOREP_Hashtab_Find( scorep_posix_io_aio_request_table,
                                                   aiocbp,
                                                   NULL );
    if ( e != NULL && mode != NULL )
    {
        *mode = e->value.uint64;
    }
    SCOREP_MutexUnlock( scorep_posix_io_aio_request_table_mutex );

    return ( e != NULL ) ? 0 : -1;
}

static inline void
aio_cancel_all_requests_of_fd( int                   fd,
                               SCOREP_IoHandleHandle handle )
{
    SCOREP_MutexLock( scorep_posix_io_aio_request_table_mutex );
    SCOREP_Hashtab_Iterator* iter  = SCOREP_Hashtab_IteratorCreate( scorep_posix_io_aio_request_table );
    SCOREP_Hashtab_Entry*    entry = SCOREP_Hashtab_IteratorFirst( iter );
    while ( entry )
    {
        struct aiocb* tmp_aiocbp = ( struct aiocb* )entry->key;
        if ( tmp_aiocbp->aio_fildes == fd )
        {
            SCOREP_IoOperationCancelled( handle,
                                         ( uint64_t )tmp_aiocbp );

            SCOREP_Hashtab_Remove( scorep_posix_io_aio_request_table,
                                   ( const void* )tmp_aiocbp,
                                   SCOREP_Hashtab_DeleteNone,
                                   SCOREP_Hashtab_DeleteNone,
                                   NULL );
        }
        entry = SCOREP_Hashtab_IteratorNext( iter );
    }
    SCOREP_Hashtab_IteratorFree( iter );
    SCOREP_MutexUnlock( scorep_posix_io_aio_request_table_mutex );
}

static inline int
aio_translate_mode( int aio_mode, SCOREP_IoOperationMode* scorep_mode )
{
    switch ( aio_mode )
    {
        case LIO_READ:
            *scorep_mode = SCOREP_IO_OPERATION_MODE_READ;
            break;
        case LIO_WRITE:
            *scorep_mode = SCOREP_IO_OPERATION_MODE_WRITE;
            break;
        default:
            return -1;
    }
    return 0;
}

#if HAVE( POSIX_IO_SYMBOL_AIO_CANCEL )
int
SCOREP_LIBWRAP_FUNC_NAME( aio_cancel )( int fd, struct aiocb* aiocbp )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( aio_cancel );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_aio_cancel );

        SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_POSIX,
                                                                       &fd );

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( aio_cancel,
                                        ( fd, aiocbp ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( handle != SCOREP_INVALID_IO_HANDLE && ret == AIO_CANCELED )
        {
            if ( aiocbp == NULL )
            {
                aio_cancel_all_requests_of_fd( fd, handle );
            }
            else if ( aio_find_request( aiocbp, NULL ) == 0 )
            {
                SCOREP_IoOperationCancelled( handle,
                                             ( uint64_t )aiocbp );
                aio_delete_request( aiocbp );
            }
        }

        SCOREP_IoMgmt_PopHandle( handle );

        SCOREP_ExitRegion( scorep_posix_io_region_aio_cancel );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( aio_cancel,
                                        ( fd, aiocbp ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_AIO_ERROR )
int
SCOREP_LIBWRAP_FUNC_NAME( aio_error )( const struct aiocb* aiocbp )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( aio_error );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_aio_error );

        SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_POSIX,
                                                                       &( aiocbp->aio_fildes ) );

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( aio_error,
                                        ( aiocbp ) );
        SCOREP_EXIT_WRAPPED_REGION();

        SCOREP_IoOperationMode io_mode;
        if ( handle != SCOREP_INVALID_IO_HANDLE && aio_find_request( aiocbp, &io_mode ) == 0 )
        {
            if ( ret == 0 )
            {
                SCOREP_IoOperationComplete( handle,
                                            io_mode,
#if defined( __GLIBC__ )
                                            ( uint64_t )aiocbp->__return_value,
#else
                                            SCOREP_IO_UNKOWN_TRANSFER_SIZE,
#endif
                                            ( uint64_t )aiocbp );

                aio_delete_request( aiocbp );
            }
            else
            {
                SCOREP_IoOperationTest( handle,
                                        ( uint64_t )aiocbp );
            }
        }

        SCOREP_IoMgmt_PopHandle( handle );

        SCOREP_ExitRegion( scorep_posix_io_region_aio_error );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( aio_error,
                                        ( aiocbp ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_AIO_FSYNC )
int
SCOREP_LIBWRAP_FUNC_NAME( aio_fsync )( int op, struct aiocb* aiocbp )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( aio_fsync );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_aio_fsync );

        SCOREP_IoHandleHandle io_handle = SCOREP_INVALID_IO_HANDLE;

        if ( aiocbp != NULL )
        {
            io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_POSIX,
                                                        &( aiocbp->aio_fildes ) );
        }

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( aio_fsync,
                                        ( op, aiocbp ) );
        SCOREP_EXIT_WRAPPED_REGION();

        SCOREP_IoMgmt_PopHandle( io_handle );

        SCOREP_ExitRegion( scorep_posix_io_region_aio_fsync );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( aio_fsync,
                                        ( op, aiocbp ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_AIO_READ )
int
SCOREP_LIBWRAP_FUNC_NAME( aio_read )( struct aiocb* aiocbp )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( aio_read );
    ssize_t ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_aio_read );

        SCOREP_IoHandleHandle io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_POSIX,
                                                                          &( aiocbp->aio_fildes ) );

        if ( io_handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationBegin( io_handle,
                                     SCOREP_IO_OPERATION_MODE_READ,
                                     SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_NON_BLOCKING,
                                     ( uint64_t )aiocbp->aio_nbytes,
                                     ( uint64_t )aiocbp );
        }

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( aio_read,
                                        ( aiocbp ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( io_handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationIssued( io_handle,
                                      ( uint64_t )aiocbp );

            aio_add_request( aiocbp, SCOREP_IO_OPERATION_MODE_READ );
        }

        SCOREP_IoMgmt_PopHandle( io_handle );

        SCOREP_ExitRegion( scorep_posix_io_region_aio_read );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( aio_read,
                                        ( aiocbp ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_AIO_RETURN )
ssize_t
SCOREP_LIBWRAP_FUNC_NAME( aio_return )( struct aiocb* aiocbp )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( aio_return );
    ssize_t ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_aio_return );

        SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_POSIX,
                                                                       &( aiocbp->aio_fildes ) );

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( aio_return,
                                        ( aiocbp ) );
        SCOREP_EXIT_WRAPPED_REGION();

        SCOREP_IoOperationMode io_mode;
        if ( handle != SCOREP_INVALID_IO_HANDLE && aio_find_request( aiocbp, &io_mode ) == 0 )
        {
            SCOREP_IoOperationComplete( handle,
                                        io_mode,
                                        ( ret != -1 ) ? ret : SCOREP_IO_UNKOWN_TRANSFER_SIZE,
                                        ( uint64_t )aiocbp );
            aio_delete_request( aiocbp );
        }

        SCOREP_IoMgmt_PopHandle( handle );

        SCOREP_ExitRegion( scorep_posix_io_region_aio_return );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( aio_return,
                                        ( aiocbp ) );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_AIO_SUSPEND )
int
SCOREP_LIBWRAP_FUNC_NAME( aio_suspend )( const struct aiocb* const aiocb_list[], int nitems, const struct timespec* timeout )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( aio_suspend );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_aio_suspend );
        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( aio_suspend,
                                        ( aiocb_list, nitems, timeout ) );
        SCOREP_EXIT_WRAPPED_REGION();
        SCOREP_ExitRegion( scorep_posix_io_region_aio_suspend );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( aio_suspend,
                                        ( aiocb_list, nitems, timeout ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_AIO_WRITE )
int
SCOREP_LIBWRAP_FUNC_NAME( aio_write )( struct aiocb* aiocbp )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( aio_write );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_aio_write );

        SCOREP_IoHandleHandle io_handle = SCOREP_IoMgmt_GetAndPushHandle( SCOREP_IO_PARADIGM_POSIX,
                                                                          &( aiocbp->aio_fildes ) );

        if ( io_handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationBegin( io_handle,
                                     SCOREP_IO_OPERATION_MODE_WRITE,
                                     SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_NON_BLOCKING,
                                     ( uint64_t )aiocbp->aio_nbytes,
                                     ( uint64_t )aiocbp );
        }

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( aio_write,
                                        ( aiocbp ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( io_handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationIssued( io_handle,
                                      ( uint64_t )aiocbp );

            aio_add_request( aiocbp, SCOREP_IO_OPERATION_MODE_WRITE );
        }

        SCOREP_IoMgmt_PopHandle( io_handle );

        SCOREP_ExitRegion( scorep_posix_io_region_aio_write );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( aio_write,
                                        ( aiocbp ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif

#if HAVE( POSIX_IO_SYMBOL_LIO_LISTIO )
int
SCOREP_LIBWRAP_FUNC_NAME( lio_listio )( int mode, struct aiocb* const aiocb_list[], int nitems, struct sigevent* sevp )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    INITIALIZE_FUNCTION_POINTER( lio_listio );
    int ret;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_lio_listio );

        struct aiocb*          aiocbp;
        SCOREP_IoHandleHandle  handle;
        SCOREP_IoOperationMode io_mode;

        for ( int i = 0; i < nitems; ++i )
        {
            aiocbp = aiocb_list[ i ];
            if ( aio_translate_mode( aiocbp->aio_lio_opcode, &io_mode ) == -1 )
            {
                continue;
            }

            handle = SCOREP_IoMgmt_GetIoHandle( SCOREP_IO_PARADIGM_POSIX,
                                                &( aiocbp->aio_fildes ) );

            if ( handle != SCOREP_INVALID_IO_HANDLE )
            {
                SCOREP_IoOperationBegin( handle,
                                         io_mode,
                                         SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE | SCOREP_IO_OPERATION_FLAG_NON_BLOCKING,
                                         ( uint64_t )aiocbp->aio_nbytes,
                                         ( uint64_t )aiocbp );
            }
        }

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( lio_listio,
                                        ( mode, aiocb_list, nitems, sevp ) );
        SCOREP_EXIT_WRAPPED_REGION();

        for ( int i = 0; i < nitems; ++i )
        {
            aiocbp = aiocb_list[ i ];
            handle = SCOREP_IoMgmt_GetIoHandle( SCOREP_IO_PARADIGM_POSIX,
                                                &( aiocbp->aio_fildes ) );

            if ( handle != SCOREP_INVALID_IO_HANDLE )
            {
                if ( aio_translate_mode( aiocbp->aio_lio_opcode, &io_mode ) == -1 )
                {
                    continue;
                }

                int error = SCOREP_LIBWRAP_FUNC_CALL( aio_error, ( aiocbp ) );

                if ( error == 0 )
                {
                    SCOREP_IoOperationComplete( handle,
                                                io_mode,
#if defined( __GLIBC__ )
                                                ( uint64_t )aiocbp->__return_value,
#else
                                                SCOREP_IO_UNKOWN_TRANSFER_SIZE,
#endif
                                                ( uint64_t )aiocbp );
                }
                else if ( error == EINPROGRESS )
                {
                    SCOREP_IoOperationIssued( handle,
                                              ( uint64_t )aiocbp );
                }
            }
        }

        SCOREP_ExitRegion( scorep_posix_io_region_lio_listio );
    }
    else
    {
        ret = SCOREP_LIBWRAP_FUNC_CALL( lio_listio,
                                        ( mode, aiocb_list, nitems, sevp ) );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return ret;
}
#endif
