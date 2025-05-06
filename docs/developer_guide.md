# Timing Analysis Tool Developer Guide

## Architecture Overview

The Timing Analysis Tool is designed with a modular architecture to separate concerns and make the codebase maintainable and extensible. The main components are:

1. **Parser**: Responsible for reading timing reports and converting them into an in-memory representation.
2. **Analyzer**: Processes the timing paths to identify critical paths and generate optimization suggestions.
3. **Utils**: Provides utility functions for formatting and outputting results.
4. **Main**: Command-line interface that ties everything together.
5. **Tcl Scripts**: Automation layer for batch processing.

```
                +-------------+
                |    Main     |
                +------+------+
                       |
        +------+-------+-------+------+
        |      |               |      |
+-------v----+ |       +-------v----+ |
|   Parser   | |       |  Analyzer  | |
+------------+ |       +------------+ |
               |                      |
               |     +------------+   |
               +---->|   Utils    |<--+
                     +------------+
```

## Directory Structure

```
timing-analysis-tool/
├── src/                   # C++ source code
│   ├── main.cpp           # Entry point
│   ├── parser.cpp/.h      # Timing report parser
│   ├── analyzer.cpp/.h    # Path analysis and optimization
│   └── utils.cpp/.h       # Utility functions
├── scripts/               # Tcl automation scripts
│   └── run_timing_analysis.tcl
├── tests/                 # Unit tests
├── examples/              # Sample timing reports
├── docs/                  # Documentation
├── CMakeLists.txt         # Build system configuration
└── README.md              # Project overview
```

## Core Classes and Data Structures

### TimingNode

Represents a node in a timing path (e.g., a cell or pin).

```cpp
struct TimingNode {
    std::string name;
    std::string type;       // e.g., "flop", "gate", "pin"
    double capacitance{0.0};
    double slew{0.0};
    
    TimingNode(std::string name, std::string type);
};
```

### TimingEdge

Represents a connection between two TimingNodes.

```cpp
struct TimingEdge {
    std::shared_ptr<TimingNode> from;
    std::shared_ptr<TimingNode> to;
    double delay{0.0};
    double netDelay{0.0};
    double cellDelay{0.0};
    
    TimingEdge(std::shared_ptr<TimingNode> from, 
               std::shared_ptr<TimingNode> to,
               double delay = 0.0);
};
```

### TimingPath

Represents a complete timing path from startpoint to endpoint.

```cpp
struct TimingPath {
    std::string id;
    std::string startpoint;
    std::string endpoint;
    double totalDelay{0.0};
    std::vector<std::shared_ptr<TimingEdge>> edges;
    
    std::pair<double, std::shared_ptr<TimingEdge>> getWorstStage() const;
};
```

### TimingParser

Parses static timing reports into TimingPath objects.

```cpp
class TimingParser {
public:
    std::vector<TimingPath> parseFile(const std::string& filename);
    
private:
    // Helper methods
    std::pair<TimingPath, size_t> parsePath(const std::vector<std::string>& lines, size_t startLine);
    std::tuple<std::string, std::string, std::string, double> parsePathHeader(const std::string& line);
    std::shared_ptr<TimingEdge> parsePathStage(const std::string& line);
    
    // Node cache to avoid creating duplicate nodes
    std::unordered_map<std::string, std::shared_ptr<TimingNode>> nodeCache;
};
```

### TimingAnalyzer

Analyzes timing paths and generates optimization suggestions.

```cpp
class TimingAnalyzer {
public:
    std::vector<TimingPathAnalysis> findCriticalPaths(
        const std::vector<TimingPath>& paths, int topK);
    TimingPathAnalysis analyzePath(const TimingPath& path);
    
private:
    // Suggestion generation methods
    std::string suggestFanoutOptimization(const std::shared_ptr<TimingEdge>& edge);
    std::string suggestCellReplacement(const std::shared_ptr<TimingEdge>& edge);
    std::string suggestPipelineInsertion(const std::shared_ptr<TimingEdge>& edge);
};
```

