/**
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2017,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */

#ifndef SCOREP_PTHREAD_EVENT_FUNCTIONS_H
#define SCOREP_PTHREAD_EVENT_FUNCTIONS_H

/**
 * Declaration of all __real_* functions used by the pthread library wrapper
 */

#include <pthread.h>

#include "scorep_pthread.h"

#include <scorep/SCOREP_Libwrap_Macros.h>

#define SCOREP_PTHREAD_REGION( rettype, name, NAME, TYPE, ARGS ) \
    SCOREP_LIBWRAP_DECLARE_REAL_FUNC( ( rettype ), name, ARGS );

SCOREP_PTHREAD_REGIONS

#undef SCOREP_PTHREAD_REGION

#endif /* SCOREP_PTHREAD_EVENT_FUNCTIONS_H */
