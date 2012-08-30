#! /bin/sh

#
# Extracts all config header templates from the backend config.h intended
# for the frontend
#

echo "/* Generated from $1 by $0.  */"
echo
LC_ALL=C grep -B 1 '^#undef HAVE_BACKEND_' "$1" |
    LC_ALL=C sed -e 's/^--//'
