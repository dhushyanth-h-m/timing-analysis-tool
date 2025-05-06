#!/usr/bin/env tclsh

# Timing Analysis Automation Script
# This script runs the timing analysis tool on multiple reports in batch

# Process command line arguments
proc parse_args {argv} {
    global reports_dir output_dir topk binary_path verbose
    
    set reports_dir ""
    set output_dir "."
    set topk 10
    set binary_path "timing_analysis"
    set verbose 0
    
    for {set i 0} {$i < [llength $argv]} {incr i} {
        set arg [lindex $argv $i]
        switch -- $arg {
            "-reports" -
            "--reports" {
                incr i
                set reports_dir [lindex $argv $i]
            }
            "-out" -
            "--out" {
                incr i
                set output_dir [lindex $argv $i]
            }
            "-topk" -
            "--topk" {
                incr i
                set topk [lindex $argv $i]
            }
            "-bin" -
            "--bin" {
                incr i
                set binary_path [lindex $argv $i]
            }
            "-v" -
            "--verbose" {
                set verbose 1
            }
            "-h" -
            "--help" {
                print_usage
                exit 0
            }
            default {
                puts "Error: Unknown option '$arg'"
                print_usage
                exit 1
            }
        }
    }
    
    # Validate arguments
    if {$reports_dir eq ""} {
        puts "Error: Reports directory (-reports) is required"
        print_usage
        exit 1
    }
    
    if {![file exists $reports_dir]} {
        puts "Error: Reports directory does not exist: $reports_dir"
        exit 1
    }
    
    # Ensure output directory exists
    file mkdir $output_dir
}

# Print usage information
proc print_usage {} {
    puts "Usage: run_timing_analysis.tcl \[OPTIONS\]"
    puts "Options:"
    puts "  -reports, --reports DIR   Directory containing timing reports (required)"
    puts "  -out, --out DIR           Output directory for analysis results (default: .)"
    puts "  -topk, --topk N           Number of critical paths to show (default: 10)"
    puts "  -bin, --bin PATH          Path to timing_analysis binary (default: timing_analysis)"
    puts "  -v, --verbose             Enable verbose output"
    puts "  -h, --help                Show this help message"
}

# Run timing analysis on a single report file
proc process_report {report_file} {
    global output_dir topk binary_path verbose
    
    set report_name [file tail [file rootname $report_file]]
    set output_file [file join $output_dir "${report_name}_analysis.txt"]
    
    if {$verbose} {
        puts "Processing: $report_file"
        puts "Output: $output_file"
    }
    
    # Execute timing analysis command
    set cmd_args [list $binary_path "-f" $report_file "-o" $output_file "-k" $topk]
    if {$verbose} {
        puts "Running: [join $cmd_args]"
    }
    
    if {[catch {exec {*}$cmd_args} result]} {
        puts "Error executing timing analysis: $result"
        return 0
    }
    
    return 1
}

# Generate summary of all analyzed reports
proc generate_summary {processed_reports} {
    global output_dir
    
    set summary_file [file join $output_dir "summary_analysis.txt"]
    puts "Generating summary in $summary_file"
    
    set results {}
    foreach report $processed_reports {
        set report_name [file tail [file rootname $report]]
        set output_file [file join $output_dir "${report_name}_analysis.txt"]
        
        if {[file exists $output_file]} {
            set fd [open $output_file r]
            set contents [read $fd]
            close $fd
            
            # Extract the worst path from each report
            if {[regexp {1\.\s+([^:]+):\s+Delay\s+=\s+([\d\.]+)\s+ns} $contents -> path_id delay]} {
                lappend results [list $report_name $path_id $delay]
            }
        }
    }
    
    # Sort by delay (descending)
    set sorted_results [lsort -decreasing -real -index 2 $results]
    
    # Write summary
    set fd [open $summary_file w]
    puts $fd "Timing Analysis Summary"
    puts $fd "======================="
    puts $fd ""
    puts $fd "Worst Path Summary (sorted by delay):"
    puts $fd ""
    puts $fd [format "%-20s %-10s %-10s" "Report" "Path" "Delay (ns)"]
    puts $fd [string repeat "-" 50]
    
    foreach entry $sorted_results {
        lassign $entry report_name path_id delay
        puts $fd [format "%-20s %-10s %-10.3f" $report_name $path_id $delay]
    }
    
    puts $fd "\n"
    puts $fd "Full analysis results are available in individual report files."
    close $fd
    
    puts "Summary generated in $summary_file"
}

# Main procedure
proc main {argv} {
    # Parse command line arguments
    parse_args $argv
    
    global reports_dir output_dir verbose
    
    # Find all timing report files
    set report_files {}
    foreach file [glob -nocomplain -directory $reports_dir *.rpt] {
        lappend report_files $file
    }
    
    # Check if we found any reports
    if {[llength $report_files] == 0} {
        puts "Error: No timing report (.rpt) files found in $reports_dir"
        exit 1
    }
    
    puts "Found [llength $report_files] timing report files"
    
    # Process each report
    set processed_reports {}
    foreach report $report_files {
        if {[process_report $report]} {
            lappend processed_reports $report
        }
    }
    
    puts "Successfully processed [llength $processed_reports] reports"
    
    # Generate summary
    if {[llength $processed_reports] > 0} {
        generate_summary $processed_reports
    }
}

# Run the script
main $argv 