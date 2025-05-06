/**
 * @file analyzer.h
 * @brief Defines the TimingAnalyzer class for analyzing timing paths
 */

#pragma once

#include <vector>
#include <string>
#include "parser.h"

/**
 * @struct TimingPathAnalysis
 * @brief Extended information about a timing path including optimization suggestions
 */
struct TimingPathAnalysis {
    std::shared_ptr<TimingPath> path;
    double worstStageDelay{0.0};
    std::shared_ptr<TimingEdge> worstStage;
    std::string optimizationSuggestion;
    
    TimingPathAnalysis(std::shared_ptr<TimingPath> path) : path(std::move(path)) {
        // Safety check for null path
        if (!this->path) {
            worstStageDelay = 0.0;
            worstStage = nullptr;
            optimizationSuggestion = "No path data available";
            return;
        }
        
        auto [delay, edge] = this->path->getWorstStage();
        worstStageDelay = delay;
        worstStage = edge;
        
        // Initialize optimization suggestion with a default message
        if (!worstStage) {
            optimizationSuggestion = "Insufficient path data for optimization suggestions";
        }
    }
};

/**
 * @class TimingAnalyzer
 * @brief Analyzes timing paths and generates optimization suggestions
 */
class TimingAnalyzer {
public:
    /**
     * @brief Find the top N critical paths by total delay
     * @param paths Vector of timing paths to analyze
     * @param topK Number of critical paths to return
     * @return Vector of critical path analyses
     */
    std::vector<TimingPathAnalysis> findCriticalPaths(
        const std::vector<TimingPath>& paths, int topK);
    
    /**
     * @brief Generate optimization suggestion for a timing path
     * @param path The timing path to analyze
     * @return Analysis with optimization suggestion
     */
    TimingPathAnalysis analyzePath(const TimingPath& path);
    
private:
    /**
     * @brief Generate suggestion for fan-out optimization
     * @param edge The edge with high fan-out
     * @return Optimization suggestion string
     */
    std::string suggestFanoutOptimization(const std::shared_ptr<TimingEdge>& edge);
    
    /**
     * @brief Generate suggestion for cell replacement
     * @param edge The edge with high cell delay
     * @return Optimization suggestion string
     */
    std::string suggestCellReplacement(const std::shared_ptr<TimingEdge>& edge);
    
    /**
     * @brief Generate suggestion for pipeline register insertion
     * @param edge The edge with high net delay
     * @return Optimization suggestion string
     */
    std::string suggestPipelineInsertion(const std::shared_ptr<TimingEdge>& edge);
}; 