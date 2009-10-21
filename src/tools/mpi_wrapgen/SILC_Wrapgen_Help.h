#ifndef SILC_WRAPGEN_HELP_H_
#define SILC_WRAPGEN_HELP_H_

#include <iostream>
using std::cout;
using std::endl;

namespace SILC {
namespace Wrapgen {
void
help
(
    string name
)
{
    cout << "'" << name << "' generates EPILOG measurement wrappers for MPI function\n" << endl;

    cout << "USAGE :\nThere are 2 possibilities to use " << name << ":\n" << endl;
    cout << "(1) " << name << " <file1.txt> specfile <-r...> <file2.txt>" << endl;
    cout << " a specfile contains the functions combined with the used wrapper\n" << endl;
    cout << "(2) " << name << " <file1.txt> wrapper.w <-r...> <file2.txt>" << endl;
    cout << " you can specify a wrapper which is used for all functions instead of a specfile\n" << endl;
    cout << "-expressions in < > are optional" << endl;
    cout << "-textfiles can be included" << endl;
    cout << "-the b options disables the banner output before generating new code" << endl;
    cout << "-the r (restrict option) can specify the output depending on the specifier" << endl;
    cout << "  -g specifies the group : '" << name << " -rgp2p' (for Peer to Peer)" << endl;
    cout << "  -i specifies the ID of a function: '" << name << " -ri92' (Function with ID 92)" << endl;
    cout << "  -n specifies a string in the functionname: '" << name << " -rncast'" << endl;
    cout << "     (all function including the string 'cast')" << endl;
    cout << "  -t specifies the returntype of the MPI function: '" << name << " -rtint'" << endl;
    cout << "  -v specifies the version: '" << name << " -rv1' (for MPI Version 1)" << endl;
    cout << " A combination of -r options is possible using a '+' between the options\n" << endl;
    cout << " Example: " << name << " header.txt name.w -rntype+v1+gp2p footer.txt" << endl << endl;
    cout << " Please use a - for each option or add the r option at last position!" << endl;
}
}   // namespace Wrapgen
}   // namespace SILC


#endif /* SILC_WRAPGEN_HELP_H_ */
