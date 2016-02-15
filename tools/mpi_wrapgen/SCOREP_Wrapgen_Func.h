/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011,
 *    Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 *    University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011,
 *    Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 *    Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2011                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2010-2011                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#ifndef SCOREP_FUNC_H
#define SCOREP_FUNC_H

/**
 * @file       SCOREP_Wrapgen_Func.h
 * @ingroup    Wrapgen_module
 *
 * @brief Class representing a function prototype.
 */

#include <string>
#include <vector>
#include <map>

#include "SCOREP_Wrapgen_Funcparam.h"
using SCOREP::Wrapgen::Funcparam;

namespace SCOREP
{
namespace Wrapgen
{
/** List of function parameters */
typedef std::vector<Funcparam> paramlist_t;

/**
 * General function class
 */
class Func
{
public:
    /**
     * @brief Constructor for creating an object without yet knowing about the
     *        function call parameters.
     */
    Func( const std::string& rtype,
          const std::string& name,
          const std::string& group,
          const std::string& guard );

    /**
     * @brief Construtor for creating an object with the knowledge of
     *        the function call parameters.
     */
    Func( const std::string& rtype,
          const std::string& name,
          const std::string& group,
          const std::string& guard,
          const paramlist_t& params );

    virtual
    ~
    Func();

    /** return unique identifier of function
     * @return unique identifier
     */
    std::string
    get_id
        ()     const
    {
        return m_id;
    };

    /** return return type of function
     * @return string name of return type
     */
    std::string
    get_rtype
        ()  const
    {
        return m_rtype;
    };

    /** return name of function
     * @return name of function
     */
    std::string
    get_name
        ()   const
    {
        return m_name;
    };

    /** return family of prototype
     * @return family of prototype
     */
    std::string
    get_family
        () const
    {
        return m_family;
    };

    /** return value of attribute
     * @param attribute
     * @return attribute value
     */
    std::string
    get_attribute
        ( const std::string& attribute ) const
    {
        return m_attributes.find( attribute )->second;
    };

    /** set the extended attribute
     * @param attribute extended attribute
     * @param value extended attribute value
     */
    void
    set_attribute
    (
        const std::string& attribute,
        const std::string& value
    )
    {
        m_attributes[ attribute ] = value;
    }

    /** return group of prototype
     * @return group of prototype
     */
    std::string
    get_group
        ()  const
    {
        return m_group;
    };

    /** get optional guarding define for this wrapper
     * @return guard name
     */
    std::string
    get_guard
        () const
    {
        return m_guard;
    };

    /** get special initialization block for wrapper
     * @return string of initialization block
     */
    std::string
    get_init_block() const
    {
        std::map<std::string, std::string>::const_iterator it = m_init_block.find( "" );
        if ( it != m_init_block.end() )
        {
            return it->second;
        }
        else
        {
            return std::string( "" );
        }
    }

    /** get special initialization block for wrapper
     * @param  id     id of initialization block
     * @return string of initialization block
     */
    std::string
    get_init_block( const std::string& id ) const
    {
        std::map<std::string, std::string>::const_iterator it = m_init_block.find( id );
        if ( it != m_init_block.end() )
        {
            return it->second;
        }
        else
        {
            return "";
        }
    }

    /** set special initialization block for wrapper
     * @param init string of initialization block
     */
    void
    set_init_block( const std::string& decl )
    {
        m_init_block[ "" ] = decl;
    }

    /** set special initialization block for wrapper
     * @param id   id of initialization block
     * @param init string of initialization block
     */
    void
    set_init_block( const std::string& id,
                    const std::string& decl )
    {
        m_init_block[ id ] = decl;
    }

    /** add statements to special initialization block
     * @param init string of initialization statement
     */
    void
    add_init_block( const std::string& decl )
    {
        m_init_block[ "" ] += decl;
    }

    /** add statements to special initialization block
     * @param id   id of initialization block
     * @param init string of initialization statement
     */
    void
    add_init_block( const std::string& id,
                    const std::string& decl )
    {
        m_init_block[ id ] += decl;
    }

    /** get special declaration block for wrapper
     * @return string of declaration block
     */
    std::string
    get_decl_block() const
    {
        std::map<std::string, std::string>::const_iterator it = m_decl_block.find( "" );
        if ( it != m_decl_block.end() )
        {
            return it->second;
        }
        else
        {
            return "";
        }
    }

    /** get special declaration block for wrapper
     * @param  id     id of declaration block
     * @return string of declaration block
     */
    std::string
    get_decl_block( const std::string& id ) const
    {
        std::map<std::string, std::string>::const_iterator it = m_decl_block.find( id );
        if ( it != m_decl_block.end() )
        {
            return it->second;
        }
        else
        {
            return "";
        }
    }

    /** set special declaration block for wrapper
     * @param decl string of declaration block
     */
    void
    set_decl_block( const std::string& decl )
    {
        m_decl_block[ "" ] = decl;
    }

    /** set special declaration block for wrapper
     * @param id   id of declaration block
     * @param decl string of declaration block
     */
    void
    set_decl_block( const std::string& id,
                    const std::string& decl )
    {
        m_decl_block[ id ] = decl;
    }

    /** add statements to special declaration block
     * @param decl string of additional statement in declaration block
     */
    void
    add_decl_block( const std::string& decl )
    {
        m_decl_block[ "" ] += decl;
    }

