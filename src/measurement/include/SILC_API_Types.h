#ifndef SILC_API_TYPES_H
#define SILC_API_TYPES_H

/**
 * @file    SILC_API_Types.h
 * @author  Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 * @date    Started Thu Sep  3 13:11:11 2009
 *
 * @brief Types needed by SILC_API_* functions.
 *
 */


#include <stdint.h>


typedef uint32_t SILC_API_SourceFileHandle;
#define SILC_API_NO_SOURCE_FILE_HANDLE UINT32_MAX

typedef uint32_t SILC_API_LineNo;
#define SILC_API_NO_LINE_NO UINT32_MAX

typedef uint32_t SILC_API_CounterGroupHandle;
#define SILC_API_NO_COUNTER_GROUP UINT32_MAX

typedef uint32_t SILC_API_CounterHandle;
#define SILC_API_NO_COUNTER UINT32_MAX

typedef enum
{
    SILC_ADAPTER_USER,
    SILC_ADAPTER_COMPILER,
    SILC_ADAPTER_MPI,
    SILC_ADAPTER_POMP,
    SILC_ADAPTER_PTHREAD
}  SILC_API_AdapterType;

typedef enum
{
    SILC_REGION_UNKNOWN = 0,
    SILC_REGION_FUNCTION,
    SILC_REGION_LOOP,
    SILC_REGION_USER_REGION,

    SILC_REGION_MPI_COLL_BARRIER,
    SILC_REGION_MPI_COLL_ONE2ALL,
    SILC_REGION_MPI_COLL_ALL2ONE,
    SILC_REGION_MPI_COLL_ALL2ALL,
    SILC_REGION_MPI_COLL_OTHER,

    SILC_REGION_OMP_PARALLEL,
    SILC_REGION_OMP_LOOP,
    SILC_REGION_OMP_SECTIONS,
    SILC_REGION_OMP_SECTION,
    SILC_REGION_OMP_WORKSHARE,
    SILC_REGION_OMP_SINGLE,
    SILC_REGION_OMP_MASTER,
    SILC_REGION_OMP_CRITICAL,
    SILC_REGION_OMP_ATOMIC,
    SILC_REGION_OMP_BARRIER,
    SILC_REGION_OMP_IMPLICIT_BARRIER,
    SILC_REGION_OMP_FLUSH,
    SILC_REGION_OMP_CRITICAL_SBLOCK, // what is SBLOCK?
    SILC_REGION_OMP_SINGLE_SBLOCK
} SILC_API_RegionType;

typedef uint32_t SILC_API_RegionHandle;

typedef uint32_t SILC_API_MPICommunicatorHandle;

typedef uint32_t SILC_API_MPIWindowHandle;

typedef uint32_t SILC_API_MPICartTopolHandle;

#endif /* SILC_API_TYPES_H */
