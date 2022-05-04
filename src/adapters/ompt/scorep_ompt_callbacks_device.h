/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 */

#ifndef SCOREP_OMPT_CALLBACKS_TARGET_H
#define SCOREP_OMPT_CALLBACKS_TARGET_H


void
scorep_ompt_cb_device_initialize( int                    device_num,
                                  const char*            type,
                                  ompt_device_t*         device,
                                  ompt_function_lookup_t lookup,
                                  const char*            documentation );


#endif /* SCOREP_OMPT_CALLBACKS_TARGET_H */
