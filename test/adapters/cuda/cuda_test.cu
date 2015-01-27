/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2015,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 *
 * @brief Test program for the CUDA adapter. Several parts of this program have
 * been extracted from the NVIDIA computing samples 'simpleStreams' and
 * 'concurrentKernels'
 *
 * The basic test runs one kernel in (1+num_streams) streams.
 *
 * This advanced test runs (1+nreps*num_streams) instances of kernel 'init_array' and
 * (num_streams) instances of kernel 'clock_block'.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>

#include <SCOREP_User.h>

// CUDA utilities and system includes
#include <cuda_runtime.h>

#define CUDART_CALL( _err, _msg ) \
    if ( cudaSuccess != _err )         \
        __checkCUDACall( _err, _msg, __FILE__, __LINE__ )

static uint32_t kernel_workload = 20;

static uint64_t cpu_usleeptime = 10000;

static uint32_t num_streams = 3;

static int basic_mode = 0;

/* function declarations */
static void
__checkCUDACall( cudaError_t ecode,
                 const char* msg,
                 const char* file,
                 const int   line );

static void
runBasicTest( int nstreams );

static void
runCopyComputeOverlap( int nstreams );

static void
runConcurrentKernels( int nstreams );

static void
show_help( void );

static char
getopt( char* argument );

static void setArguments( int argc, char* argv[] );


__global__ void
init_array( int* g_data, int* factor, int num_iterations )
{
    int idx = blockIdx.x * blockDim.x + threadIdx.x;

    for ( int i = 0; i < num_iterations; i++ )
    {
        g_data[ idx ] += *factor; // non-coalesced on purpose, to burn time
    }
}

// This is a kernel that does no real work but runs at least for a specified number of clocks
__global__ void
clock_block( clock_t* d_o, clock_t clock_count )
{
    unsigned int start_clock = ( unsigned int )clock();

    clock_t clock_offset = 0;

    while ( clock_offset < clock_count )
    {
        unsigned int end_clock = ( unsigned int )clock();

        // The code below should work like
        // this (thanks to modular arithmetics):
        //
        // clock_offset = (clock_t) (end_clock > start_clock ?
        //                           end_clock - start_clock :
        //                           end_clock + (0xffffffffu - start_clock));
        //
        // Indeed, let m = 2^32 then
        // end - start = end + m - start (mod m).

        clock_offset = ( clock_t )( end_clock - start_clock );
    }

    d_o[ 0 ] = clock_offset;
}

int
main( int argc, char** argv )
{
    SCOREP_USER_FUNC_BEGIN()

    // check the compute capability of the device
    int num_devices = 0;

    setArguments( argc, argv );

    CUDART_CALL( cudaGetDeviceCount( &num_devices ), "cudaGetDeviceCount" );
    if ( 0 == num_devices )
    {
        printf( "your system does not have a CUDA capable device, waiving test...\n" );
        SCOREP_USER_FUNC_END()
        exit( 77 ); /* denote the test as skipped */
    }

    /* check if the command-line chosen device ID is within range, exit if not
       if( cuda_device >= num_devices ){
       printf("cuda_device=%d is invalid, must choose device ID between 0 and %d\n", cuda_device, num_devices-1);
       SCOREP_USER_FUNC_END()
       exit(-1);
       }*/

    if ( basic_mode )
    {
        runBasicTest( num_streams );
    }
    else
    {
        runCopyComputeOverlap( num_streams );

        runConcurrentKernels( num_streams );
    }

    cudaDeviceReset();

    SCOREP_USER_FUNC_END()
}

static void
runBasicTest( int nstreams )
{
    SCOREP_USER_FUNC_BEGIN()

    int n = 512 * 1024;                 // number of integers in the data set
    int  nbytes = n * sizeof( int );    // number of data bytes
    dim3 threads, blocks;               // kernel launch configuration
    int  niterations = kernel_workload; // number of iterations for the loop inside the kernel_time

    // allocate host memory
    int  c   = 5;               // value to which the array will be initialized
    int* h_a = 0;               // pointer to the array data in host memory

    printf( "Starting basic test\n" );
    h_a = ( int* )malloc( nbytes );

    // allocate device memory
    int* d_a = 0, * d_c = 0;         // pointers to data and init value in the device memory
    CUDART_CALL( cudaMalloc( ( void** )&d_a, nbytes ), "cudaMalloc" );
    CUDART_CALL( cudaMalloc( ( void** )&d_c, sizeof( int ) ), "cudaMalloc" );

    CUDART_CALL( cudaMemset( d_a, 0, nbytes ), "cudaMemset" );
    CUDART_CALL( cudaMemcpy( d_c, &c, sizeof( int ), cudaMemcpyHostToDevice ), "cudaMemcpy" );

    threads = dim3( 512, 1 );
    blocks  = dim3( n / threads.x, 1 );
    cudaStream_t* streams = ( cudaStream_t* )malloc( nstreams * sizeof( cudaStream_t ) );
    for ( int i = 0; i < nstreams; i++ )
    {
        CUDART_CALL( cudaStreamCreate( &( streams[ i ] ) ), "cudaStreamCreate" );
    }

    SCOREP_USER_REGION_BY_NAME_BEGIN( "init_array", SCOREP_USER_REGION_TYPE_COMMON )
    init_array<<<blocks, threads>>>( d_a, d_c, niterations );
    SCOREP_USER_REGION_BY_NAME_END( "init_array" )
    CUDART_CALL( cudaDeviceSynchronize(), "cudaDeviceSynchronize" );

    for ( int i = 0; i < nstreams; i++ )
    {
        SCOREP_USER_REGION_BY_NAME_BEGIN( "init_array", SCOREP_USER_REGION_TYPE_COMMON )
        init_array<<<blocks, threads, 0, streams[ i ]>>>( d_a, d_c, niterations );
        SCOREP_USER_REGION_BY_NAME_END( "init_array" )
        CUDART_CALL( cudaDeviceSynchronize(), "cudaDeviceSynchronize" );
    }

    // cleanup
    for ( int i = 0; i < nstreams; i++ )
    {
        cudaStreamDestroy( streams[ i ] );
    }
    free( streams );

    cudaFree( d_a );
    cudaFree( d_c );
    free( h_a );

    SCOREP_USER_FUNC_END()
}

