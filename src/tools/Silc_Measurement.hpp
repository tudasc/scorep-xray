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
    virtual bool
    silc_readConfigFile
    (
        std::string fileName
    );

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
