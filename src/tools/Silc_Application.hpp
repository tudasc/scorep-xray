#ifndef SILC_APPLICATION_H_
#define SILC_APPLICATION_H_

/*! @brief Class Silc_Application
 *         base class for SILC user interaction tools
 *
 */

#include <iostream>
#include <string>

#include <SILC_Utils.h>


class Silc_Application {
public:

    Silc_Application
        ();
    virtual ~
    Silc_Application
        ();


    virtual SILC_Error_Code
    silc_readConfigFile
    (
        std::string fileName
    )
    {
    };

/** @brief
 * get command line attributes
 */
    virtual SILC_Error_Code
    silc_parseCmdLine
    (
        int    argc,
        char** argv
    )
    {
    };


/** @brief
 * perform instrumentation stage
 */
    virtual int
    silc_run
    (
    )
    {
    };


    virtual void
    silc_printParameter
    (
    )
    {
    };


private:
};

#endif /*SILC_APPLICATION_H_*/
