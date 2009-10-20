/*! @brief Class Silc_ApplicationType.
 *         Selector class of the user instrumentation stage
 *
 *  This class provides an mechanism to switch between the different stages of SILC
 */

#ifndef SILC_APPLICATIONTYPE_H_
#define SILC_APPLICATIONTYPE_H_

#include <iostream>
#include <string>

#include "Silc_Application.h"


class Silc_ApplicationType
{
public:

    virtual ~Silc_ApplicationType()
    {
        ;
    }

    static Silc_ApplicationType&
    getInstance
    (
    );

/** @brief
 *  provide the right stage of silc
 */
    Silc_Application*
    getSilcStage
    (
        std::string type
    );


private:
/** @brief
 *  protect the constructor to ensure only one instance at a time
 */
    Silc_ApplicationType();

/** @brief
 *  do not allow the copy constructor to be called unwillingly
 */
    Silc_ApplicationType
    (
        const Silc_ApplicationType &type
    );

/** @brief
 *  returns the pointer to a new SILC application (intrumenter, measurement, (vizualizer)? )
 */
    static Silc_ApplicationType* _appType;
};


#endif /*SILC_APPLICATIONTYPE_H_*/
