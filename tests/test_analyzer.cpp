#include <gtest/gtest.h>
#include "analyzer.h"
#include <memory>
#include <vector>

// Helper function to create a test TimingPath
TimingPath createTestPath(const std::string& id, double delay) {
    TimingPath path;
    path.id = id;
    path.startpoint = "TEST_START";
    path.endpoint = "TEST_END";
    path.totalDelay = delay;
    return path;
}

// Helper function to add a test edge to a path
void addTestEdge(TimingPath& path, const std::string& fromName, const std::string& toName, 
                 double delay, double netDelay, double cellDelay) {
    auto fromNode = std::make_shared<TimingNode>(fromName, fromName.find("NET") != std::string::npos ? "net" : "cell");
    auto toNode = std::make_shared<TimingNode>(toName, toName.find("NET") != std::string::npos ? "net" : "cell");
    
    auto edge = std::make_shared<TimingEdge>(fromNode, toNode, delay);
    edge->netDelay = netDelay;
    edge->cellDelay = cellDelay;
    
    path.edges.push_back(edge);
}

class AnalyzerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test timing paths
        path1 = createTestPath("P1", 5.0);
        path2 = createTestPath("P2", 4.0);
        path3 = createTestPath("P3", 6.0);
        
        // Add edges to path1 (cell-delay dominated)
        addTestEdge(path1, "START", "NET1", 1.0, 0.3, 0.7);
        addTestEdge(path1, "NET1", "CELL1", 2.0, 0.5, 1.5); // Worst stage
        addTestEdge(path1, "CELL1", "NET2", 1.0, 0.6, 0.4);
        addTestEdge(path1, "NET2", "END", 1.0, 0.3, 0.7);
        
        // Add edges to path2 (net-delay dominated)
        addTestEdge(path2, "START", "NET3", 0.5, 0.5, 0.0);
        addTestEdge(path2, "NET3", "CELL2", 0.5, 0.0, 0.5);
        addTestEdge(path2, "CELL2", "NET4", 2.5, 2.0, 0.5); // Worst stage
        addTestEdge(path2, "NET4", "END", 0.5, 0.0, 0.5);
        
        // Add edges to path3 (fan-out dominated)
        addTestEdge(path3, "START", "NET5", 1.0, 0.5, 0.5);
        addTestEdge(path3, "NET5", "CELL3", 1.0, 0.5, 0.5);
        addTestEdge(path3, "CELL3", "NET6", 2.0, 1.0, 1.0); // Equal contribution
        addTestEdge(path3, "NET6", "END", 2.0, 1.0, 1.0); // Equal contribution
        
        // Store paths in vector
        testPaths = {path1, path2, path3};
    }
    
    TimingPath path1, path2, path3;
    std::vector<TimingPath> testPaths;
    TimingAnalyzer analyzer;
};

// Test that the analyzer correctly identifies critical paths
TEST_F(AnalyzerTest, FindsCriticalPaths) {
    auto criticalPaths = analyzer.findCriticalPaths(testPaths, 2);
    
    // Check that we found 2 paths
    ASSERT_EQ(criticalPaths.size(), 2);
    
    // Check that the paths are sorted by total delay
    EXPECT_EQ(criticalPaths[0].path->id, "P3"); // 6.0 delay
    EXPECT_EQ(criticalPaths[1].path->id, "P1"); // 5.0 delay
}

// Test that the analyzer correctly identifies the worst stage
TEST_F(AnalyzerTest, FindsWorstStage) {
    auto analysis = analyzer.analyzePath(path1);
    
    // Check that the worst stage is found
    ASSERT_TRUE(analysis.worstStage != nullptr);
    EXPECT_DOUBLE_EQ(analysis.worstStageDelay, 2.0);
    EXPECT_EQ(analysis.worstStage->from->name, "NET1");
    EXPECT_EQ(analysis.worstStage->to->name, "CELL1");
}

// Test that the analyzer generates cell replacement suggestions
TEST_F(AnalyzerTest, SuggestsCellReplacement) {
    auto analysis = analyzer.analyzePath(path1);
    
    // The suggestion should mention cell replacement since this path has high cell delay
    EXPECT_TRUE(analysis.optimizationSuggestion.find("replace") != std::string::npos ||
                analysis.optimizationSuggestion.find("faster") != std::string::npos);
}

// Test that the analyzer generates pipeline insertion suggestions
TEST_F(AnalyzerTest, SuggestsPipelineInsertion) {
    auto analysis = analyzer.analyzePath(path2);
    
    // The suggestion should mention pipeline insertion since this path has high net delay
    EXPECT_TRUE(analysis.optimizationSuggestion.find("pipeline") != std::string::npos ||
                analysis.optimizationSuggestion.find("register") != std::string::npos);
}

// Test that the analyzer generates fan-out optimization suggestions
TEST_F(AnalyzerTest, SuggestsFanoutOptimization) {
    auto analysis = analyzer.analyzePath(path3);
    
    // The suggestion should mention fan-out optimization when net and cell delays are equal
    EXPECT_TRUE(analysis.optimizationSuggestion.find("fan-out") != std::string::npos ||
                analysis.optimizationSuggestion.find("balance") != std::string::npos);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 