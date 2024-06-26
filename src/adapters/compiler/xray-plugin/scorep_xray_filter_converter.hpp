#ifndef SCOREP_XRAY_FILTERCONVERTER_HPP
#define SCOREP_XRAY_FILTERCONVERTER_HPP

#include "config.h"
#include <string>
#include <set>
#include "SCOREP_Filter.h"

namespace XRayPlugin {

    /**
     * Helper class to transparently read a filter file and convert it into a xray filter file if requested.
     * The data is kept during the lifetime of this class so that the contents can be written multiple times,
     * e.g. if an export is wanted.
     * The conversion xray=>scorep is currently not supported, and scorep=>xray is only
     * supported on a very basic level and may produce errors
     * TODO: Currently, the conversion process is faulty since XRay/LLVM Sanitizer list filter files behave differently
     * TODO: Support real conversion
     */
    class FilterConverter {

    public:
        explicit FilterConverter(std::string inFilePath);

        ~FilterConverter();

        /**
         * Saves the filter content in a xray readable format to the provided file output path.
         * @param outFilePath Path to output file
         * @return true if successful, false otherwise
         */
        bool saveAsXRay(std::string &outFilePath);

        /**
         * Saves the filter content in a scorep readable format to the provided file output path.
         * Note that saving as scorep is currently not supported if the input file type was xray
         * @param outFilePath Path to output file
         * @return true if successful, false otherwise
         */
        bool saveAsScorep(std::string &outFilePath);


    private:
        std::string inFilePath; // Provided input path

        /**
         * File content of input file. Buffer this because if the format is already correct, it can be written to
         * multiple locations without re-reading the file
         */
        std::string inFileContent;

        // Whether provided input file was in xray format (true) or scorep/undefined (false)
        bool inIsXray = false;

        // True if a sensible guess about the format was possible
        bool formatOkay;

        // Filled with converted xray content, if it was needed
        std::string convertedXrayContent;

        // Scorep Filter of provided input file (if the file was in scorep format and the filter was needed)
        SCOREP_Filter *filter = nullptr;

        /**
         * Read contents of provided input file and keep them as string
         * @return
         */
        bool readInFile();

        /**
         * Determine whether input file is in XRAY or SCOREP format by searching for identifying keywords
         * @retur: true if sensible guess was made, false if input format could not be determined
         */
        bool determineInputFormat();

        /**
         * Converts buffered input to xray format
         * @return
         */
        bool convertToXRay();

        /**
         * Writes a content string to a file at the provided output path
         * @param outFilePath path to output file
         * @param content reference to content that should be written into file
         * @return true if successful, false otherwise
         */
        static bool writeFile(std::string &outFilePath, std::string &content);

        /**
         * Parses the provided input file to a scorep filter using the scorep internal filter infrastructure
         * @return true if successful, false otherwise
         */
        bool parseFilter();

    };

} // XRayPlugin

#endif //SCOREP_XRAY_FILTERCONVERTER_HPP