static void
runCopyComputeOverlap( int nstreams )
{
    SCOREP_USER_FUNC_BEGIN()

    int nreps = 3;                      // number of times each experiment is repeated
    int  n      = 512 * 1024;           // number of integers in the data set
    int  nbytes = n * sizeof( int );    // number of data bytes
    dim3 threads, blocks;               // kernel launch configuration
    int  niterations = kernel_workload; // number of iterations for the loop inside the kernel_time

    // allocate host memory
    int  c          = 5;      // value to which the array will be initialized
    int* h_a        = 0;      // pointer to the array data in host memory
    int* hAligned_a = 0;      // pointer to the array data in host memory (aligned to MEMORY_ALIGNMENT)

    // allocate host memory (pinned is required for achieve asynchronicity)
    CUDART_CALL( cudaMallocHost( ( void** )&h_a, nbytes ), "cudaMallocHost" );
    hAligned_a = h_a;

    // allocate device memory
    int* d_a = 0, * d_c = 0;                // pointers to data and init value in the device memory
    CUDART_CALL( cudaMalloc( ( void** )&d_a, nbytes ), "cudaMalloc" );
    CUDART_CALL( cudaMalloc( ( void** )&d_c, sizeof( int ) ), "cudaMalloc" );
    CUDART_CALL( cudaMemcpy( d_c, &c, sizeof( int ), cudaMemcpyHostToDevice ), "cudaMemcpy" );

    threads = dim3( 512, 1 );
    blocks  = dim3( n / threads.x, 1 );
    SCOREP_USER_REGION_BY_NAME_BEGIN( "init_array", SCOREP_USER_REGION_TYPE_COMMON )
    init_array<<<blocks, threads>>>( d_a, d_c, niterations );
    SCOREP_USER_REGION_BY_NAME_END( "init_array" )
    usleep( cpu_usleeptime );
    cudaMemcpyAsync( hAligned_a, d_a, nbytes, cudaMemcpyDeviceToHost );

    // allocate and initialize an array of stream handles
    cudaStream_t* streams = ( cudaStream_t* )malloc( nstreams * sizeof( cudaStream_t ) );
    for ( int i = 0; i < nstreams; i++ )
    {
        CUDART_CALL( cudaStreamCreate( &( streams[ i ] ) ), "cudaStreamCreate" );
    }

    niterations = kernel_workload;

    printf( "Starting Copy/Compute overlap test\n" );
    threads = dim3( 512, 1 );
    blocks  = dim3( n / ( nstreams * threads.x ), 1 );
    memset( hAligned_a, 255, nbytes ); // set host memory bits to all 1s, for testing correctness
    cudaMemset( d_a, 0, nbytes );      // set device memory to all 0s, for testing correctness
    for ( int k = 0; k < nreps; k++ )
    {
        // asynchronously launch nstreams kernels, each operating on its own portion of data
        for ( int i = 0; i < nstreams; i++ )
        {
            SCOREP_USER_REGION_BY_NAME_BEGIN( "init_array", SCOREP_USER_REGION_TYPE_COMMON )
            init_array<<<blocks, threads, 0, streams[ i ]>>>( d_a + i * n / nstreams, d_c, niterations );
            SCOREP_USER_REGION_BY_NAME_END( "init_array" )
        }
        // asynchronously launch nstreams memcopies.  Note that memcopy in stream x will only
        //   commence executing when all previous CUDA calls in stream x have completed
        for ( int i = 0; i < nstreams; i++ )
        {
            cudaMemcpyAsync( hAligned_a + i * n / nstreams, d_a + i * n / nstreams, nbytes / nstreams, cudaMemcpyDeviceToHost, streams[ i ] );
        }
    }
    CUDART_CALL( cudaDeviceSynchronize(), "cudaDeviceSynchronize" );

    // release resources
    for ( int i = 0; i < nstreams; i++ )
    {
        cudaStreamDestroy( streams[ i ] );
    }
    free( streams );

    cudaFreeHost( h_a );
    cudaFree( d_a );
    cudaFree( d_c );

    SCOREP_USER_FUNC_END()
}

