/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015,
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

#ifndef SCOREP_PRIVATE_DEFINITIONS_PARADIGM_H
#define SCOREP_PRIVATE_DEFINITIONS_PARADIGM_H


/**
 * @file
 *
 */


/* Not really a definition, but we need to store them somewhere */
typedef struct SCOREP_Paradigm SCOREP_Paradigm;
struct SCOREP_Paradigm
{
    SCOREP_Paradigm*     next;

    SCOREP_ParadigmType  paradigm_type;
    SCOREP_ParadigmClass paradigm_class;
    SCOREP_StringHandle  name_handle;
    const char*          name;

    SCOREP_ParadigmFlags paradigm_flags;

    SCOREP_AnyHandle     property_handles[ SCOREP_INVALID_PARADIGM_PROPERTY ];
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
SCOREP_Paradigm*
SCOREP_Definitions_NewParadigm( SCOREP_ParadigmType  paradigm,
                                SCOREP_ParadigmClass paradigmClass,
                                const char*          name,
                                SCOREP_ParadigmFlags paradigmFlags );



/**
 *  Assigns a paradigm property to an existing paradigm object.
 *
 *  Assignment is only allowed once per property.
 *
 *  @param paradigm         The paradigm object.
 *  @param paradigmProperty The property to set.
 *  @param propertyValue    The property value as an definition handle.
 */
void
SCOREP_Definitions_ParadigmSetProperty( SCOREP_Paradigm*        paradigm,
                                        SCOREP_ParadigmProperty paradigmProperty,
                                        SCOREP_AnyHandle        propertyValue );


/** Call cb for each paradigm */
void
SCOREP_ForAllParadigms( void ( * cb )( SCOREP_Paradigm*,
                                       void* ),
                        void*    userData );

/**
 * Returns paradigm class (@see SCOREP_PublicTypes.h)
 * @param handle to paradigm
 * @return class (e.g. SCOREP_PARADIGM_CLASS_MPP for MPI regions)
 */
SCOREP_ParadigmClass
SCOREP_ParadigmHandle_GetClass( SCOREP_ParadigmHandle handle );

/**
 * Returns paradigm name  (@see SCOREP_PublicTypes.h)
 * @param handle to paradigm
 * @return name (lowercase, e.g., mpi)
 */
const char*
SCOREP_ParadigmHandle_GetName( SCOREP_ParadigmHandle handle );

/**
 * Returns paradigm type (@see SCOREP_PublicTypes.h)
 * @param handle to paradigm
 * @return type (e.g. SCOREP_PARADIGM_MPI for MPI regions)
 */
SCOREP_ParadigmType
SCOREP_ParadigmHandle_GetType( SCOREP_ParadigmHandle handle );

#endif /* SCOREP_PRIVATE_DEFINITIONS_PARADIGM_H */
