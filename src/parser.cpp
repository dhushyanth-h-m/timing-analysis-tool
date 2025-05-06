/**
 * @file parser.cpp
 * @brief Implementation of the TimingParser class
 */

#include "parser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <regex>
#include <stdexcept>

std::vector<TimingPath> TimingParser::parseFile(const std::string& filename) {
    std::vector<TimingPath> paths;
    std::ifstream file(filename);
    
    if (!file) {
        throw std::runtime_error("Failed to open file: " + filename);
    }
    
    // Read the entire file into memory
    std::vector<std::string> lines;
    std::string line;
    
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    
    // Process the file line by line
    size_t lineIndex = 0;
    while (lineIndex < lines.size()) {
        // Look for path header lines
        if (lineIndex < lines.size() && lines[lineIndex].find("Path ") == 0) {
            try {
                // Parse path and get next line index
                auto [path, nextLine] = parsePath(lines, lineIndex);
                paths.push_back(path);
                lineIndex = nextLine;
            } catch (const std::exception& e) {
                std::cerr << "Warning: Failed to parse path at line " << lineIndex 
                          << ": " << e.what() << std::endl;
                lineIndex++;
            }
        } else {
            lineIndex++;
        }
    }
    
    return paths;
}

std::pair<TimingPath, size_t> TimingParser::parsePath(
    const std::vector<std::string>& lines, size_t startLine) {
    
    TimingPath path;
    
    // Parse the path header line
    auto [id, startpoint, endpoint, delay] = parsePathHeader(lines[startLine]);
    path.id = id;
    path.startpoint = startpoint;
    path.endpoint = endpoint;
    path.totalDelay = delay;
    
    // Move to the next line
    size_t lineIndex = startLine + 1;
    
    // Parse path stages until we reach the end of the path section
    while (lineIndex < lines.size()) {
        const auto& line = lines[lineIndex];
        
        // Check if we've reached the end of the path section
        if (line.empty() || line.find("Path ") == 0 || line.find("End of") != std::string::npos) {
            break;
        }
        
        // Check if this is a path stage line
        if (line.find(path.id + ".") != std::string::npos) {
            try {
                auto edge = parsePathStage(line);
                if (edge) {
                    path.edges.push_back(edge);
                }
            } catch (const std::exception& e) {
                std::cerr << "Warning: Failed to parse stage at line " << lineIndex 
                          << ": " << e.what() << std::endl;
            }
        }
        
        lineIndex++;
    }
    
    return {path, lineIndex};
}

std::tuple<std::string, std::string, std::string, double> 
TimingParser::parsePathHeader(const std::string& line) {
    // Example header: "Path P1     FF_Q        PI          2.345"
    std::string id, startpoint, endpoint;
    double delay = 0.0;
    
    std::regex headerPattern(R"(Path\s+(\S+)\s+(\S+)\s+(\S+)\s+([\d\.]+))");
    std::smatch matches;
    
    if (std::regex_search(line, matches, headerPattern) && matches.size() >= 5) {
        id = matches[1].str();
        endpoint = matches[2].str();
        startpoint = matches[3].str();
        delay = std::stod(matches[4].str());
    } else {
        throw std::runtime_error("Invalid path header format: " + line);
    }
    
    return {id, startpoint, endpoint, delay};
}

std::shared_ptr<TimingEdge> TimingParser::parsePathStage(const std::string& line) {
    // Example stage: "P1.1   NET1        PI          0.123"
    std::regex stagePattern(R"((\S+\.\d+)\s+(\S+)\s+(\S+)\s+([\d\.]+))");
    std::smatch matches;
    
    if (std::regex_search(line, matches, stagePattern) && matches.size() >= 5) {
        std::string stageId = matches[1].str();
        std::string toName = matches[2].str();
        std::string fromName = matches[3].str();
        double delay = std::stod(matches[4].str());
        
        // Get or create nodes
        std::shared_ptr<TimingNode> fromNode;
        std::shared_ptr<TimingNode> toNode;
        
        auto fromIt = nodeCache.find(fromName);
        if (fromIt != nodeCache.end()) {
            fromNode = fromIt->second;
        } else {
            // Try to determine node type based on name patterns
            std::string fromType = "unknown";
            if (fromName.find("NET") != std::string::npos) {
                fromType = "net";
            } else if (fromName.find("FF") != std::string::npos || 
                       fromName.find("FLOP") != std::string::npos) {
                fromType = "flop";
            } else if (fromName.find("PI") != std::string::npos) {
                fromType = "primary_input";
            }
            
            fromNode = std::make_shared<TimingNode>(fromName, fromType);
            nodeCache[fromName] = fromNode;
        }
        
        auto toIt = nodeCache.find(toName);
        if (toIt != nodeCache.end()) {
            toNode = toIt->second;
        } else {
            // Try to determine node type based on name patterns
            std::string toType = "unknown";
            if (toName.find("NET") != std::string::npos) {
                toType = "net";
            } else if (toName.find("INV") != std::string::npos) {
                toType = "inverter";
            } else if (toName.find("BUF") != std::string::npos) {
                toType = "buffer";
            } else if (toName.find("NAND") != std::string::npos) {
                toType = "nand";
            } else if (toName.find("NOR") != std::string::npos) {
                toType = "nor";
            } else if (toName.find("FF") != std::string::npos || 
                       toName.find("FLOP") != std::string::npos) {
                toType = "flop";
            } else if (toName.find("PO") != std::string::npos) {
                toType = "primary_output";
            }
            
            toNode = std::make_shared<TimingNode>(toName, toType);
            nodeCache[toName] = toNode;
        }
        
        // Create edge
        auto edge = std::make_shared<TimingEdge>(fromNode, toNode, delay);
        
        // Determine if delay is net or cell delay based on the from/to types
        if (fromNode->type == "net") {
            edge->netDelay = delay;
        } else {
            edge->cellDelay = delay;
        }
        
        return edge;
    }
    
    return nullptr;
} 