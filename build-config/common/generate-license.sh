#!/bin/sh
##
## NAME
##     generate-license.sh <format> <specfile>
##
## DESCRIPTION
##     Generates a BSD-style license agreement in the requested format. The
##     details (project name, copyright holders & years, etc.) are taken from
##     the provided specification file. This file has to provide definitions
##     of the following shell variables:
##
##         PROJECT_NAME
##             The project name as used in normal text.
##         PROJECT_HEADER
##             The project name as used in the license agreement header
##             (typically all uppercase).
##         COPYRIGHT_HOLDERS
##             The full list of copyright years and holders, separated by
##             newline. Everything up to the first space is interpreted as
##             year (or year range), the remainder of the line is taken as
##             the corresponding copyright holder. For the list of
##             institutions in clause 3 of the license, everything up to
##             the first comma of the copyright holder entry is used.
##         DOX_HEADER
##             Copyright header to put at the top of the generated license
##             file in Doxygen format.
##
##     The generate-license script is intended to be called during bootstrap,
##     generating the top-level COPYING file as well as license text files
##     for doxygen documentation.
##
##     NOTES:
##       - IT IS ESSENTIAL TO GENERATE THE COPYING FILE BEFORE CALLING
##         AUTORECONF!!! Otherwise, it will not be included in the
##         distribution tarball.
##       - The doxygen version requires the 'tabularx' package when LaTeX
##         output is requested.
##
## ARGUMENTS
##     format      Requested output format. Can be 'text' or 'doxygen'.
##     specfile    Project-specific specification file
##


#--- Helper functions --------------------------------------------------------

# Print usage information to stderr and exit
usage() {
    echo "Usage: generate-license.sh <format> <specfile>" >&2
    echo >&2
    echo "Valid output formats:" >&2
    echo "  text     Plain text" >&2
    echo "  doxygen  Doxygen markup" >&2
    echo >&2
    exit 1
}


#--- Preparation -------------------------------------------------------------

# Validate number of arguments
if test $# -ne 2; then
    usage
fi

# Validate output format
FORMAT="$1"
case ${FORMAT} in
    text|doxygen)
        ;;

    *)
        usage
        ;;
esac

# Load specfile
SPECFILE="$2"
if test ! -r "${SPECFILE}"; then
    echo "Specification file '${SPECFILE}' not found!" >&2
    echo >&2
    usage
fi
. "${SPECFILE}"

# Check whether all required variables are defined by the specfile
if test -z "${PROJECT_NAME}"; then
    echo "Project name undefined!" >&2
    exit 1
fi
if test -z "${PROJECT_HEADER}"; then
    echo "Project name for license agreement header undefined!" >&2
    exit 1
fi
if test -z "${COPYRIGHT_HOLDERS}"; then
    echo "Copyright holders undefined!" >&2
    exit 1
fi
if test -z "${DOX_HEADER}"; then
    echo "Doxygen file copyright header undefined!" >&2
    exit 1
fi


#--- Main program ------------------------------------------------------------

# Generate license text
if test "${FORMAT}" = "doxygen"; then
    cat <<[[EOT]]
${DOX_HEADER}


