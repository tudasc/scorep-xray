/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2015-2016, 2020, 2022,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012, 2015, 2021-2022,
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
    UTILS_DEFINE_DEBUG_MODULE( MPIPROFILING,        9 ), \
    UTILS_DEFINE_DEBUG_MODULE( FILTERING,          10 ), \
    UTILS_DEFINE_DEBUG_MODULE( METRIC,             11 ), \
    UTILS_DEFINE_DEBUG_MODULE( TRACING,            12 ), \
    UTILS_DEFINE_DEBUG_MODULE( ALLOCATOR,          13 ), \
    UTILS_DEFINE_DEBUG_MODULE( CUDA,               14 ), \
    UTILS_DEFINE_DEBUG_MODULE( UNIFY,              15 ), \
    UTILS_DEFINE_DEBUG_MODULE( THREAD_FORK_JOIN,   16 ), \
    UTILS_DEFINE_DEBUG_MODULE( SHMEM,              17 ), \
    UTILS_DEFINE_DEBUG_MODULE( PTHREAD,            18 ), \
    UTILS_DEFINE_DEBUG_MODULE( THREAD_CREATE_WAIT, 19 ), \
    UTILS_DEFINE_DEBUG_MODULE( THREAD,             20 ), \
    UTILS_DEFINE_DEBUG_MODULE( OPENCL,             21 ), \
    UTILS_DEFINE_DEBUG_MODULE( SUBSTRATE,          22 ), \
    UTILS_DEFINE_DEBUG_MODULE( UNWINDING,          23 ), \
    UTILS_DEFINE_DEBUG_MODULE( SAMPLING,           24 ), \
    UTILS_DEFINE_DEBUG_MODULE( MEMORY,             25 ), \
    UTILS_DEFINE_DEBUG_MODULE( OPENACC,            26 ), \
    UTILS_DEFINE_DEBUG_MODULE( TOPOLOGIES,         27 ), \
    UTILS_DEFINE_DEBUG_MODULE( IO,                 28 ), \
    UTILS_DEFINE_DEBUG_MODULE( IO_MANAGEMENT,      29 ), \
    UTILS_DEFINE_DEBUG_MODULE( MOUNTINFO,          30 ), \
    UTILS_DEFINE_DEBUG_MODULE( KOKKOS,             31 ), \
    UTILS_DEFINE_DEBUG_MODULE( ADDR2LINE,          32 ), \
    UTILS_DEFINE_DEBUG_MODULE( HIP,                33 ), \
    UTILS_DEFINE_DEBUG_MODULE( OMPT,               34 )

#endif /* CONFIG_CUSTOM_H */
