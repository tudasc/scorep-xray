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

typedef uint64_t SILC_API_Time;

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

typedef uint64_t SILC_API_LocationHandle;
/**
 * decision still open! if we start with 1, we take 0 as invalid
 */
#define SILC_API_INVALID_LOCATION UINT64_MAX
#define SILC_API_MAX_LOCATION ( UINT64_MAX - 1 )

typedef uint32_t SILC_API_RegionHandle;

typedef uint32_t SILC_API_MPICommunicatorHandle;

typedef uint32_t SILC_API_MPIWindowHandle;

typedef uint32_t SILC_API_MPICartTopolHandle;

typedef uint32_t SILC_API_CollectiveType;

/* types for the configure system */
typedef enum
{
    /**
     * A string with variable expension
     * (path normalization?)
     */
    SILC_CONFIG_TYPE_PATH,

    /** A string */
    SILC_CONFIG_TYPE_STRING,

    /** A bool */
    SILC_CONFIG_TYPE_BOOL,

    /** A number (uin64_t) */
    SILC_CONFIG_TYPE_NUMBER,

    /** A number with size suffixes (uin64_t) */
    SILC_CONFIG_TYPE_SIZE,

    /**
     * A symbolic set
     * .variableContext should point to a NULL terminated string
     * list with valid set members
     */
    SILC_CONFIG_TYPE_SET
} SILC_API_ConfigType;

typedef struct SILC_API_ConfigVariable
{
    const char*         nameSpace; /* NULL for 'root'/'global' namespace */
    const char*         name;
    SILC_API_ConfigType type;
    void*               variableReference;
    void*               variableContext;
    const char*         defaultValue;
    const char*         shortHelp;
    const char*         longHelp;
} SILC_API_ConfigVariable;

#endif /* SILC_API_TYPES_H */
