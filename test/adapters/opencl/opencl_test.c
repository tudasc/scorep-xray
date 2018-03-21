/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014-2015,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 *
 * @brief Test program for the OpenCL adapter.
 */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "sys/time.h"
#include <CL/cl.h>

#define OPENCL_CHECK( err ) \
    do \
    { \
        cl_int _err = ( err ); \
        if ( _err != CL_SUCCESS ) \
        { \
            fprintf( stderr, "[OpenCL] Error '%s' (%d)\n", \
                     scorep_opencl_get_error_string( _err ), _err ); \
            return false; \
        } \
    } while ( 0 )

#define BUF_SIZE ( 1024 * 4 )

static uint32_t num_rounds        = 5;
static uint32_t num_queues        = 1;
static uint32_t num_kernels       = 5;
static uint32_t kernelID          = 1;
static uint32_t deviceID          = 0;
static char     kernel_name[ 12 ] = "concKernel2";

cl_device_id* clDevices   = NULL;
cl_uint       deviceCount = 0;
int           platform_id = 0;

cl_context clContext;
cl_program clProgram;

cl_kernel         clKernel;
cl_mem*           buffer_dev;
cl_command_queue* queue;
float**           buffer;

static const char*
scorep_opencl_get_error_string( cl_int error );


static double
gtod( void )
{
    struct timeval act_time;
    gettimeofday( &act_time, NULL );
    return ( double )act_time.tv_sec + ( double )act_time.tv_usec / 1000000.0;
}

static void
show_help( void )
{
    printf( "\nopencl_test [OPTION]\n" \
            "\t-p  platform ID\n" \
            "\t-d  device ID\n" \
            "\t-r  number of rounds\n" \
            "\t-q  number of queues\n" \
            "\t-k  kernel size\n" \
            "\t-s  number of kernel invocations\n\n"
            );
}

static char
get_options( const char* argument )
{
    if ( argument[ 0 ] == '-' )
    {
        return argument[ 1 ];
    }
    return 'f';
}

static void
set_arguments( int argc, char* argv[] )
{
    int j = 1;

    while ( j < argc )
    {
        switch ( get_options( argv[ j ] ) )
        {
            case 'q': // number of queues
                num_queues = atoi( argv[ ++j ] );
                break;

            case 's': // kernel invocations
                num_kernels = atoi( argv[ ++j ] );
                break;

            case 'r': // number of rounds to ...
                num_rounds = atoi( argv[ ++j ] );
                break;

            case 'k': // size of kernel
                kernelID = atoi( argv[ ++j ] );
                if ( kernelID != 0 )
                {
                    kernel_name[ 10 ] = *argv[ j ];
                }
                break;

            case 'd': // the device number
                deviceID = atoi( argv[ ++j ] );
                break;

            case 'p': // the platform number
                platform_id = atoi( argv[ ++j ] );
                break;

            default:
                show_help();
                exit( EXIT_FAILURE );
        }
        j++;
    }

    buffer_dev = ( cl_mem* )malloc( num_queues * sizeof( cl_mem ) );
    queue      = ( cl_command_queue* )malloc( num_queues * sizeof( cl_command_queue ) );

    //printf( "Number of sequential kernels: %d \n", num_kernels );
}

