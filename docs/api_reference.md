# Timing Analysis Tool API Reference

This document provides detailed information about the classes, methods, and data structures in the Timing Analysis Tool codebase. It's intended for developers who want to understand the code or extend the tool.

## Table of Contents

1. [Data Structures](#data-structures)
    - [TimingNode](#timingnode)
    - [TimingEdge](#timingedge)
    - [TimingPath](#timingpath)
    - [TimingPathAnalysis](#timingpathanalysis)
2. [Core Classes](#core-classes)
    - [TimingParser](#timingparser)
    - [TimingAnalyzer](#timinganalyzer)
    - [Utils Namespace](#utils-namespace)
3. [Command Line Interface](#command-line-interface)
4. [Tcl Script Interface](#tcl-script-interface)

## Data Structures

### TimingNode

Represents a node in a timing path, such as a cell or pin.

#### Members

| Name | Type | Description |
|------|------|-------------|
| `name` | `std::string` | Node name (e.g., "INV1", "FF_Q") |
| `type` | `std::string` | Node type (e.g., "flop", "inverter", "net") |
| `capacitance` | `double` | Node capacitance (not used in current implementation) |
| `slew` | `double` | Signal slew (not used in current implementation) |

#### Constructors

```cpp
TimingNode(std::string name, std::string type);
```

Creates a new TimingNode with the specified name and type.

### TimingEdge

Represents a connection between two TimingNodes.

#### Members

| Name | Type | Description |
|------|------|-------------|
| `from` | `std::shared_ptr<TimingNode>` | Source node |
| `to` | `std::shared_ptr<TimingNode>` | Destination node |
| `delay` | `double` | Total edge delay |
| `netDelay` | `double` | Net component of delay |
| `cellDelay` | `double` | Cell component of delay |

#### Constructors

```cpp
TimingEdge(std::shared_ptr<TimingNode> from, std::shared_ptr<TimingNode> to, double delay = 0.0);
```

Creates a new TimingEdge with the specified source and destination nodes, and optional delay.

### TimingPath

Represents a complete timing path from startpoint to endpoint.

#### Members

| Name | Type | Description |
|------|------|-------------|
| `id` | `std::string` | Path identifier (e.g., "P1") |
| `startpoint` | `std::string` | Path startpoint name |
| `endpoint` | `std::string` | Path endpoint name |
| `totalDelay` | `double` | Total path delay |
| `edges` | `std::vector<std::shared_ptr<TimingEdge>>` | Edges in this path |

#### Methods

```cpp
std::pair<double, std::shared_ptr<TimingEdge>> getWorstStage() const;
```

Returns the edge with the highest delay and its delay value as a pair.

### TimingPathAnalysis

Extended information about a timing path including optimization suggestions.

#### Members

| Name | Type | Description |
|------|------|-------------|
| `path` | `std::shared_ptr<TimingPath>` | The analyzed timing path |
| `worstStageDelay` | `double` | Delay of the worst stage |
| `worstStage` | `std::shared_ptr<TimingEdge>` | Edge with the worst delay |
| `optimizationSuggestion` | `std::string` | Suggested optimization |

#### Constructors

```cpp
TimingPathAnalysis(std::shared_ptr<TimingPath> path);
```

Creates a new TimingPathAnalysis for the specified path, automatically calculating the worst stage.

## Core Classes

### TimingParser

Parses static timing reports into TimingPath objects.

#### Public Methods

```cpp
std::vector<TimingPath> parseFile(const std::string& filename);
```

Parses the specified timing report file and returns a vector of TimingPath objects.

**Parameters:**
- `filename`: Path to the timing report file

**Returns:**
- Vector of TimingPath objects

**Throws:**
- `std::runtime_error`: If the file cannot be opened or has an invalid format

#### Private Methods

```cpp
std::pair<TimingPath, size_t> parsePath(
    const std::vector<std::string>& lines, size_t startLine);
```

Parses a single timing path section from the report.

**Parameters:**
- `lines`: Vector of lines from the report
- `startLine`: Line index where the path section starts

**Returns:**
- A pair containing the parsed TimingPath and the next line index to process

```cpp
std::tuple<std::string, std::string, std::string, double> 
parsePathHeader(const std::string& line);
```

Parses a timing path header line.

**Parameters:**
- `line`: Header line from the report

**Returns:**
- A tuple containing (path ID, startpoint, endpoint, total delay)

```cpp
std::shared_ptr<TimingEdge> parsePathStage(const std::string& line);
```

Parses a timing path stage line.

**Parameters:**
- `line`: Stage line from the report

**Returns:**
- A TimingEdge representing the stage, or nullptr if parsing fails

### TimingAnalyzer

Analyzes timing paths and generates optimization suggestions.

#### Public Methods

```cpp
std::vector<TimingPathAnalysis> findCriticalPaths(
    const std::vector<TimingPath>& paths, int topK);
```

Finds the top K critical paths by total delay.

**Parameters:**
- `paths`: Vector of timing paths to analyze
- `topK`: Number of critical paths to return

**Returns:**
- Vector of critical path analyses, sorted by delay (highest first)

```cpp
TimingPathAnalysis analyzePath(const TimingPath& path);
```

Generates an optimization suggestion for a timing path.

**Parameters:**
- `path`: The timing path to analyze

**Returns:**
- TimingPathAnalysis with optimization suggestion

#### Private Methods

```cpp
std::string suggestFanoutOptimization(const std::shared_ptr<TimingEdge>& edge);
```

Generates a suggestion for fan-out optimization.

**Parameters:**
- `edge`: The edge with high fan-out

**Returns:**
- Optimization suggestion string

```cpp
std::string suggestCellReplacement(const std::shared_ptr<TimingEdge>& edge);
```

Generates a suggestion for cell replacement.

**Parameters:**
- `edge`: The edge with high cell delay

**Returns:**
- Optimization suggestion string

```cpp
std::string suggestPipelineInsertion(const std::shared_ptr<TimingEdge>& edge);
```

Generates a suggestion for pipeline register insertion.

**Parameters:**
- `edge`: The edge with high net delay

**Returns:**
- Optimization suggestion string

### Utils Namespace

Utility functions for the timing analysis tool.

#### Functions

```cpp
void printResults(const std::vector<TimingPathAnalysis>& criticalPaths, 
                  const std::string& outputFile = "");
```

Prints the analysis results to console and/or file.

**Parameters:**
- `criticalPaths`: Vector of critical path analyses
- `outputFile`: Optional file path to write results to (empty string for console only)

```cpp
std::string formatPathResult(int index, const TimingPathAnalysis& analysis);
```

Formats a timing path analysis result as a string.

**Parameters:**
- `index`: Index of the path (1-based)
- `analysis`: The timing path analysis

**Returns:**
- Formatted result string

```cpp
std::string formatTime(double seconds);
```

Converts time in seconds to a human-readable format.

**Parameters:**
- `seconds`: Time in seconds

**Returns:**
- Formatted time string (e.g., "123 ms", "1.23 s")

## Command Line Interface

The tool provides a command-line interface with the following options:

```
Usage: timing_analysis [OPTIONS]
Options:
  -f, --file PATH       Input timing report file path
  -d, --dir PATH        Directory containing timing reports
  -o, --output PATH     Output analysis results to file
  -k, --topk N          Number of critical paths to show (default: 10)
  -h, --help            Show this help message
```

## Tcl Script Interface

The tool provides a Tcl script for batch processing with the following options:

```
Usage: run_timing_analysis.tcl [OPTIONS]
Options:
  -reports, --reports DIR   Directory containing timing reports (required)
  -out, --out DIR           Output directory for results (default: .)
  -topk, --topk N           Number of critical paths to show (default: 10)
  -bin, --bin PATH          Path to timing_analysis binary (default: timing_analysis)
  -v, --verbose             Enable verbose output
  -h, --help                Show this help message
```

The script processes all timing report files in the specified directory, generates individual analysis files, and creates a summary file with the worst path from each report. 