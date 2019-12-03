#!/bin/sh

set -e

bindir=${0%/*}
: ${bindir:=.}
cd "$bindir"

cmd_get()
{
    local vcs=$1
    shift
    local external=$1
    shift
    test ! -d "$external" && {
        case $vcs in
        svn)
            echo "svn checkout $* $bindir/$external"
            eval 'svn '"${EXTERNALS_SVN_OPTIONS-}"' checkout --quiet "$@" "$external"'
        ;;
        git)
            echo "git clone $* $bindir/$external"
            eval 'git '"${EXTERNALS_GIT_OPTIONS-}"' clone --quiet "$@" "$external"'
        ;;
        esac
    }
}

cmd_update()
{
    local vcs=$1
    shift
    local external=$1
    test -d "$external" && {
        case $vcs in
        svn)
            echo "svn update $bindir/$external"
            eval 'svn '"${EXTERNALS_SVN_OPTIONS-}"' update --quiet "$external"'
        ;;
        git)
            echo "git -C $bindir/$external pull"
            eval 'git '"${EXTERNALS_GIT_OPTIONS-}"' -C "$external" pull --quiet'
        ;;
        esac
    }
}

cmd_clean()
{
    local vcs=$1
    shift
    local external=$1
    echo "rm -rf $bindir/$external"
    rm -rf "$external"
}

cmd_help()
{
    printf "Usage: $0 [get|update|clean]\n"
    exit 1
}

cmd=cmd_${1:-get}

${cmd} svn otf2 https://silc.zih.tu-dresden.de/svn/otf2-root/tags/REL-2.2
${cmd} svn opari2 https://silc.zih.tu-dresden.de/svn/opari2-root/tags/REL-2.0.5
${cmd} svn cubew https://svn.version.fz-juelich.de/scalasca_soft/Cube2.0/CubeW/tags/release-4.4.3
${cmd} svn cubelib https://svn.version.fz-juelich.de/scalasca_soft/Cube2.0/CubeLib/tags/release-4.4.4
