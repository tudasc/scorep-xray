/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2022,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * Collection of 'ompt to string' functions for debugging.
 */


#if HAVE( UTILS_DEBUG )

static const char*
thread2string( ompt_thread_t t )
{
    switch ( t )
    {
        case ompt_thread_initial:
            return "initial";
        case ompt_thread_worker:
            return "worker";
        case ompt_thread_other:
            return "other";
        case ompt_thread_unknown:
            return "unknown";
    }
    UTILS_BUG( "invalid thread type '%d' provided", t );
    return "";
}

#endif /* HAVE( UTILS_DEBUG ) */
