# Timing Analysis Tool for Digital Design

A command-line application for parsing static timing reports, identifying critical paths, and suggesting optimization strategies. The tool is specifically designed for ASIC/FPGA designers who need to analyze timing reports from tools like Synopsys PrimeTime.

## Features

- Parse text-based static timing reports
- Build in-memory representation of timing paths
- Identify and rank top critical paths
- Calculate path delays and identify worst stage delays
- Suggest optimization strategies based on timing characteristics
- Process multiple reports in batch via Tcl scripts

## Requirements

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.12 or higher
- UNIX-like environment (Linux, macOS, WSL)
- Tcl interpreter (for automation scripts)

## Building from Source

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

## Usage

### Command Line

```bash
# Analyze a single report
timing_analysis -f path/to/report.rpt -k 10 -o results.txt

# Options:
# -f, --file PATH       Input timing report file path
# -d, --dir PATH        Directory containing timing reports
# -o, --output PATH     Output analysis results to file
# -k, --topk N          Number of critical paths to show (default: 10)
# -h, --help            Show this help message
```

### Batch Processing with Tcl

```bash
# Process multiple reports
./run_timing_analysis.tcl -reports ./reports/ -out ./analysis/ -topk 10

# Options:
# -reports, --reports DIR   Directory containing timing reports
# -out, --out DIR           Output directory for results (default: .)
# -topk, --topk N           Number of critical paths to show (default: 10)
# -bin, --bin PATH          Path to timing_analysis binary (default: timing_analysis)
# -v, --verbose             Enable verbose output
```

## Example Input & Output

### Input Format (Timing Report)

```
Path   Endpoint   Startpoint   Delay
------------------------------------------------
P1     FF_Q        PI          2.345
P1.1   NET1        INV1        0.123
P1.2   INV1_Q      BRANCH      0.456
```

### Output Format

```
Top 3 Critical Paths:
1. P7: Delay = 5.678 ns (Stage max: 1.234 ns at INV42) — Suggest: replace INV42 with INV42_HF
2. P3: Delay = 5.432 ns (Stage max: 1.111 ns at NAND17) — Suggest: balance fan-out after net NET7
3. P5: Delay = 5.321 ns (Stage max: 1.200 ns at BUF9) — Suggest: insert pipeline register between NET9 → FF2
```

## Project Structure

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
└── README.md              # This file
```

## Documentation

For more detailed documentation, see:

- [User Guide](docs/user_guide.md)
- [Developer Guide](docs/developer_guide.md)
- [API Reference](docs/api_reference.md)

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request 