/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011, 2015-2016,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file scorep_oa_registry_protocol.h
 *
 * This file contains protocol definitions of communication with Periscope registry service
 */

#ifndef SCOREP_OA_REGISTRY_PROTOCOL_H_INCLUDED
#define SCOREP_OA_REGISTRY_PROTOCOL_H_INCLUDED

/* server version and build date */

#define PERISCOPE_VERSION        "2.0"
#define PERISCOPE_BUILD_DATE     "Jan/2016"


/* command names, general */
#define CMD_QUIT          "QUIT"
#define CMD_HELP          "HELP"     /* give help message */
#define CMD_CREATE        "CREATE"   /* create a new entry*/
#define CMD_SEARCH        "SEARCH"   /* search for entries */
#define CMD_DELETE        "DELETE"   /* delete an entry */
#define CMD_DELETE_SHORT  "DEL"      /* short version to ease interactive usage */
#define CMD_LIST          "LIST"     /* list entries */
#define CMD_SHOW          "SHOW"     /* show a specific entry */
#define CMD_CLEAN         "CLEAN"    /* delete all entries (debugging) */
#define CMD_CHANGE        "CHANGE"   /* change an entry */
#define CMD_TEST          "TEST"     /* start testing of the all connected monitoring processes */


/* command names, string handling */
#define CMD_STR_ADD       "ADDSTR"   /* store a string */
#define CMD_STR_GET       "GETSTR"   /* retrieve a specific string */
#define CMD_STR_DEL       "DELSTR"   /* delete a string */
#define CMD_STR_LIST      "LISTSTR"  /* list all strings */
#define CMD_STR_COUNT     "COUNTSTR" /* count the number of available strings */


/* short description for the commands */
#define DESCR_QUIT        "terminates the connection"
#define DESCR_HELP        "shows this message"
#define DESCR_CREATE      "creates a new entry"
#define DESCR_SEARCH      "searches for entries"
#define DESCR_DELETE      "deletes an entry"
#define DESCR_LIST        "lists all entries"
#define DESCR_SHOW        "shows a specific entry"
#define DESCR_STR_ADD     "adds a string to an entry"
#define DESCR_STR_GET     "retrieves a specific string"
#define DESCR_STR_DEL     "deletes the specified string"
#define DESCR_STR_LIST    "lists all string for the specified entry"
#define DESCR_STR_COUNT   "counts the number of available strings for the specified entry"
#define DESCR_CLEAN       "deletes all entries (** DEBUGGING / use with care **)"
#define DESCR_CHANGE      "changes an existing entry"


/* message prefixes */

#define PREFIX_SUCCESS "+OK"
#define PREFIX_ERROR   "-ERR"

#define SUCCESS( msg__ ) PREFIX_SUCCESS " " msg__ "\n"
#define ERROR( msg__ )   PREFIX_ERROR " " msg__ "\n"

/* message components */
#define STR_WELCOME                     "%s:%d PERISCOPE registry server ver. %s (%s)"
#define STR_QUIT_OK                     "%s:%d PERISCOPE registry server closing connection"
#define STR_HELP_OK                     "showing available commands"
#define STR_COMMAND_UNKNOWN             "unknown command: %s"

#define STR_BAD_FORMAT                  "bad format."
#define STR_INCOMPLETE_DATA             "incomplete data."
#define STR_ENTRY_NOT_FOUND             "no such entry ID"
#define STR_STRING_NOT_FOUND            "no such string ID"


#define STR_CREATE_USAGE                "Usage: " CMD_CREATE " app=<app_name> site=<site_name> mach=<mach_name> node=<node_name> port=<port> pid=<pid> comp=<component> tag=<tag>"
#define STR_CREATE_BAD_FORMAT           STR_BAD_FORMAT
#define STR_CREATE_INCOMPLETE_DATA      STR_INCOMPLETE_DATA
#define STR_CREATE_OK                   "entry ID is %d"

#define STR_SEARCH_USAGE                "Usage: " CMD_SEARCH " [app=<app_name>] [site=<site_name>] [mach=<mach_name>] [node=<node_name>] [port=<port>] [pid=<pid>] [comp=<component>] [tag=<tag>]"
#define STR_SEARCH_BAD_FORMAT           STR_BAD_FORMAT
#define STR_SEARCH_OK                   "displaying search results"

