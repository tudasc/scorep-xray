/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2021,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */

/**
 * @file
 *
 * @brief If addr2line is not supported, still provide init and
 * finalize to please measurement initialization and finalization.
 */

void
SCOREP_Addr2line_Initialize( void )
{
}

void
SCOREP_Addr2line_Finalize( void )
{
}
