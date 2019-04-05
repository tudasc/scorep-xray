/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2015-2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012, 2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012, 2015,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef CONFIG_CUSTOM_H
#define CONFIG_CUSTOM_H


/**
 * @file
 *
 * @brief This file gets included by config.h (resp. config-frontend.h and
 * config-backend.h) and contains supplementary macros to be used with the
 * macros in config.h.
 *
 */

#define UTILS_DEBUG_MODULES \
    UTILS_DEFINE_DEBUG_MODULE( CORE,                0 ), \
    UTILS_DEFINE_DEBUG_MODULE( MPI,                 1 ), \
    UTILS_DEFINE_DEBUG_MODULE( USER,                2 ), \
    UTILS_DEFINE_DEBUG_MODULE( COMPILER,            3 ), \
    UTILS_DEFINE_DEBUG_MODULE( OPARI2,              4 ), \
    UTILS_DEFINE_DEBUG_MODULE( EVENTS,              5 ), \
    UTILS_DEFINE_DEBUG_MODULE( DEFINITIONS,         6 ), \
    UTILS_DEFINE_DEBUG_MODULE( CONFIG,              7 ), \
    UTILS_DEFINE_DEBUG_MODULE( PROFILE,             8 ), \
    UTILS_DEFINE_DEBUG_MODULE( OA,                  9 ), \
    UTILS_DEFINE_DEBUG_MODULE( MPIPROFILING,       10 ), \
    UTILS_DEFINE_DEBUG_MODULE( FILTERING,          11 ), \
    UTILS_DEFINE_DEBUG_MODULE( METRIC,             12 ), \
    UTILS_DEFINE_DEBUG_MODULE( TRACING,            13 ), \
    UTILS_DEFINE_DEBUG_MODULE( ALLOCATOR,          14 ), \
    UTILS_DEFINE_DEBUG_MODULE( CUDA,               15 ), \
    UTILS_DEFINE_DEBUG_MODULE( UNIFY,              16 ), \
    UTILS_DEFINE_DEBUG_MODULE( THREAD_FORK_JOIN,   17 ), \
    UTILS_DEFINE_DEBUG_MODULE( SHMEM,              18 ), \
    UTILS_DEFINE_DEBUG_MODULE( PTHREAD,            19 ), \
    UTILS_DEFINE_DEBUG_MODULE( THREAD_CREATE_WAIT, 20 ), \
    UTILS_DEFINE_DEBUG_MODULE( THREAD,             21 ), \
    UTILS_DEFINE_DEBUG_MODULE( OPENCL,             22 ), \
    UTILS_DEFINE_DEBUG_MODULE( SUBSTRATE,          23 ), \
    UTILS_DEFINE_DEBUG_MODULE( UNWINDING,          24 ), \
    UTILS_DEFINE_DEBUG_MODULE( SAMPLING,           25 ), \
    UTILS_DEFINE_DEBUG_MODULE( MEMORY,             26 ), \
    UTILS_DEFINE_DEBUG_MODULE( OPENACC,            27 ), \
    UTILS_DEFINE_DEBUG_MODULE( TOPOLOGIES,         28 ), \
    UTILS_DEFINE_DEBUG_MODULE( IO,                 29 ), \
    UTILS_DEFINE_DEBUG_MODULE( IO_MANAGEMENT,      30 ), \
    UTILS_DEFINE_DEBUG_MODULE( MOUNTINFO,          31 )

#endif /* CONFIG_CUSTOM_H */
