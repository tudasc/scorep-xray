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
 *
 *
 */


UTILS_BEGIN_C_DECLS


SCOREP_ErrorCode
cutest_scorep_error_callback( void*            userData,
                              const char*      file,
                              uint64_t         line,
                              const char*      function,
                              SCOREP_ErrorCode errorCode,
                              const char*      msgFormatString,
                              va_list          va );


UTILS_END_C_DECLS


#endif /* SCOREP_TEST_MEASUREMENT_CONFIG_ERROR_CALLBACK */
