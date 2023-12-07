#!/bin/sh

set -e

PACKAGE="$1"
ERRORS="$2"
HEADER="$3"
DECLS="$4"
TEMPLATE="${5-$HEADER}"

cleanup()
{
    rm -rf $HEADER.*
}
trap cleanup EXIT

mkdir -p "$(dirname "$HEADER")"
mkdir -p "$(dirname "$DECLS")"

exec 5>"$HEADER.2"
exec 6>"$DECLS"

last=
error_code()
{
    last="${PACKAGE}_ERROR_$1"
    printf "    /** %s */\n    %s,\n" "$2" "$last" >&5
    printf "    UTILS_ERROR_DECL( %s, \"%s\"),\n" "$1" "$2" >&6
}

case "$ERRORS"
in
    (*/*) : accept ;;
    (*) : make it relative
        ERRORS="./$ERRORS"
    ;;
esac

. "$ERRORS"

exec 5<&-
exec 6<&-

# remove last comma
sed -e "s/^    ${last},/    ${last}/" $HEADER.2 >$HEADER.3

sed \
    -e "s/@AFS_PACKAGE_NAME@/${PACKAGE}/g" \
    -e "
/^    @PACKAGE_SPECIFIC_ERROR_CODES@/ {
    g
    r $HEADER.3
}" "$TEMPLATE" >"$HEADER.1"
mv -f "$HEADER.1" "$HEADER"
