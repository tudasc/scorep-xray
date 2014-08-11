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

void
test_ipc_get_size( CuTest* tc );

void
test_ipc_get_rank( CuTest* tc );

void
test_ipc_send_recive_one_to_one( CuTest* tc );

void
test_ipc_send_recive_all_to_one( CuTest* tc );

void
test_ipc_send_recive_one_to_all( CuTest* tc );

void
test_ipc_send_recive_all_to_all( CuTest* tc );

void
test_ipc_broadcast( CuTest* tc );

void
test_ipc_scatter( CuTest* tc );

void
test_ipc_scatterv( CuTest* tc );

void
test_ipc_gather( CuTest* tc );

void
test_ipc_gatherv( CuTest* tc );

void
test_ipc_gatherv0( CuTest* tc );

void
test_ipc_allgather( CuTest* tc );

void
test_ipc_reduce( CuTest* tc );

void
test_ipc_allreduce( CuTest* tc );
