AC_DEFUN([AH_SCOREP_CONFIG_HEADER_CUSTOM],
[
## Please note that text gets included "verbatim" to the template file, not to
## the resulting config header, so it can easily get mangled when the template
## is processed. There is rarely a need for something other than
##
##     AH_BOTTOM([#include <custom.h>])

## Include text at the top of the header template file. 
##AH_TOP([text])

## Include text at the bottom of the header template file. 
AH_BOTTOM([#include <config-custom.h>])
])
