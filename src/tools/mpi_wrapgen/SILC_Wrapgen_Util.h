/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
**  KOJAK       http://www.fz-juelich.de/jsc/kojak/                        **
*****************************************************************************
**  Copyright (c) 1998-2009                                                **
**  Forschungszentrum Juelich, Juelich Supercomputing Centre               **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/
#ifndef SILC_WRAPGEN_UTIL_H_
#define SILC_WRAPGEN_UTIL_H_

#include <vector>
using std::vector;
#include <string>
using std::string;
#include <fstream>
using std::ifstream;
#include <cctype>
using std::toupper;
using std::tolower;
#include <sstream>
using std::ostringstream;
using std::istringstream;

namespace SILC {
namespace Wrapgen {
/**
 * Ignore the rest of the line in the input file stream, and move to the
 * next line.
 * @param istr Input file stream
 */
void
ignoreRestOfLine
(
    ifstream& istr
);

/**
 * Convert an integer to a string.
 * @param i Integer to be converted
 * @return String variable representing the decimal notation of the
 *         input
 */
string
int2string
(
    int i
);

/**
 * Convert a string to an integer.
 * @param s String to be converted
 * @result Integer value of input string or -1 if input string could not
 * be converted.
 */
int
string2int
(
    const string& s
);

/**
 * Convert all characters to uppercase.
 * @param s String variable that is to be converted to uppercase.
 */
void
toupper
(
    string& s
);

/**
 * Convert all characters to lowercase.
 * @param s String variable that is to be converted to lowercase.
 */
void
tolower
(
    string& s
);

/**
 * Convert first character to uppercase
 */
string
start_caps
(
    const string& s
);

/**
 * Trim string of leading and trailing spaces
 */
string
trim
(
    const string& s
);

/**
 * Tokenize a string according to a list of delimiting characters
 * @param str        Input string to be tokenized
 * @param delimiters Delimiting characters
 * @param tokens     List of tokens found
 */
void
tokenize
(
    const string&   str,
    const string&   delimiters,
    vector<string>& tokens
);

string
padding
(
    const string& str,
    char          pad_char,
    int           width,
    char          dir
);
}   // end of namespace 'Wrapgen'
}   // end of namespace 'SILC'

#endif
