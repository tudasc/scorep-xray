/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
**  KOJAK       http://www.fz-juelich.de/jsc/kojak/                        **
*****************************************************************************
**  Copyright (c) 1998-2009                                                **
**  Forschungszentrum Juelich, Juelich Supercomputing Centre               **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/
/**
 * @file SILC_Wrapgen_Func.h
 *
 * Class representing function prototypes
 */
#ifndef SILC_FUNC_H
#define SILC_FUNC_H

#include <string>
using std::          string;
#include <vector>
using std::          vector;
#include "SILC_Wrapgen_Funcparam.h"
using SILC::Wrapgen::Funcparam;

namespace SILC {
namespace Wrapgen {
/** List of function parameters */
typedef vector<Funcparam> paramlist_t;

/**
 * General function class
 */
class Func {
public:
    Func( const string &rtype, const string &name, const string &group,
          const string &guard );
    Func( const string &rtype, const string &name, const string &group,
          const string &guard, const paramlist_t &params );
    virtual ~Func();

    /** return unique identifier of function
     * @return unique identifier
     */
    string
    get_id
        ()     const
    {
        return m_id;
    };

    /** return return type of function
     * @return string name of return type
     */
    string
    get_rtype
        ()  const
    {
        return m_rtype;
    };

    /** return name of function
     * @return name of function
     */
    string
    get_name
        ()   const
    {
        return m_name;
    };

    /** return family of prototype
     * @return family of prototype
     */
    string
    get_family
        () const
    {
        return m_family;
    };

    /** return group of prototype
     * @return group of prototype
     */
    string
    get_group
        ()  const
    {
        return m_group;
    };

    /** get optional guarding define for this wrapper
     * @return guard name
     */
    string
    get_guard
        () const
    {
        return m_guard;
    };

    /** get special initialization block for wrapper
     * @return string of initialization block
     */
    string
    get_init_block
        () const
    {
        return m_init_block;
    }

    /** set special initialization block for wrapper
     * @param init string of initialization block
     */
    void
    set_init_block
    (
        const string& decl
    )
    {
        m_init_block = decl;
    }

    /** add statements to special initialization block
     * @param init string of initialization statement
     */
    void
    add_init_block
    (
        const string& decl
    )
    {
        m_init_block += decl;
    }

    /** get special declaration block for wrapper
     * @return string of declaration block
     */
    string
    get_decl_block
        () const
    {
        return m_decl_block;
    }

    /** set special declaration block for wrapper
     * @param decl string of declaration block
     */
    void
    set_decl_block
    (
        const string& decl
    )
    {
        m_decl_block = decl;
    }

    /** add statements to special declaration block
     * @param decl string of additional statement in declaration block
     */
    void
    add_decl_block
    (
        const string& decl
    )
    {
        m_decl_block += decl;
    }

    /** get special expression block for wrapper
     * @return string of expression block
     */
    string
    get_expr_block
        () const
    {
        return m_expr_block;
    }

    /** set special expression block for wrapper
     * @param string of expression block
     */
    void
    set_expr_block
    (
        const string& expr
    )
    {
        m_expr_block = expr;
    }

    /** get special cleanup block for wrapper
     * @return string of cleanup block
     */
    string
    get_cleanup_block
        () const
    {
        return m_cleanup_block;
    }

    /** set special cleanup block for wrapper
     * @param statement string of cleanup block
     */
    void
    set_cleanup_block
    (
        const string& statement
    )
    {
        m_cleanup_block = statement;
    }

    /** add statements to special cleanup block
     * @param statement string of cleanup statement
     */
    void
    add_cleanup_block
    (
        const string& statement
    )
    {
        m_cleanup_block += statement;
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
     * @param type parameter type
     * @param name parameter name
     * @param suffix parameter suffix, like [] for arrays, etc.
     */
    void
    add_param
    (
        const string& type,
        const string& name,
        const string& suffix
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
    virtual string
    generate_wrapper
    (
        const string& filename
    ) const;

    /** write function configuration
     * @return string holding the configurations values for the function
     *         call
     */
    virtual string
    write_conf
        () const;

protected:
    /** set the prototype family
     * @param family prototype family
     */
    void
    set_family
    (
        const string& family
    )
    {
        m_family = family;
    }

private:
    /** unique function id */
    string      m_id;
    /** return type of function */
    string      m_rtype;
    /** name of function */
    string      m_name;
    /** functional group */
    string      m_group;
    /** guard name that is used to decide whether this wrapper is built */
    string      m_guard;
    /** family of prototype */
    string      m_family;
    /** special declaration block */
    string      m_decl_block;
    /** special initialization block */
    string      m_init_block;
    /** special expression block */
    string      m_expr_block;
    /** special cleanup block */
    string      m_cleanup_block;
    /** vector of function parameters */
    paramlist_t m_params;

    /** number of created function objects */
    static int num_func;
};
}
}

#endif
