# Contribution

First off, thank you for taking the time to prepare a contribution to Score-P!

## Contribution Flow for gitlab.jsc.fz-juelich.de

1. Create an issue
   1. Assign yourself to the issue

1. Work on your contribution
   1. Best practice on creating a commit series

      https://understandlegacycode.com/blog/key-points-of-refactoring/

   1. Best practice on writing good commit messages

      https://cbea.ms/git-commit/

1. Create a branch referencing the issue in the branch name `<#issue>-<branch-name>`
   1. Assign yourself to the merge request
   1. Check _Conformity_ section in the merge request description, assignee is
      responsible to keep these up-to-date
   1. Check the pipeline result in the comments from @swat_ci_connector, it reads
      something like
      > cafebabe triggered CI/CD pipeline #12345 - PASS
   1. After approval, ensure branch is rebased onto target branch and signal ready
      state by adding the `~ready` label
