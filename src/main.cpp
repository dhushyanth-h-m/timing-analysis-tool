/**
 * @file main.cpp
 * @brief Main entry point for Timing Analysis Tool
 * 
 * This tool parses static timing reports, identifies critical paths,
 * and suggests optimization strategies.
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

// Filesystem include based on compiler support
#if defined(HAVE_STD_FILESYSTEM)
  #include <filesystem>
  namespace fs = std::filesystem;
#elif defined(HAVE_STD_EXPERIMENTAL_FILESYSTEM)
  #include <experimental/filesystem>
  namespace fs = std::experimental::filesystem;
#else
  #error "No filesystem support available"
#endif

#include <algorithm>
#include "parser.h"
#include "analyzer.h"
#include "utils.h"

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [OPTIONS]\n"
              << "Options:\n"
              << "  -f, --file PATH       Input timing report file path\n"
              << "  -d, --dir PATH        Directory containing timing reports\n"
              << "  -o, --output PATH     Output analysis results to file\n"
              << "  -k, --topk N          Number of critical paths to show (default: 10)\n"
              << "  -h, --help            Show this help message\n";
}

int main(int argc, char* argv[]) {
    // Default parameters
    std::string inputFile;
    std::string inputDir;
    std::string outputFile;
    int topK = 10;
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if ((arg == "-f" || arg == "--file") && i + 1 < argc) {
            inputFile = argv[++i];
        } else if ((arg == "-d" || arg == "--dir") && i + 1 < argc) {
            inputDir = argv[++i];
        } else if ((arg == "-o" || arg == "--output") && i + 1 < argc) {
            outputFile = argv[++i];
        } else if ((arg == "-k" || arg == "--topk") && i + 1 < argc) {
            topK = std::stoi(argv[++i]);
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    }
    
    if (inputFile.empty() && inputDir.empty()) {
        std::cerr << "Error: Input file or directory must be specified\n";
        printUsage(argv[0]);
        return 1;
    }
    
    try {
        if (!inputFile.empty()) {
            // Process single file
            std::cout << "Processing timing report: " << inputFile << std::endl;
            
            // Parse the timing report
            TimingParser parser;
            auto timingPaths = parser.parseFile(inputFile);
            
            // Analyze the timing paths
            TimingAnalyzer analyzer;
            auto criticalPaths = analyzer.findCriticalPaths(timingPaths, topK);
            
            // Generate and display results
            Utils::printResults(criticalPaths, outputFile);
            
        } else {
            // Process multiple files in directory
            std::cout << "Processing timing reports in: " << inputDir << std::endl;
            
            std::vector<TimingPath> allPaths;
            TimingParser parser;
            
            for (const auto& entry : fs::directory_iterator(inputDir)) {
                if (entry.is_regular_file() && entry.path().extension() == ".rpt") {
                    std::cout << "  Processing: " << entry.path().filename() << std::endl;
                    auto paths = parser.parseFile(entry.path().string());
                    allPaths.insert(allPaths.end(), paths.begin(), paths.end());
                }
            }
            
            // Analyze all collected paths
            TimingAnalyzer analyzer;
            auto criticalPaths = analyzer.findCriticalPaths(allPaths, topK);
            
            // Generate and display results
            Utils::printResults(criticalPaths, outputFile);
        }
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
} 