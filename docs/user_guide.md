# Timing Analysis Tool User Guide

## Overview

The Timing Analysis Tool is designed to help digital designers identify and optimize critical timing paths in their designs. It parses static timing reports (e.g., from Synopsys PrimeTime), identifies critical paths, and suggests optimization strategies.

## Installation

### Prerequisites

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.12 or higher
- UNIX-like environment (Linux, macOS, WSL)
- Tcl interpreter (for automation scripts)

### Building from Source

```bash
# Clone the repository
git clone https://github.com/yourusername/timing-analysis-tool.git
cd timing-analysis-tool

# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
make

# Install (optional)
sudo make install
```

The default installation locations are:
- Binary: `/usr/local/bin/timing_analysis`
- Scripts: `/usr/local/bin/run_timing_analysis.tcl`

## Input Format

The tool expects timing reports in a specific format. Here's an example of the expected format:

```
Path   Endpoint   Startpoint   Delay
------------------------------------------------
P1     FF_Q        PI          2.345
P1.1   NET1        PI          0.123
P1.2   INV1        NET1        0.456
P1.3   NET2        INV1        0.789
P1.4   FF_D        NET2        0.977
```

Each path starts with a header line that contains:
- Path ID (e.g., "P1")
- Endpoint name (e.g., "FF_Q")
- Startpoint name (e.g., "PI")
- Total path delay in nanoseconds (e.g., "2.345")

Following the header, there are one or more stage lines that contain:
- Stage ID (e.g., "P1.1")
- From node name (e.g., "NET1")
- To node name (e.g., "INV1")
- Stage delay in nanoseconds (e.g., "0.456")

## Using the Command Line Tool

### Basic Usage

```bash
# Analyze a single report
timing_analysis -f path/to/report.rpt -o results.txt

# Analyze all reports in a directory
timing_analysis -d path/to/reports/ -o results.txt
```

### Command Line Options

| Option | Description |
| ------ | ----------- |
| `-f, --file PATH` | Input timing report file path |
| `-d, --dir PATH` | Directory containing timing reports |
| `-o, --output PATH` | Output analysis results to file |
| `-k, --topk N` | Number of critical paths to show (default: 10) |
| `-h, --help` | Show help message |

## Using the Tcl Automation Script

### Basic Usage

```bash
# Process multiple reports
./run_timing_analysis.tcl -reports ./reports/ -out ./analysis/ -topk 10
```

### Tcl Script Options

| Option | Description |
| ------ | ----------- |
| `-reports, --reports DIR` | Directory containing timing reports (required) |
| `-out, --out DIR` | Output directory for results (default: .) |
| `-topk, --topk N` | Number of critical paths to show (default: 10) |
| `-bin, --bin PATH` | Path to timing_analysis binary (default: timing_analysis) |
| `-v, --verbose` | Enable verbose output |
| `-h, --help` | Show help message |

## Understanding the Output

The tool generates a list of critical paths, sorted by total delay. For each path, it provides:
- Path ID
- Total delay
- Worst stage delay and location
- Optimization suggestion

Example output:

```
Top 3 Critical Paths:
1. P7: Delay = 5.678 ns (Stage max: 1.234 ns at INV42) — Suggest: replace INV42 with INV42_HF
2. P3: Delay = 5.432 ns (Stage max: 1.111 ns at NAND17) — Suggest: balance fan-out after net NET7
3. P5: Delay = 5.321 ns (Stage max: 1.200 ns at BUF9) — Suggest: insert pipeline register between NET9 → FF2
```

### Optimization Suggestions

The tool provides three types of optimization suggestions:

1. **Cell Replacement**: When a cell has high delay, the tool suggests replacing it with a faster variant.
2. **Pipeline Insertion**: When a net has high delay, the tool suggests inserting a pipeline register.
3. **Fan-out Balancing**: When a node drives many loads, the tool suggests balancing the fan-out.

## Advanced Usage

### Processing Multiple Reports

When you have multiple timing reports (e.g., from different corners or runs), you can use the Tcl script to process them all and generate a summary:

```bash
./run_timing_analysis.tcl -reports ./reports/ -out ./analysis/ -topk 10 -v
```

This will:
1. Process each report in the `./reports/` directory
2. Generate individual analysis files in the `./analysis/` directory
3. Create a summary file `summary_analysis.txt` with the worst path from each report

### Integrating with Design Flows

You can integrate the tool into your design flow by calling the Tcl script from your design scripts:

```tcl
# In your design flow script
exec run_timing_analysis.tcl -reports $timing_report_dir -out $analysis_dir -topk 10
```

## Troubleshooting

### Common Issues

**Issue**: The tool reports "Failed to open file"
**Solution**: Check that the file path is correct and the file has read permissions.

**Issue**: The tool doesn't find any paths in the report
**Solution**: Check that the report format matches the expected format. The tool looks for lines starting with "Path" to identify path headers.

**Issue**: The Tcl script fails with "No such file or directory"
**Solution**: Check that the timing_analysis binary is in your PATH or specify its location with the `-bin` option.

## Getting Help

If you encounter issues or have questions, please open an issue on GitHub.

## Next Steps

For more advanced usage and development information, see:
- [Developer Guide](developer_guide.md)
- [API Reference](api_reference.md) 