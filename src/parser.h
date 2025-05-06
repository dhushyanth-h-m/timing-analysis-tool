/**
 * @file parser.h
 * @brief Defines the TimingParser class for parsing static timing reports
 */

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

/**
 * @struct TimingNode
 * @brief Represents a node in a timing path (e.g., cell or pin)
 */
struct TimingNode {
    std::string name;
    std::string type;       // e.g., "flop", "gate", "pin"
    double capacitance{0.0};
    double slew{0.0};
    
    TimingNode(std::string name, std::string type) 
        : name(std::move(name)), type(std::move(type)) {}
};

/**
 * @struct TimingEdge
 * @brief Represents a connection between two TimingNodes
 */
struct TimingEdge {
    std::shared_ptr<TimingNode> from;
    std::shared_ptr<TimingNode> to;
    double delay{0.0};
    double netDelay{0.0};
    double cellDelay{0.0};
    
    TimingEdge(std::shared_ptr<TimingNode> from, 
               std::shared_ptr<TimingNode> to,
               double delay = 0.0) 
        : from(std::move(from)), to(std::move(to)), delay(delay) {}
};

/**
 * @struct TimingPath
 * @brief Represents a complete timing path from startpoint to endpoint
 */
struct TimingPath {
    std::string id;
    std::string startpoint;
    std::string endpoint;
    double totalDelay{0.0};
    std::vector<std::shared_ptr<TimingEdge>> edges;
    
    // Calculate worst stage delay and its location
    std::pair<double, std::shared_ptr<TimingEdge>> getWorstStage() const {
        double maxDelay = 0.0;
        std::shared_ptr<TimingEdge> worstEdge = nullptr;
        
        // Safety check for empty paths
        if (edges.empty()) {
            return {0.0, nullptr};
        }
        
        for (const auto& edge : edges) {
            // Safety check for null edges
            if (!edge) continue;
            
            if (edge->delay > maxDelay) {
                maxDelay = edge->delay;
                worstEdge = edge;
            }
        }
        
        return {maxDelay, worstEdge};
    }
};

/**
 * @class TimingParser
 * @brief Parses static timing reports into TimingPath objects
 */
class TimingParser {
public:
    /**
     * @brief Parse timing report from a file
     * @param filename Path to timing report file
     * @return Vector of TimingPath objects
     */
    std::vector<TimingPath> parseFile(const std::string& filename);
    
private:
    /**
     * @brief Parse a single timing path section from the report
     * @param lines Vector of lines from the report
     * @param startLine Line index where the path section starts
     * @return A TimingPath object and the next line to process
     */
    std::pair<TimingPath, size_t> parsePath(
        const std::vector<std::string>& lines, size_t startLine);
    
    /**
     * @brief Parse a timing path header line
     * @param line Header line from the report
     * @return Extracted path ID, startpoint, endpoint, and total delay
     */
    std::tuple<std::string, std::string, std::string, double> 
    parsePathHeader(const std::string& line);
    
    /**
     * @brief Parse a timing path stage line
     * @param line Stage line from the report
     * @return A TimingEdge representing the stage
     */
    std::shared_ptr<TimingEdge> parsePathStage(const std::string& line);
    
    // Node cache to avoid creating duplicate nodes
    std::unordered_map<std::string, std::shared_ptr<TimingNode>> nodeCache;
}; 