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
            if [ -x "$external"/vendor/externals.sh ]; then
                "$external"/vendor/externals.sh get
            fi
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
            echo "cd $bindir/$external && git pull"
            eval '( cd "$external" && git '"${EXTERNALS_GIT_OPTIONS-}"' pull --quiet )'
            if [ -x "$external"/vendor/externals.sh ]; then
                "$external"/vendor/externals.sh update
            fi
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

${cmd} git otf2 -b v2.3 https://gitlab.jsc.fz-juelich.de/perftools/otf2.git
${cmd} git opari2 -b v2.0.6 https://gitlab.jsc.fz-juelich.de/perftools/opari2.git
${cmd} git cubew -b v4.6 https://gitlab.jsc.fz-juelich.de/perftools/cubew.git
${cmd} git cubelib -b v4.6 https://gitlab.jsc.fz-juelich.de/perftools/cubelib.git
