#ifndef CONFIG_H
#define CONFIG_H

/**
 * @file       config.h
 * @author     Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 * @status     ALPHA
 *
 * @brief      Provide a single config.h that hides the frontend/backend
 *             issues from the developer
 *
 */

#if defined CROSS_BUILD
    #if defined FRONTEND_BUILD
        #include <config-frontend.h>
    #elif defined BACKEND_BUILD
        #include <config-backend.h>
    #else
        #error "You can not use config.h without defining either FRONTEND_BUILD or BACKEND_BUILD."
    #endif
#elif defined NOCROSS_BUILD
    #include <config-backend.h>
#else
    #error "You can not use config.h without defining either CROSS_BUILD or NOCROSS_BUILD."
#endif

#endif /* CONFIG_H */
