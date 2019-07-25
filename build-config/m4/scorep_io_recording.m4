## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2015-2017,
## Technische Universitaet Dresden, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license. See the COPYING file in the package base
## directory for details.
##

## file build-config/m4/scorep_io_recording.m4

AC_DEFUN([SCOREP_IO_RECORDING], [
AFS_SUMMARY_SECTION_BEGIN([I/O Recording features])
    _SCOREP_IO_RECORDING_POSIX
AFS_SUMMARY_SECTION_END
]) # AC_DEFUN(SCOREP_IO_RECORDING)

dnl ----------------------------------------------------------------------------

## _SCOREP_IO_RECORDING_POSIX_CHECK_SYMBOLS( FOUND-SYMBOL-VARIABLE )
AC_DEFUN([_SCOREP_IO_RECORDING_POSIX_CHECK_SYMBOLS], [
SCOREP_CHECK_SYMBOLS([POSIX I/O], [], $1,
           [close,
            closedir,
            creat,
            creat64,
            dup,
            dup2,
            dup3,
            fclose,
            fcntl,
            fdatasync,
            fdopen,
            fflush,
            fgetc,
            fgetpos,
            fgets,
            flockfile,
            fopen,
            fopen64,
            fputc,
            fputs,
            fread,
            freopen,
            fscanf,
            fseek,
            fseeko,
            fseeko64,
            fsync,
            ftell,
            ftello,
            ftrylockfile,
            funlockfile,
            fwrite,
            getchar,
            gets,
            lockf,
            lseek,
            lseek64,
            open,
            open64,
            openat,
            pread,
            pread64,
            printf,
            pselect,
            putchar,
            puts,
            pwrite,
            pwrite64,
            read,
            rewind,
            scanf,
            select,
            sync,
            syncfs,
            ungetc,
            unlink,
            unlinkat,
            vfscanf,
            vprintf,
            vscanf,
            write,
            pwritev,
            preadv,
            writev,
            readv])
])

## _SCOREP_IO_RECORDING_POSIX_AIO_CHECK_SYMBOLS( FOUND-SYMBOL-VARIABLE )
AC_DEFUN([_SCOREP_IO_RECORDING_POSIX_AIO_CHECK_SYMBOLS], [
SCOREP_CHECK_SYMBOLS([POSIX I/O], [], $1,
           [aio_cancel,
            aio_error,
            aio_fsync,
            aio_read,
            aio_return,
            aio_suspend,
            aio_write,
            lio_listio])
])

dnl ----------------------------------------------------------------------------

AC_DEFUN([_SCOREP_IO_RECORDING_POSIX], [
AC_REQUIRE([SCOREP_THREAD_LOCAL_STORAGE])dnl
AC_REQUIRE([SCOREP_LIBRARY_WRAPPING])dnl
AC_REQUIRE([AC_SCOREP_DETECT_PLATFORMS]) dnl

# will be used for IO and AIO
scorep_posix_io_wrap_symbols=""

dnl Check for POSIX synchronous I/O

AFS_SUMMARY_PUSH

scorep_posix_io_support="yes"
scorep_posix_io_summary_reason=

# check for library wrapping support
AS_IF([test x"${scorep_libwrap_linktime_support}" != x"yes" &&
       test x"${scorep_libwrap_runtime_support}" != x"yes"],
      [scorep_posix_io_support="no"
       scorep_posix_io_summary_reason+=", missing library wrapping support"])

# check result of TLS
AS_IF([test x"${scorep_posix_io_support}" = x"yes"],
      [AS_IF([test x"${scorep_have_thread_local_storage}" != x"yes"],
             [scorep_posix_io_support="no"
              scorep_posix_io_summary_reason+=", missing TLS support"])])

AC_SCOREP_COND_HAVE([POSIX_IO_SUPPORT],
                    [test x"${scorep_posix_io_support}" = x"yes"],
                    [Defined if recording calls to POSIX I/O is possible.],
                    [_SCOREP_IO_RECORDING_POSIX_CHECK_SYMBOLS([scorep_posix_io_wrap_symbols])
                     # do not wrap 'vfprintf' on Cray platform
                     AS_CASE([${ac_scorep_platform}],
                             [cray*], [# as we do not check for vfprintf corresponding symbol should not be defined
                                     AC_DEFINE_UNQUOTED([HAVE_POSIX_IO_SYMBOL_VFPRINTF], [0]),
                                     AC_DEFINE_UNQUOTED([HAVE_POSIX_IO_SYMBOL_FPRINTF], [0])],
                             [SCOREP_CHECK_SYMBOLS([POSIX I/O], [], [scorep_posix_io_wrap_symbols], [vfprintf,fprintf])])])

AFS_SUMMARY_POP([POSIX I/O support], [${scorep_posix_io_support}${scorep_posix_io_summary_reason}])

dnl Check for POSIX asynchronous I/O
AFS_SUMMARY_PUSH

scorep_posix_aio_support="yes"
scorep_posix_aio_summary_reason=

# check result of TLS
AS_IF([test x"${scorep_have_thread_local_storage}" != x"yes"],
      [scorep_posix_aio_support="no"
       scorep_posix_aio_summary_reason+=", missing TLS support"])

AS_IF([test x"${scorep_posix_aio_support}" = x"yes"],
      [AC_CHECK_HEADER([aio.h],
                       [],
                       [AC_MSG_NOTICE([no aio.h file found])
                        scorep_posix_aio_support="no"
                        scorep_posix_aio_summary_reason+=", missing aio.h header"])])

AS_IF([test x"${scorep_posix_aio_support}" = x"yes"],
      [AC_SEARCH_LIBS([aio_read],
                      [rt],
                      [AS_CASE([${ac_cv_search_aio_read}],
                               [no*],  [with_posix_aio_libs=""],
                               [with_posix_aio_libs="${ac_cv_search_aio_read}"])],
                      [AC_MSG_NOTICE([no library found providing aio_read])
                       scorep_posix_aio_support="no"
                       scorep_posix_aio_summary_reason+=", missing aio library"])])

AS_IF([test x"${scorep_posix_aio_support}" = x"yes"],
      [AC_CHECK_MEMBER([struct aiocb.__return_value], [],
                       [AC_MSG_NOTICE([we need 'struct aiocb.__return_value' member])
                        scorep_posix_aio_support="no"
                        scorep_posix_aio_summary_reason+=", missing struct aiocb.__return_value member"],
                       [[#include <aio.h>]])])

AC_SCOREP_COND_HAVE([POSIX_AIO_SUPPORT],
                    [test x"${scorep_posix_aio_support}" = x"yes"],
                    [Defined if recording calls to POSIX asynchronous I/O is possible.],
                    [libs_save=${LIBS}
                     LIBS="$LIBS ${with_posix_aio_libs}"
                     _SCOREP_IO_RECORDING_POSIX_AIO_CHECK_SYMBOLS([scorep_posix_io_wrap_symbols])
                     LIBS="$libs_save"
                     AC_SUBST([SCOREP_POSIX_AIO_LIBS],    ["${with_posix_aio_libs}"])
                     scorep_posix_aio_summary_reason="${with_posix_aio_libs:+, using ${with_posix_aio_libs}}"],
                    [AC_SUBST([SCOREP_POSIX_AIO_LIBS],    [""])])

AFS_SUMMARY_POP([POSIX asynchronous I/O support], [${scorep_posix_aio_support}${scorep_posix_aio_summary_reason}])
])
