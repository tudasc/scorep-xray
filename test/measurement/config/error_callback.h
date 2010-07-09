#ifndef SILC_TEST_MEASUREMENT_CONFIG_ERROR_CALLBACK
#define SILC_TEST_MEASUREMENT_CONFIG_ERROR_CALLBACK


/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


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
cutest_silc_error_callback( const char*           function,
                            const char*           file,
                            const uint64_t        line,
                            const SILC_Error_Code errorCode,
                            const char*           msgFormatString,
                            va_list               va );


#ifdef _cplusplus
}
#endif


#endif /* SILC_TEST_MEASUREMENT_CONFIG_ERROR_CALLBACK */
