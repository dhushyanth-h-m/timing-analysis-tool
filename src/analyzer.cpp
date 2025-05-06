/**
 * @file analyzer.cpp
 * @brief Implementation of the TimingAnalyzer class
 */

#include "analyzer.h"
#include <algorithm>
#include <sstream>
#include <memory>

std::vector<TimingPathAnalysis> TimingAnalyzer::findCriticalPaths(
    const std::vector<TimingPath>& paths, int topK) {
    
    // Create a copy of paths that we can sort
    std::vector<std::shared_ptr<TimingPath>> pathPtrs;
    for (const auto& path : paths) {
        pathPtrs.push_back(std::make_shared<TimingPath>(path));
    }
    
    // Sort paths by total delay in descending order
    std::sort(pathPtrs.begin(), pathPtrs.end(), 
              [](const auto& a, const auto& b) {
                  return a->totalDelay > b->totalDelay;
              });
    
    // Get the top K paths
    std::vector<TimingPathAnalysis> criticalPaths;
    for (int i = 0; i < std::min(topK, static_cast<int>(pathPtrs.size())); ++i) {
        auto analysis = analyzePath(*pathPtrs[i]);
        criticalPaths.push_back(analysis);
    }
    
    return criticalPaths;
}

TimingPathAnalysis TimingAnalyzer::analyzePath(const TimingPath& path) {
    auto pathCopy = std::make_shared<TimingPath>(path);
    TimingPathAnalysis analysis(pathCopy);
    
    // Check if worst stage exists
    if (!analysis.worstStage) {
        analysis.optimizationSuggestion = "Insufficient path data for optimization suggestions.";
        return analysis;
    }
    
    // Determine optimization strategy based on the worst stage characteristics
    auto worstEdge = analysis.worstStage;
    
    // Safety check for null pointers
    if (!worstEdge || !worstEdge->from || !worstEdge->to) {
        analysis.optimizationSuggestion = "Insufficient path data for optimization suggestions.";
        return analysis;
    }
    
    // If this is a net with high delay, suggest pipeline insertion
    if (worstEdge->netDelay > worstEdge->cellDelay && worstEdge->netDelay > 0.5 * analysis.worstStageDelay) {
        analysis.optimizationSuggestion = suggestPipelineInsertion(worstEdge);
    }
    // If this is a cell with high delay, suggest cell replacement
    else if (worstEdge->cellDelay > worstEdge->netDelay && worstEdge->cellDelay > 0.5 * analysis.worstStageDelay) {
        analysis.optimizationSuggestion = suggestCellReplacement(worstEdge);
    }
    // Otherwise, suggest fan-out optimization
    else {
        analysis.optimizationSuggestion = suggestFanoutOptimization(worstEdge);
    }
    
    return analysis;
}

std::string TimingAnalyzer::suggestFanoutOptimization(const std::shared_ptr<TimingEdge>& edge) {
    std::stringstream suggestion;
    
    if (edge && edge->from && edge->to) {
        suggestion << "balance fan-out after " << edge->from->name;
    } else {
        suggestion << "balance fan-out in the critical path";
    }
    
    return suggestion.str();
}

std::string TimingAnalyzer::suggestCellReplacement(const std::shared_ptr<TimingEdge>& edge) {
    std::stringstream suggestion;
    
    if (edge && edge->from && edge->from->type != "net") {
        // Suggest faster cell variant
        std::string cellName = edge->from->name;
        suggestion << "replace " << cellName << " with ";
        
        // Determine cell type and suggest appropriate faster variant
        if (cellName.find("INV") != std::string::npos) {
            suggestion << cellName << "_HF"; // High-speed variant
        } else if (cellName.find("BUF") != std::string::npos) {
            suggestion << cellName << "_X4"; // Stronger buffer
        } else if (cellName.find("NAND") != std::string::npos || 
                   cellName.find("NOR") != std::string::npos) {
            suggestion << cellName << "_HS"; // High-speed variant
        } else {
            suggestion << "a faster variant of " << cellName;
        }
    } else {
        suggestion << "use faster cell library for critical path cells";
    }
    
    return suggestion.str();
}

std::string TimingAnalyzer::suggestPipelineInsertion(const std::shared_ptr<TimingEdge>& edge) {
    std::stringstream suggestion;
    
    if (edge && edge->from && edge->to) {
        suggestion << "insert pipeline register between " 
                   << edge->from->name << " → " << edge->to->name;
    } else {
        suggestion << "insert pipeline register in the critical path";
    }
    
    return suggestion.str();
} 