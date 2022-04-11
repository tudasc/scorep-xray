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
 */

#ifndef SCOREP_OMPT_CALLBACKS_HOST_H
#define SCOREP_OMPT_CALLBACKS_HOST_H


void
scorep_ompt_cb_host_thread_begin( ompt_thread_t thread_type,
                                  ompt_data_t*  thread_data );

void
scorep_ompt_cb_host_thread_end( ompt_data_t* thread_data );


#endif /* SCOREP_OMPT_CALLBACKS_HOST_H */
