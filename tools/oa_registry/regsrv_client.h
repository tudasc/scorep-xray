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
 * @file regsrv_client.h
 *
 * Registry service client class
 */

#ifndef REGSRV_CLIENT_H_INCLUDED
#define REGSRV_CLIENT_H_INCLUDED

#include "regsrv.h"

#define MAX_MESSAGE_SIZE 2000

class RegServClient
{
private:
    RegServ* serv_;
    int      sock_;

protected:


public:
    RegServClient( RegServ* s )
    {
        serv_ = s;
        sock_ = s->get_connection_socket();
    }

    int
    receive_and_handle_request();


    std::string
    get_hostname()
    {
        return serv_->get_hostname();
    }

    int
    get_port()
    {
        return serv_->get_port();
    }

    int
    write_line( char* buf );
    int
    read_line( char* buf,
               int   maxlen );


    //
    // basic commands
    //

    void
    on_quit( std::string&           line,
             std::string::size_type pos );
    void
    on_help( std::string&           line,
             std::string::size_type pos );
    void
    on_unknown( std::string&           line,
                std::string::size_type pos );

    void
    on_create( std::string&           line,
               std::string::size_type pos );
    void
    on_search( std::string&           line,
               std::string::size_type pos );
    void
    on_delete( std::string&           line,
               std::string::size_type pos );
    void
    on_list( std::string&           line,
             std::string::size_type pos );
    void
    on_show( std::string&           line,
             std::string::size_type pos );

    void
    on_clean( std::string&           line,
              std::string::size_type pos );
    void
    on_change( std::string&           line,
               std::string::size_type pos );

    //
    // string storage and retrieval
    //
    void
    on_add_string( std::string&           line,
                   std::string::size_type pos );
    void
    on_get_string( std::string&           line,
                   std::string::size_type pos );
    void
    on_delete_string( std::string&           line,
                      std::string::size_type pos );
    void
    on_list_strings( std::string&           line,
                     std::string::size_type pos );
    void
    on_count_strings( std::string&           line,
                      std::string::size_type pos );
    void
    on_test( std::string&           line,
             std::string::size_type pos );
};



#endif // REGSRV_CLIENT_H_INCLUDED
