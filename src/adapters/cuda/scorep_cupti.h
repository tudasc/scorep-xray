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

#define SCOREP_CUDA_DRIVER_CALL( fct ) \
    if ( fct != CUDA_SUCCESS ) { \
        UTILS_WARNING( "[CUDA] Call to '%s' failed.", #fct ); \
    }

#define SCOREP_CUPTI_CALL( fct )                                        \
    {                                                                   \
        CUptiResult status = fct;                                       \
        if ( status != CUPTI_SUCCESS )                                  \
        {                                                               \
            const char* msg;                                            \
            cuptiGetResultString( status, &msg );                       \
            UTILS_WARNING( "[CUPTI] Call to '%s' failed with message: '%s'",  #fct, msg ); \
        }                                                               \
    }


#endif  /* SCOREP_CUPTI_H */
