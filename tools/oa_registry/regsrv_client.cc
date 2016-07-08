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
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file regsrv_client.cc
 *
 * This file implements registry service client class
 */

#include <config.h>

#include <strings.h>
#include <string>
#include <stdlib.h>
#include <cstdio>

/* *INDENT-OFF* */

#include "stringutil.h"
#include "regsrv_client.h"
#include "regsrv_sockets.h"
#include "scorep_oa_registry_protocol.h"

using std::string;
using std::map;
using std::pair;

int RegServClient::receive_and_handle_request()
{
    printf( "Registry service: listening for input..." );

    if ( sock_ < 0 )
    {
        fprintf( stderr, "Connection is down!" );
        return 1;
    }

    int  maxlen = MAX_MESSAGE_SIZE;
    char buf[ MAX_MESSAGE_SIZE ];
    buf[ 0 ] = 0;
    memset( buf, '\0', maxlen );

    int length;
    while ( ( length = scorep_oa_sockets_read_line( sock_, buf, maxlen ) ) == 0 )
    {
    }

    for ( int i = 0; i < length; i++ )
    {
        if ( buf[ i ] == 13 ) buf[ i ] = 32;
        //printf( " %d ", buf[ i ] );
    }
    printf( " %s \n", buf );

    string::size_type pos;
    string            line;
    string            command;


    line = buf;
    pos  = get_token( line, 0, " ", command );

    printf( "Registry service: command: %s \n", command.c_str() );

    if ( !strcasecmp( command.c_str(), CMD_QUIT ) )
    {
        on_quit( line, pos );
        return 1;
    }
    if ( !strcasecmp( command.c_str(), CMD_HELP ) )
    {
        on_help( line, pos );
        return 0;
    }

    if ( !strcasecmp( command.c_str(), CMD_CREATE ) )
    {
        on_create( line, pos );
        return 0;
    }

    if ( !strcasecmp( command.c_str(), CMD_SEARCH ) )
    {
        on_search( line, pos );
        return 0;
    }

    if ( !strcasecmp( command.c_str(), CMD_DELETE ) ||
         !strcasecmp( command.c_str(), CMD_DELETE_SHORT ) )
    {
        on_delete( line, pos );
        return 0;
    }

    if ( !strcasecmp( command.c_str(), CMD_LIST ) )
    {
        on_list( line, pos );
        return 0;
    }

    if ( !strcasecmp( command.c_str(), CMD_SHOW ) )
    {
        on_show( line, pos );
        return 0;
    }
    // string storage and retrieval
    if ( !strcasecmp( command.c_str(), CMD_STR_ADD ) )
    {
        on_add_string( line, pos );
        return 0;
    }

    if ( !strcasecmp( command.c_str(), CMD_STR_GET ) )
    {
        on_get_string( line, pos );
        return 0;
    }

    if ( !strcasecmp( command.c_str(), CMD_STR_DEL ) )
    {
        on_delete_string( line, pos );
        return 0;
    }

    if ( !strcasecmp( command.c_str(), CMD_STR_LIST ) )
    {
        on_list_strings( line, pos );
        return 0;
    }

    if ( !strcasecmp( command.c_str(), CMD_STR_COUNT ) )
    {
        on_count_strings( line, pos );
        return 0;
    }

    if ( !strcasecmp( command.c_str(), CMD_CLEAN ) )
    {
        on_clean( line, pos );
        return 0;
    }

    if ( !strcasecmp( command.c_str(), CMD_CHANGE ) )
    {
        on_change( line, pos );
        return 0;
    }
    if ( !strcasecmp( command.c_str(), CMD_TEST) )
    {
        on_test( line, pos );
        return 2;
    }

    on_unknown( line, pos );
    return 0;
}



int RegServClient::write_line( char* buf )
{
    if ( sock_ < 0 )
    {
        fprintf( stderr, "Connection is down!" );
        return 1;
    }
    scorep_oa_sockets_write_line( sock_, buf );
    return 0;
}



void RegServClient::on_quit( string&           line,
                             string::size_type pos )
{
    char buf[ 400 ];
    sprintf( buf, MSG_QUIT, get_hostname().c_str(), get_port() );
    write_line( buf );
}



void RegServClient::on_unknown( string&           line,
                                string::size_type pos )
{
    char buf[ 400 ];
    string command;

    get_token( line, 0, " ", command );
    sprintf( buf, MSG_UNKNOWN_COMMAND, command.c_str() );
    write_line( buf );
}


