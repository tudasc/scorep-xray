#ifndef SILC_APPLICATION_H_
#define SILC_APPLICATION_H_

/*! @brief Class Silc_Application
 *         base class for SILC user interaction tools
 *
 */

#include <iostream>
#include <string>

class Silc_Application {
public:

    Silc_Application
        ();
    virtual ~
    Silc_Application
        ();


    virtual bool
    silc_readConfigFile
    (
        std::string fileName
    );

/** @brief
 * get command line attributes
 */
    virtual bool
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
    );


private:
};

#endif /*SILC_APPLICATION_H_*/