    /** add statements to special declaration block
     * @param decl string of additional statement in declaration block
     */
    void
    add_decl_block( const std::string& id,
                    const std::string& decl )
    {
        m_decl_block[ id ] += decl;
    }

    /** get special expression block for wrapper
     * @return string of expression block
     */
    std::string
    get_expr_block() const
    {
        std::map<std::string, std::string>::const_iterator it = m_expr_block.find( "" );
        if ( it != m_expr_block.end() )
        {
            return it->second;
        }
        else
        {
            return "";
        }
    }

    /** get special expression block for wrapper
     * @return string of expression block
     */
    std::string
    get_expr_block( const std::string& id ) const
    {
        std::map<std::string, std::string>::const_iterator it = m_expr_block.find( id );
        if ( it != m_expr_block.end() )
        {
            return it->second;
        }
        else
        {
            return "";
        }
    }

    /** set special expression block for wrapper
     * @param string of expression block
     */
    void
    set_expr_block( const std::string& expr )
    {
        m_expr_block[ "" ] = expr;
    }

    /** set special expression block for wrapper
     * @param id     id of expression block
     * @param string content of expression block
     */
    void
    set_expr_block( const std::string& id,
                    const std::string& expr )
    {
        m_expr_block[ id ] = expr;
    }


    /** get special cleanup block for wrapper
     * @return string of cleanup block
     */
    std::string
    get_cleanup_block() const
    {
        std::map<std::string, std::string>::const_iterator it = m_cleanup_block.find( "" );
        if ( it != m_cleanup_block.end() )
        {
            return it->second;
        }
        else
        {
            return "";
        }
    }

    /** get special cleanup block for wrapper
     * @param  id     id of cleanup block
     * @return string of cleanup block
     */
    std::string
    get_cleanup_block( const std::string& id ) const
    {
        std::map<std::string, std::string>::const_iterator it = m_cleanup_block.find( id );
        if ( it != m_cleanup_block.end() )
        {
            return it->second;
        }
        else
        {
            return "";
        }
    }

    /** set special cleanup block for wrapper
     * @param statement string of cleanup block
     */
    void
    set_cleanup_block( const std::string& statement )
    {
        m_cleanup_block[ "" ] = statement;
    }

    /** set special cleanup block for wrapper
     * @param id        id of cleanup block
     * @param statement string of cleanup block
     */
    void
    set_cleanup_block( const std::string& id,
                       const std::string& statement )
    {
        m_cleanup_block[ id ] = statement;
    }

    /** add statements to special cleanup block
     * @param statement string of cleanup statement
     */
    void
    add_cleanup_block( const std::string& statement )
    {
        m_cleanup_block[ "" ] += statement;
    }

    /** add statements to special cleanup block
     * #param id        id of cleanup block
     * @param statement string of cleanup statement
     */
    void
    add_cleanup_block( const std::string& id,
                       const std::string& statement )
    {
        m_cleanup_block[ id ] += statement;
    }

    /** get number of parameters of this function
     * @return number of parameters
     */
    size_t
    get_param_count
        () const
    {
        return m_params.size();
    }

    /** return list of parameters
     * @return parameter list
     */
    paramlist_t
    get_params
        () const
    {
        return m_params;
    };

    /** set parameter list of prototype
     * @param params list of parameters
     */
    void
    set_params
    (
        const paramlist_t& params
    );

    /** add a parameter to function prototype
     * @param type_modifier modifier of parameter type (e.g. 'const')
     * @param type parameter type
     * @param name parameter name
     * @param suffix parameter suffix, like [] for arrays, etc.
     */
    void
    add_param
    (
        const std::string& type_modifier,
        const std::string& type,
        const std::string& name,
        const std::string& suffix
    );

    /** get the n-th function parameter
     * @return n-th function parameter
     */
    Funcparam
    get_param
    (
        const int n
    ) const
    {
        return m_params[ n ];
    }

    /** gernerate wrapper from template file for function
     * @param filename associated wrapper template
     * @return string object containing the function wrapper
     */
    virtual std::string
    generate_wrapper
    (
        const std::string& filename
    ) const;

    /** write function configuration
     * @return string holding the configurations values for the function
     *         call
     */
    virtual std::string
    write_conf
        () const;

protected:
    /** set the prototype family
     * @param family prototype family
     */
    void
    set_family
    (
        const std::string& family
    )
    {
        m_family = family;
    }

private:
    /** unique function id */
    std::string m_id;
    /** return type of function */
    std::string m_rtype;
    /** name of function */
    std::string m_name;
    /** functional group */
    std::string m_group;
    /** guard name that is used to decide whether this wrapper is built */
    std::string m_guard;
    /** special attributes */
    std::map<std::string, std::string> m_attributes;
    /** family of prototype */
    std::string m_family;
    /** special declaration block */
    std::map<std::string, std::string> m_decl_block;
    /** special initialization block */
    std::map<std::string, std::string> m_init_block;
    /** special expression block */
    std::map<std::string, std::string> m_expr_block;
    /** special cleanup block */
    std::map<std::string, std::string> m_cleanup_block;
    /** vector of function parameters */
    paramlist_t m_params;

    /** number of created function objects */
    static int num_func;
};
}
}

#endif
