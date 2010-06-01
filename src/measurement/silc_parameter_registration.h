/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 * @file        silc_parameter_registration.c
 * @maintainer  Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @brief   Handling of string parameters.
 *
 * In parameter-based profiling, the calltree is split due to the value of a parameter.
 * To avoid storing multiple strings and comparing strings, the measurement system uses
 * handles. To effectively dealing with strings, the profiling system (and mayby the
 * tracing system and trace analyzer) only compares and stores handles. Thus, it is
 * important that same string values have the same handle. Furthermore, less tsorage is
 * required.
 * To guarantee that same string values have same handles, all string parameter values
 * must be compared with previous string values. A hashtable is used to efficiently find,
 * compare and store previous string values. The string value is used as key and the
 * handle is the value of the table entry.
 *
 */

/**
   Initilaizes the hashtable, which contains the string values from parameter events.
 */
void
silc_parameter_table_initialize();

/**
   Deletes the hashtable for string values from parameter events. Frees all used memory
 */
void
silc_parameter_table_finalize();

/**
   Finds a string in the hashtable. If it does not already exist in the hashtable a new
   string is registered and the handle, string pair is stored in the hashtable. If the
   @a value was already in the hashtable the stored handle is returned, else the newly
   created handle is returned.
   @param value The string for which the handle should be found.
   @return A string handle for @a value.
 */
SILC_StringHandle
silc_get_parameter_string_handle( const char* value );
