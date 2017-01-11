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

#ifndef SCOREP_PARADIGMS_H
#define SCOREP_PARADIGMS_H

/**
 * @file
 *
 * @brief Paradigm management.
 *
 * A long time ago in a galaxy far, far away....
 *
 * ....there was once only MPI. Than there was OpenMP. All was well as
 * computer scientists and performance engineers can only count to two.
 *
 * Than there came CUDA along, SHMEM, Pthreads, and countless more. Now the
 * computer scientists were in trouble to keep up with the pace and the
 * performance engineers to remember what each of the paradigms mean.
 *
 * At one fine day a boy was born who brought enlightenment to the world
 * by inventing an extensible and self-describing paradigm framework.
 *
 * Instead of relying on the performance engineers memory, each (possibly) used
 * parallel paradigm will be described by the performance data format itself.
 *
 * This includes the following properties:
 *  - The class of this parallel paradigm. Currently known are:
 *     - multi-process (MPP)
 *     - fork/join threading
 *     - create/wait threading
 *     - accelerator
 *  - A human readable name
 *
 * Additionally, an extensible properties framework exists too. It is split
 * between boolean types and other typed properties. See SCOREP_ParadigmFlags
 * and SCOREP_ParadigmProperty respectively.
 *
 * The computer scientists now needs to call SCOREP_Paradigms_RegisterParallelParadigm
 * at measurement init time for all compiled-in adapters which generates
 * events for parallel paradigms. And possibly also set appropriate properties.
 *
 * And all lived happily ever after...
 */

#include <SCOREP_Types.h>

/**
 *  Registers a known parallel paradigm type to be used in this measurement.
 *
 *  This should be called at measurement init time before any use of the
 *  paradigm type. Must be called by all processes, regardless the paradigm
 *  class and the current mode of measurement (i.e., in MPP mode not only by the)
 *  root process).
 *
 *  @param paradigm      The paradigm to register.
 *  @param paradigmClass The class of this paradigm.
 *  @param name          The human readable name of this paradigm.
 *  @param paradigmFlags Any flags for this paradigm, pass
 *                       SCOREP_PARADIGM_FLAG_NONE for no flags.
 */
void
SCOREP_Paradigms_RegisterParallelParadigm( SCOREP_ParadigmType  paradigm,
                                           SCOREP_ParadigmClass paradigmClass,
                                           const char*          name,
                                           SCOREP_ParadigmFlags paradigmFlags );


/**
 *  Sets a paradigm string valued property to an already registered paradigm.
 *
 *  This should be called at measurement init time before any use of the
 *  paradigm type. Must be called by all processes, regardless the paradigm
 *  class and the current mode of measurement (i.e., in MPP mode not only by the)
 *  root process).
 *
 *  @param paradigm         The paradigm to annotate.
 *  @param paradigmProperty The property to set.
 *  @param propertyValue    The string value of the property.
 */
void
SCOREP_Paradigms_SetStringProperty( SCOREP_ParadigmType     paradigm,
                                    SCOREP_ParadigmProperty paradigmProperty,
                                    const char*             propertyValue );


/**
 *  Returns the given name of a paradigm.
 *
 *  Returns for non-parallel paradigm the internal hard-coded name.
 *
 *  @param paradigm The paradigm to query.
 *
 *  @return The name of the paradigm.
 */
const char*
SCOREP_Paradigms_GetParadigmName( SCOREP_ParadigmType paradigm );


/**
 *  Returns the paradigm class of a parallel paradigm.
 *
 *  Returns for non-parallel paradigm @a SCOREP_INVALID_PARADIGM_CLASS.
 *
 *  @param paradigm The paradigm to query.
 *
 *  @return The class of the paradigm.
 */
SCOREP_ParadigmClass
SCOREP_Paradigms_GetParadigmClass( SCOREP_ParadigmType paradigm );


/**
 *  Convenience macro to test a paradigm for a given paradigm class.
 *
 *  @param paradigm The paradigm type to test.
 *  @param CLASS    The paradigm class to test for, without the
 *                  `SCOREP_PARADIGM_CLASS_` prefix.
 *
 *  @return True if the given paradigm is of class CLASS.
 */
#define SCOREP_PARADIGM_TEST_CLASS( paradigm, CLASS ) \
    ( SCOREP_Paradigms_GetParadigmClass( paradigm ) == SCOREP_PARADIGM_CLASS_ ## CLASS )


#endif /* SCOREP_PARADIGMS_H */
