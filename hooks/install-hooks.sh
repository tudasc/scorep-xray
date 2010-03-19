#! /bin/sh

# default repositories
repos="silc otf2 opari2 utility"
hooks="pre-commit post-commit"

# take repos from the command line, if any
if [ $# -gt 0 ]; then
    repos="$*"
fi

# use absolute path
SVN=/usr/bin/svn
SVNLOOK=/usr/bin/svnlook
CP=/bin/cp
RM=/bin/rm
CHOWN=/bin/chown
CHMOD=/bin/chmod
MKTEMP=/bin/mktemp
FIND=/usr/bin/find

# path to the common repo
# TODO make this an option to this script and replace places in the
# hooks with this
COMMON_REPOS=/svn-base/common-root

cat_common()
{
    $SVNLOOK cat $COMMON_REPOS "$@"
}

HOOK="$($MKTEMP)"
trap "rm -f \"$HOOK\"" 0

for hook in $hooks
do
    cat_common trunk/hooks/$hook > "$HOOK" &&
        $CHMOD 0755 "$HOOK" &&
        $CHOWN apache:svnsilc "$HOOK"
    if [ $? -ne 0 ]; then
        echo "Can't generate $hook hook." >&2
        exit 1
    fi

    for repo in $repos
    do
        if [ ! -d /svn-base/$repo-root/hooks ]; then
            echo >&2 "No repository for $repo."
            continue
        fi

        # install multiplexer hook
        $CP --archive --backup=numbered "$HOOK" /svn-base/$repo-root/hooks/$hook
        if [ $? -eq 0 ]; then
            echo "$hook hook installed for repository $repo."
        else
            echo >&2 "Installation failed for repository $repo."
        fi

        # checkout/update .d directory
        if [ -d /svn-base/$repo-root/hooks/$hook.d/.svn ]; then
            (
                cd /svn-base/$repo-root/hooks/$hook.d
                $SVN up
            )
        else
            rm -rf /svn-base/$repo-root/hooks/$hook.d
            (
                cd /svn-base/$repo-root/hooks
                $SVN co file://$COMMON_REPOS/hooks/$hook.d
            )
        fi

        # fix owner of hooks.d
        # don't fix permission, hooks are only active if they have the
        # executable bit set in the repository
        $CHOWN apache:svnsilc /svn-base/$repo-root/hooks/$hook.d
        $CHOWN apache:svnsilc /svn-base/$repo-root/hooks/$hook.d/[0-9][0-9]*
    done
done
