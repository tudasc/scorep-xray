#ifndef SILC_MEASUREMENT_H_
#define SILC_MEASUREMENT_H_

/*! @brief Class Silc_Measurement
 *         runs the instrumented user program and steers the measurements system
 *
 *  This class examines system and user settings to run the measurement system
 */

class Silc_Measurement : public Silc_Application {
public:

    Silc_Measurement();
    ~Silc_Measurement()
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

private:
};
#endif /*SILC_MEASUREMENT_H_*/
