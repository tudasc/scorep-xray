/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2015, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */


/**
 * @file       handle_test.c
 *
 *
 */


#include <config.h>

#include <CuTest.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <UTILS_Error.h>

#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Memory.h>
#include <SCOREP_IoManagement.h>

#define ASSERT_STR_NOT_EQUALS( tc, actual, notExpected ) \
    CuAssertIntNotEquals( tc, strcmp( actual, notExpected ), 0 )

/**
 * The following test_* functions are examples for wrapper functions
 * which using the I/O management API. Each test routine describes
 * a test case e.g. test_open can be a template for open wrapper function
 * of e.g. the POSIX I/O adapter.
 */
static int
test_open( const char* path );

static int
test_close( int fd );

static int
test_dup( int oldfd );

static int
test_dup( int oldfd );

static int
test_dup2( int oldfd,
           int newfd );

int
test_open( const char* path )
{
    int fd;

    SCOREP_IoMgmt_BeginHandleCreation( SCOREP_IO_PARADIGM_POSIX,
                                       SCOREP_IO_HANDLE_FLAG_NONE,
                                       SCOREP_INVALID_INTERIM_COMMUNICATOR,
                                       0, "" );

    fd = open( path, O_RDONLY );

    if ( fd != -1 )
    {
        SCOREP_IoFileHandle   file   = SCOREP_Definitions_NewIoFile( path, SCOREP_INVALID_SYSTEM_TREE_NODE );
        SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_CompleteHandleCreation(
            SCOREP_IO_PARADIGM_POSIX, file, &fd );
    }
    else
    {
        SCOREP_IoMgmt_DropIncompleteHandle();
    }

    return fd;
}

int
test_close( int fd )
{
    SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_RemoveHandle( SCOREP_IO_PARADIGM_POSIX, &fd );

    SCOREP_IoMgmt_PushHandle( handle );

    int ret = close( fd );

    SCOREP_IoMgmt_PopHandle( handle );

    if ( handle != SCOREP_INVALID_IO_HANDLE )
    {
        if ( ret == -1  && errno != EBADF )
        {
            SCOREP_IoMgmt_ReinsertHandle( SCOREP_IO_PARADIGM_POSIX, handle );
        }
        else
        {
            SCOREP_IoMgmt_DestroyHandle( handle );
        }
    }
    return ret;
}

int
test_dup( int oldfd )
{
    int                   newfd;
    SCOREP_IoHandleHandle old_handle = SCOREP_IoMgmt_GetIoHandle( SCOREP_IO_PARADIGM_POSIX, &oldfd );

    SCOREP_IoMgmt_BeginHandleDuplication( SCOREP_IO_PARADIGM_POSIX,
                                          old_handle, 0 );

    newfd = dup( oldfd );

    if ( newfd != -1 )
    {
        SCOREP_IoHandleHandle new_handle = SCOREP_IoMgmt_CompleteHandleDuplication(
            SCOREP_IO_PARADIGM_POSIX, SCOREP_INVALID_IO_FILE, &newfd );
    }
    else
    {
        SCOREP_IoMgmt_DropIncompleteHandle();
    }

    return newfd;
}

int
test_dup2( int oldfd, int newfd )
{
    int                   ret        = 0;
    SCOREP_IoHandleHandle new_handle = SCOREP_IoMgmt_GetIoHandle( SCOREP_IO_PARADIGM_POSIX, &newfd );
    SCOREP_IoHandleHandle old_handle = SCOREP_IoMgmt_GetIoHandle( SCOREP_IO_PARADIGM_POSIX, &oldfd );

    if ( new_handle != SCOREP_INVALID_IO_HANDLE )
    {
        // newfd is still open
        if ( oldfd != newfd )
        {
            // newfd will be closed
            SCOREP_IoMgmt_RemoveHandle( SCOREP_IO_PARADIGM_POSIX, &newfd );
            SCOREP_IoMgmt_DestroyHandle( new_handle );
            SCOREP_IoMgmt_BeginHandleDuplication( SCOREP_IO_PARADIGM_POSIX, old_handle, 0 );
        }
        // else
        // oldfd == newfd dup2 will return newfd
        // the FD_CLOEXEC file descriptor flag is cleared for newfd
    }
    else
    {
        SCOREP_IoMgmt_BeginHandleDuplication( SCOREP_IO_PARADIGM_POSIX, old_handle, 0 );
    }

    ret = dup2( oldfd, newfd );

    if ( oldfd != newfd )
    {
        if ( ret != -1 )
        {
            SCOREP_IoHandleHandle new_handle = SCOREP_IoMgmt_CompleteHandleDuplication(
                SCOREP_IO_PARADIGM_POSIX, SCOREP_INVALID_IO_FILE, &newfd );
        }
        else
        {
            SCOREP_IoMgmt_DropIncompleteHandle();
        }
    }

    return ret;
}