static bool
get_devices( void )
{
    cl_uint         platformCount = 0;
    cl_platform_id* clPlatforms;

    // get the number of available OpenCL platforms
    OPENCL_CHECK( clGetPlatformIDs( 0, NULL, &platformCount ) );

    if ( platformCount == 0 )
    {
        printf( "No OpenCL platforms found!\n" );
        return false;
    }
    else
    {   // get the OpenCL platform IDs and print their name to stdout
        clPlatforms = ( cl_platform_id* )malloc( platformCount * sizeof( cl_platform_id ) );
        clGetPlatformIDs( platformCount, clPlatforms, NULL );

        printf( "Platforms:\n" );

        char* platformName = ( char* )malloc( 256 * sizeof( char ) );

        size_t i = 0;
        ;
        for ( i = 0; i < platformCount; i++ )
        {
            OPENCL_CHECK( clGetPlatformInfo( clPlatforms[ i ], CL_PLATFORM_NAME,
                                             256, platformName, NULL ) );
            printf( "%zu: %s\n", i, platformName );
        }

        free( platformName );
    }

    if ( platform_id >= platformCount )
    {
        fprintf( stderr, "Platform with ID %d is not available\n", platform_id );
        return false;
    }

    // get devices on default platform
    OPENCL_CHECK( clGetDeviceIDs( clPlatforms[ platform_id ], CL_DEVICE_TYPE_ALL, 0, NULL, &deviceCount ) );

    if ( deviceCount == 0 )
    {
        printf( "No OpenCL devices found for platform %d!\n", platform_id );

        free( clPlatforms );

        return false;
    }
    else
    {
        clDevices = ( cl_device_id* )malloc( deviceCount * sizeof( cl_device_id ) );
        clGetDeviceIDs( clPlatforms[ platform_id ], CL_DEVICE_TYPE_ALL, deviceCount, clDevices, NULL );

        printf( "Devices on platform %d:\n", platform_id );

        char* deviceName = ( char* )malloc( 256 * sizeof( char ) );

        size_t i = 0;
        for ( i = 0; i < deviceCount; i++ )
        {
            OPENCL_CHECK( clGetDeviceInfo( clDevices[ i ], CL_DEVICE_NAME, 256,
                                           deviceName, NULL ) );
            printf( "%zu: %s\n", i, deviceName );
        }

        free( deviceName );
    }

    return true;
}

bool
read_kernel_file( const char* filename, char** str, size_t* strSize )
{
    if ( filename == NULL )
    {
        fprintf( stderr, "No filename for kernel given\n" );
        return false;
    }

    FILE* file = fopen( filename, "r" );
    if ( file == NULL )
    {
        fprintf( stderr, "Failed to open file %s for reading!\n", filename );
        return false;
    }

    fseek( file, 0, SEEK_END );
    *strSize = ftell( file ) + 1;

    rewind( file );

    *str = ( char* )malloc( *strSize * sizeof( char ) );
    size_t bytesRead = fread( *str, 1, *strSize - 1, file );
    if ( bytesRead != *strSize - 1 )
    {
        fprintf( stderr, "Failed to read file %s!\n", filename );

        free( *str );

        return false;
    }
    ( *str )[ *strSize - 1 ] = 0;
    return true;
}