void RegServClient::on_help( string&           line,
                             string::size_type pos )
{
    char buf[ 400 ];
    sprintf( buf, MSG_HELP );
    write_line( buf );

    sprintf( buf, "%10s %s\n", CMD_QUIT     , DESCR_QUIT );
    write_line( buf );

    sprintf( buf, "%10s %s\n", CMD_HELP     , DESCR_HELP );
    write_line( buf );

    sprintf( buf, "%10s %s\n", CMD_CREATE   , DESCR_CREATE );
    write_line( buf );

    sprintf( buf, "%10s %s\n", CMD_SEARCH   , DESCR_SEARCH );
    write_line( buf );

    sprintf( buf, "%10s %s\n", CMD_CHANGE   , DESCR_CHANGE );
    write_line( buf );

    sprintf( buf, "%10s %s\n", CMD_DELETE   , DESCR_DELETE );
    write_line( buf );

    sprintf( buf, "%10s %s\n", CMD_LIST     , DESCR_LIST );
    write_line( buf );

    sprintf( buf, "%10s %s\n", CMD_SHOW     , DESCR_SHOW );
    write_line( buf );

    sprintf( buf, "%10s %s\n", CMD_CLEAN    , DESCR_CLEAN );
    write_line( buf );

    sprintf( buf, "%10s %s\n", CMD_STR_ADD  , DESCR_STR_ADD  );
    write_line( buf );

    sprintf( buf, "%10s %s\n", CMD_STR_GET  , DESCR_STR_GET );
    write_line( buf );

    sprintf( buf, "%10s %s\n", CMD_STR_DEL  , DESCR_STR_DEL );
    write_line( buf );

    sprintf( buf, "%10s %s\n", CMD_STR_LIST , DESCR_STR_LIST );
    write_line( buf );

    sprintf( buf, "%10s %s\n", CMD_STR_COUNT, DESCR_STR_COUNT );
    write_line( buf );

    sprintf( buf, "%s\n", STR_END_OF_MULTILINE );
    write_line( buf );
}


void RegServClient::on_create( string&           line,
                               string::size_type pos )
{
    char                   buf[ 400 ];
    string                 key, value;
    string                 command;
    pair< string, string > mypair;

    RegEntry *entry = new RegEntry();

    do
    {
        pos   = get_key_value_pair( line, pos, mypair );
        key   = mypair.first;
        value = mypair.second;

        if ( key == "" )
            continue;

        if ( key == "app" )
        {
            entry->app = value;
            continue;
        }
        if ( key == "site" )
        {
            entry->site = value;
            continue;
        }
        if ( key == "mach" )
        {
            entry->mach = value;
            continue;
        }
        if ( key == "node" )
        {
            entry->node = value;
            continue;
        }
        if ( key == "port" )
        {
            entry->port = atoi( value.c_str() );
            continue;
        }
        if ( key == "pid" )
        {
            entry->pid = atoi( value.c_str() );
            continue;
        }
        if ( key == "comp" )
        {
            entry->comp = value;
            continue;
        }
        if ( key == "tag" )
        {
            entry->tag = value;
            continue;
        }

        sprintf( buf, MSG_CREATE_BAD_FORMAT );
        write_line( buf );
        return;
    } while ( pos != string::npos );

  // check if we have the information we need:
    if ( entry->app  == "" || entry->site == "" ||
         entry->mach == "" || entry->node == "" ||
         entry->port < 0   || entry->pid  == 0  ||
         entry->comp == "" || entry->tag  == "" )
    {
        sprintf( buf, MSG_CREATE_INCOMPLETE );
        write_line( buf );
        delete entry;
        return;
    }
    entry->test_comm_sock = -1;

    int id = serv_->unique_id();
    serv_->set_entry( id, entry );

#ifdef EBUG
    fprintf( stdout, "Added new entry[%d]: %s", id, entry->c_str() );
    fflush( stdout );
#endif

    sprintf( buf, MSG_CREATE_SUCCESS, id );
    write_line( buf );

}


void RegServClient::on_list( string&           line,
                             string::size_type pos )
{
    char buf[ 400 ];
    map< int , RegEntry* >::iterator it;

    sprintf( buf, MSG_LIST_SUCCESS, ( int )serv_->reg_data_.size() );
    write_line( buf );

    for( it = serv_->reg_data_.begin(); it != serv_->reg_data_.end(); it++ )
    {
        sprintf( buf, STR_ENTRYDATA, it->first,
                 it->second->app.c_str(),
                 it->second->site.c_str(),
                 it->second->mach.c_str(),
                 it->second->node.c_str(),
                 it->second->port,
                 it->second->pid,
                 it->second->comp.c_str(),
                 it->second->tag.c_str() );
        write_line( buf );
    }

    sprintf( buf, "%s\n", STR_END_OF_MULTILINE );
    write_line( buf );
}


