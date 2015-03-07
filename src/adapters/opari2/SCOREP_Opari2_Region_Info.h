/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011, 2014
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_OPARI2_REGION_INFO_H
#define SCOREP_OPARI2_REGION_INFO_H

/**
 * @file
 *
 * @ingroup OPARI2
 *
 * @brief Declares functionality for interpretation of opari2 region strings.
 */

#include <stdbool.h>
#include <stdint.h>

#include <scorep/SCOREP_PublicTypes.h>
#include <opari2_region_info.h>

/**
 * SCOREP_Opari2_RegionType
 * @ingroup OPARI2
 * @{
 *
 */

/**
 * Flag to indicate, whether OPARI2-related events get recorded.
 * Use POMP2_On/POMP2_Off to switch mode. POMP2_On/POMP2_Off is implemented
 * is the OPARI2 user adapter, but scorep_opari2_recording_on is used in the
 * OpenMP adapter as well. This coupling needs to be addressed sometime.
 */
extern bool scorep_opari2_recording_on;

/** Struct which contains all data for a opari2 region. */
typedef struct SCOREP_Opari2_Region_struct
{
    char*   startFileName;                 /* File containing opening statement                  */
    int32_t startLine1;                    /* First line of the opening statement                */
    int32_t startLine2;                    /* Last line of the opening statement                 */

    char*   endFileName;                   /* File containing the closing statement              */
    int32_t endLine1;                      /* First line of the closing statement                */
    int32_t endLine2;                      /* Last line of the closing statement                 */
} SCOREP_Opari2_Region;

/* Allocate and assign generic region info */
void
scorep_opari2_assign_generic_region_info( SCOREP_Opari2_Region* region,
                                          OPARI2_Region_info*   regionInfo );

/* Free region members of region */
void
scorep_opari2_free_region_members( SCOREP_Opari2_Region* region );

/* Get handle to source file. A new file will be registere if it doesn't exist */
SCOREP_SourceFileHandle
scorep_opari2_get_file_handle( SCOREP_Opari2_Region* region );

/** @} */

#endif /* SCOREP_OPARI2_REGION_INFO_H */
