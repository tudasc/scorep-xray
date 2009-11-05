#ifndef SILC_MEASUREMENT_H_
#define SILC_MEASUREMENT_H_

/*! @brief Class Silc_Measurement
 *         runs the instrumented user program and steers the measurements system
 *
 *  This class examines system and user settings to run the measurement system
 */

#include <iostream>
#include <string>

#include "Silc_Application.hpp"

class Silc_Measurement : public Silc_Application {
public:

    Silc_Measurement
        ();

    virtual ~
    Silc_Measurement
        ()
    {
        ;
    };

/** @brief
 * reads the defined input data file
 */
    virtual SILC_Error_Code
    silc_readConfigFile
    (
        std::string fileName
    );


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



/** @brief
 *  prints all measurement parameters as read from input file
 */
    virtual void
    silc_printParameter
    (
    );


private:
};
#endif /*SILC_MEASUREMENT_H_*/
