#ifndef SCOREP_PLUGIN_INTERFACE_H
#define SCOREP_PLUGIN_INTERFACE_H

/* This file should be included before config.h, otherwise gcc-plugin.h
 * will mess up some defines */

/* Unfortunately the GCC 5 headers are not warning safe with C++11.
 * See https://bugzilla.redhat.com/show_bug.cgi?id=1227828 */
#pragma GCC system_header
#include <gcc-plugin.h>
#undef PACKAGE_BUGREPORT
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_VERSION
#undef PACKAGE_TARNAME

#endif /* SCOREP_PLUGIN_INTERFACE_H */
