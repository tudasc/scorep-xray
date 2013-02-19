/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 *    RWTH Aachen University, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 *  @status     alpha
 *  @file       src/adapters/cuda/scorep_cupti.h
 *  @maintainer Robert Dietrich <robert.dietrich@zih.tu-dresden.de>
 *
 *  This file provides commonly used definitions and functionality in the CUDA
 *  adapter.
 */

#ifndef SCOREP_CUPTI_H
#define SCOREP_CUPTI_H

#include <stdint.h>
#include <stdbool.h>

#include <UTILS_Error.h>

#include <cupti.h>

#define SCOREP_CUDA_NO_ID      0xFFFFFFFF

#define SCOREP_CUDA_DRIVER_CALL( _err ) \
    if ( _err != CUDA_SUCCESS ) { \
        UTILS_WARNING( "[CUDA] Call to %s failed.", #_err ); \
    }

#define SCOREP_CUPTI_CALL( _err ) \
    if ( _err != CUPTI_SUCCESS ) { \
        scorep_cuda_handle_cupti_error( _err, __FILE__, __LINE__ ); \
    }

/*
 * Handles errors returned from CUPTI function calls.
 *
 * @param err the CUDA driver API error code
 * @param file the corresponding ScoreP file
 * @param line the line the error occurred
 */
extern void
scorep_cuda_handle_cupti_error( CUptiResult err,
                                const char* file,
                                const int   line );


#endif  /* SCOREP_CUPTI_H */