## Building the Project

### Prerequisites

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.12 or higher
- UNIX-like environment (Linux, macOS, WSL)
- Tcl interpreter (for automation scripts)

### Build Commands

```bash
# Create build directory
mkdir build && cd build

# Configure with debug symbols
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Configure with optimizations
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build
make

# Run tests
make test

# Generate docs (if configured)
make docs

# Install
sudo make install
```

## Development Workflow

1. **Set Up Development Environment**:
   - Clone the repository
   - Install prerequisites
   - Create a build directory for out-of-source builds

2. **Implement Features or Fix Bugs**:
   - Write tests first (TDD approach)
   - Implement the feature or fix
   - Run tests to verify

3. **Testing**:
   - Unit tests with Google Test
   - Manual testing with sample reports

4. **Documentation**:
   - Update code comments
   - Update user and developer documentation
   - Document new features in README.md

5. **Pull Request**:
   - Create a PR with your changes
   - Ensure all CI checks pass
   - Address review comments

## Extending the Tool

### Adding a New Optimization Suggestion Type

1. Add a new suggestion generator method to `TimingAnalyzer`:

```cpp
std::string suggestNewOptimization(const std::shared_ptr<TimingEdge>& edge) {
    // Logic to generate the suggestion
    return suggestion;
}
```

2. Update the `analyzePath` method to call your new suggestion generator:

```cpp
TimingPathAnalysis TimingAnalyzer::analyzePath(const TimingPath& path) {
    // ...
    if (/* condition for your new optimization */) {
        analysis.optimizationSuggestion = suggestNewOptimization(worstEdge);
    }
    // ...
}
```

### Supporting a New Timing Report Format

1. Create a new parser class derived from a common interface:

```cpp
class NewFormatParser : public ITimingParser {
public:
    std::vector<TimingPath> parseFile(const std::string& filename) override;
    
private:
    // Format-specific parsing methods
};
```

2. Update the main program to select the appropriate parser:

```cpp
std::unique_ptr<ITimingParser> createParser(const std::string& format) {
    if (format == "primetime") {
        return std::make_unique<TimingParser>();
    } else if (format == "newformat") {
        return std::make_unique<NewFormatParser>();
    }
    // ...
}
```

## Code Style Guidelines

This project follows the Google C++ Style Guide. Key points:

- Use 4 spaces for indentation
- Use `snake_case` for variable and function names
- Use `CamelCase` for class names
- Use `UPPER_CASE` for constants
- Always use braces for control statements, even for single-line bodies
- Limit line length to 100 characters
- C++17 features are encouraged (e.g., `std::optional`, `std::filesystem`)

## Performance Considerations

1. **Large Report Handling**: 
   - The parser can handle reports up to 50 MB, but for larger files, consider implementing streaming parsing.

2. **Memory Optimization**:
   - The node cache in TimingParser prevents duplicate node creation, reducing memory usage.

3. **Multi-threading**:
   - For batch processing, multi-threading can be implemented in the Tcl script to process multiple reports in parallel.

## Testing

### Unit Testing

Unit tests are written using the Google Test framework. To run the tests:

```bash
cd build
make test
```

### Adding New Tests

1. Create a new test file in the `tests/` directory
2. Include the Google Test header and the headers for the classes you want to test
3. Write test fixtures and test cases
4. Update `CMakeLists.txt` to include your new test file

### Code Coverage

To generate code coverage reports:

```bash
cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON ..
make
make test
make coverage
```

This will generate coverage reports in the `build/coverage` directory.

## Debugging

For debugging, build the project with debug symbols:

```bash
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```

You can then use GDB or LLDB to debug the executable:

```bash
gdb bin/timing_analysis
```

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Write tests for your changes
5. Ensure all tests pass
6. Submit a pull request

## Resources

- [C++17 Standard](https://isocpp.org/std/the-standard)
- [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
- [Google Test Framework](https://github.com/google/googletest)
- [CMake Documentation](https://cmake.org/documentation/) 