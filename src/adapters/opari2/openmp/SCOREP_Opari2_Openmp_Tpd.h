/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2016-2017,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_OPARI2_OPENMP_TPD_H
#define SCOREP_OPARI2_OPENMP_TPD_H

/**
 * @file
 */


#include <stdint.h>

#if HAVE( SCOREP_OMP_TPD )
/* The following define is needed to please the PGI compiler */
#define PRAGMA_OP( x ) _Pragma( x )
#define SCOREP_PRAGMA( pragma ) PRAGMA_OP( UTILS_STRINGIFY( pragma ) )
#define SCOREP_PRAGMA_OMP( omp_pragma ) SCOREP_PRAGMA( omp omp_pragma )

/* Take care of name mangling the Fortran compiler might perform. */
#define POMP_TPD_MANGLED FORTRAN_MANGLED( pomp_tpd )

/* The pomp_tpd[_] variable that gets instrumented by OPARI2 to maintain
 * parent-child relationship if the --tpd option is given. Will be used by
 * scorep only if there is no OpenMP ancestry support. Codes that are
 * instrumented with pomp_tpd[_] while ancestry support is available need
 * this definition although parent-child relation is obtained via ancestry
 * functions. */
extern int64_t FORTRAN_ALIGNED POMP_TPD_MANGLED;

/* We want to write #pragma omp threadprivate(POMP_TPD_MANGLED) here
 * but as POMP_TPD_MANGLED is a macro itself, we need to do some
 * preprocessor magic to be portable. */
SCOREP_PRAGMA_OMP( threadprivate( POMP_TPD_MANGLED ) )
#endif /* HAVE( SCOREP_OMP_TPD ) */

struct scorep_thread_private_data;

#if HAVE( SCOREP_OMP_TPD )
  #if SIZEOF_VOID_P == 8
    #define SCOREP_OMP_SET_POMP_TPD_TO( newTpd ) POMP_TPD_MANGLED = ( uint64_t )newTpd
  #elif SIZEOF_VOID_P == 4
    #define SCOREP_OMP_SET_POMP_TPD_TO( newTpd ) POMP_TPD_MANGLED = ( uint32_t )newTpd
  #else
    #error Unsupported architecture. Only 32 bit and 64 bit architectures are supported.
  #endif
#else /* !HAVE( SCOREP_OMP_TPD ) */
  #define SCOREP_OMP_SET_POMP_TPD_TO( newTpd )
#endif /* !HAVE( SCOREP_OMP_TPD ) */

#endif /* SCOREP_OPARI2_OPENMP_TPD_H */