static void
tc_open( CuTest* tc )
{
    int fd = test_open( "/tmp" );

    SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_GetIoHandle( SCOREP_IO_PARADIGM_POSIX, &fd );
    CuAssertIntNotEquals( tc, handle, SCOREP_INVALID_IO_HANDLE );

    SCOREP_IoHandleDef* def = SCOREP_LOCAL_HANDLE_DEREF( handle, IoHandle );

    CuAssertIntEquals( tc, def->io_paradigm_type, SCOREP_IO_PARADIGM_POSIX );
    CuAssertIntNotEquals( tc, def->file_handle, SCOREP_INVALID_IO_FILE );
    CuAssertIntEquals( tc, def->parent_handle, SCOREP_INVALID_IO_HANDLE );

    SCOREP_IoFileDef* file_handle_def = SCOREP_LOCAL_HANDLE_DEREF( def->file_handle, IoFile );
    const char*       tmp_path        = SCOREP_StringHandle_Get( file_handle_def->file_name_handle );

    CuAssertStrEquals( tc, tmp_path, "/tmp" );

    CuAssertIntNotEquals( tc, fd, -1 );
    CuAssertIntNotEquals( tc, test_close( fd ), -1 );
}

static void
tc_close( CuTest* tc )
{
    int fd = test_open( "/tmp" );
    CuAssertIntNotEquals( tc, fd, -1 );
    CuAssertIntNotEquals( tc, test_close( fd ), -1 );
    CuAssertIntEquals( tc, test_close( fd ), -1 );
}

static void
tc_duplicate( CuTest* tc )
{
    const char* fd_path  = "/tmp";
    const char* fd2_path = "/var";
    int         fd       = test_open( fd_path );
    CuAssertIntNotEquals( tc, fd, -1 );
    int fd2 = test_dup( fd );
    CuAssertIntNotEquals( tc, fd2, -1 );
    CuAssertIntNotEquals( tc, test_close( fd ), -1 );
    CuAssertIntNotEquals( tc, test_close( fd2 ), -1 );

    fd = test_open( fd_path );
    CuAssertIntNotEquals( tc, fd, -1 );
    int ret = test_dup2( fd, fd );
    CuAssertIntEquals( tc, ret, fd );

    fd2 = test_open( fd2_path );
    CuAssertIntNotEquals( tc, fd2, -1 );

    ret = test_dup2( fd, fd2 );

    SCOREP_IoHandleHandle old_handle = SCOREP_IoMgmt_GetIoHandle( SCOREP_IO_PARADIGM_POSIX, &fd2 );
    CuAssertIntNotEquals( tc, old_handle, SCOREP_INVALID_IO_HANDLE );

    SCOREP_IoHandleDef* io_handle_def   = SCOREP_LOCAL_HANDLE_DEREF( old_handle, IoHandle );
    SCOREP_IoFileDef*   file_handle_def = SCOREP_LOCAL_HANDLE_DEREF( io_handle_def->file_handle, IoFile );

    const char* new_path = SCOREP_StringHandle_Get( file_handle_def->file_name_handle );

    ASSERT_STR_NOT_EQUALS( tc, new_path, fd2_path );

    CuAssertIntNotEquals( tc, test_close( fd ), -1 );
}

/**
 * The following routines tests the I/O management API.
 */
static void
tc_create_handle( CuTest* tc )
{
    int fd = 3;

    SCOREP_IoMgmt_BeginHandleCreation( SCOREP_IO_PARADIGM_POSIX,
                                       SCOREP_IO_HANDLE_FLAG_NONE,
                                       SCOREP_INVALID_INTERIM_COMMUNICATOR,
                                       0, "" );
    SCOREP_IoFileHandle   file   = SCOREP_Definitions_NewIoFile( "/tmp", SCOREP_INVALID_SYSTEM_TREE_NODE );
    SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_CompleteHandleCreation(
        SCOREP_IO_PARADIGM_POSIX, file, &fd );

    CuAssertIntNotEquals( tc, handle, SCOREP_INVALID_IO_HANDLE );

    SCOREP_IoHandleDef* def = SCOREP_LOCAL_HANDLE_DEREF( handle, IoHandle );
    CuAssertIntEquals( tc, def->io_paradigm_type, SCOREP_IO_PARADIGM_POSIX );
    CuAssertIntNotEquals( tc, def->file_handle, SCOREP_INVALID_IO_FILE );
    CuAssertIntEquals( tc, def->parent_handle, SCOREP_INVALID_IO_HANDLE );

    void* payload = SCOREP_IoHandleHandle_GetPayload( handle );
    CuAssertPtrNotNull( tc, payload );
}

