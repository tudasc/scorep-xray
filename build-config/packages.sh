#!/bin/sh

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2024,
## Technische Universitaet Dresden, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##

set -e

argv0=${0##*/}
srcdir=${0%/*}
: "${srcdir:=.}"

die()
{
    printf >&2 "%s: " "$argv0"
    printf >&2 "$@"
    printf >&2 "\n"
    exit 1
}

silent=--silent
quiet=--quiet
do_download=false
while test $# -gt 0
do
    case ${1} in
    (--help)
        cat <<EOH
Usage: ${argv0}
Usage: ${argv0} --download [DIRECTORY]

Lists URLs to packages which will be download during the build process if the
configure option \`--with-lib<foo>=download\` was provided.

With \`--download\` download all packages and place them into DIRECTORY
(defaults to build-config/packages).
EOH
        exit 0
    ;;
    (--download)
        do_download=true
        shift
    ;;
    (--verbose)
        unset silent quiet
        shift
    ;;
    (--)
        shift
        break
    ;;
    (*)
        die "unknown option '%s'" "$1"
    ;;
    esac
done

packages_directory="${srcdir}/packages/"
if $do_download
then
    if test $# -gt 0
    then
        packages_directory="$1"
        shift

        if ! mkdir -p "${packages_directory}"
        then
            die "cannot create download directory '%s'" "${packages_directory}"
        fi
    fi

    fetch="$(which wget) $quiet --content-disposition --continue" ||
        fetch="$(which curl) $silent -S -O -J -L -C -" ||
            die "neither wget nor curl found, cannot download packages"
fi

if test $# -ne 0
then
    die "unused arguments '%s'" "$*"
fi

test -r "$srcdir/downloads" ||
    die "cannot read '%s'" "$srcdir/downloads"

. "$srcdir/downloads" ||
    die "malformed '%s'" "$srcdir/downloads"

sep=""
for package in $packages
do
    printf "$sep"

    libname="${package#lib}"
    if eval "test x\${${package}_libname:+set} = xset"
    then
        eval libname="\${${package}_libname}"
    fi
    eval url="\${${package}_url}"

    printf "Package:            %s\n" "$package"
    printf "  Configure option: --with-lib%s=download\n" "$libname"
    printf "  URL:\n\n"
    printf "    %s\n" "$url"

    if $do_download
    then
        printf "\n  Downloading...\n"
        ( cd "${packages_directory}" && $fetch "$url" )
    fi

    sep="\n"
done
