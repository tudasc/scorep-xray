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
    if [ -d /svn-root/$repo-root/hooks ]; then
        echo >&2 "No repository for $repo."
        continue
    fi

    $SVNLOOK cat /svn-root/common-root trunk/beautify/pre-commit > \
        /svn-root/$repo-root/hooks/pre-commit && \
        chmod 0755 /svn-root/$repo-root/hooks/pre-commit && \
        echo "pre-commit hook installed for repository $repo." || \
        echo >&2 "Installation failed for repository $repo."
done
