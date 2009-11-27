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

#ifndef SILC_MPIFUNC_H_
#define SILC_MPIFUNC_H_

/**
 * @file       SILC_Wrapgen_MpiFunc.h
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     ALPHA
 * @ingroup    Wrapgen_module
 *
 * @brief Class representing MPI function prototypes.
 */

#include <string>
using std::string;
#include <vector>
using std::vector;

#include "SILC_Wrapgen_Funcparam.h"
#include "SILC_Wrapgen_Func.h"
#include "SILC_Wrapgen_Util.h"
using namespace SILC::Wrapgen;

/**
 * Class for representing MPI function prototype
 */
namespace SILC
{
namespace Wrapgen
{
class MPIFunc : public Func
{
public:
    MPIFunc( const string &rtype, const string &name, const string &group,
             const string &guard, const string &version, const paramlist_t &params );

    /** get string representing the send count calculation rule
     * @return string representing the send count calculation rule */
    string
    get_sendcount_rule
        () const
    {
        return m_scnt;
    }
    /** get string representing the receive count calculation rule
     * @return string representing the receive count calculation rule */
    string
    get_recvcount_rule
        () const
    {
        return m_rcnt;
    }
    /** get MPI version this function call was introduced
     * @return MPI major version
     */
    int
    get_version
        () const
    {
        return m_version;
    }
    /** set string representing the send count calculation rule
     * @param rule string representing the send count calculation rule */
    void
    set_sendcount_rule
    (
        const string& rule
    );

    /** set string representing the receive count calculation rule
     * @param rule string representing the receive count calculation rule */
    void
    set_recvcount_rule
    (
        const string& rule
    );

    /** write function configuration
     * @return string holding the configurations values for the function
     *         call
     */
    virtual string
    write_conf
        () const;

private:
    /** expression to evaluate the number of bytes sent */
    string m_scnt;
    /** expression to evaluate the number of bytes received */
    string m_rcnt;
    /** additional expressions within a wrapper */
    string m_expr_block;
    /** initial declarations special to this function */
    string m_init;
    /** major version of MPI where this function was introduced */
    int    m_version;
};
}   // namespace Wrapgen
}   // namespace SILC

#endif
