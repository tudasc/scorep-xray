## -*- mode: autoconf -*-

##
## This file is part of the Score-P software ecosystem (http://www.score-p.org)
## The macro below is based code of autoconf-2-69's general.m4.
##
## Copyright (C) 1992-1996, 1998-2012 Free Software Foundation, Inc.
##
## Copyright (c) 2009-2011, 2021,
## Forschungszentrum Juelich GmbH, Germany
##
## This program is free
## software; you can redistribute it and/or modify it under the
## terms of the GNU General Public License as published by the
## Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## Under Section 7 of GPL version 3, you are granted additional
## permissions described in the Autoconf Configure Script Exception,
## version 3.0, as published by the Free Software Foundation.
##
## You should have received a copy of the GNU General Public License
## and a copy of the Autoconf Configure Script Exception along with
## this program; see the files COPYINGv3 and COPYING.EXCEPTION
## respectively.  If not, see <http://www.gnu.org/licenses/>.


# AFS_CONSISTENT_DIR( DIRECTORY_VAR, [CONFIGURE_OPTION] )
# -----------------------------------------------------
# Check DIRECTORY_VAR for consistency, i.e. remove trailing slashes
# and abort if argument is not an absolute directory name. Optionally
# use CONFIGURE_OPTION to report which option caused an error.
AC_DEFUN([AFS_CONSISTENT_DIR], [
for ac_var in $1
do
  eval ac_val=\$$ac_var
  # Remove trailing slashes.
  case $ac_val in
    */ )
      ac_val=`expr "X$ac_val" : 'X\(.*[[^/]]\)' \| "X$ac_val" : 'X\(.*\)'`
      eval $ac_var=\$ac_val;;
  esac
  # Be sure to have absolute directory names.
  case $ac_val in
    [[\\/$]]* | ?:[[\\/]]* )  continue;;
    NONE | '' ) case $ac_var in *prefix ) continue;; esac;;
  esac
  m4_ifnblank([$2],
    [AC_MSG_ERROR([Expected an absolute directory name for $2 but got $ac_val.])],
    [AC_MSG_ERROR([Expected an absolute directory name for $ac_var but got $ac_val.])])
done
])# AFS_CONSISTENT_DIR