void RegServClient::on_show( string&           line,
                             string::size_type pos )
{
    int                              id = 0;
    string                           tok;
    char                             buf[ 400 ];
    map< int , RegEntry* >::iterator it;

    get_token( line, pos, " ", tok );
    id = atoi( tok.c_str() );

    if ( ( it = serv_->reg_data_.find( id ) ) == serv_->reg_data_.end() )
    {
        sprintf( buf, MSG_SHOW_NOT_FOUND );
        write_line( buf );
    }
    else
    {
        sprintf( buf, MSG_SHOW_SUCCESS, id );
        write_line( buf );
        // write entry
        sprintf( buf, STR_ENTRYDATA, it->first,
                 it->second->app.c_str(),
                 it->second->site.c_str(),
                 it->second->mach.c_str(),
                 it->second->node.c_str(),
                 it->second->port,
                 it->second->pid,
                 it->second->comp.c_str(),
                 it->second->tag.c_str() );
      write_line( buf );
  }
}


void RegServClient::on_delete( string&           line,
                               string::size_type pos )
{
    int                              id = 0;
    string                           tok;
    char                             buf[ 400 ];
    map< int , RegEntry* >::iterator it;

    get_token( line, pos, " ", tok );
    id = atoi( tok.c_str() );

    if ( ( it = serv_->reg_data_.find( id ) ) == serv_->reg_data_.end() )
    {
        sprintf( buf, MSG_DELETE_NOT_FOUND );
        write_line( buf );
    }
    else
    {
#ifdef EBUG
        fprintf( stdout, "Deleted entry[%d]: %s", it->first, ( ( RegEntry* )it->second )->c_str() );
        fflush( stdout );
#endif

        delete it->second;
        serv_->reg_data_.erase( it );
        sprintf( buf, MSG_DELETE_SUCCESS, id );
        write_line( buf );
    }
}



void RegServClient::on_add_string( string&           line,
                                   string::size_type pos )
{
    string                           tok;
    char                             buf[ 400 ];
    int                              id, strid;
    map< int , RegEntry* >::iterator it;

    pos = get_token( line, pos, " ", tok );
    id  = atoi( tok.c_str() );

    pos = strskip_ws( line, pos );
    if ( pos == string::npos )
    {
        sprintf( buf, MSG_ADDSTR_BAD_FORMAT );
        write_line( buf );
        return;
    }

    if ( line[pos] == '"' )
    {
        pos = get_token( line, pos, "\"", tok );
    }
    else
    {
        pos = get_token( line, pos, " ", tok );
    }

    if ( id == 0 || tok == "" )
    {
        sprintf( buf, MSG_ADDSTR_BAD_FORMAT );
        write_line( buf );
        return;
    }

    if ( ( it = serv_->reg_data_.find( id ) ) == serv_->reg_data_.end() )
    {
        sprintf( buf, MSG_ADDSTR_NOT_FOUND );
        write_line( buf );
    }
    else
    {
        strid = it->second->store_string( tok );
        sprintf( buf, MSG_ADDSTR_SUCCESS, strid );
        write_line( buf );
    }
}


void RegServClient::on_count_strings( string&           line,
                                      string::size_type pos )
{
    string                           tok;
    char                             buf[ 200 ];
    int                              id, strcount;
    map< int , RegEntry* >::iterator it;

    pos = get_token( line, pos, " ", tok );
    id  = atoi( tok.c_str() );

    if ( id == 0 )
    {
        sprintf( buf, MSG_COUNTSTR_BAD_FORMAT );
        write_line( buf );
        return;
    }

    if ( ( it = serv_->reg_data_.find( id ) ) == serv_->reg_data_.end() )
    {
        sprintf( buf, MSG_COUNTSTR_NOT_FOUND );
        write_line( buf );
    }
    else
    {
        strcount = it->second->count_strings();
        sprintf( buf, MSG_COUNTSTR_SUCCESS, strcount, id );
        write_line( buf );
    }
}


