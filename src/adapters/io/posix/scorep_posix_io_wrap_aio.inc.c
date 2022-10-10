/*
 * Asynchronous I/O
 */

static inline int
aio_translate_opcode( int                     aioOpcode,
                      SCOREP_IoOperationMode* scorepMode )
{
    switch ( aioOpcode )
    {
        case LIO_READ:
            *scorepMode = SCOREP_IO_OPERATION_MODE_READ;
            break;
        case LIO_WRITE:
            *scorepMode = SCOREP_IO_OPERATION_MODE_WRITE;
            break;
        default:
            return -1;
    }
    return 0;
}

static inline int
aio_translate_mode( int                     aioMode,
                    SCOREP_IoOperationFlag* scorepIoOpFlag )
{
    switch ( aioMode )
    {
        case LIO_WAIT:
            *scorepIoOpFlag |= SCOREP_IO_OPERATION_FLAG_BLOCKING;
            break;
        case LIO_NOWAIT:
            *scorepIoOpFlag |= SCOREP_IO_OPERATION_FLAG_NON_BLOCKING;
            break;
        default:
            return -1;
    }
    return 0;
}

static inline uint64_t
aio_get_transfer_size( const struct aiocb* aiocbp )
{
#if defined( __GLIBC__ )
    return ( uint64_t )aiocbp->__return_value;
#else
    return SCOREP_IO_UNKOWN_TRANSFER_SIZE;
#endif
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
                scorep_posix_io_aio_request_cancel_all( fd, handle );
            }
            else if ( scorep_posix_io_aio_request_find( aiocbp, NULL ) )
            {
                SCOREP_IoOperationCancelled( handle,
                                             ( uint64_t )aiocbp );
                scorep_posix_io_aio_request_delete( aiocbp );
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
        if ( handle != SCOREP_INVALID_IO_HANDLE && scorep_posix_io_aio_request_find( aiocbp, &io_mode ) )
        {
            if ( ret == 0 )
            {
                SCOREP_IoOperationComplete( handle,
                                            io_mode,
                                            aio_get_transfer_size( aiocbp ),
                                            ( uint64_t )aiocbp );

                scorep_posix_io_aio_request_delete( aiocbp );
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
                                     ( uint64_t )aiocbp,
                                     aiocbp->aio_offset );
        }

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( aio_read,
                                        ( aiocbp ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( io_handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationIssued( io_handle,
                                      ( uint64_t )aiocbp );

            scorep_posix_io_aio_request_insert( aiocbp, SCOREP_IO_OPERATION_MODE_READ );
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
        if ( handle != SCOREP_INVALID_IO_HANDLE && scorep_posix_io_aio_request_find( aiocbp, &io_mode ) )
        {
            SCOREP_IoOperationComplete( handle,
                                        io_mode,
                                        ( ret != -1 ) ? ret : SCOREP_IO_UNKOWN_TRANSFER_SIZE,
                                        ( uint64_t )aiocbp );
            scorep_posix_io_aio_request_delete( aiocbp );
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
                                     ( uint64_t )aiocbp,
                                     aiocbp->aio_offset );
        }

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( aio_write,
                                        ( aiocbp ) );
        SCOREP_EXIT_WRAPPED_REGION();

        if ( io_handle != SCOREP_INVALID_IO_HANDLE )
        {
            SCOREP_IoOperationIssued( io_handle,
                                      ( uint64_t )aiocbp );

            scorep_posix_io_aio_request_insert( aiocbp, SCOREP_IO_OPERATION_MODE_WRITE );
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

    SCOREP_IoOperationFlag io_operation_flags = SCOREP_IO_OPERATION_FLAG_NON_COLLECTIVE;

    if ( trigger && SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) && ( aio_translate_mode( mode, &io_operation_flags ) != -1 ) )
    {
        SCOREP_EnterWrappedRegion( scorep_posix_io_region_lio_listio );

        struct aiocb*          aiocbp;
        SCOREP_IoHandleHandle  handle;
        SCOREP_IoOperationMode io_mode;

        for ( int i = 0; i < nitems; ++i )
        {
            aiocbp = aiocb_list[ i ];
            if ( aio_translate_opcode( aiocbp->aio_lio_opcode, &io_mode ) == -1 )
            {
                continue;
            }

            handle = SCOREP_IoMgmt_GetIoHandle( SCOREP_IO_PARADIGM_POSIX,
                                                &( aiocbp->aio_fildes ) );

            if ( handle != SCOREP_INVALID_IO_HANDLE )
            {
                SCOREP_IoOperationBegin( handle,
                                         io_mode,
                                         io_operation_flags,
                                         ( uint64_t )aiocbp->aio_nbytes,
                                         ( uint64_t )aiocbp,
                                         aiocbp->aio_offset );
            }
        }

        SCOREP_ENTER_WRAPPED_REGION();
        ret = SCOREP_LIBWRAP_FUNC_CALL( lio_listio,
                                        ( mode, aiocb_list, nitems, sevp ) );
        SCOREP_EXIT_WRAPPED_REGION();

        for ( int i = 0; i < nitems; ++i )
        {
            aiocbp = aiocb_list[ i ];
            if ( aio_translate_opcode( aiocbp->aio_lio_opcode, &io_mode ) == -1 )
            {
                continue;
            }

            handle = SCOREP_IoMgmt_GetIoHandle( SCOREP_IO_PARADIGM_POSIX,
                                                &( aiocbp->aio_fildes ) );

            if ( handle != SCOREP_INVALID_IO_HANDLE )
            {
                int error = SCOREP_LIBWRAP_FUNC_CALL( aio_error, ( aiocbp ) );

                if ( error == 0 || error == EINPROGRESS )
                {
                    if ( io_operation_flags & SCOREP_IO_OPERATION_FLAG_NON_BLOCKING )
                    {
                        SCOREP_IoOperationIssued( handle, ( uint64_t )aiocbp );
                    }

                    if ( error == 0 )
                    {
                        SCOREP_IoOperationComplete( handle,
                                                    io_mode,
                                                    aio_get_transfer_size( aiocbp ),
                                                    ( uint64_t )aiocbp );
                    }
                    else if ( io_operation_flags & SCOREP_IO_OPERATION_FLAG_NON_BLOCKING )
                    {
                        scorep_posix_io_aio_request_insert( aiocbp, io_mode );
                    }
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
