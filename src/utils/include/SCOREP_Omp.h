/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
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

#ifndef SCOREP_OMP_H
#define SCOREP_OMP_H



/**
 * @file SCOREP_Omp.h In order to reduce conditional compilation for OpenMP
 * runtime library functions, we provide some defines that replace the OpenMP
 * functions when compiled without OpenMP flags. Note that not all of the
 * OpenMP runtime library functions can be handled this way.
 *
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 *
 */

#define SCOREP_PRAGMA_STR_( str ) #str
#define SCOREP_PRAGMA_STR( str ) SCOREP_PRAGMA_STR_( str )
// the following define is needed to please the PGI compiler
#define PRAGMA_OP( x ) _Pragma( x )
#define SCOREP_PRAGMA( pragma ) PRAGMA_OP( SCOREP_PRAGMA_STR( pragma ) )

#ifdef _OPENMP

    #include <omp.h>

    #define SCOREP_PRAGMA_OMP( omp_pragma ) SCOREP_PRAGMA( omp omp_pragma )

#else

#ifdef __cplusplus
extern "C" {
#endif

    #define omp_set_num_threads( n ) do { } while ( 0 )
    #define omp_get_num_threads() 1
    #define omp_get_max_threads() 1
    #define omp_get_thread_num() 1
// omp_get_num_procs, no easy way to handle, but probably not used in scorep
    #define omp_in_parallel() 0
    #define omp_set_dynamic( n ) do { } while ( 0 )
    #define omp_get_dynamic() 0
    #define omp_set_nested( n ) do { } while ( 0 )
    #define omp_get_nested() 0

// omp_get/set_schedule not easy to handle due to the omp_sched_t
// parameter, but probably not used in scorep
    #define omp_get_thread_limit() 1
    #define omp_set_max_active_levels( n ) do { } while ( 0 )
    #define omp_get_max_active_levels() 0
    #define omp_get_level() 0
    #define omp_get_ancestor_thread_num( n ) 0
    #define omp_get_team_size( n ) 1
    #define omp_get_active_level() 0

// lock functions can't be handled easily because the require omp_lock_t
// parameters

// omp_get_wtime/wtick also need special treatment

    #define SCOREP_PRAGMA_OMP( omp_pragma )

#ifdef __cplusplus
} /* "C" */
#endif

#endif

#endif /* SCOREP_OMP_H */
