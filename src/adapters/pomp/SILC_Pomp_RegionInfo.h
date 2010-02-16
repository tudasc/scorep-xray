/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

#ifndef SILC_POMP_REGION_INFO_H
#define SILC_POMP_REGION_INFO_H

/**
 * @file       SILC_Pomp_RegionInfo.h
 * @author     Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     ALPHA
 * @ingroup    POMP
 *
 * @brief Declares functionality for interpretation of pomp region strings.
 */

#include <stdbool.h>
#include "SILC_Types.h"

/**
 * SILC_Pomp_RegionType
 * @ingroup POMP
 * @{
 *
 */
typedef enum /* SILC_Pomp_RegionType */
{
    /* Entries must be in same order like silc_pomp_region_type_map to allow lookup. */
    SILC_Pomp_Atomic = 0,
    SILC_Pomp_Barrier,
    SILC_Pomp_Critical,
    SILC_Pomp_Do,
    SILC_Pomp_Flush,
    SILC_Pomp_For,
    SILC_Pomp_Master,
    SILC_Pomp_Parallel,
    SILC_Pomp_ParallelDo,
    SILC_Pomp_ParallelFor,
    SILC_Pomp_ParallelSections,
    SILC_Pomp_ParallelWorkshare,
    SILC_Pomp_Sections,
    SILC_Pomp_Single,
    SILC_Pomp_UserRegion,
    SILC_Pomp_Workshare,
    SILC_Pomp_NoType
} SILC_Pomp_RegionType;


/** Struct which contains all data for a pomp region. */
typedef struct
{
    SILC_Pomp_RegionType regionType;    /* region type of construct                  */
    char*                name;          /* critical or user region name              */
    int32_t              numSections;   /* sections only: number of sections         */
    /* For combined statements (parallel sections, parallel for) we need up to
       four SILC regions. So we use a pair of SILC regions for the parallel
       statements and a pair of regions for other statements.                        */
    SILC_RegionHandle outerParallel;    /* SILC handle for the outer parallel region */
    SILC_RegionHandle innerParallel;    /* SILC handle for the inner parallel region */
    SILC_RegionHandle outerBlock;       /* SILC handle for the enclosing region      */
    SILC_RegionHandle innerBlock;       /* SILC handle for the enclosed region       */

    char*             startFileName;    /* File containing opening statement         */
    int32_t           startLine1;       /* First line of the opening statement       */
    int32_t           startLine2;       /* Last line of the opening statement        */

    char*             endFileName;      /* File containing the closing statement     */
    int32_t           endLine1;         /* First line of the closing statement       */
    int32_t           endLine2;         /* Last line of the closing statement        */
    char*             regionName;       /* For user regions and criticals            */
} SILC_Pomp_Region;


/**
 * SILC_Pomp_ParseInitString() fills the SILC_Pomp_Region object with data read
 * from the @a initString. If the initString does not comply with the
 * specification, the program aborts with exit code 1. @n Rationale:
 * SILC_Pomp_ParseInitString() is used during initialization of the measurement
 * system. If an error occurs, it is better to abort than to struggle with
 * undefined behaviour or @e guessing the meaning of the broken string.
 *
 * @note Can be called from multiple threads concurrently, assuming malloc is
 * thread-safe.
 *
 * @note SILC_Pomp_ParseInitString() will assign memory to the members of @e
 * region.
 *
 * @param initString A string in the format
 * "length*key=value*[key=value]*". The length field is parsed but not used by
 * this implementation. Possible values for key are listed in
 * silc_pomp_token_map. The string must at least contain values for the keys @c
 * regionType, @c sscl and @c escl. Possible values for the key @c regionType
 * are listed in @ref silc_pomp_region_type_map. The format for @c sscl resp.
 * @c escl values
 * is @c "filename:lineNo1:lineNo2".
 *
 * @param region must be a valid object
 *
 * @post At least the required attributes (see SILC_Pomp_Region) are set. @n
 * All other members of @e region are set to 0 resp. false.
 * @n If @c regionType=sections than
 * SILC_Pomp_Region::numSections has a value > 0. @n If @c regionType=region
 * than SILC_Pomp_Region::regionName has a value != 0. @n If @c
 * regionType=critical than SILC_Pomp_RegionInfo::regionName may have a value
 * != 0.
 *
 */
void
SILC_Pomp_ParseInitString( const char        initString[],
                           SILC_Pomp_Region* region );

/** @} */

#endif /* SILC_POMP_REGION_INFO_H */
