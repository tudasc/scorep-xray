#include <fstream>
#include <sstream>
#include <iostream>
#include <utility>
#include <vector>
#include <regex>

#include "scorep_xray_filter_converter.hpp"
#include "scorep_filter_matching.h"
#include "UTILS_Error.h"

namespace XRayPlugin {


    FilterConverter::FilterConverter(std::string inFilePath) : inFilePath(std::move(inFilePath)) {
        if (readInFile()) {
            formatOkay = determineInputFormat();
        } else {
            formatOkay = false;
        }
    }

    bool FilterConverter::readInFile() {
        std::ifstream file(inFilePath);
        std::stringstream buffer;
        if (file.is_open()) {
            buffer << file.rdbuf();
            inFileContent = buffer.str();
            file.close();
            return file.good();
        } else {
            UTILS_WARNING("Unable to open instrument filter file!");
            return false;
        }
    }

    bool FilterConverter::writeFile(std::string &outFilePath, std::string &content) {
        std::ofstream file(outFilePath);
        if (file.is_open()) {
            file << content;
            file.close();
            return file.good();
        } else {
            UTILS_WARNING("Unable to write instrument filter file!");
            return false;
        }
    }

    bool FilterConverter::determineInputFormat() {
        bool xray = false;
        bool scorep = false;
        // Keywords to look out for that will determine the file type
        std::vector<std::string> formatKeysScoreP = {"SCOREP_FILE_NAMES_BEGIN", "SCOREP_REGION_NAMES_BEGIN", "INCLUDE",
                                                     "EXCLUDE", "MANGLED"};
        std::vector<std::string> formatKeysXRay = {"[always]", "[never]"};

        // tokenize
        std::vector<std::string> tokens;
        std::istringstream stream(inFileContent);
        std::string token;
        while (stream >> token) {
            tokens.push_back(token);
        }

        // Search for scorep keywords in input file
        for (const auto &substr: formatKeysScoreP) {
            if (std::find(tokens.begin(), tokens.end(), substr) != tokens.end()) {
                scorep = true;
                break;
            }
        }
        // Now search for xray words to make sure
        for (const auto &substr: formatKeysXRay) {
            if (std::find(tokens.begin(), tokens.end(), substr) != tokens.end()) {
                xray = true;
                break;
            }
        }

        // Check that they are mutually exclusive but one is true
        if (xray != scorep) {
            inIsXray = xray;
            return true;
        } else {
            UTILS_WARNING("Could not determine format of instrumentation filter file. "
                          "Make sure your syntax is correct.");
            return false;
        }
    }

    bool FilterConverter::saveAsScorep(std::string &outFilePath) {
        if (!formatOkay) {
            // Better to fail than to risk undefined instrumentation behaviour
            return false;
        }
        if (!inIsXray) {
            return writeFile(outFilePath, inFileContent);
        }
        UTILS_WARNING("Converting XRay Filter files to ScoreP filter is not supported!");
        return false;
    }

    bool FilterConverter::saveAsXRay(std::string &outFilePath) {
        if (!formatOkay) {
            // Better to fail than to risk undefined instrumentation behaviour
            return false;
        }
        if (inIsXray) {
            return writeFile(outFilePath, inFileContent);
        }
        if (convertedXrayContent.empty()) {
            // Only now is a conversion really necessary
            if (!parseFilter()) {
                return false;
            }
            if (!convertToXRay()) {
                return false;
            }
        }
        return writeFile(outFilePath, convertedXrayContent);
    }

    bool FilterConverter::parseFilter() {
        filter = SCOREP_Filter_New();
        if (!filter) {
            UTILS_WARNING("Could not alloc instrumentation filter!");
            return false;
        }
        SCOREP_ErrorCode result = SCOREP_Filter_ParseFile(filter, inFilePath.c_str());
        if (result != SCOREP_SUCCESS) {
            UTILS_WARNING("Could not read or parse instrumentation filter file!");
            return false;
        }
        return true;
    }

    bool FilterConverter::convertToXRay() {
        UTILS_WARNING("Note that converting Score-P filters to XRay filters XRay is currently only supported on a "
                      "very basic level. The behaviour therefore differs from Score-P filters in many aspects.\n"
                      " Consider using --no-xray-delete-converted-filter and then edit the xray filter to your needs"
                      " manually.\n Differences include:\n"
                      "\t(!) Regions and Files are included if they match a single inclusion filter, regardless of"
                      " an exclusion filter matching the file/region\n"
                      "\t(!) Demangled names are not properly recognized by XRay!\n"
                      "Therefore, be wary of using \"*\" without any further specification in your INCLUDE sections, as"
                      " that will effectively disable all exclusion filters! Furthermore, consider using mangled names."
                      "\nNote that runtime filtering is not affected by these restrictions.");
        std::stringstream xrayOutAlways;
        std::stringstream xrayOutNever;
        std::stringstream xrayInfo;

        xrayInfo << "# converted Score-P filter file" << std::endl;
        xrayOutAlways << "[always]" << std::endl;
        xrayOutNever << "[never]" << std::endl;

        scorep_filter_rule_t *fileRule = filter->file_rules;
        while (fileRule != nullptr) {
            if (fileRule->is_exclude) {
                xrayOutNever << "src:" << fileRule->pattern << std::endl;
            } else {
                xrayOutAlways << "src:" << fileRule->pattern << std::endl;
            }
            fileRule = fileRule->next;
        }

        scorep_filter_rule_t *regionRule = filter->function_rules;
        bool emittedWarning = false; // Only emit warning to xray file once
        while (regionRule != nullptr) {
            if (!regionRule->is_mangled && !emittedWarning) {
                std::string warn("A demangled region rule was specified in the filter file, but XRay uses mangled "
                                 "names. Consider specifying mangled names in your instrumentation filter file.");
                UTILS_WARNING(warn.c_str());
                xrayInfo << "# " << warn << std::endl;
                emittedWarning = true;
            }
            if (regionRule->is_exclude) {
                xrayOutNever << "fun:" << regionRule->pattern << std::endl;
            } else {
                xrayOutAlways << "fun:" << regionRule->pattern << std::endl;
            }
            regionRule = regionRule->next;
        }

        convertedXrayContent = xrayInfo.str() + xrayOutAlways.str() + xrayOutNever.str();
        return true;
    }

    FilterConverter::~FilterConverter() {
        if (filter) {
            SCOREP_Filter_Delete(filter);
        }
    }
} // XRayPlugin