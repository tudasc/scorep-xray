#ifndef SILC_TYPES_H
#define SILC_TYPES_H

/**
 * @file    SILC_Types.h
 * @author  Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 * @date    Started Thu Sep  3 13:11:11 2009
 *
 * @brief Types needed by SILC_* functions.
 *
 */


#include <stdint.h>

typedef uint64_t SILC_Time;

typedef uint32_t SILC_SourceFileHandle;
#define SILC_INVALID_SOURCE_FILE UINT32_MAX

typedef uint32_t SILC_LineNo;
#define SILC_INVALID_LINE_NO 0

typedef uint32_t SILC_CounterGroupHandle;
#define SILC_INVALID_COUNTER_GROUP UINT32_MAX

typedef uint32_t SILC_CounterHandle;
#define SILC_INVALID_COUNTER UINT32_MAX

typedef enum
{
    SILC_ADAPTER_USER,
    SILC_ADAPTER_COMPILER,
    SILC_ADAPTER_MPI,
    SILC_ADAPTER_POMP,
    SILC_ADAPTER_PTHREAD
}  SILC_AdapterType;

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
} SILC_RegionType;

typedef uint64_t SILC_LocationHandle;
/**
 * decision still open! if we start with 1, we take 0 as invalid
 */
#define SILC_INVALID_LOCATION UINT64_MAX
#define SILC_MAX_LOCATION ( UINT64_MAX - 1 )

typedef uint32_t SILC_RegionHandle;

typedef uint32_t SILC_MPICommunicatorHandle;

typedef uint32_t SILC_MPIWindowHandle;

typedef uint32_t SILC_MPICartTopolHandle;

/* Parameter types */
typedef enum
{
    SILC_PARAMETER_INT64,
    SILC_PARAMETER_DOUBLE,
    SILC_PARAMETER_STRING
} SILC_ParameterType;

typedef uint32_t SILC_ParameterHandle;

/* types for the configure system */
typedef enum
{
    /**
     * A string value with variable expension (Ie. ${HOME})
     * (path normalization?)
     *
     * .variableReference should point to a variable of type char*.
     */
    SILC_CONFIG_TYPE_PATH,

    /**
     * A string value.
     *
     * .variableReference should point to a variable of type char*.
     */
    SILC_CONFIG_TYPE_STRING,

    /**
     * A boolean value.
     *
     * .variableReference should point to a variable of type bool.
     */
    SILC_CONFIG_TYPE_BOOL,

    /**
     * A numerical value
     *
     * .variableReference should point to a variable of type uint64_t.
     */
    SILC_CONFIG_TYPE_NUMBER,

    /**
     * A numerical value with size suffixes (Ie. Kb, Gb, KiBi, ...)
     *
     * .variableReference should point to a variable of type uint64_t.
     */
    SILC_CONFIG_TYPE_SIZE,

    /**
     * A symbolic set.
     *
     * .variableReference should point to a variable of type char**.
     *
     * .variableContext should point to a NULL terminated string
     * list with valid set members
     */
    SILC_CONFIG_TYPE_SET
} SILC_ConfigType;

typedef struct SILC_ConfigVariable
{
    const char*     nameSpace;     /* NULL for 'root'/'global' namespace */
    const char*     name;
    SILC_ConfigType type;
    void*           variableReference;
    void*           variableContext;
    const char*     defaultValue;
    const char*     shortHelp;
    const char*     longHelp;
} SILC_ConfigVariable;

#endif /* SILC_TYPES_H */