static void
tc_recursive_create_handle( CuTest* tc )
{
    SCOREP_IoFileHandle file = SCOREP_Definitions_NewIoFile( "/tmp", SCOREP_INVALID_SYSTEM_TREE_NODE );

    int fd = 3;
    SCOREP_IoMgmt_BeginHandleCreation( SCOREP_IO_PARADIGM_POSIX,
                                       SCOREP_IO_HANDLE_FLAG_NONE,
                                       SCOREP_INVALID_INTERIM_COMMUNICATOR,
                                       0, "" );

    /* recursive create */
    SCOREP_IoMgmt_BeginHandleCreation( SCOREP_IO_PARADIGM_POSIX,
                                       SCOREP_IO_HANDLE_FLAG_NONE,
                                       SCOREP_INVALID_INTERIM_COMMUNICATOR,
                                       0, "" );

    SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_CompleteHandleCreation( SCOREP_IO_PARADIGM_POSIX, file, &fd );
    CuAssertIntEquals( tc, handle, SCOREP_INVALID_IO_HANDLE );

    handle = SCOREP_IoMgmt_CompleteHandleCreation( SCOREP_IO_PARADIGM_POSIX, file, &fd );
    CuAssertIntNotEquals( tc, handle, SCOREP_INVALID_IO_HANDLE );

    SCOREP_IoHandleDef* def = SCOREP_LOCAL_HANDLE_DEREF( handle, IoHandle );
    CuAssertIntEquals( tc, def->io_paradigm_type, SCOREP_IO_PARADIGM_POSIX );
    CuAssertIntNotEquals( tc, def->file_handle, SCOREP_INVALID_IO_FILE );
    CuAssertIntEquals( tc, def->parent_handle, SCOREP_INVALID_IO_HANDLE );

    void* payload = SCOREP_IoHandleHandle_GetPayload( handle );
    CuAssertPtrNotNull( tc, payload );
}

static void
tc_insert_handle( CuTest* tc )
{
    int         fd   = 3;
    const char* path = "/tmp";

    SCOREP_IoMgmt_BeginHandleCreation( SCOREP_IO_PARADIGM_POSIX,
                                       SCOREP_IO_HANDLE_FLAG_NONE,
                                       SCOREP_INVALID_INTERIM_COMMUNICATOR,
                                       0, "" );
    SCOREP_IoFileHandle   file   = SCOREP_Definitions_NewIoFile( "/tmp", SCOREP_INVALID_SYSTEM_TREE_NODE );
    SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_CompleteHandleCreation(
        SCOREP_IO_PARADIGM_POSIX, file, &fd );
    CuAssertIntNotEquals( tc, handle, SCOREP_INVALID_IO_HANDLE );

    SCOREP_IoFileHandle file_handle = SCOREP_IoHandleHandle_GetIoFile( handle );
    CuAssertIntNotEquals( tc, file_handle, SCOREP_INVALID_IO_FILE );

    SCOREP_IoHandleHandle tmp_handle = SCOREP_IoMgmt_GetIoHandle( SCOREP_IO_PARADIGM_POSIX, &fd );
    CuAssertIntEquals( tc, tmp_handle, handle );

    SCOREP_IoHandleDef* def = SCOREP_LOCAL_HANDLE_DEREF( handle, IoHandle );

    CuAssertIntEquals( tc, def->io_paradigm_type, SCOREP_IO_PARADIGM_POSIX );
    CuAssertIntEquals( tc, def->file_handle, file_handle );
    CuAssertIntEquals( tc, def->parent_handle, SCOREP_INVALID_IO_HANDLE );
    CuAssertIntEquals( tc, def->scope_handle, SCOREP_INVALID_INTERIM_COMMUNICATOR );
    CuAssertIntEquals( tc, def->flags, SCOREP_IO_HANDLE_FLAG_NONE );

    tmp_handle = SCOREP_IoMgmt_RemoveHandle( SCOREP_IO_PARADIGM_POSIX, &fd );
    SCOREP_IoMgmt_DestroyHandle( tmp_handle );
}

