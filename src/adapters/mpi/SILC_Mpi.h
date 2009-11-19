#ifndef SILC_MPI_H
#define SILC_MPI_H

/**
   @file  SILC_Mpi.h
   @ingroup MPI_Wrapper

   It includes all source files for the MPI wrappers.
 */

#include "SILC_Mpi_Communicator.h"
#include "SILC_Mpi_Reg.h"
#include "SILC_Error.h"
#include "SILC_Debug.h"

#include "SILC_Definitions.h"
#include "SILC_Events.h"
#include "SILC_RuntimeManagement.h"

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

/** @defgroup MPI_Wrapper SILC MPI wrapper library
   The MPI wrapper provides an implementation of the MPI functions which use the
   PMPI functions internally to provide the desired MPI functionality. Additionally,
   calls to the measurement system are generated for measurement. To enable the
   MPI measurement, this implementation must be linked to the instrumentated application
   before it is linked with MPI. When using a instrumenter tool, this should be handled
   by the instrumenter tool.

   All functions except MPI_Wtime and MPI_Ticks are instrumented to generate an enter
   event on function begin and an exit event on function end. Additionally, some
   functions of special interest provide additional events between the enter and exit
   event. E.g. the MPI_Send provide an send event before the data
   is send, the MPI_Recv provide a receive event after the date is received and the
   Collective functions provide a collective event. The paricular events, generated
   by each function should be defined in the function specific documentation.

   The wrapper functions are automatically generated by a tool from a set of templates.
   Thus, do not edit the generated file manually, but the templates instead. Else the
   modifications might be lost.

   @{
 */

/** internal array of statuses */
extern MPI_Status* silc_my_status_array;

/** size of internal status array */
extern int32_t silc_my_status_array_size;

/**
 * Get a pointer to a status array of at least 'size' statuses
 * @param  size minimal requested size
 * @return pointer to status array
 */
extern MPI_Status*
silc_get_status_array( int32_t size );

/** Flag to indicate whether event generation is turned on or off. If it is set to 0,
    events are generated. If it is set to non-zero, no events are generated.
 */
extern int32_t silc_mpi_nogen;

extern int     silc_mpi_status_size;

/** Check whether event generation is turned on. Returns the inverse value of
    silc_mpi_nogen
 */
#define SILC_MPI_IS_EVENT_GEN_ON            ( !silc_mpi_nogen )

/** Check whether event generation is turned on for a specific group.
 */
#define SILC_MPI_IS_EVENT_GEN_ON_FOR( group ) ( ( !silc_mpi_nogen ) && \
                                                ( silc_mpi_enabled & group ) )


/** turn off event generation */
#define SILC_MPI_EVENT_GEN_OFF()            silc_mpi_nogen = 1

/** turn on event generation */
#define SILC_MPI_EVENT_GEN_ON()             silc_mpi_nogen = 0

/** @} */

#endif /* SILC_MPI_H */