#define STR_CHANGE_USAGE                "Usage: " CMD_CHANGE " <entry id> [app=<app_name>] [site=<site_name>] [mach=<mach_name>] [node=<node_name>] [port=<port>] [pid=<pid>] [comp=<component>] [tag=<tag>]"
#define STR_CHANGE_BAD_FORMAT           STR_BAD_FORMAT
#define STR_CHANGE_NOT_FOUND            STR_ENTRY_NOT_FOUND
#define STR_CHANGE_OK                   "applied changes to entry %d"

#define STR_DELETE_USAGE                "Usage: " CMD_DELETE " <entry id>"
#define STR_DELETE_BAD_FORMAT           STR_BAD_FORMAT
#define STR_DELETE_NOT_FOUND            STR_ENTRY_NOT_FOUND
#define STR_DELETE_OK                   "entry %d deleted"

#define STR_LIST_USAGE                  "Usage: " CMD_LIST
#define STR_LIST_OK                     "%d entries in registry"

#define STR_CLEAN_USAGE                 "Usage: " CMD_CLEAN
#define STR_CLEAN_OK                    "%d entries deleted"

#define STR_SHOW_USAGE                  "Usage: " CMD_SHOW " <entry id>"
#define STR_SHOW_BAD_FORMAT             STR_BAD_FORMAT
#define STR_SHOW_NOT_FOUND              STR_ENTRY_NOT_FOUND
#define STR_SHOW_OK                     "showing entry %d"


#define STR_ADDSTR_USAGE                "Usage: " CMD_STR_ADD " <entry id> <string>"
#define STR_ADDSTR_NOT_FOUND            STR_ENTRY_NOT_FOUND
#define STR_ADDSTR_BAD_FORMAT           STR_BAD_FORMAT
#define STR_ADDSTR_OK                   "string ID is %d"

#define STR_GETSTR_USAGE                "Usage: " CMD_STR_GET " <entry id> <string id>"
#define STR_GETSTR_ENTRY_NOT_FOUND      STR_ENTRY_NOT_FOUND
#define STR_GETSTR_STRING_NOT_FOUND     STR_STRING_NOT_FOUND
#define STR_GETSTR_BAD_FORMAT           STR_BAD_FORMAT
#define STR_GETSTR_OK                   "string %d of entry %d size %d"

#define STR_DELSTR_USAGE                "Usage: " CMD_STR_DEL " <entry id> <string id>"
#define STR_DELSTR_ENTRY_NOT_FOUND      STR_ENTRY_NOT_FOUND
#define STR_DELSTR_STRING_NOT_FOUND     STR_STRING_NOT_FOUND
#define STR_DELSTR_BAD_FORMAT           STR_BAD_FORMAT
#define STR_DELSTR_OK                   "deleted string %d of entry %d"

#define STR_LISTSTR_USAGE               "Usage: " CMD_STR_LIST " <entry id>"
#define STR_LISTSTR_NOT_FOUND           STR_ENTRY_NOT_FOUND
#define STR_LISTSTR_BAD_FORMAT          STR_BAD_FORMAT
#define STR_LISTSTR_OK                  "listing %d strings for entry %d"

#define STR_COUNTSTR_USAGE              "Usage: " CMD_STR_COUNT " <entry id>"
#define STR_COUNTSTR_NOT_FOUND          STR_ENTRY_NOT_FOUND
#define STR_COUNTSTR_BAD_FORMAT         STR_BAD_FORMAT
#define STR_COUNTSTR_OK                 "%d strings stored for entry %d"




/* this is sent as the last line of multiline responses of the server */
#define STR_END_OF_MULTILINE "."

/* */
#define STR_ENTRYDATA                  "ID %d app=\"%s \" site=\"%s \" mach=\"%s \" node=\"%s \" port=%d pid=%d comp=\"%s \" tag=\"%s \"\n"

/*
 * the protocol messages
 */
#define MSG_WELCOME                    SUCCESS( STR_WELCOME )
#define MSG_QUIT                       SUCCESS( STR_QUIT_OK )
#define MSG_HELP                       SUCCESS( STR_HELP_OK )

#define MSG_UNKNOWN_COMMAND            ERROR( STR_COMMAND_UNKNOWN )

