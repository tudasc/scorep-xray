#ifndef SILC_APPLICATION_H_
#define SILC_APPLICATION_H_

/*! @brief Class Silc_Application
 *         base class for SILC user interaction tools
 *
 */


class Silc_Application {
public:

    Silc_Application();
    virtual ~Silc_Application()
    {
        ;
    };

    virtual bool
    readConfigFile
    (
        std::string fileName
    );


private:
};

#endif /*SILC_APPLICATION_H_*/