bool
init_opencl_program( const char* sProgramStr, size_t deviceID, cl_uint deviceCount )
{
    cl_int err = CL_SUCCESS;

    if ( clDevices == NULL || clDevices[ deviceID ] == NULL )
    {
        printf( "No devices given for program creation!\n" );
        return false;
    }

    if ( sProgramStr == NULL )
    {
        printf( "No program string given!\n" );
        return false;
    }

    clProgram = clCreateProgramWithSource( clContext, 1,
                                           ( const char** )&sProgramStr,
                                           NULL, &err );
    OPENCL_CHECK( err );

    err = clBuildProgram( clProgram, 1, &( clDevices[ deviceID ] ), /*"-Werror"*/ NULL, NULL, NULL );
    if ( err != CL_SUCCESS )
    {
        fprintf( stderr, "err == %d\n", err );
        size_t logSize = 0;
        err = clGetProgramBuildInfo( clProgram, clDevices[ deviceID ], CL_PROGRAM_BUILD_LOG, 0,
                                     NULL, &logSize );

        char* buildLog = ( char* )malloc( ( logSize + 1 ) * sizeof( char ) );
        err |= clGetProgramBuildInfo( clProgram, clDevices[ deviceID ], CL_PROGRAM_BUILD_LOG,
                                      logSize, buildLog, NULL );
        fprintf( stderr, "Build returned error: %s\n", buildLog );

        size_t binarySizes[ deviceCount ];
        err |= clGetProgramInfo( clProgram, CL_PROGRAM_BINARY_SIZES,
                                 sizeof( size_t ) * deviceCount, binarySizes, NULL );

        if ( binarySizes[ deviceID ] != 0 )
        {
            size_t i        = 0;
            char** binaries = ( char** )malloc( deviceCount * sizeof( char* ) ); //new char*[deviceCount];

            for ( i = 0; i < deviceCount; i++ )
            {
                binaries[ i ] = ( char* )malloc( ( binarySizes[ i ] + 1 ) * sizeof( char ) ); //new char[binarySizes[i] + 1];
            }
            err |= clGetProgramInfo( clProgram, CL_PROGRAM_BINARIES,
                                     deviceCount * sizeof( size_t ), binaries, NULL );

            for ( i = 0; i < deviceCount; i++ )
            {
                binaries[ i ][ binarySizes[ i ] ] = '\0';
                fprintf( stderr, "Program %zu\n %s\n", i, binaries[ i ] );
            }

            for ( i = 0; i < deviceCount; i++ )
            {
                free( binaries[ i ] ); //delete[] binaries[i];
            }
            free( binaries );          //delete[] binaries;
        }

        free( buildLog );
        return false;
    }
    return true;
}

bool
setup_opencl_objects( void )
{
    char*  programStr     = NULL;
    size_t programStrSize = 0;

    if ( !read_kernel_file( "kernel.cl", &programStr, &programStrSize ) )
    {
        printf( "Could not read kernel from file!\n" );
        free( programStr ); //delete[] programStr;
        return false;
    }

    cl_int err = CL_SUCCESS;
    clContext = clCreateContext( NULL, 1, &( clDevices[ deviceID ] ),
                                 NULL, NULL, &err );
    OPENCL_CHECK( err );

    if ( !init_opencl_program( programStr, deviceID, deviceCount ) )
    {
        printf( "Could not initialize OpenCL program!\n" );
        return false;
    }

    cl_command_queue_properties prop = 0;
    prop |= CL_QUEUE_PROFILING_ENABLE;

    // allocate buffer array (for num_queues queues)
    buffer = ( float** )malloc( num_queues * sizeof( float* ) );

    int q = 0;
    for ( q = 0; q < num_queues; q++ )
    {
        queue[ q ] = clCreateCommandQueue( clContext, clDevices[ deviceID ], prop, &err );
        OPENCL_CHECK( err );

        buffer[ q ] = ( float* )malloc( BUF_SIZE * sizeof( float ) );  //new float[BUF_SIZE];

        buffer_dev[ q ] = clCreateBuffer( clContext,
                                          CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                                          BUF_SIZE * sizeof( float ), buffer[ q ], &err );
        OPENCL_CHECK( err );
    }

    clKernel = clCreateKernel( clProgram, kernel_name, &err );
    OPENCL_CHECK( err );

    for ( q = 0; q < num_queues; q++ )
    {
        OPENCL_CHECK( clSetKernelArg( clKernel, 0, sizeof( cl_mem ), &( buffer_dev[ q ] ) ) );
    }

    return true;
}

static bool
finish_opencl_objects( void )
{
    OPENCL_CHECK( clReleaseKernel( clKernel ) );

    int q = 0;
    for ( q = 0; q < num_queues; q++ )
    {
        OPENCL_CHECK( clReleaseMemObject( buffer_dev[ q ] ) );

        free( buffer[ q ] ); //delete[] buffer[q];
        OPENCL_CHECK( clReleaseCommandQueue( queue[ q ] ) );
    }

    if ( clContext != NULL )
    {
        OPENCL_CHECK( clReleaseContext( clContext ) );
    }

    if ( clProgram != NULL )
    {
        OPENCL_CHECK( clReleaseProgram( clProgram ) );
    }

    // free buffer array
    free( buffer );

    return true;
}