static void
tc_reinsert_handle( CuTest* tc )
{
    int         fd   = 3;
    const char* path = "/tmp";

    SCOREP_IoMgmt_BeginHandleCreation( SCOREP_IO_PARADIGM_POSIX,
                                       SCOREP_IO_HANDLE_FLAG_NONE,
                                       SCOREP_INVALID_INTERIM_COMMUNICATOR,
                                       0, "" );
    SCOREP_IoFileHandle file = SCOREP_Definitions_NewIoFile( "/tmp", SCOREP_INVALID_SYSTEM_TREE_NODE );

    SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_CompleteHandleCreation(
        SCOREP_IO_PARADIGM_POSIX, file, &fd );
    CuAssertIntNotEquals( tc, handle, SCOREP_INVALID_IO_HANDLE );

    SCOREP_IoFileHandle file_handle = SCOREP_IoHandleHandle_GetIoFile( handle );
    CuAssertIntNotEquals( tc, file_handle, SCOREP_INVALID_IO_FILE );

    SCOREP_IoHandleHandle removed_handle = SCOREP_IoMgmt_RemoveHandle( SCOREP_IO_PARADIGM_POSIX, &fd );
    CuAssertIntEquals( tc, removed_handle, handle );

    SCOREP_IoMgmt_ReinsertHandle( SCOREP_IO_PARADIGM_POSIX, handle );

    SCOREP_IoHandleHandle tmp_handle = SCOREP_IoMgmt_GetIoHandle( SCOREP_IO_PARADIGM_POSIX, &fd );
    CuAssertIntEquals( tc, tmp_handle, handle );

    SCOREP_IoMgmt_RemoveHandle( SCOREP_IO_PARADIGM_POSIX, &fd );
    SCOREP_IoMgmt_DestroyHandle( tmp_handle );
}

static void
tc_remove_handle( CuTest* tc )
{
    int         fd   = 3;
    const char* path = "/tmp";

    SCOREP_IoMgmt_BeginHandleCreation( SCOREP_IO_PARADIGM_POSIX,
                                       SCOREP_IO_HANDLE_FLAG_NONE,
                                       SCOREP_INVALID_INTERIM_COMMUNICATOR,
                                       0, "" );
    SCOREP_IoFileHandle   file   = SCOREP_Definitions_NewIoFile( path, SCOREP_INVALID_SYSTEM_TREE_NODE );
    SCOREP_IoHandleHandle handle = SCOREP_IoMgmt_CompleteHandleCreation(
        SCOREP_IO_PARADIGM_POSIX, file, &fd );

    SCOREP_IoHandleHandle tmp_handle = SCOREP_IoMgmt_GetIoHandle( SCOREP_IO_PARADIGM_POSIX, &fd );
    CuAssertIntEquals( tc, tmp_handle, handle );

    tmp_handle = SCOREP_IoMgmt_RemoveHandle( SCOREP_IO_PARADIGM_POSIX, &fd );
    CuAssertIntEquals( tc, tmp_handle, handle );

    tmp_handle = SCOREP_IoMgmt_GetIoHandle( SCOREP_IO_PARADIGM_POSIX, &fd );
    CuAssertIntEquals( tc, tmp_handle, SCOREP_INVALID_IO_HANDLE );

    SCOREP_IoMgmt_DestroyHandle( handle );
}

