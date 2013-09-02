/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen University, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 * @file regsrv.h
 *
 * Registry service client and registry entry classes
 */

#ifndef REGSRV_H_INCLUDED
#define REGSRV_H_INCLUDED

#include <string>
#include <vector>
#include <list>
#include <map>
#include <UTILS_CStr.h>
#include <string.h>

#define REGSRV_MODE_LISTEN              0
#define REGSRV_MODE_QUIT                1
#define REGSRV_MODE_TEST                2

class RegEntry
{
private:
    std::vector< std::string > strings_;

public:
    RegEntry()
    {
    }

    std::string app;
    std::string site;
    std::string mach;
    std::string node;
    int         port;
    int         pid;
    std::string comp;
    std::string tag;
    int         test_comm_sock;

    const char*
    c_str()
    {
        char buf[ 512 ];
        sprintf( buf, "app=\"%s\" "
                 "site=\"%s\" "
                 "mach=\"%s\" "
                 "node=\"%s\" "
                 "pid=%d "
                 "comp=\"%s\" "
                 "tag=\"%s\"\n", app.c_str(), site.c_str(), mach.c_str(),
                 node.c_str(),  pid, comp.c_str(), tag.c_str() );
        return UTILS_CStr_dup( buf );
    }

    int
    store_string( std::string& str )
    {
        strings_.push_back( str );
        return strings_.size();
    }

    int
    get_string_at( int          id,
                   std::string& str )
    {
        if ( id < 1 || id > strings_.size() )
        {
            return 0;
        }

        str = strings_[ id - 1 ];
        return 1;
    }

    int
    delete_string_at( int id )
    {
        if ( id < 1 || id > strings_.size() )
        {
            return 0;
        }

        strings_.erase( strings_.begin() + ( id - 1 ) );
        return 1;
    }

    int
    count_strings()
    {
        return strings_.size();
    }
};



class RegServ
{
public:
    std::map< int, RegEntry* > reg_data_;

private:
    std::string hostname_;
    int         port_;
    int         next_id_;
    int         server_socket_;
    int         connection_socket_;

public:
    RegServ()
    {
        next_id_           = 1;
        server_socket_     = -1;
        connection_socket_ = -1;
    }

    ~
    RegServ()
    {
    }

    int
    open( int port );
    int
    blocking_accept();
    void
    close_connection();
    void
    close_server();
    int
    execute_test( const char* scenario_file );

    int
    get_connection_socket()
    {
        return connection_socket_;
    }

    RegEntry*
    get_entry( int id )
    {
        return reg_data_[ id ];
    }

    void
    set_entry( int       id,
               RegEntry* entry )
    {
        reg_data_[ id ] = entry;
    }


    int
    unique_id()
    {
        return next_id_++;
    }

    int
    get_port()
    {
        return port_;
    }

    std::string
    get_hostname()
    {
        return hostname_;
    }
};


#endif // REGSRV_H_INCLUDED