#define MSG_CREATE_BAD_FORMAT          ERROR( STR_CREATE_BAD_FORMAT " " STR_CREATE_USAGE )
#define MSG_CREATE_INCOMPLETE          ERROR( STR_CREATE_INCOMPLETE_DATA " " STR_CREATE_USAGE )
#define MSG_CREATE_SUCCESS             SUCCESS( STR_CREATE_OK )

#define MSG_SEARCH_BAD_FORMAT          ERROR( STR_SEARCH_BAD_FORMAT " " STR_SEARCH_USAGE )
#define MSG_SEARCH_SUCCESS             SUCCESS( STR_SEARCH_OK )


/* search missing here */


#define MSG_CHANGE_BAD_FORMAT          ERROR( STR_CHANGE_BAD_FORMAT " " STR_CHANGE_USAGE )
#define MSG_CHANGE_ENTRY_NOT_FOUND     ERROR( STR_SHOW_NOT_FOUND )
#define MSG_CHANGE_INCOMPLETE          ERROR( STR_CREATE_INCOMPLETE_DATA " " STR_CHANGE_USAGE )
#define MSG_CHANGE_SUCCESS             SUCCESS( STR_CHANGE_OK )



#define MSG_DELETE_BAD_FORMAT          ERROR( STR_DELETE_BAD_FORMAT " " STR_DELETE_USAGE )
#define MSG_DELETE_NOT_FOUND           ERROR( STR_DELETE_NOT_FOUND )
#define MSG_DELETE_SUCCESS             SUCCESS( STR_DELETE_OK )

#define MSG_LIST_SUCCESS               SUCCESS( STR_LIST_OK )

#define MSG_CLEAN_SUCCESS              SUCCESS( STR_CLEAN_OK )

#define MSG_SHOW_BAD_FORMAT            ERROR( STR_SHOW_BAD_FORMAT " " STR_SHOW_USAGE )
#define MSG_SHOW_NOT_FOUND             ERROR( STR_SHOW_NOT_FOUND )
#define MSG_SHOW_SUCCESS               SUCCESS( STR_SHOW_OK )

#define MSG_ADDSTR_BAD_FORMAT          ERROR( STR_ADDSTR_BAD_FORMAT " " STR_ADDSTR_USAGE )
#define MSG_ADDSTR_NOT_FOUND           ERROR( STR_ADDSTR_NOT_FOUND )
#define MSG_ADDSTR_SUCCESS             SUCCESS( STR_ADDSTR_OK )

#define MSG_GETSTR_BAD_FORMAT          ERROR( STR_GETSTR_BAD_FORMAT " " STR_GETSTR_USAGE )
#define MSG_GETSTR_ENTRY_NOT_FOUND     ERROR( STR_GETSTR_ENTRY_NOT_FOUND )
#define MSG_GETSTR_STRING_NOT_FOUND    ERROR( STR_GETSTR_STRING_NOT_FOUND )
#define MSG_GETSTR_SUCCESS             SUCCESS( STR_GETSTR_OK )

#define MSG_DELSTR_BAD_FORMAT          ERROR( STR_DELSTR_BAD_FORMAT " " STR_DELSTR_USAGE )
#define MSG_DELSTR_ENTRY_NOT_FOUND     ERROR( STR_DELSTR_ENTRY_NOT_FOUND )
#define MSG_DELSTR_STRING_NOT_FOUND    ERROR( STR_DELSTR_STRING_NOT_FOUND )
#define MSG_DELSTR_SUCCESS             SUCCESS( STR_DELSTR_OK )

#define MSG_LISTSTR_BAD_FORMAT         ERROR( STR_LISTSTR_BAD_FORMAT " " STR_LISTSTR_USAGE )
#define MSG_LISTSTR_NOT_FOUND          ERROR( STR_LISTSTR_NOT_FOUND )
#define MSG_LISTSTR_SUCCESS            SUCCESS( STR_LISTSTR_OK )

#define MSG_COUNTSTR_BAD_FORMAT        ERROR( STR_COUNTSTR_BAD_FORMAT " " STR_COUNTSTR_USAGE )
#define MSG_COUNTSTR_NOT_FOUND         ERROR( STR_COUNTSTR_NOT_FOUND )
#define MSG_COUNTSTR_SUCCESS           SUCCESS( STR_COUNTSTR_OK )


#endif /* SCOREP_OA_REGISTRY_PROTOCOL_H_INCLUDED */