static bool
run( void )
{
    size_t* globalWorkSize = ( size_t* )malloc( 1 * sizeof( size_t ) ); //new size_t[1];
    size_t* localWorkSize  = ( size_t* )malloc( 1 * sizeof( size_t ) ); //new size_t[1];
    globalWorkSize[ 0 ] = BUF_SIZE;
    localWorkSize[ 0 ]  = 128;


    // launching serial kernels
    int q = 0;
    for ( q = 0; q < num_queues; q++ )
    {
        int i = 0;
        for ( i = 0; i < num_kernels; i++ )
        {
            OPENCL_CHECK( clEnqueueNDRangeKernel( queue[ q ], clKernel,
                                                  1, NULL, globalWorkSize,
                                                  localWorkSize, 0, NULL, NULL ) );
        }

        OPENCL_CHECK( clEnqueueReadBuffer( queue[ q ], buffer_dev[ q ],
                                           CL_FALSE, 0,
                                           BUF_SIZE * sizeof( float ),
                                           buffer[ q ], 0, NULL, NULL ) );
    }

    for ( q = 0; q < num_queues; q++ )
    {
        OPENCL_CHECK( clFinish( queue[ q ] ) );
    }

    free( localWorkSize );  //delete[] localWorkSize;
    free( globalWorkSize ); //delete[] globalWorkSize;

    return true;
}

int
main( int argc, char** argv )
{
    set_arguments( argc, argv );

    if ( !get_devices() )
    {
        fprintf( stderr, "An error occurred when getting devices\n" );
    }
    else if ( setup_opencl_objects() )
    {
        double  start, end, sum;
        int32_t r   = 0;
        bool    res = true;
        for ( r = 0; r < num_rounds; r++ )
        {
            start = gtod();

            res = run();

            end = gtod();

            sum += end - start;
        }

        if ( !res )
        {
            printf( "run failed\n" );
            return EXIT_FAILURE;
        }

        finish_opencl_objects();

        printf( "Runtime = %lf sec (total=%lf)\n", sum / num_rounds, sum );

        return EXIT_SUCCESS;
    }

    /* if we get here, we just skipping the whole test */
    return 77;
}

/*
 * Returns the OpenCL error string for the given error code
 *
 * @param error the error code
 * @return the error string
 */