static void
runConcurrentKernels( int nstreams )
{
    SCOREP_USER_FUNC_BEGIN()

    float kernel_time = 10;                                    // time the kernel should run in ms
    cudaDeviceProp deviceProp;
    clock_t*       a           = NULL;                         // pointer to the array data in host memory
    int            nbytes      = nstreams * sizeof( clock_t ); // number of data bytes
    int            cuda_device = 0;

    CUDART_CALL( cudaGetDevice( &cuda_device ), "cudaGetDevice" );

    CUDART_CALL( cudaGetDeviceProperties( &deviceProp, cuda_device ), "cudaGetDeviceProperties" );
    if ( ( deviceProp.concurrentKernels == 0 ) )
    {
        printf( "> GPU does not support concurrent kernel execution\n" );
        printf( "  CUDA kernel runs will be serialized\n" );
    }

    // allocate host memory
    CUDART_CALL( cudaMallocHost( ( void** )&a, nbytes ), "cudaMallocHost" );

    // allocate device memory
    clock_t* d_ac = 0;                 // pointers to data and init value in the device memory
    CUDART_CALL( cudaMalloc( ( void** )&d_ac, nbytes ), "cudaMalloc" );

    // allocate and initialize an array of stream handles
    cudaStream_t* streams = ( cudaStream_t* )malloc( nstreams * sizeof( cudaStream_t ) );
    for ( int i = 0; i < nstreams; i++ )
    {
        CUDART_CALL( cudaStreamCreate( &( streams[ i ] ) ), "cudaStreamCreate" );
    }

    // time execution with nkernels streams
    clock_t total_clocks = 0;
    clock_t time_clocks  = kernel_time * deviceProp.clockRate;
    printf( "Starting concurrent kernel test\n" );

    // queue nkernels in separate streams and record when they are done
    for ( int i = 0; i < nstreams; ++i )
    {
        SCOREP_USER_REGION_BY_NAME_BEGIN( "clock_block", SCOREP_USER_REGION_TYPE_COMMON )
        clock_block<<<1, 1, 0, streams[ i ]>>>( &d_ac[ i ], time_clocks );
        SCOREP_USER_REGION_BY_NAME_END( "clock_block" )
        total_clocks += time_clocks;
    }

    CUDART_CALL( cudaDeviceSynchronize(), "cudaDeviceSynchronize" );

    // release resources
    for ( int i = 0; i < nstreams; i++ )
    {
        cudaStreamDestroy( streams[ i ] );
    }
    free( streams );
    cudaFreeHost( a );
    cudaFree( d_ac );

    SCOREP_USER_FUNC_END()
}

/*
 * Checks if a CUDA runtime API call returns successful and respectively prints
 * the error.
 *
 * @param ecode the CUDA error code
 * @param msg a message to get more detailed information about the error
 * @param the corresponding file
 * @param the line the error occurred
 */
static void
__checkCUDACall( cudaError_t ecode, const char* msg,
                 const char* file, const int line )
{
    if ( msg != NULL )
    {
        printf( "[CUDART] %s",  msg );
    }
    printf( "[CUDA Error <%s>:%i] %s", file, line, cudaGetErrorString( ecode ) );
}

static void
show_help( void )
{
    printf( "\ncuda_test [OPTION]\n"
            "\t-g  kernel workload as number of loop iterations (positive integer)\n"
            "\t-c  sleep time of host after first kernel launch in seconds (positive integer)\n"
            "\t-s  number of CUDA streams (positive integer)\n"
            "\t-b  run only basic test (one kernel)\n\n"
            );
}

static char
getopt( char* argument )
{
    if ( argument[ 0 ] == '-' )
    {
        return argument[ 1 ];
    }
    return 'f';
}

static void
setArguments( int argc, char* argv[] )
{
    int j = 1;

    while ( j < argc )
    {
        switch ( getopt( argv[ j ] ) )
        {
            case 'g': // number of loop iterations inside GPU kernel
                kernel_workload = atoi( argv[ ++j ] );
                break;

            case 'c': // seconds to sleep after launch of first kernel
                cpu_usleeptime = atoi( argv[ ++j ] ) * 1000;
                break;

            case 's': // number of CUDA streams to use
                num_streams = atoi( argv[ ++j ] );
                break;

            case 'b': // test only basic CUDA features
                basic_mode = 1;
                break;

            default:
                show_help();
                exit( 1 );
        }
        j++;
    }
}
