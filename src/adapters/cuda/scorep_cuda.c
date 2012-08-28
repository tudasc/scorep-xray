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
 *  @file       scorep_cuda.c
 *  @maintainer Robert Dietrich <robert.dietrich@zih.tu-dresden.de>
 *
 *  Implementation of commonly used functionality within the CUDA adapter.
 */

#include <config.h>

#include "scorep_cuda.h"


uint8_t scorep_cuda_record_kernels = 0;

bool scorep_cuda_record_idle = false;

bool scorep_cuda_record_memcpy = false;

void
scorep_cuda_handle_cupti_error( CUptiResult err,
                                const char* file,
                                const int   line )
{
    const char* errstr;

    cuptiGetResultString( err, &errstr );

    UTILS_WARNING( "[CUPTI] %s:%d:'%s'", file, line, errstr );
}