static const char*
scorep_opencl_get_error_string( cl_int error )
{
    switch ( ( int )error )
    {
        case CL_DEVICE_NOT_FOUND:
            return "CL_DEVICE_NOT_FOUND";
        case CL_DEVICE_NOT_AVAILABLE:
            return "CL_DEVICE_NOT_AVAILABLE";
        case CL_COMPILER_NOT_AVAILABLE:
            return "CL_COMPILER_NOT_AVAILABLE";
        case CL_MEM_OBJECT_ALLOCATION_FAILURE:
            return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
        case CL_OUT_OF_RESOURCES:
            return "CL_OUT_OF_RESOURCES";
        case CL_OUT_OF_HOST_MEMORY:
            return "CL_OUT_OF_HOST_MEMORY";
        case CL_PROFILING_INFO_NOT_AVAILABLE:
            return "CL_PROFILING_INFO_NOT_AVAILABLE";
        case CL_MEM_COPY_OVERLAP:
            return "CL_MEM_COPY_OVERLAP";
        case CL_IMAGE_FORMAT_MISMATCH:
            return "CL_IMAGE_FORMAT_MISMATCH";
        case CL_IMAGE_FORMAT_NOT_SUPPORTED:
            return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
        case CL_BUILD_PROGRAM_FAILURE:
            return "CL_BUILD_PROGRAM_FAILURE";
        case CL_MAP_FAILURE:
            return "CL_MAP_FAILURE";
        case CL_MISALIGNED_SUB_BUFFER_OFFSET:
            return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
        case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:
            return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
        case CL_INVALID_VALUE:
            return "CL_INVALID_VALUE";
        case CL_INVALID_DEVICE_TYPE:
            return "CL_INVALID_DEVICE_TYPE";
        case CL_INVALID_PLATFORM:
            return "CL_INVALID_PLATFORM";
        case CL_INVALID_DEVICE:
            return "CL_INVALID_DEVICE";
        case CL_INVALID_CONTEXT:
            return "CL_INVALID_CONTEXT";
        case CL_INVALID_QUEUE_PROPERTIES:
            return "CL_INVALID_QUEUE_PROPERTIES";
        case CL_INVALID_COMMAND_QUEUE:
            return "CL_INVALID_COMMAND_QUEUE";
        case CL_INVALID_HOST_PTR:
            return "CL_INVALID_HOST_PTR";
        case CL_INVALID_MEM_OBJECT:
            return "CL_INVALID_MEM_OBJECT";
        case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
            return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
        case CL_INVALID_IMAGE_SIZE:
            return "CL_INVALID_IMAGE_SIZE";
        case CL_INVALID_SAMPLER:
            return "CL_INVALID_SAMPLER";
        case CL_INVALID_BINARY:
            return "CL_INVALID_BINARY";
        case CL_INVALID_BUILD_OPTIONS:
            return "CL_INVALID_BUILD_OPTIONS";
        case CL_INVALID_PROGRAM:
            return "CL_INVALID_PROGRAM";
        case CL_INVALID_PROGRAM_EXECUTABLE:
            return "CL_INVALID_PROGRAM_EXECUTABLE";
        case CL_INVALID_KERNEL_NAME:
            return "CL_INVALID_KERNEL_NAME";
        case CL_INVALID_KERNEL_DEFINITION:
            return "CL_INVALID_KERNEL_DEFINITION";
        case CL_INVALID_KERNEL:
            return "CL_INVALID_KERNEL";
        case CL_INVALID_ARG_INDEX:
            return "CL_INVALID_ARG_INDEX";
        case CL_INVALID_ARG_VALUE:
            return "CL_INVALID_ARG_VALUE";
        case CL_INVALID_ARG_SIZE:
            return "CL_INVALID_ARG_SIZE";
        case CL_INVALID_KERNEL_ARGS:
            return "CL_INVALID_KERNEL_ARGS";
        case CL_INVALID_WORK_DIMENSION:
            return "CL_INVALID_WORK_DIMENSION";
        case CL_INVALID_WORK_GROUP_SIZE:
            return "CL_INVALID_WORK_GROUP_SIZE";
        case CL_INVALID_WORK_ITEM_SIZE:
            return "CL_INVALID_WORK_ITEM_SIZE";
        case CL_INVALID_GLOBAL_OFFSET:
            return "CL_INVALID_GLOBAL_OFFSET";
        case CL_INVALID_EVENT_WAIT_LIST:
            return "CL_INVALID_EVENT_WAIT_LIST";
        case CL_INVALID_EVENT:
            return "CL_INVALID_EVENT";
        case CL_INVALID_OPERATION:
            return "CL_INVALID_OPERATION";
        case CL_INVALID_GL_OBJECT:
            return "CL_INVALID_GL_OBJECT";
        case CL_INVALID_BUFFER_SIZE:
            return "CL_INVALID_BUFFER_SIZE";
        case CL_INVALID_MIP_LEVEL:
            return "CL_INVALID_MIP_LEVEL";
        case CL_INVALID_GLOBAL_WORK_SIZE:
            return "CL_INVALID_GLOBAL_WORK_SIZE";
        case CL_INVALID_PROPERTY:
            return "CL_INVALID_PROPERTY";
        default:
            return "unknown error code";
    }

    return "unknown error code";
}