static void
tc_duplicate_handle( CuTest* tc )
{
    int         old_fd = 3;
    int         new_fd = 4;
    const char* path   = "/tmp";

    SCOREP_IoMgmt_BeginHandleCreation( SCOREP_IO_PARADIGM_POSIX,
                                       SCOREP_IO_HANDLE_FLAG_NONE,
                                       SCOREP_INVALID_INTERIM_COMMUNICATOR,
                                       0, "" );
    SCOREP_IoFileHandle   file       = SCOREP_Definitions_NewIoFile( path, SCOREP_INVALID_SYSTEM_TREE_NODE );
    SCOREP_IoHandleHandle old_handle = SCOREP_IoMgmt_CompleteHandleCreation(
        SCOREP_IO_PARADIGM_POSIX, file, &old_fd );

    SCOREP_IoMgmt_BeginHandleDuplication( SCOREP_IO_PARADIGM_POSIX, old_handle, 0 );
    SCOREP_IoHandleHandle new_handle = SCOREP_IoMgmt_CompleteHandleDuplication(
        SCOREP_IO_PARADIGM_POSIX, SCOREP_INVALID_IO_FILE, &new_fd );

    SCOREP_IoHandleDef* old_def = SCOREP_LOCAL_HANDLE_DEREF( old_handle, IoHandle );
    SCOREP_IoHandleDef* new_def = SCOREP_LOCAL_HANDLE_DEREF( new_handle, IoHandle );

    CuAssertIntEquals( tc, old_def->io_paradigm_type, new_def->io_paradigm_type );
    CuAssertIntEquals( tc, old_def->file_handle, new_def->file_handle );
    CuAssertIntEquals( tc, old_def->parent_handle, new_def->parent_handle );
    CuAssertIntEquals( tc, old_def->scope_handle, new_def->scope_handle );
    CuAssertIntEquals( tc, old_def->flags, new_def->flags );
    CuAssertIntEquals( tc, old_def->name_handle, new_def->name_handle );

    SCOREP_IoMgmt_RemoveHandle( SCOREP_IO_PARADIGM_POSIX, &new_fd );
    SCOREP_IoMgmt_RemoveHandle( SCOREP_IO_PARADIGM_POSIX, &old_fd );

    SCOREP_IoMgmt_DestroyHandle( old_handle );
    SCOREP_IoMgmt_DestroyHandle( new_handle );
}

static inline void
init( void )
{
    SCOREP_IoMgmt_RegisterParadigm( SCOREP_IO_PARADIGM_POSIX,
                                    SCOREP_IO_PARADIGM_CLASS_SERIAL,
                                    "POSIX I/O",
                                    SCOREP_IO_PARADIGM_FLAG_NONE,
                                    sizeof( int ),
                                    SCOREP_IO_PARADIGM_PROPERTY_VERSION, "2.3",
                                    SCOREP_INVALID_IO_PARADIGM_PROPERTY );
}

static inline void
fini( void )
{
    SCOREP_IoMgmt_DeregisterParadigm( SCOREP_IO_PARADIGM_POSIX );
}

static SCOREP_ErrorCallback orig_error_callback;

static SCOREP_ErrorCode
ignore_warnings( void*            userData,
                 const char*      file,
                 uint64_t         line,
                 const char*      function,
                 SCOREP_ErrorCode errorCode,
                 const char*      msgFormatString,
                 va_list          va )
{
    if ( errorCode == SCOREP_WARNING )
    {
        return SCOREP_WARNING;
    }
    return orig_error_callback( NULL, file, line, function, errorCode, msgFormatString, va );
}

int
main()
{
    CuUseColors();
    CuString* output = CuStringNew();
    CuSuite*  suite  = CuSuiteNew( "I/O" );

    SUITE_ADD_TEST_NAME( suite, tc_create_handle, "mgmt: create handle" );
    SUITE_ADD_TEST_NAME( suite, tc_recursive_create_handle, "mgmt: create recursive handle" );
    SUITE_ADD_TEST_NAME( suite, tc_insert_handle, "mgmt: insert handle" );
    SUITE_ADD_TEST_NAME( suite, tc_reinsert_handle, "mgmt: reinsert handle" );
    SUITE_ADD_TEST_NAME( suite, tc_remove_handle, "mgmt: remove handle" );
    SUITE_ADD_TEST_NAME( suite, tc_duplicate_handle, "mgmt: duplicate handle" );
    SUITE_ADD_TEST_NAME( suite, tc_open, "wrapper: open" );
    SUITE_ADD_TEST_NAME( suite, tc_close, "wrapper: close" );
    SUITE_ADD_TEST_NAME( suite, tc_duplicate, "wrapper: duplicate" );

    SCOREP_InitMeasurement();
    orig_error_callback = SCOREP_Error_RegisterCallback( ignore_warnings, NULL );

    init();

    CuSuiteRun( suite );

    fini();

    SCOREP_FinalizeMeasurement();

    CuSuiteSummary( suite, output );
    int failCount = suite->failCount;
    if ( failCount )
    {
        printf( "%s", output->buffer );
    }

    CuSuiteFree( suite );
    CuStringFree( output );

    return failCount ? EXIT_FAILURE : EXIT_SUCCESS;
}
