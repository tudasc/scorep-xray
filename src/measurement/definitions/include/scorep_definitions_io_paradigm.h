/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2017,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_INTERNAL_DEFINITIONS_H
#error "Do not include this header directly, use SCOREP_Definitions.h instead."
#endif

#ifndef SCOREP_PRIVATE_DEFINITIONS_IO_PARADIGM_H
#define SCOREP_PRIVATE_DEFINITIONS_IO_PARADIGM_H


/**
 * @file
 *
 */


/* Not really a definition, but we need to store them somewhere */
typedef struct SCOREP_IoParadigm SCOREP_IoParadigm;
struct SCOREP_IoParadigm
{
    uint32_t               sequence_number;
    SCOREP_IoParadigm*     next;

    SCOREP_IoParadigmType  paradigm_type;
    SCOREP_IoParadigmClass paradigm_class;
    SCOREP_StringHandle    identification_handle;
    SCOREP_StringHandle    name_handle;
    const char*            name;

    SCOREP_IoParadigmFlag  paradigm_flags;
    SCOREP_AnyHandle       property_handles[ SCOREP_INVALID_IO_PARADIGM_PROPERTY ];
};


/**
 *  Creates a new paradigm definition object. Though it is not in the
 *  unification machinery. When we gain proper MPMD support, where different
 *  programs use different paradigms, we need to go through unification though.
 *
 *
 *  @param paradigm      The paradigm to register.
 *  @param paradigmClass The class of this paradigm.
 *  @param name          The human readable name of this paradigm.
 *  @param paradigmFlags Any flags for this paradigm, pass 0 for no flags.
 *
 *  @return The newly created paradigm object.
 */
SCOREP_IoParadigm*
SCOREP_Definitions_NewIoParadigm( SCOREP_IoParadigmType  paradigm,
                                  const char*            identification,
                                  const char*            name,
                                  SCOREP_IoParadigmClass paradigmClass,
                                  SCOREP_IoParadigmFlag  paradigmFlags );



/**
 *  Assigns a I/O paradigm property to an existing I/O paradigm object.
 *
 *  Assignment is only allowed once per property.
 *
 *  @param paradigm         The paradigm object.
 *  @param paradigmProperty The property to set.
 *  @param propertyValue    The property value as an definition handle.
 */
void
SCOREP_Definitions_IoParadigmSetProperty( SCOREP_IoParadigm*        paradigm,
                                          SCOREP_IoParadigmProperty paradigmProperty,
                                          SCOREP_AnyHandle          propertyValue );


/** Call cb for each paradigm */
void
SCOREP_ForAllIoParadigms( void ( * cb )( SCOREP_IoParadigm*,
                                         void* ),
                          void*    userData );


#endif /* SCOREP_PRIVATE_DEFINITIONS_IO_PARADIGM_H */
