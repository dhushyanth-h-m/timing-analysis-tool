#include <gtest/gtest.h>
#include "parser.h"
#include <fstream>
#include <string>
#include <memory>

// Helper function to create a temporary test file
std::string createTempTimingReport() {
    const std::string tempFilePath = "temp_test_timing.rpt";
    std::ofstream tempFile(tempFilePath);
    
    tempFile << "Path   Endpoint   Startpoint   Delay\n";
    tempFile << "------------------------------------------------\n";
    tempFile << "P1     FF_Q        PI          2.345\n";
    tempFile << "P1.1   NET1        PI          0.123\n";
    tempFile << "P1.2   INV1        NET1        0.456\n";
    
    tempFile << "P2     NAND1_Y     PI2         3.210\n";
    tempFile << "P2.1   NET3        PI2         0.210\n";
    tempFile << "P2.2   BUF1        NET3        0.450\n";
    
    tempFile.close();
    return tempFilePath;
}

// Clean up the temporary file
void removeTempFile(const std::string& filePath) {
    std::remove(filePath.c_str());
}

class ParserTest : public ::testing::Test {
protected:
    void SetUp() override {
        tempFilePath = createTempTimingReport();
    }
    
    void TearDown() override {
        removeTempFile(tempFilePath);
    }
    
    std::string tempFilePath;
};

// Test that the parser can read a file
TEST_F(ParserTest, CanReadFile) {
    TimingParser parser;
    ASSERT_NO_THROW(parser.parseFile(tempFilePath));
}

// Test that the parser correctly extracts paths
TEST_F(ParserTest, ExtractsPaths) {
    TimingParser parser;
    auto paths = parser.parseFile(tempFilePath);
    
    // Check that we found 2 paths
    ASSERT_EQ(paths.size(), 2);
    
    // Check the first path
    ASSERT_EQ(paths[0].id, "P1");
    ASSERT_EQ(paths[0].startpoint, "PI");
    ASSERT_EQ(paths[0].endpoint, "FF_Q");
    ASSERT_DOUBLE_EQ(paths[0].totalDelay, 2.345);
    
    // Check that the first path has the correct number of edges
    ASSERT_EQ(paths[0].edges.size(), 2);
    
    // Check the second path
    ASSERT_EQ(paths[1].id, "P2");
    ASSERT_EQ(paths[1].startpoint, "PI2");
    ASSERT_EQ(paths[1].endpoint, "NAND1_Y");
    ASSERT_DOUBLE_EQ(paths[1].totalDelay, 3.21);
    
    // Check that the second path has the correct number of edges
    ASSERT_EQ(paths[1].edges.size(), 2);
}

// Test that the parser correctly extracts edges
TEST_F(ParserTest, ExtractsEdges) {
    TimingParser parser;
    auto paths = parser.parseFile(tempFilePath);
    
    // Check the first edge of the first path
    ASSERT_EQ(paths[0].edges[0]->from->name, "PI");
    ASSERT_EQ(paths[0].edges[0]->to->name, "NET1");
    ASSERT_DOUBLE_EQ(paths[0].edges[0]->delay, 0.123);
    
    // Check the second edge of the first path
    ASSERT_EQ(paths[0].edges[1]->from->name, "NET1");
    ASSERT_EQ(paths[0].edges[1]->to->name, "INV1");
    ASSERT_DOUBLE_EQ(paths[0].edges[1]->delay, 0.456);
}

// Test that the parser handles node types correctly
TEST_F(ParserTest, HandlesNodeTypes) {
    TimingParser parser;
    auto paths = parser.parseFile(tempFilePath);
    
    // Find nodes by name and check their types
    for (const auto& path : paths) {
        for (const auto& edge : path.edges) {
            if (edge->from->name == "PI" || edge->from->name == "PI2") {
                ASSERT_EQ(edge->from->type, "primary_input");
            } else if (edge->from->name == "NET1" || edge->from->name == "NET3") {
                ASSERT_EQ(edge->from->type, "net");
            } else if (edge->to->name == "INV1") {
                ASSERT_EQ(edge->to->type, "inverter");
            } else if (edge->to->name == "BUF1") {
                ASSERT_EQ(edge->to->type, "buffer");
            }
        }
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 