/**
@page license ${PROJECT_NAME} License Agreement

[[EOT]]
fi

cat <<[[EOT]]
The entire code of ${PROJECT_NAME} is licensed under the BSD-style license
agreement given below, except for the third-party code distributed in the
'vendor/' subdirectory.  Please see the corresponding COPYING files in the
subdirectories of 'vendor/' included in the distribution tarball for details.

[[EOT]]

# Generate license text
if test "${FORMAT}" = "text"; then
    cat <<[[EOT]]

-----------------------------------------------------------------------------


[[EOT]]
    header="${PROJECT_HEADER} LICENSE AGREEMENT"
    indent=`expr \( 76 - ${#header} \) / 2`
    printf "%${indent}s${header}\n" ""
    header=`echo "${header}" | sed -e 's/./=/g'`
    printf "%${indent}s${header}" ""
    echo
    echo
elif test "${FORMAT}" = "doxygen"; then
    cat <<[[EOT]]
@htmlonly
    <hr/>
    <p/>
    <table border=0 cellspacing=0 cellpadding=0>
@endhtmlonly
@latexonly
    \\hrulefill

    \\begin{sffamily}
    \\begin{center}
        \\bfseries
        \\Large
        ${PROJECT_NAME} License Agreement
    \\end{center}
    \\begin{tabularx}{\\linewidth}{@{}lX}
@endlatexonly
[[EOT]]
fi
echo "${COPYRIGHT_HOLDERS}" | while read cp_holder; do
    years=`echo "${cp_holder}" | sed -e 's/ .*$//'`
    holder=`echo "${cp_holder}" | sed -e 's/^[^ ]* //' | fold -w 53`
    if test "${FORMAT}" = "text"; then
        printf "Copyright (c) %-9s" "${years}"
        prefix=""
        echo "${holder}" | while read line; do
            printf "${prefix} %s\n" "${line}"
            prefix="                       "
        done
    elif test "${FORMAT}" = "doxygen"; then
        years=`echo "${years}" | sed -e 's/-/\&ndash;/'`
        holder=`echo "${holder}" | sed -e 's/ae/\&auml;/' -e 's/ue/\&uuml;/' -e 's/oe/\&ouml;/'`
        cat <<[[EOT]]
@htmlonly <tr><td> @endhtmlonly
    <b>Copyright &copy;&nbsp;${years}&nbsp;&nbsp;</b>
@htmlonly </td><td> @endhtmlonly @latexonly & @endlatexonly
    ${holder}
@htmlonly </td></tr> @endhtmlonly @latexonly \\\\ @endlatexonly
[[EOT]]
    fi
done
if test "${FORMAT}" = "text"; then
    echo
    echo "All rights reserved."
elif test "${FORMAT}" = "doxygen"; then
    cat <<[[EOT]]
@htmlonly
    </table>
@endhtmlonly
@latexonly
    \\end{tabularx}
@endlatexonly

<b>All rights reserved.</b>

[[EOT]]
fi


cat <<[[EOT]]


Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

[[EOT]]
if test "${FORMAT}" = "text"; then
    printf "* "
elif test "${FORMAT}" = "doxygen"; then
    echo "<ul><li>"
fi
cat <<[[EOT]]
Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.
[[EOT]]
if test "${FORMAT}" = "text"; then
    echo
    printf "* "
elif test "${FORMAT}" = "doxygen"; then
    echo "</li><li>"
fi
cat <<[[EOT]]
Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.
[[EOT]]
if test "${FORMAT}" = "text"; then
    echo
    printf "* "
elif test "${FORMAT}" = "doxygen"; then
    echo "</li><li>"
fi
echo "Neither the names of"
if test "${FORMAT}" = "doxygen"; then
    echo "<ul>"
fi
echo "${COPYRIGHT_HOLDERS}" | while read cp_holder; do
    holder=`echo "${cp_holder}" | sed -e 's/^[^ ]* //'  -e 's/,.*$/,/'`
    if test "${FORMAT}" = "doxygen"; then
        holder=`echo "${holder}" | sed -e 's/ae/\&auml;/' -e 's/ue/\&uuml;/' -e 's/oe/\&ouml;/'`
        echo "<li>"
    fi
    echo "    the ${holder}"
    if test "${FORMAT}" = "doxygen"; then
        echo "</li>"
    fi
done
if test "${FORMAT}" = "doxygen"; then
    echo "</ul>"
fi
cat <<[[EOT]]
  nor the names of their contributors may be used to endorse or promote
  products derived from this software without specific prior written
  permission.
[[EOT]]
if test "${FORMAT}" = "text"; then
    echo
elif test "${FORMAT}" = "doxygen"; then
    cat << [[EOT]]
</li></ul>
@latexonly
    \\begin{sloppypar}
@endlatexonly
[[EOT]]
fi
cat <<[[EOT]]
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
[[EOT]]

if test "${FORMAT}" = "doxygen"; then
    cat << [[EOT]]
@latexonly
    \\end{sloppypar}
    \\end{sffamily}
@endlatexonly
*/
[[EOT]]
fi

exit 0