void RegServClient::on_get_string( string&           line,
                                   string::size_type pos )
{
    string                           tok;
    char                             buf[ 200 ];
    int                              id, strid;
    string                           str;
    map< int , RegEntry* >::iterator it;

    pos = get_token( line, pos, " ", tok );
    id  = atoi( tok.c_str() );

    pos   = get_token( line, pos, " ", tok );
    strid = atoi( tok.c_str() );
    if ( id == 0 || strid == 0 )
    {
        sprintf( buf, MSG_GETSTR_BAD_FORMAT );
        write_line( buf );
        return;
    }

    if ( ( it = serv_->reg_data_.find( id ) ) == serv_->reg_data_.end() )
    {
        sprintf( buf, MSG_GETSTR_ENTRY_NOT_FOUND );
        write_line( buf );
        return;
    }

    if ( !it->second->get_string_at( strid, str ) )
    {
        sprintf( buf, MSG_GETSTR_STRING_NOT_FOUND );
        write_line( buf );
        return;
    }

    sprintf( buf, MSG_GETSTR_SUCCESS, strid, id, ( int )strlen( str.c_str() ) );
    write_line( buf );

    sprintf( buf, "%s\n", str.c_str() );
    write_line( buf );
}

void RegServClient::on_delete_string( string&           line,
                                      string::size_type pos )
{
    string                           tok;
    char                             buf[ 200 ];
    int                              id, strid;
    string                           str;
    map< int , RegEntry* >::iterator it;

    pos = get_token( line, pos, " ", tok );
    id  = atoi( tok.c_str() );

    pos   = get_token( line, pos, " ", tok );
    strid = atoi( tok.c_str() );
    if ( id == 0 || strid == 0 )
    {
        sprintf( buf, MSG_DELSTR_BAD_FORMAT );
        write_line( buf );
        return;
    }

    if ( ( it = serv_->reg_data_.find( id ) ) == serv_->reg_data_.end() )
    {
        sprintf( buf, MSG_DELSTR_ENTRY_NOT_FOUND );
        write_line( buf );
        return;
    }

    if ( !it->second->delete_string_at( strid ) )
    {
        sprintf( buf, MSG_DELSTR_STRING_NOT_FOUND );
        write_line( buf );
        return;
    }

    sprintf( buf, MSG_DELSTR_SUCCESS, strid, id );
    write_line( buf );
}


void RegServClient::on_search( string&           line,
                               string::size_type pos )
{
    char                             buf[ 200 ];
    string                           key, value;
    string                           command;
    pair< string, string >           mypair;
    map< int , RegEntry* >::iterator it;
    RegEntry entry;

    entry.pid  = 0;
    entry.port = 0;

    do
    {
        pos   = get_key_value_pair( line, pos, mypair) ;
        key   = mypair.first;
        value = mypair.second;

        if ( key == "" )
            continue;

        if ( key == "app" )
        {
            entry.app = value;
            continue;
        }
        if ( key == "site" )
        {
            entry.site = value;
            continue;
        }
        if ( key == "mach" )
        {
            entry.mach = value;
            continue;
        }
        if ( key == "node" )
        {
            entry.node = value;
            continue;
        }
        if ( key == "port" )
        {
            entry.port = atoi( value.c_str() );
            continue;
        }
        if ( key == "pid" )
        {
            entry.pid = atoi( value.c_str() );
            continue;
        }
        if ( key == "comp" )
        {
            entry.comp = value;
            continue;
        }
        if ( key == "tag" )
        {
          entry.tag = value;
          continue;
        }

        sprintf( buf, MSG_SEARCH_BAD_FORMAT );
        write_line( buf );
        return;
    } while ( pos != string::npos );


    sprintf( buf, MSG_SEARCH_SUCCESS );
    write_line( buf );

    for( it = serv_->reg_data_.begin(); it != serv_->reg_data_.end(); it++ )
    {
        if ( (entry.app  == "" || entry.app  == it->second->app  ) &&
             (entry.site == "" || entry.site == it->second->site ) &&
             (entry.mach == "" || entry.mach == it->second->mach ) &&
             (entry.node == "" || entry.node == it->second->node ) &&
             (entry.port == 0  || entry.port == it->second->port ) &&
             (entry.pid  == 0  || entry.pid  == it->second->pid  ) &&
             (entry.comp == "" || entry.comp == it->second->comp ) &&
             (entry.tag  == "" || entry.tag  == it->second->tag) )
      {
          sprintf( buf, STR_ENTRYDATA, it->first,
                   it->second->app.c_str(),
                   it->second->site.c_str(),
                   it->second->mach.c_str(),
                   it->second->node.c_str(),
                   it->second->port,
                   it->second->pid,
                   it->second->comp.c_str(),
                   it->second->tag.c_str() );
          write_line( buf );
      }
    }

    sprintf( buf, "%s\n", STR_END_OF_MULTILINE );
    write_line( buf );
}


