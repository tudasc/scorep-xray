#! /bin/sh

# default repositories
repos="silc otf2 opari2 utility"

# take repos from the command line, if any
if [ $# -gt 0 ]; then
    repos="$*"
fi

# use absolute path to svnlook
SVNLOOK=/usr/bin/svnlook

for repo in $repos
do
    if [ ! -d /svn-base/$repo-root/hooks ]; then
        echo >&2 "No repository for $repo."
        continue
    fi


    if $SVNLOOK cat /svn-base/common-root trunk/beautifier/pre-commit > \
            /svn-base/$repo-root/hooks/pre-commit && \
       chmod 0755 /svn-base/$repo-root/hooks/pre-commit; then
        echo "pre-commit hook installed for repository $repo."
    else
        echo >&2 "Installation failed for repository $repo."
    fi
done
