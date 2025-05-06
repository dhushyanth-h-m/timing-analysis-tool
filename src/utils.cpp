/**
 * @file utils.cpp
 * @brief Implementation of utility functions for the timing analysis tool
 */

#include "utils.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>

namespace Utils {

void printResults(const std::vector<TimingPathAnalysis>& criticalPaths, 
                  const std::string& outputFile) {
    
    std::stringstream output;
    
    // Format header
    output << "Top " << criticalPaths.size() << " Critical Paths:\n";
    
    // Format each critical path
    for (size_t i = 0; i < criticalPaths.size(); ++i) {
        output << formatPathResult(i + 1, criticalPaths[i]) << "\n";
    }
    
    // Print to console
    std::cout << output.str();
    
    // Write to file if specified
    if (!outputFile.empty()) {
        std::ofstream outFile(outputFile);
        if (outFile) {
            outFile << output.str();
            std::cout << "Results written to " << outputFile << std::endl;
        } else {
            std::cerr << "Error: Failed to open output file: " << outputFile << std::endl;
        }
    }
}

std::string formatPathResult(int index, const TimingPathAnalysis& analysis) {
    std::stringstream result;
    
    // Safety check for null path
    if (!analysis.path) {
        result << index << ". <Invalid path>";
        return result.str();
    }
    
    // Format path ID, delay, worst stage, and suggestion
    result << index << ". " 
           << analysis.path->id << ": Delay = " 
           << std::fixed << std::setprecision(3) << analysis.path->totalDelay << " ns";
    
    // Add worst stage info if available
    if (analysis.worstStage && analysis.worstStage->from) {
        result << " (Stage max: " 
               << std::fixed << std::setprecision(3) << analysis.worstStageDelay 
               << " ns at " << analysis.worstStage->from->name << ")";
    }
    
    // Add optimization suggestion
    result << " — Suggest: " << analysis.optimizationSuggestion;
    
    return result.str();
}

std::string formatTime(double seconds) {
    std::stringstream result;
    
    if (seconds < 0.001) {
        // Format as microseconds
        result << std::fixed << std::setprecision(0) << (seconds * 1000000) << " μs";
    } else if (seconds < 1) {
        // Format as milliseconds
        result << std::fixed << std::setprecision(2) << (seconds * 1000) << " ms";
    } else {
        // Format as seconds
        result << std::fixed << std::setprecision(2) << seconds << " s";
    }
    
    return result.str();
}

} // namespace Utils 