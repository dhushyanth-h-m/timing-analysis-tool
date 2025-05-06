/**
 * @file utils.h
 * @brief Utility functions for the timing analysis tool
 */

#pragma once

#include <vector>
#include <string>
#include "analyzer.h"

/**
 * @namespace Utils
 * @brief Utility functions for the timing analysis tool
 */
namespace Utils {

/**
 * @brief Print the analysis results to console or file
 * @param criticalPaths Vector of critical path analyses
 * @param outputFile Optional file path to write results to
 */
void printResults(const std::vector<TimingPathAnalysis>& criticalPaths, 
                  const std::string& outputFile = "");

/**
 * @brief Format a timing path analysis result as a string
 * @param index Index of the path (1-based)
 * @param analysis The timing path analysis
 * @return Formatted result string
 */
std::string formatPathResult(int index, const TimingPathAnalysis& analysis);

/**
 * @brief Convert time in seconds to a human-readable format
 * @param seconds Time in seconds
 * @return Formatted time string
 */
std::string formatTime(double seconds);

} // namespace Utils 