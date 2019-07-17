#ifndef SCOREP_POSIX_IO_PROCESS_FUNC
#error Macro 'SCOREP_POSIX_IO_PROCESS_FUNC' was not defined
#endif

/*
 * POSIX I/O routines
 */

#if HAVE( POSIX_IO_SYMBOL_CLOSE )
SCOREP_POSIX_IO_PROCESS_FUNC( POSIX_IO, FILE_IO_METADATA, int,     close,        ( int fd ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_CREAT )
SCOREP_POSIX_IO_PROCESS_FUNC( POSIX_IO, FILE_IO_METADATA, int,     creat,        ( const char* pathname, mode_t mode ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_CREAT64 )
SCOREP_POSIX_IO_PROCESS_FUNC( POSIX_IO, FILE_IO_METADATA, int,     creat64,      ( const char* pathname, mode_t mode ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_DUP )
SCOREP_POSIX_IO_PROCESS_FUNC( POSIX_IO, FILE_IO_METADATA, int,     dup,          ( int oldfd ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_DUP2 )
SCOREP_POSIX_IO_PROCESS_FUNC( POSIX_IO, FILE_IO_METADATA, int,     dup2,         ( int oldfd, int newfd ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_DUP3 )
SCOREP_POSIX_IO_PROCESS_FUNC( POSIX_IO, FILE_IO_METADATA, int,     dup3,         ( int oldfd, int newfd, int flags ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_FCNTL )
SCOREP_POSIX_IO_PROCESS_FUNC( POSIX_IO, FILE_IO_METADATA, int,     fcntl,        ( int fd, int cmd, ... ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_FDATASYNC )
SCOREP_POSIX_IO_PROCESS_FUNC( POSIX_IO, FILE_IO,          int,     fdatasync,    ( int fd ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_FSYNC )
SCOREP_POSIX_IO_PROCESS_FUNC( POSIX_IO, FILE_IO,          int,     fsync,        ( int fd ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_LOCKF )
SCOREP_POSIX_IO_PROCESS_FUNC( POSIX_IO, FILE_IO,          int,     lockf,        ( int fd, int cmd, off_t len ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_LSEEK )
SCOREP_POSIX_IO_PROCESS_FUNC( POSIX_IO, FILE_IO_METADATA, off_t,   lseek,        ( int fd, off_t offset, int whence ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_LSEEK64 )
SCOREP_POSIX_IO_PROCESS_FUNC( POSIX_IO, FILE_IO_METADATA, scorep_off64_t, lseek64,      ( int fd, scorep_off64_t offset, int whence ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_OPEN )
SCOREP_POSIX_IO_PROCESS_FUNC( POSIX_IO, FILE_IO_METADATA, int,     open,         ( const char *pathname, int flags, ... ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_OPEN64 )
SCOREP_POSIX_IO_PROCESS_FUNC( POSIX_IO, FILE_IO_METADATA, int,     open64,       ( const char *pathname, int flags, ... ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_OPENAT )
SCOREP_POSIX_IO_PROCESS_FUNC( POSIX_IO, FILE_IO_METADATA, int,     openat,       ( int dirfd, const char* pathname, int flags, ... ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_PREAD )
SCOREP_POSIX_IO_PROCESS_FUNC( POSIX_IO, FILE_IO,          ssize_t, pread,        ( int fd, void* buf, size_t count, off_t offset ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_PREAD64 )
SCOREP_POSIX_IO_PROCESS_FUNC( POSIX_IO, FILE_IO,          ssize_t, pread64,      ( int fd, void* buf, size_t count, scorep_off64_t offset ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_PREADV )
SCOREP_POSIX_IO_PROCESS_FUNC( POSIX_IO, FILE_IO,          ssize_t, preadv,       ( int fd, const struct iovec* iov, int iovcnt, off_t offset ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_PSELECT )
SCOREP_POSIX_IO_PROCESS_FUNC( POSIX_IO, WRAPPER,     int,     pselect,      ( int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, const struct timespec *timeout, const sigset_t *sigmask ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_PWRITE )
SCOREP_POSIX_IO_PROCESS_FUNC( POSIX_IO, FILE_IO,          ssize_t, pwrite,       ( int fd, const void* buf, size_t count, off_t offset ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_PWRITE64 )
SCOREP_POSIX_IO_PROCESS_FUNC( POSIX_IO, FILE_IO,          ssize_t, pwrite64,     ( int fd, const void *buf, size_t count, scorep_off64_t offset ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_PWRITEV )
SCOREP_POSIX_IO_PROCESS_FUNC( POSIX_IO, FILE_IO,          ssize_t, pwritev,      ( int fd, const struct iovec* iov, int iovcnt, off_t offset ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_READ )
SCOREP_POSIX_IO_PROCESS_FUNC( POSIX_IO, FILE_IO,          ssize_t, read,         ( int fd, void *buf, size_t count ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_READV )
SCOREP_POSIX_IO_PROCESS_FUNC( POSIX_IO, FILE_IO,          ssize_t, readv,        ( int fd, const struct iovec* iov, int iovcnt ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_SELECT )
SCOREP_POSIX_IO_PROCESS_FUNC( POSIX_IO, WRAPPER,     int,     select,       ( int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_SYNC )
SCOREP_POSIX_IO_PROCESS_FUNC( POSIX_IO, FILE_IO,          void,    sync,         () )
#endif
#if HAVE( POSIX_IO_SYMBOL_SYNCFS )
SCOREP_POSIX_IO_PROCESS_FUNC( POSIX_IO, FILE_IO,          int,     syncfs,       ( int fd ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_UNLINK )
SCOREP_POSIX_IO_PROCESS_FUNC( POSIX_IO, FILE_IO_METADATA, int,     unlink,       ( const char* pathname ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_UNLINKAT )
SCOREP_POSIX_IO_PROCESS_FUNC( POSIX_IO, FILE_IO_METADATA, int,     unlinkat,     ( int dirfd, const char* pathname, int flags ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_WRITE )
SCOREP_POSIX_IO_PROCESS_FUNC( POSIX_IO, FILE_IO,          ssize_t, write,        ( int fd, const void *buf, size_t count ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_WRITEV )
SCOREP_POSIX_IO_PROCESS_FUNC( POSIX_IO, FILE_IO,          ssize_t, writev,       ( int fd, const struct iovec* iov, int iovcnt ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_CLOSEDIR )
SCOREP_POSIX_IO_PROCESS_FUNC( POSIX_IO, FILE_IO_METADATA, int, closedir,        ( DIR * dirp ) )
#endif
/*
 * POSIX Async I/O routines
 */

#if HAVE( POSIX_IO_SYMBOL_AIO_CANCEL )
SCOREP_POSIX_IO_PROCESS_FUNC( POSIX_IO, FILE_IO,          int,     aio_cancel,   ( int fd, struct aiocb *aiocbp ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_AIO_ERROR )
SCOREP_POSIX_IO_PROCESS_FUNC( POSIX_IO, FILE_IO,          int,     aio_error,    ( const struct aiocb *aiocbp ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_AIO_FSYNC )
SCOREP_POSIX_IO_PROCESS_FUNC( POSIX_IO, FILE_IO,          int,     aio_fsync,    ( int op, struct aiocb *aiocbp ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_AIO_READ )
SCOREP_POSIX_IO_PROCESS_FUNC( POSIX_IO, FILE_IO,          int,     aio_read,     ( struct aiocb *aiocbp ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_AIO_RETURN )
SCOREP_POSIX_IO_PROCESS_FUNC( POSIX_IO, WRAPPER,     ssize_t, aio_return,   ( struct aiocb *aiocbp ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_AIO_SUSPEND )
SCOREP_POSIX_IO_PROCESS_FUNC( POSIX_IO, WRAPPER,     int,     aio_suspend,  ( const struct aiocb * const aiocb_list[], int nitems, const struct timespec *timeout ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_AIO_WRITE )
SCOREP_POSIX_IO_PROCESS_FUNC( POSIX_IO, FILE_IO,          int,     aio_write,    ( struct aiocb *aiocbp ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_LIO_LISTIO )
SCOREP_POSIX_IO_PROCESS_FUNC( POSIX_IO, FILE_IO,          int,     lio_listio,   ( int mode, struct aiocb *const aiocb_list[], int nitems, struct sigevent *sevp ) )
#endif

/*
 * ISO C I/O routines
 */

#if HAVE( POSIX_IO_SYMBOL_FCLOSE )
SCOREP_POSIX_IO_PROCESS_FUNC( ISOC,     FILE_IO_METADATA, int,     fclose,       ( FILE* fp ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_FDOPEN )
SCOREP_POSIX_IO_PROCESS_FUNC( ISOC,     FILE_IO_METADATA, FILE*,   fdopen,       ( int fd, const char *mode ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_FFLUSH )
SCOREP_POSIX_IO_PROCESS_FUNC( ISOC,     FILE_IO,          int,     fflush,       ( FILE* stream ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_FGETC )
SCOREP_POSIX_IO_PROCESS_FUNC( ISOC,     FILE_IO,          int,     fgetc,        ( FILE* stream ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_FGETPOS )
SCOREP_POSIX_IO_PROCESS_FUNC( ISOC,     WRAPPER,     int,     fgetpos,      ( FILE *stream, fpos_t *pos ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_FGETS )
SCOREP_POSIX_IO_PROCESS_FUNC( ISOC,     FILE_IO,          char*,   fgets,        ( char* s, int size, FILE* steam ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_FLOCKFILE )
SCOREP_POSIX_IO_PROCESS_FUNC( ISOC,     FILE_IO,          void,    flockfile,    ( FILE* filehandle ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_FOPEN )
SCOREP_POSIX_IO_PROCESS_FUNC( ISOC,     FILE_IO_METADATA, FILE*,   fopen,        ( const char* path, const char* mode ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_FOPEN64 )
SCOREP_POSIX_IO_PROCESS_FUNC( ISOC,     FILE_IO_METADATA, FILE*,   fopen64,      ( const char* path, const char* mode ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_FPRINTF ) && HAVE( POSIX_IO_SYMBOL_VFPRINTF )
SCOREP_POSIX_IO_PROCESS_FUNC( ISOC,     FILE_IO,          int,     fprintf,      ( FILE* stream, const char* format, ... ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_FPUTC )
SCOREP_POSIX_IO_PROCESS_FUNC( ISOC,     FILE_IO,          int,     fputc,        ( int c, FILE* stream ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_FPUTS )
SCOREP_POSIX_IO_PROCESS_FUNC( ISOC,     FILE_IO,          int,     fputs,        ( const char* s, FILE* stream ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_FREAD )
SCOREP_POSIX_IO_PROCESS_FUNC( ISOC,     FILE_IO,          size_t,  fread,        ( void* ptr, size_t size, size_t nmemb, FILE* stream ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_FREOPEN )
SCOREP_POSIX_IO_PROCESS_FUNC( ISOC,     FILE_IO_METADATA, FILE*,   freopen,      ( const char* path, const char* mode, FILE* stream ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_FSCANF ) && HAVE( POSIX_IO_SYMBOL_VFSCANF )
SCOREP_POSIX_IO_PROCESS_FUNC( ISOC,     FILE_IO,          int,     fscanf,       ( FILE* stream, const char* format, ... ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_FSEEK )
SCOREP_POSIX_IO_PROCESS_FUNC( ISOC,     FILE_IO_METADATA, int,     fseek,        ( FILE* stream, long offset, int whence ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_FSEEKO )
SCOREP_POSIX_IO_PROCESS_FUNC( ISOC,     FILE_IO_METADATA, int,     fseeko,       ( FILE* stream, off_t offset, int whence ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_FSEEKO64 )
SCOREP_POSIX_IO_PROCESS_FUNC( ISOC,     FILE_IO_METADATA, int,     fseeko64,     ( FILE* stream, scorep_off64_t offset, int whence ) )
#endif
// #if HAVE( POSIX_IO_SYMBOL_FSETPOS )
// SCOREP_POSIX_IO_PROCESS_FUNC( ISOC,     FILE_IO_METADATA, int,     fsetpos,      ( FILE* stream, const fpos_t *pos ) )
// #endif
// #if HAVE( POSIX_IO_SYMBOL_FSETPOS64 )
// SCOREP_POSIX_IO_PROCESS_FUNC( ISOC,     FILE_IO_METADATA, int,     fsetpos64,    ( FILE* stream, const fpos64_t *pos ) )
// #endif
#if HAVE( POSIX_IO_SYMBOL_FTELL )
SCOREP_POSIX_IO_PROCESS_FUNC( ISOC,     WRAPPER,     long,    ftell,        ( FILE* stream ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_FTELLO )
SCOREP_POSIX_IO_PROCESS_FUNC( ISOC,     WRAPPER,     off_t,   ftello,       ( FILE* stream ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_FTRYLOCKFILE )
SCOREP_POSIX_IO_PROCESS_FUNC( ISOC,     FILE_IO,          int,     ftrylockfile, ( FILE* filehandle ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_FUNLOCKFILE )
SCOREP_POSIX_IO_PROCESS_FUNC( ISOC,     FILE_IO,          void,    funlockfile,  ( FILE* filehandle ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_FWRITE )
SCOREP_POSIX_IO_PROCESS_FUNC( ISOC,     FILE_IO,          size_t,  fwrite,       ( const void* ptr, size_t size, size_t nmemb, FILE* stream ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_GETC )
SCOREP_POSIX_IO_PROCESS_FUNC( ISOC,     FILE_IO,          int,     getc,         ( FILE* stream ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_GETCHAR )
SCOREP_POSIX_IO_PROCESS_FUNC( ISOC,     FILE_IO,          int,     getchar,      () )
#endif
#if HAVE( POSIX_IO_SYMBOL_GETS )
SCOREP_POSIX_IO_PROCESS_FUNC( ISOC,     FILE_IO,          char*,   gets,         ( char* s ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_PRINTF ) && HAVE( POSIX_IO_SYMBOL_VPRINTF )
SCOREP_POSIX_IO_PROCESS_FUNC( ISOC,     FILE_IO,          int,     printf,       ( const char *format, ... ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_PUTCHAR )
SCOREP_POSIX_IO_PROCESS_FUNC( ISOC,     FILE_IO,          int,     putchar,      ( int c ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_PUTS )
SCOREP_POSIX_IO_PROCESS_FUNC( ISOC,     FILE_IO,          int,     puts,         ( const char* s ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_REWIND )
SCOREP_POSIX_IO_PROCESS_FUNC( ISOC,     FILE_IO_METADATA, void,    rewind,       ( FILE* stream ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_SCANF ) && HAVE( POSIX_IO_SYMBOL_VSCANF )
SCOREP_POSIX_IO_PROCESS_FUNC( ISOC,     FILE_IO,          int,     scanf,        ( const char* format, ... ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_UNGETC )
SCOREP_POSIX_IO_PROCESS_FUNC( ISOC,     FILE_IO,          int,     ungetc,       ( int c, FILE* stream ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_VFPRINTF )
SCOREP_POSIX_IO_PROCESS_FUNC( ISOC,     FILE_IO,          int,     vfprintf,     ( FILE* stream, const char* format, va_list ap ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_VFSCANF )
SCOREP_POSIX_IO_PROCESS_FUNC( ISOC,     FILE_IO,          int,     vfscanf,      ( FILE* stream, const char* format, va_list ap ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_VPRINTF )
SCOREP_POSIX_IO_PROCESS_FUNC( ISOC,     FILE_IO,          int,     vprintf,      ( const char *format, va_list ap ) )
#endif
#if HAVE( POSIX_IO_SYMBOL_VSCANF )
SCOREP_POSIX_IO_PROCESS_FUNC( ISOC,     FILE_IO,          int,     vscanf,       ( const char* format, va_list ap ) )
#endif

#undef SCOREP_POSIX_IO_PROCESS_FUNC
