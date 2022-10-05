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


#include "scorep_compiler_func_addr_hash.h"

#include <SCOREP_Addr2line.h>
#include <SCOREP_RuntimeManagement.h>

static void
func_addr_hash_register_obj_close_cb( void )
{
    if ( !SCOREP_IsUnwindingEnabled() )
    {
        SCOREP_Addr2line_RegisterObjcloseCb( scorep_compiler_func_addr_hash_dlclose_cb );
    }
}
