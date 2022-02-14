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
 * @brief rtld-audit library to notify addr2line about dlopened and
 * dlclosed shared objects. To be used either via environment variable
 * LD_AUDIT=<libdir>/libscorep_rtld_audit.so or via instrumenters
 * --depaudit|audit option (the latter works only for glibc >= 2.32).
 */

#include <config.h>

#include <dlfcn.h>
#include <link.h>
#include <stddef.h>

static void* scorep_so;
static void  (* scorep_la_preinit)( uintptr_t* cookie );
static void  (* scorep_la_objopen)( const char* name,
                                    uintptr_t*  cookie );
static void (* scorep_la_objclose)( uintptr_t* cookie );

unsigned int
la_version( unsigned int version )
{
    /* see https://man7.org/linux/man-pages/man7/rtld-audit.7.html */
    return LAV_CURRENT;
}

void
la_preinit( uintptr_t* cookie )
{
    /* Called after ctors. Link time shared objects already loaded at
       ctor time. Thus, calls to scorep_la_objopen and
       scorep_la_objclose before the exit of main correspond to
       dlopened and dlclosed shared objects only. */

    /* opens in applications namespace */
    scorep_so = dlmopen( LM_ID_BASE, SCOREP_BACKEND_LIBDIR "/libscorep_measurement.so", RTLD_LAZY );
    if ( scorep_so )
    {
        scorep_la_preinit  = dlsym( scorep_so, "scorep_la_preinit" );
        scorep_la_objopen  = dlsym( scorep_so, "scorep_la_objopen" );
        scorep_la_objclose = dlsym( scorep_so, "scorep_la_objclose" );
        if ( scorep_la_preinit && scorep_la_objopen && scorep_la_objclose )
        {
            scorep_la_preinit( cookie );
        }
        else
        {
            scorep_la_objopen  = NULL;
            scorep_la_objclose = NULL;
        }
    }
}

unsigned int
la_objopen( struct link_map* map, Lmid_t lmid, uintptr_t* cookie )
{
    if ( scorep_la_objopen )
    {
        scorep_la_objopen( map->l_name, cookie );
    }

    /* don't audit symbol bindings */
    return 0;
}

unsigned int
la_objclose( uintptr_t* cookie )
{
    if ( scorep_la_objclose )
    {
        scorep_la_objclose( cookie );
    }
    return 0;
}
