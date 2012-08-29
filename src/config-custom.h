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

#ifndef CONFIG_CUSTOM_H
#define CONFIG_CUSTOM_H


/**
 * @file       config-custom.h
 * @author     Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 * @author     Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 * @maintainer Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @brief This file gets included by config.h (resp. config-frontend.h and
 * config-backend.h) and contains supplementary macros to be used with the
 * macros in config.h.
 *
 */

#define UTILS_DEBUG_MODULES \
    UTILS_DEFINE_DEBUG_MODULE( CORE,          0 ), \
    UTILS_DEFINE_DEBUG_MODULE( MPI,           1 ), \
    UTILS_DEFINE_DEBUG_MODULE( USER,          2 ), \
    UTILS_DEFINE_DEBUG_MODULE( COMPILER,      3 ), \
    UTILS_DEFINE_DEBUG_MODULE( OPENMP,        4 ), \
    UTILS_DEFINE_DEBUG_MODULE( EVENTS,        5 ), \
    UTILS_DEFINE_DEBUG_MODULE( DEFINITIONS,   6 ), \
    UTILS_DEFINE_DEBUG_MODULE( CONFIG,        7 ), \
    UTILS_DEFINE_DEBUG_MODULE( PROFILE,       8 ), \
    UTILS_DEFINE_DEBUG_MODULE( OA,            9 ), \
    UTILS_DEFINE_DEBUG_MODULE( MPIPROFILING, 10 ), \
    UTILS_DEFINE_DEBUG_MODULE( FILTERING,    11 ), \
    UTILS_DEFINE_DEBUG_MODULE( METRIC,       12 ), \
    UTILS_DEFINE_DEBUG_MODULE( TRACING,      13 ), \
    UTILS_DEFINE_DEBUG_MODULE( MEMORY,       14 ), \
    UTILS_DEFINE_DEBUG_MODULE( CUDA,         15 )

#endif /* CONFIG_CUSTOM_H */
