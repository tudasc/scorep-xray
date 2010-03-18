#! /bin/sh

# default repositories
repos="silc otf2 opari2 utility"
hooks="pre-commit post-commit"

# take repos from the command line, if any
if [ $# -gt 0 ]; then
    repos="$*"
fi

# use absolute path to svnlook
SVNLOOK=/usr/bin/svnlook

# path to the common repo
# TODO make this an option to this script and replace places in the
# hooks with this
COMMON_REPOS=/svn-base/common-root

cat_common()
{
    $SVNLOOK cat $COMMON_REPOS "$@"
}

HOOK="$(mktemp)"
trap "rm -f \"$HOOK\"" 0

for hook in $hooks
do

    cat_common trunk/hooks/$hook > "$HOOK" &&
        chmod 0755 "$HOOK" &&
        chown apache:svnsilc "$HOOK"
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

        cp --archive --backup=numbered "$HOOK" /svn-base/$repo-root/hooks/$hook
        if [ $? -eq 0 ]; then
            echo "$hook hook installed for repository $repo."
        else
            echo >&2 "Installation failed for repository $repo."
        fi
    done

done