void RegServClient::on_list_strings( string&           line,
                                     string::size_type pos )
{
    string                           tok;
    char                             buf[ 200 ];
    int                              i, id, strcount;
    map< int , RegEntry* >::iterator it;
    string                           str;

    pos = get_token( line, pos, " ", tok );
    id  = atoi( tok.c_str() );

    if ( id == 0 )
    {
        sprintf( buf, MSG_LISTSTR_BAD_FORMAT );
        write_line( buf );
        return;
    }

    if ( ( it = serv_->reg_data_.find( id ) ) == serv_->reg_data_.end() )
    {
        sprintf( buf, MSG_LISTSTR_NOT_FOUND );
        write_line( buf );
        return;
    }
    else
    {
        strcount = it->second->count_strings();
        sprintf( buf, MSG_LISTSTR_SUCCESS, strcount, id );
        write_line( buf );
    }

    for( i = 0; i < strcount; i++ )
    {
        it->second->get_string_at( i + 1, str );
        sprintf( buf, "%s\n", str.c_str() );
        write_line( buf );
    }

    sprintf( buf, "%s\n", STR_END_OF_MULTILINE );
    write_line( buf );
}



void RegServClient::on_clean( string&           line,
                              string::size_type pos )
{
    char buf[ 200 ];
    int  count = 0;
    map< int , RegEntry* >::iterator it;

    for( it = serv_->reg_data_.begin(); it != serv_->reg_data_.end(); it++ )
    {
        if ( it->second )
            delete it->second;

        count++;
        serv_->reg_data_.erase( it );
    }

#ifdef EBUG
    fprintf( stdout, "Registry cleaned successfully!\n" );
    fflush( stdout);
#endif

    sprintf( buf, MSG_CLEAN_SUCCESS, count );
    write_line( buf );
}

void RegServClient::on_test( string&           line,
                             string::size_type pos )
{
    char buf[ 400 ];
    sprintf( buf, "Registry service: starting testing of %d registered monitoring processes. Results will be printed to stdout. Goodbye.",
             ( int ) serv_->reg_data_.size() );
    write_line( buf );
}

void RegServClient::on_change( string&           line,
                               string::size_type pos )
{
    int                    id = 0;
    string                 tok;
    char                   buf[ 200 ];
    string                 key, value;
    pair< string, string > mypair;

    map< int , RegEntry* >::iterator it;
    pos = get_token( line, pos, " ", tok );
    id  = atoi( tok.c_str() );

    if ( ( it = serv_->reg_data_.find( id ) ) == serv_->reg_data_.end() )
    {
        sprintf( buf, MSG_CHANGE_ENTRY_NOT_FOUND );
        write_line( buf );
        return;
    }

    RegEntry entry = *( it->second );


    do
    {
        pos   = strskip_ws( line, pos );
        pos   = get_key_value_pair( line, pos, mypair );
        key   = mypair.first;
        value = mypair.second;


        if ( key == "" )
            continue;

        if ( key == "app" )
        {
            entry.app = value;
            continue;
        }
        if ( key == "site" )
        {
            entry.site = value;
            continue;
        }
        if ( key == "mach" )
        {
            entry.mach = value;
            continue;
        }
        if ( key == "node" )
        {
            entry.node = value;
            continue;
        }
        if ( key == "port" )
        {
            entry.port = atoi( value.c_str() );
            continue;
        }
        if ( key == "pid" )
        {
            entry.pid = atoi( value.c_str() );
            continue;
        }
        if ( key == "comp" )
        {
            entry.comp = value;
            continue;
        }
        if ( key == "tag" )
        {
            entry.tag = value;
            continue;
        }

        sprintf( buf, MSG_CHANGE_BAD_FORMAT );
        write_line( buf );
        return;
    } while ( pos != string::npos );

    if ( entry.app  == "" || entry.site == "" ||
         entry.mach == "" || entry.node == "" ||
         entry.port < 0   || entry.pid  == 0  ||
         entry.comp == "" || entry.tag  == "" )
    {
          sprintf( buf, MSG_CHANGE_INCOMPLETE );
          write_line( buf );
          return;
    }

    ( *it->second ) = entry;

    sprintf( buf, MSG_CHANGE_SUCCESS, id );
    write_line( buf );

}
