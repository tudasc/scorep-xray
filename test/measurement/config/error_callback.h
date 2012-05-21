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

#ifndef SCOREP_TEST_MEASUREMENT_CONFIG_ERROR_CALLBACK
#define SCOREP_TEST_MEASUREMENT_CONFIG_ERROR_CALLBACK




/**
 * @file       test/measurement/config/error_callback.h
 * @maintainer Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @status alpha
 *
 */


#ifdef _cplusplus
extern "C" {
#endif


extern CuTest* the_test;


int64_t
cutest_scorep_error_callback( const char*             package,
                              const char*             file,
                              const uint64_t          line,
                              const char*             function,
                              const SCOREP_Error_Code errorCode,
                              const char*             msgFormatString,
                              va_list                 va );


#ifdef _cplusplus
}
#endif


#endif /* SCOREP_TEST_MEASUREMENT_CONFIG_ERROR_CALLBACK */
