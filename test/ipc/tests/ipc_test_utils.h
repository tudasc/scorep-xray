/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2012-2014,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 */

#define CALCULATE_TRIANGULAR_NUMBER( INITIAL ) \
    ( ( ( ( INITIAL )*( INITIAL ) ) + ( INITIAL ) ) / 2 );

#define DO_ASSERTIONS( CONDITION, NUM_EXPECT_EMELM )                                     \
    if ( CONDITION )                                                                     \
    {                                                                                    \
        CuAssertIntEqualsMsg( tc, "return value", 0, return_value );                     \
    }                                                                                    \
    else                                                                                 \
    {                                                                                    \
        CuAssertIntEqualsMsg( tc, "return value", 0, 0 );                                \
    }                                                                                    \
                                                                                         \
    for ( int i = 0; i < NUM_EXPECT_EMELM; ++i )                                         \
    {                                                                                    \
        if ( expected_data                                                               \
             && recived_data )                                                           \
        {                                                                                \
            CuAssertIntEquals( tc, expected_data[ i ], recived_data[ i ] );              \
        }                                                                                \
        else                                                                             \
        {                                                                                \
            CuAssertIntEqualsMsg( tc, "expected_data[ i ], recived_data[ i ]", 0, 0 );   \
        }                                                                                \
    }
