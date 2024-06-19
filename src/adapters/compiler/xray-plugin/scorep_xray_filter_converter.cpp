//
// Created by paul on 6/19/24.
//

#include <fstream>
#include <sstream>
#include <iostream>
#include <utility>
#include <vector>
#include <regex>

#include "scorep_xray_filter_converter.hpp"
#include "scorep_filter_matching.h"

namespace XRayPlugin {


    FilterConverter::FilterConverter(std::string inFilePath) : inFilePath(std::move(inFilePath)){
        if(!readInFile()){
            formatOkay = false;
        }
        formatOkay = determineInputFormat();
    }

    bool FilterConverter::readInFile() {
        std::ifstream file(inFilePath);
        std::stringstream buffer;
        if (file.is_open()) {
            buffer << file.rdbuf();
            inFileContent = buffer.str();
            file.close();
            return true;
        } else {
            std::cout << "Unable to open instrument filter file!" << std::endl;
            return false;
        }
    }

    bool FilterConverter::writeFile(std::string& outFilePath, std::string& content){
        std::ofstream file(outFilePath);
        if (file.is_open()) {
            file << content;
            file.close();
            return true;
        } else {
            std::cout << "Unable to write instrument filter file!" << std::endl;
            return false;
        }
    }

    bool FilterConverter::determineInputFormat() {
        bool xray = false;
        bool scorep = false;
        // Keywords to look out for that will determine the file type
        std::vector<std::string> formatKeysScoreP = {"SCOREP_FILE_NAMES_BEGIN", "SCOREP_REGION_NAMES_BEGIN", "INCLUDE", "EXCLUDE", "MANGLED"};
        std::vector<std::string> formatKeysXRay = {"[always]", "[never]"};

        // tokenize
        std::vector<std::string> tokens;
        std::istringstream stream(inFileContent);
        std::string token;
        while (stream >> token) {
            tokens.push_back(token);
        }

        // Search for scorep keywords in input file
        for (const auto& substr : formatKeysScoreP) {
            if (std::find(tokens.begin(), tokens.end(), substr) != tokens.end()){
                scorep = true;
                break;
            }
        }
        // Now search for xray words to make sure
        for (const auto& substr : formatKeysXRay) {
            if (std::find(tokens.begin(), tokens.end(), substr) != tokens.end()){
                xray = true;
                break;
            }
        }

        // Check that they are mutually exclusive but one is true
        if (xray != scorep){
            inIsXray = xray;
            return true;
        }else{
            std::cout << "Could not determine format of instrumentation filter file. "
                         "Make sure your syntax is correct." << std::endl;
            return false;
        }
    }

    bool FilterConverter::saveAsScorep(std::string &outFilePath) {
        if(!formatOkay){
            // Better to fail than to risk undefined instrumentation behaviour
            return false;
        }
        if(!inIsXray){
            return writeFile(outFilePath, inFileContent);
        }
        std::cout << "Converting XRay Filter files to ScoreP filter is not supported!" << std::endl;
        return false;
    }

    bool FilterConverter::saveAsXRay(std::string &outFilePath) {
        if(!formatOkay){
            // Better to fail than to risk undefined instrumentation behaviour
            return false;
        }
        if(inIsXray){
            return writeFile(outFilePath, inFileContent);
        }
        if(convertedXrayContent.empty()){
            // Only now is a conversion really necessary
            if(!parseFilter()){
                return false;
            }
            if(!convertToXRay()){
                return false;
            }
        }
        return writeFile(outFilePath, convertedXrayContent);
    }

    bool FilterConverter::parseFilter(){
        filter = SCOREP_Filter_New();
        if(!filter){
            std::cout << "Could not alloc instrumentation filter!" << std::endl;
            return false;
        }
        SCOREP_ErrorCode result = SCOREP_Filter_ParseFile(filter, inFilePath.c_str());
        if(result != SCOREP_SUCCESS){
            std::cout << "Could not read or parse instrumentation filter file!" << std::endl;
            return false;
        }
        return true;
    }

    bool FilterConverter::convertToXRay(){
        std::stringstream xrayOutAlways;
        std::stringstream xrayOutNever;

        xrayOutAlways << "[always]" << std::endl;
        xrayOutNever << "[never]" << std::endl;

        scorep_filter_rule_t* fileRule = filter->file_rules;
        while(fileRule != nullptr){
            if(fileRule->is_exclude){
                xrayOutNever << "src:" << fileRule->pattern << std::endl;
            }else{
                xrayOutAlways << "src:" << fileRule->pattern << std::endl;
            }
            fileRule = fileRule->next;
        }

        scorep_filter_rule_t* regionRule = filter->function_rules;
        while(regionRule != nullptr){
            if(regionRule->is_exclude){
                xrayOutNever << "fun:" << regionRule->pattern << std::endl;
            }else{
                xrayOutAlways << "fun:" << regionRule->pattern << std::endl;
            }
            regionRule = regionRule->next;
        }
        convertedXrayContent = xrayOutAlways.str() + xrayOutNever.str();
        return true;
    }

    FilterConverter::~FilterConverter() {
        if(filter){
            SCOREP_Filter_Delete(filter);
        }
    }
} // XRayPlugin