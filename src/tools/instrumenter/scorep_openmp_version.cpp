/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * Implements the class for PDT instrumentation.
 */

#include <config.h>
#include <omp.h>

#ifdef _OPENMP
int scorep_openmp_version = _OPENMP;
#else
int scorep_openmp_version = 0;
#endif
