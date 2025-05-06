cmake_minimum_required(VERSION 3.12)
project(timing_analysis_tool VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
endif()

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# Define source files
set(SOURCES
    src/main.cpp
    src/parser.cpp
    src/analyzer.cpp
    src/utils.cpp
)

# Create executable
add_executable(timing_analysis ${SOURCES})

# Set output directory
set_target_properties(timing_analysis PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Installation rules
install(TARGETS timing_analysis
    RUNTIME DESTINATION bin
)

# Install scripts
install(DIRECTORY scripts/
    DESTINATION bin
    FILES_MATCHING PATTERN "*.tcl"
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

# Add documentation target
add_custom_target(docs
    COMMAND echo "Generating documentation..."
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# Add test target
add_custom_target(test
    COMMAND echo "Running tests..."
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# Add clean target
add_custom_target(clean-all
    COMMAND ${CMAKE_MAKE_PROGRAM} clean
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Optional: Add support for Google Test if available
find_package(GTest QUIET)
if(GTEST_FOUND)
    enable_testing()
    include_directories(${GTEST_INCLUDE_DIRS})
    
    # Define test sources
    set(TEST_SOURCES
        tests/test_parser.cpp
        tests/test_analyzer.cpp
    )
    
    # Add test executable
    add_executable(run_tests ${TEST_SOURCES} src/parser.cpp src/analyzer.cpp src/utils.cpp)
    target_link_libraries(run_tests ${GTEST_LIBRARIES} pthread)
    
    # Register test
    add_test(NAME UnitTests COMMAND run_tests)
endif() 